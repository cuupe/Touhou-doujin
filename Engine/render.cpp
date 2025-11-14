#include "render.h"
#include "sprite.h"
#include "resources.h"
#include "maths.h"
#include "D3D/model.h"
namespace Engine::Render {
	using namespace D3D;
	Renderer::Renderer(SDL_Renderer* s_r, ResourceManager* r_s, 
		ID3D11Device* _device, ID3D11DeviceContext* device_ctx,
		float _width, float _height)
		:renderer(s_r), res(r_s), device(_device), device_context(device_ctx),
		screen_width(_width), screen_height(_height)
	{
		float aspectRatio = screen_width / screen_height;

		// 计算透视投影矩阵
		XMMATRIX P_Persp = XMMatrixPerspectiveFovLH(
			XM_PIDIV4,      // 视野角度：45度 (π/4 弧度)
			aspectRatio,    // 宽高比：w / h
			0.1f,           // 近裁剪平面
			100.0f          // 远裁剪平面
		);

		// 将计算结果赋值给成员变量
		m_projection = P_Persp;
		XMMATRIX P_Ortho =
			XMMatrixOrthographicOffCenterLH(
				0.0f, screen_width,
				screen_height, 0.0f,
				0.0f, 1.0f
			);

		m_projection_2d = P_Ortho;


		f[0] = f[1] = f[2] = 0.0f;
		f[3] = 1.0f;
		SDL_PropertiesID props = SDL_GetRendererProperties(renderer);
		swap_chain = (IDXGISwapChain1*)SDL_GetPointerProperty(props, SDL_PROP_RENDERER_D3D11_SWAPCHAIN_POINTER, nullptr);
		if (!swap_chain) {
			throw std::runtime_error("无法获取交换链");
		}


		D3D11_BUFFER_DESC desc{};
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.ByteWidth = sizeof(Vertex) * 4;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		_device->CreateBuffer(&desc, nullptr, vb.GetAddressOf());

		D3D11_INPUT_ELEMENT_DESC layout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		ComPtr<ID3DBlob> vs_blob, ps_blob, error_blob;
		HRESULT hr = D3DCompileFromFile(L"Engine/D3D/Shader/vs_sprite.hlsl", nullptr, nullptr, "main", "vs_5_0",
			D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_OPTIMIZATION_LEVEL3, 0, &vs_blob, &error_blob);

		if (FAILED(hr)) {
			spdlog::error("VS 编译失败: {}", error_blob ? (char*)error_blob->GetBufferPointer() : "未知错误");
			return;
		}

		hr = D3DCompileFromFile(L"Engine/D3D/Shader/ps_sprite.hlsl", nullptr, nullptr, "main", "ps_5_0",
			D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_OPTIMIZATION_LEVEL3, 0, &ps_blob, &error_blob);
		if (FAILED(hr)) {
			spdlog::error("PS 编译失败: {}", error_blob ? (char*)error_blob->GetBufferPointer() : "未知");
			return;
		}


		_device->CreateVertexShader(vs_blob->GetBufferPointer(), 
			vs_blob->GetBufferSize(), nullptr, vertex_shader.GetAddressOf());
		_device->CreatePixelShader(ps_blob->GetBufferPointer(), 
			ps_blob->GetBufferSize(), nullptr, pixel_shader.GetAddressOf());


		hr = _device->CreateInputLayout(
			layout, 3,
			vs_blob->GetBufferPointer(),
			vs_blob->GetBufferSize(),
			input_layout.GetAddressOf());

		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		device_ctx->IASetVertexBuffers(0, 1, vb.GetAddressOf(), &stride, &offset);
		device_ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		device_ctx->IASetInputLayout(input_layout.Get());
		device_ctx->VSSetShader(vertex_shader.Get(), nullptr, 0);
		device_ctx->PSSetShader(pixel_shader.Get(), nullptr, 0);

		D3D11_SAMPLER_DESC sd{};
		sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sd.AddressU = sd.AddressV = sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		sd.MaxLOD = D3D11_FLOAT32_MAX;
		_device->CreateSamplerState(&sd, sampler_state.GetAddressOf());

		D3D11_BUFFER_DESC cbd{};
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.ByteWidth = sizeof(ConstantBuffer);
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		_device->CreateBuffer(&cbd, nullptr, cb.GetAddressOf());

		UpdateProjection();

		D3D11_BLEND_DESC bd{};
		bd.RenderTarget[0].BlendEnable = TRUE;
		bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
		bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		_device->CreateBlendState(&bd, blend_state.GetAddressOf());

		ComPtr<ID3D11Texture2D> backbuffer_tex;
		hr = swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)backbuffer_tex.GetAddressOf());
		if (FAILED(hr)) {
			spdlog::error("GetBuffer 失败: 0x{:08X}", hr);
			return;
		}
		hr = device->CreateRenderTargetView(backbuffer_tex.Get(), nullptr, rtv.GetAddressOf());
		if (FAILED(hr)) {
			spdlog::error("CreateRenderTargetView 失败: 0x{:08X}", hr);
			return;
		}
		spdlog::info("初始 RTV 创建成功！");

		if (!rtv) {
			spdlog::error("无法获取 SDL D3D11 RTV！检查是否为 direct3d11 后端");
			return;
		}

		D3D11_RASTERIZER_DESC rs_desc = {};
		rs_desc.FillMode = D3D11_FILL_SOLID;
		rs_desc.CullMode = D3D11_CULL_NONE;
		rs_desc.FrontCounterClockwise = FALSE;
		rs_desc.DepthBias = 0;
		rs_desc.DepthBiasClamp = 0.0f;
		rs_desc.SlopeScaledDepthBias = 0.0f;
		rs_desc.DepthClipEnable = TRUE;
		rs_desc.ScissorEnable = FALSE;
		rs_desc.MultisampleEnable = FALSE;
		rs_desc.AntialiasedLineEnable = FALSE;
		hr = device->CreateRasterizerState(&rs_desc, rasterizer_state_2d.GetAddressOf());
		if (FAILED(hr)) {
			spdlog::error("CreateRasterizerState failed: 0x{:08X}", hr);
			return;
		}

		hr = D3DCompileFromFile(L"Engine/D3D/Shader/vs_3dbase.hlsl", nullptr, nullptr, "main", "vs_5_0",
			D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_OPTIMIZATION_LEVEL3, 0, &vs_blob, &error_blob);
		if (FAILED(hr)) {
			spdlog::error("PS 编译失败: {}", error_blob ? (char*)error_blob->GetBufferPointer() : "未知");
			return;
		}

		hr = D3DCompileFromFile(L"Engine/D3D/Shader/ps_3dbase.hlsl", nullptr, nullptr, "main", "ps_5_0",
			D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_OPTIMIZATION_LEVEL3, 0, &ps_blob, &error_blob);
		if (FAILED(hr)) {
			spdlog::error("PS 编译失败: {}", error_blob ? (char*)error_blob->GetBufferPointer() : "未知");
			return;
		}

		_device->CreateVertexShader(vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), nullptr, vertex_shader_3d.GetAddressOf());
		_device->CreatePixelShader(ps_blob->GetBufferPointer(), ps_blob->GetBufferSize(), nullptr, pixel_shader_3d.GetAddressOf());


		device->CreateInputLayout(
			layout, 3,
			vs_blob->GetBufferPointer(),
			vs_blob->GetBufferSize(),
			input_layout.GetAddressOf()
		);
		

		D3D11_TEXTURE2D_DESC backbufferDesc;
		backbuffer_tex->GetDesc(&backbufferDesc);

		D3D11_TEXTURE2D_DESC depthDesc = {};
		depthDesc.Width = backbufferDesc.Width;
		depthDesc.Height = backbufferDesc.Height;
		depthDesc.MipLevels = 1;
		depthDesc.ArraySize = 1;
		depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthDesc.SampleDesc.Count = backbufferDesc.SampleDesc.Count;
		depthDesc.Usage = D3D11_USAGE_DEFAULT;
		depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		hr = _device->CreateTexture2D(&depthDesc, nullptr, depth_stencil_buffer.GetAddressOf());

		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = depthDesc.Format;
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		hr = _device->CreateDepthStencilView(depth_stencil_buffer.Get(), &dsvDesc, dsv.GetAddressOf());
		if (FAILED(hr)) {
			spdlog::error("DSV/深度缓冲创建失败: 0x{:08X}", hr);
			return;
		}
		spdlog::info("DSV 创建成功！");

		// 深度状态 (用于 Begin3D)
		D3D11_DEPTH_STENCIL_DESC dssDesc = {};
		dssDesc.DepthEnable = true;
		dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dssDesc.DepthFunc = D3D11_COMPARISON_LESS;
		hr = _device->CreateDepthStencilState(&dssDesc, depthState_3d.GetAddressOf());


		dssDesc.DepthEnable = false;
		dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // 禁用深度写入
		dssDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
		hr = _device->CreateDepthStencilState(&dssDesc, depthState_2d.GetAddressOf());
		// 3D 光栅化状态
		D3D11_RASTERIZER_DESC rs_desc_3d = {};
		rs_desc_3d.FillMode = D3D11_FILL_SOLID;
		rs_desc_3d.CullMode = D3D11_CULL_BACK;
		rs_desc_3d.FrontCounterClockwise = FALSE;
		rs_desc_3d.DepthClipEnable = TRUE;
		hr = _device->CreateRasterizerState(&rs_desc_3d, rasterizer_state_3d.GetAddressOf());

		if (FAILED(hr)) {
			spdlog::error("3D 状态创建失败: 0x{:08X}", hr);
			return;
		}

		spdlog::info("2D RasterizerState 创建成功");
	}


	void Renderer::UpdateProjection() {
		ComPtr<ID3D11Texture2D> backbuffer;
		HRESULT hr = swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backbuffer);
		if (FAILED(hr)) return;

		D3D11_TEXTURE2D_DESC desc{};
		backbuffer->GetDesc(&desc);

		float physicalW = (float)desc.Width;
		float physicalH = (float)desc.Height;

		const float LOGICAL_W = screen_width;
		const float LOGICAL_H = screen_height;

		float targetAspect = LOGICAL_W / LOGICAL_H;
		float windowAspect = physicalW / physicalH;

		float scale = 1.0f;
		float vpX = 0, vpY = 0, vpW = physicalW, vpH = physicalH;

		if (windowAspect > targetAspect)
		{
			scale = physicalH / LOGICAL_H;
			vpW = LOGICAL_W * scale;
			vpH = physicalH;
			vpX = (physicalW - vpW) * 0.5f;
			vpY = 0;
		}
		else
		{
			// 窗口太高 → 以宽度为基准
			scale = physicalW / LOGICAL_W;
			vpW = physicalW;
			vpH = LOGICAL_H * scale;
			vpX = 0;
			vpY = (physicalH - vpH) * 0.5f;
		}

		SetViewPort(vpX, vpY, vpW, vpH);

		m_projection_2d = XMMatrixOrthographicOffCenterLH(
			0.0f, LOGICAL_W,
			LOGICAL_H, 0.0f,
			0.0f, 1.0f
		);

		m_projection = XMMatrixPerspectiveFovLH(
			XM_PIDIV4, targetAspect,
			0.1f, 1000.0f
		);
	}

	std::optional<SDL_FRect> Renderer::GetSpriteSrcRect(const Sprite& sprite)
	{
		SDL_Texture* texture = res->GetTexture(sprite.GetTextureName())->texture.get();
		if (!texture) {
			spdlog::error("无法为 ID {} 获取纹理。", sprite.GetTextureName());
			return std::nullopt;
		}

		auto src_rect = sprite.GetSourceRect();
		if (src_rect.has_value()) {
			if (src_rect.value().w <= 0 || src_rect.value().h <= 0) {
				spdlog::error("源矩形尺寸无效，ID: {}", sprite.GetTextureName());
				return std::nullopt;
			}
			return src_rect;
		}
		else { 
			SDL_FRect result = { 0, 0, 0, 0 };
			if (!SDL_GetTextureSize(texture, &result.w, &result.h)) {
				spdlog::error("无法获取纹理尺寸，ID: {}", sprite.GetTextureName());
				return std::nullopt;
			}
			return result;
		}
	}

	void Renderer::SetDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
	{
		if (!SDL_SetRenderDrawColor(renderer, r, g, b, a)) {
			spdlog::error("设置渲染绘制颜色失败：{}", SDL_GetError());
		}
	}

	void Renderer::SetDrawColorFloat(float r, float g, float b, float a)
	{
		if (!SDL_SetRenderDrawColorFloat(renderer, r, g, b, a)) {
			spdlog::error("设置渲染绘制颜色失败：{}", SDL_GetError());
		}
	}

	void Renderer::ClearScreen()
	{
		device_context->ClearRenderTargetView(rtv.Get(), f);
		if (dsv) {
			device_context->ClearDepthStencilView(dsv.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
		}
	
	}

	void Renderer::Present()
	{
		swap_chain->Present(0, 0);
	}

	//vector为二维向量
	void Renderer::DrawSprite(const Sprite& sprite, const Vec2& pos, 
		const Vec2& scale, const std::optional<SDL_FRect>& s_rc, 
		const std::optional<SDL_Color>& color, double angle)
	{
		auto texture = res->GetTexture(sprite.GetTextureName());
		if (!texture->texture.get()) {
			spdlog::error("无法获取纹理：{}", sprite.GetTextureName());
			return;
		}
		SDL_Color c = { 255, 255, 255, 255 };
		if(color.has_value()){
			c = color.value();
		}

		SDL_FRect s_r;
		if (s_rc.has_value()) {
			s_r = s_rc.value();
		}
		else {
			s_r = GetSpriteSrcRect(sprite).value();
		}


		float s_w = s_r.w * scale.x;
		float s_h = s_r.h * scale.y;

		SDL_FRect d_r = {
			pos.x,
			pos.y,
			s_w,
			s_h
		};

		if (!Draw(texture, &s_r, &d_r, angle, nullptr, &c, 0)) {
			spdlog::error("渲染旋转纹理失败（ID: {}）：{}", sprite.GetTextureName(), SDL_GetError());
		}
	}

	bool Renderer::Draw(Resource::TextureResource* res, const SDL_FRect* srcrect,
		const SDL_FRect* dstrect, double angle, const SDL_FPoint* center, 
		const SDL_Color* _color, int flip)
	{
		auto srv = res->d3d_srv.Get();
		if (!device_context || !srv || !dstrect) return false;

		float cx = dstrect->w * 0.5f;
		float cy = dstrect->h * 0.5f;
		if (center) { cx = center->x; cy = center->y; }

		float tx = dstrect->x + cx;
		float ty = dstrect->y + cy;

		float u0 = 0, v0 = 0, u1 = 1, v1 = 1;
		if (srcrect) {
			if (srcrect->w < res->width) {
				u0 = srcrect->x / res->width;
				u1 = (srcrect->x + srcrect->w) / res->width;
			}
			if (srcrect->h < res->height) {
				v0 = srcrect->y / res->height;
				v1 = (srcrect->y + srcrect->h) / res->height;
			}
		}
		if (flip == 1) std::swap(u0, u1);
		else if (flip == 2) std::swap(v0, v1);

		float rad = DirectX::XMConvertToRadians(static_cast<float>(angle));
		float c = cosf(rad), s = sinf(rad);
		XMFLOAT4 color = { static_cast<float>(_color->r) / 255.0f,  static_cast<float>(_color->g) / 255.0f
			, static_cast<float>(_color->b) / 255.0f ,  static_cast<float>(_color->a) / 255.0f };
		Vertex quad[4] = {
			{ { tx + (-cx * c + cy * s), ty + (-cx * s - cy * c), 0 }, {u0, v0}, color },
			{ { tx + (-cx * c - (dstrect->h - cy) * s), ty + (-cx * s + (dstrect->h - cy) * c), 0 }, {u0, v1}, color },
			{ { tx + ((dstrect->w - cx) * c + cy * s), ty + ((dstrect->w - cx) * s - cy * c), 0 }, {u1, v0}, color },
			{ { tx + ((dstrect->w - cx) * c - (dstrect->h - cy) * s), ty + ((dstrect->w - cx) * s + (dstrect->h - cy) * c), 0 }, {u1, v1}, color }
		};

		D3D11_MAPPED_SUBRESOURCE mapped{};
		device_context->Map(vb.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
		memcpy(mapped.pData, quad, sizeof(quad));
		device_context->Unmap(vb.Get(), 0);

		ID3D11ShaderResourceView* srv_ptr = res->d3d_srv.Get();
		device_context->PSSetShaderResources(0, 1, &srv_ptr);

		device_context->Draw(4, 0);
		return true;
	}


	void Renderer::SetViewPort(float tlx, float tly, float w, float h) {
		D3D11_VIEWPORT vp{};
		vp.TopLeftX = tlx;
		vp.TopLeftY = tly;
		vp.Width = w;
		vp.Height = h;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		device_context->RSSetViewports(1, &vp);
	}

	void Renderer::Begin2D() {
		using namespace DirectX;

		// 检查核心资源是否有效
		if (!rtv || !device_context || !depthState_2d) {
			spdlog::error("无效的RTV、上下文或2D深度状态");
			return;
		}
		device_context->OMSetDepthStencilState(depthState_2d.Get(), 0);
		device_context->OMSetRenderTargets(1, rtv.GetAddressOf(), nullptr);


		ComPtr<ID3D11Texture2D> backbuffer;
		swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backbuffer);
		D3D11_TEXTURE2D_DESC desc{};
		backbuffer->GetDesc(&desc);

		D3D11_MAPPED_SUBRESOURCE mapped;
		if (SUCCEEDED(device_context->Map(cb.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) {
			XMMATRIX P_Ortho_Transpose = XMMatrixTranspose(m_projection_2d);

			memcpy(mapped.pData, &P_Ortho_Transpose, sizeof(XMMATRIX));
			device_context->Unmap(cb.Get(), 0);
		}

		device_context->IASetInputLayout(input_layout.Get());
		device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		device_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
		device_context->PSSetShader(pixel_shader.Get(), nullptr, 0);

		device_context->VSSetConstantBuffers(0, 1, cb.GetAddressOf());

		UINT stride = sizeof(Vertex), offset = 0;
		device_context->IASetVertexBuffers(0, 1, vb.GetAddressOf(), &stride, &offset);

		device_context->PSSetSamplers(0, 1, sampler_state.GetAddressOf());
		device_context->OMSetBlendState(blend_state.Get(), nullptr, 0xFFFFFFFF);
		device_context->RSSetState(rasterizer_state_2d.Get());
	}

	void Renderer::Begin3D() {
		if (!rtv || !device_context || !depthState_3d) {
			spdlog::error("无效的RTV、上下文或3D深度状态");
			return;
		}

		device_context->VSSetShader(vertex_shader_3d.Get(), nullptr, 0);
		device_context->PSSetShader(pixel_shader_3d.Get(), nullptr, 0);
		ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
		device_context->PSSetShaderResources(0, 1, nullSRV);

		device_context->IASetInputLayout(input_layout.Get());
		device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		device_context->RSSetState(rasterizer_state_3d.Get());

		device_context->OMSetDepthStencilState(depthState_3d.Get(), 1);

		device_context->OMSetRenderTargets(1, rtv.GetAddressOf(), dsv.Get());

		device_context->PSSetSamplers(0, 1, sampler_state.GetAddressOf());
	}
}
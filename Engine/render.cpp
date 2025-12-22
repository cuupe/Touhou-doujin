#include "render.h"
#include "sprite.h"
#include "resources.h"
#include "CameraManager.h"
#include "maths.h"
#include "D3D/model.h"
#include "camera.h"
namespace Engine::Render {
	using namespace D3D;
	Renderer::Renderer(SDL_Renderer* s_r, ResourceManager* r_s, CameraManager* c_m,
		ID3D11Device* _device, ID3D11DeviceContext* device_ctx,
		float _width, float _height)
		:renderer(s_r), res(r_s), cm(c_m), device(_device), device_context(device_ctx),
		screen_width(_width), screen_height(_height)
	{
		try {
			if (!renderer || !device || !device_context) {
				throw std::runtime_error("Renderer 构造参数为空 (SDL_Renderer 或 D3D Device 无效)");
			}
	
			f[0] = f[1] = f[2] = 0.0f; f[3] = 1.0f;
			SDL_PropertiesID props = SDL_GetRendererProperties(renderer);
			swap_chain = (IDXGISwapChain1*)SDL_GetPointerProperty(props, SDL_PROP_RENDERER_D3D11_SWAPCHAIN_POINTER, nullptr);
			if (!swap_chain) {
				throw std::runtime_error("无法从 SDL 获取交换链 (Swap Chain)");
			}
	
			D3D11_BUFFER_DESC desc{};
			desc.Usage = D3D11_USAGE_DYNAMIC;
			desc.ByteWidth = sizeof(Vertex) * 6000;
			desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	
			if (FAILED(_device->CreateBuffer(&desc, nullptr, vb.GetAddressOf()))) {
				throw std::runtime_error("创建顶点缓冲区 (Vertex Buffer) 失败");
			}
	
			D3D11_INPUT_ELEMENT_DESC layout[] = {
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};
	
			ComPtr<ID3DBlob> vs_blob, ps_blob, error_blob;

			HRESULT hr = D3DCompileFromFile(L"resources/shader/vs_sprite.hlsl", nullptr, nullptr, "main", "vs_5_0",
				D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_OPTIMIZATION_LEVEL3, 0, &vs_blob, &error_blob);
			if (FAILED(hr)) {
				std::string err = error_blob ? (char*)error_blob->GetBufferPointer() : "未知错误";
				throw std::runtime_error("VS Sprite 编译失败: " + err);
			}

			hr = D3DCompileFromFile(L"resources/shader/ps_sprite.hlsl", nullptr, nullptr, "main", "ps_5_0",
				D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_OPTIMIZATION_LEVEL3, 0, &ps_blob, &error_blob);
			if (FAILED(hr)) {
				std::string err = error_blob ? (char*)error_blob->GetBufferPointer() : "未知错误";
				throw std::runtime_error("PS Sprite 编译失败: " + err);
			}
	
			if (FAILED(_device->CreateVertexShader(vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), nullptr, vertex_shader.GetAddressOf())))
				throw std::runtime_error("创建 VS Shader 对象失败");
	
			if (FAILED(_device->CreatePixelShader(ps_blob->GetBufferPointer(), ps_blob->GetBufferSize(), nullptr, pixel_shader.GetAddressOf())))
				throw std::runtime_error("创建 PS Shader 对象失败");
	
			hr = _device->CreateInputLayout(layout, 3, vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), input_layout.GetAddressOf());
			if (FAILED(hr)) throw std::runtime_error("创建 InputLayout 失败");

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
			if (FAILED(_device->CreateSamplerState(&sd, sampler_state.GetAddressOf())))
				throw std::runtime_error("创建采样器状态失败");

			D3D11_BUFFER_DESC cbd{};
			cbd.Usage = D3D11_USAGE_DYNAMIC;
			cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			cbd.MiscFlags = 0;
	
			cbd.ByteWidth = sizeof(ConstantBuffer);
			if (FAILED(_device->CreateBuffer(&cbd, nullptr, cb.GetAddressOf()))) 
				throw std::runtime_error("创建 CB 失败");
	
			cbd.ByteWidth = sizeof(CBPerFrame);
			if (FAILED(_device->CreateBuffer(&cbd, nullptr, cb_per_frame.GetAddressOf()))) 
				throw std::runtime_error("创建 CBPerFrame 失败");
	
			cbd.ByteWidth = sizeof(CBPerObject);
			if (FAILED(_device->CreateBuffer(&cbd, nullptr, cb_per_object.GetAddressOf()))) 
				throw std::runtime_error("创建 CBPerObject 失败");
	
			cbd.ByteWidth = sizeof(CBLight);
			if (FAILED(_device->CreateBuffer(&cbd, nullptr, cb_light.GetAddressOf()))) 
				throw std::runtime_error("创建 CBLight 失败");
	
			cbd.ByteWidth = sizeof(CBFog);
			if (FAILED(_device->CreateBuffer(&cbd, nullptr, cb_fog.GetAddressOf()))) 
				throw std::runtime_error("创建 CBFog 失败");
	
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
			if (FAILED(_device->CreateBlendState(&bd, blend_state.GetAddressOf()))) 
				throw std::runtime_error("创建混合状态失败");
	
			D3D11_BLEND_DESC bd_additive{};
			bd_additive.RenderTarget[0].BlendEnable = TRUE;
			bd_additive.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
			bd_additive.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
			bd_additive.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			bd_additive.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			bd_additive.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			bd_additive.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			bd_additive.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			if (FAILED(_device->CreateBlendState(&bd_additive, blend_state_additive.GetAddressOf()))) {
				throw std::runtime_error("创建加法混合状态失败");
			}

			ComPtr<ID3D11Texture2D> backbuffer_tex;
			hr = swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)backbuffer_tex.GetAddressOf());
			if (FAILED(hr)) throw std::runtime_error("GetBuffer 失败");
	
			hr = device->CreateRenderTargetView(backbuffer_tex.Get(), nullptr, rtv.GetAddressOf());
			if (FAILED(hr) || !rtv) throw std::runtime_error("CreateRenderTargetView 失败");
	
			spdlog::info("初始 RTV 创建成功！");

			D3D11_RASTERIZER_DESC rs_desc = {};
			rs_desc.FillMode = D3D11_FILL_SOLID;
			rs_desc.CullMode = D3D11_CULL_NONE;
			rs_desc.DepthClipEnable = TRUE;
			if (FAILED(device->CreateRasterizerState(&rs_desc, rasterizer_state_2d.GetAddressOf())))
				throw std::runtime_error("创建 2D 光栅化状态失败");

			hr = D3DCompileFromFile(L"resources/shader/vs_3dbase.hlsl", nullptr, nullptr, "main", "vs_5_0",
				D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_OPTIMIZATION_LEVEL3, 0, &vs_blob, &error_blob);
			if (FAILED(hr)) {
				std::string err = error_blob ? (char*)error_blob->GetBufferPointer() : "未知错误";
				throw std::runtime_error("VS 3D Base 编译失败: " + err);
			}
	
			hr = D3DCompileFromFile(L"resources/shader/ps_3dbase.hlsl", nullptr, nullptr, "main", "ps_5_0",
				D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_OPTIMIZATION_LEVEL3, 0, &ps_blob, &error_blob);
			if (FAILED(hr)) {
				std::string err = error_blob ? (char*)error_blob->GetBufferPointer() : "未知错误";
				throw std::runtime_error("PS 3D Base 编译失败: " + err);
			}
	
			if (FAILED(_device->CreateVertexShader(
				vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), nullptr, vertex_shader_3d.GetAddressOf())))
				throw std::runtime_error("创建 VS 3D Shader 失败");
			if (FAILED(_device->CreatePixelShader(
				ps_blob->GetBufferPointer(), ps_blob->GetBufferSize(), nullptr, pixel_shader_3d.GetAddressOf())))
				throw std::runtime_error("创建 PS 3D Shader 失败");
	
			D3D11_INPUT_ELEMENT_DESC layout3d[] = {
				{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,	 0,	 0,	D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"NORMAL",0, DXGI_FORMAT_R32G32B32_FLOAT,		 0,	12,	D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,        0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 }
			};
	
			if (FAILED(_device->CreateInputLayout(layout3d, 4, 
				vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), input_layout_3d.GetAddressOf())))
				throw std::runtime_error("创建 3D InputLayout 失败");

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
			if (FAILED(_device->CreateTexture2D(&depthDesc, 
				nullptr, depth_stencil_buffer.GetAddressOf())))
				throw std::runtime_error("创建深度纹理失败");
	
			D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
			dsvDesc.Format = depthDesc.Format;
			dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			if (FAILED(_device->CreateDepthStencilView(depth_stencil_buffer.Get(), &dsvDesc, dsv.GetAddressOf()))) {
				throw std::runtime_error("DSV 创建失败");
			}
			spdlog::info("DSV 创建成功！");

			D3D11_DEPTH_STENCIL_DESC dssDesc = {};
			dssDesc.DepthEnable = true;
			dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			dssDesc.DepthFunc = D3D11_COMPARISON_LESS;
			if (FAILED(_device->CreateDepthStencilState(&dssDesc, depthState_3d.GetAddressOf())))
				throw std::runtime_error("创建 3D DepthState 失败");
	
			dssDesc.DepthEnable = false;
			dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			dssDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
			if (FAILED(_device->CreateDepthStencilState(&dssDesc, depthState_2d.GetAddressOf())))
				throw std::runtime_error("创建 2D DepthState 失败");
			D3D11_RASTERIZER_DESC rs_desc_3d = {};
			rs_desc_3d.FillMode = D3D11_FILL_SOLID;
			rs_desc_3d.CullMode = D3D11_CULL_BACK;
			rs_desc_3d.FrontCounterClockwise = FALSE;
			rs_desc_3d.DepthClipEnable = TRUE;
			if (FAILED(_device->CreateRasterizerState(&rs_desc_3d, rasterizer_state_3d.GetAddressOf()))) {
				throw std::runtime_error("创建 3D RasterizerState 失败");
			}
	
			spdlog::info("Renderer 初始化完全成功");
		}
		catch (const std::exception& e) {
			spdlog::critical("Renderer 构造函数发生严重错误: {}", e.what());
			throw;
		}
	}
	
	
	void Renderer::UpdateProjection() {
		ComPtr<ID3D11Texture2D> backbuffer;
		HRESULT hr = swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backbuffer);
		if (FAILED(hr)) return;
		backbuffer->GetDesc(&tex2d_desc);
	
		float physicalW = static_cast<float>(tex2d_desc.Width);
		float physicalH = static_cast<float>(tex2d_desc.Height);
	
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
		f[0] = (float)r / 255.f, f[1] = (float)g / 255.f, f[2] = (float)b / 255.f, f[3] = (float)a / 255.f;
		if (!SDL_SetRenderDrawColor(renderer, r, g, b, a)) {
			spdlog::error("设置渲染绘制颜色失败：{}", SDL_GetError());
		}
	}

	void Renderer::SetDrawColorFloat(float r, float g, float b, float a)
	{
		f[0] = r, f[1] = g, f[2] = b, f[3] = a;
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
		if (color.has_value()) {
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

	bool Renderer::Draw(const Resource::TextureResource* res, const SDL_FRect* srcrect,
		const SDL_FRect* dstrect, float angle, const SDL_FPoint* center,
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

		float rad = DirectX::XMConvertToRadians(angle);
		float c = cosf(rad), s = sinf(rad);
		XMFLOAT4 color;
		if (_color) {
			color = { static_cast<float>(_color->r) / 255.0f,  static_cast<float>(_color->g) / 255.0f,
				static_cast<float>(_color->b) / 255.0f ,  static_cast<float>(_color->a) / 255.0f };
		}
		else {
			color = { 1.0f, 1.0f, 1.0f, 1.0f };
		}
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
		device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		device_context->Draw(4, 0);
		return true;
	}

	void Renderer::DrawVertices(ID3D11ShaderResourceView* texture, 
		const std::vector<D3D::Vertex>& vertices)
	{
		if (vertices.empty()) return;

		D3D11_MAPPED_SUBRESOURCE mapped;
		HRESULT hr = device_context->Map(vb.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
		if (SUCCEEDED(hr)) {
			memcpy(mapped.pData, vertices.data(), sizeof(D3D::Vertex) * vertices.size());
			device_context->Unmap(vb.Get(), 0);
		}
		UINT stride = sizeof(D3D::Vertex);
		UINT offset = 0;
		device_context->IASetVertexBuffers(0, 1, vb.GetAddressOf(), &stride, &offset);
		device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		device_context->IASetInputLayout(input_layout.Get());

		device_context->PSSetShaderResources(0, 1, &texture);

		device_context->Draw((UINT)vertices.size(), 0);
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

	void Renderer::UpdateCBPerFrame()
	{
		if (!cm || !cm->GetActiveCamera()) {
			spdlog::error("未找到CameraManager或有效摄像机");
			return;
		}

		Camera* active_camera = cm->GetActiveCamera();
		const DirectX::XMFLOAT4& ambient_color = cm->GetAmbientColor();

		HRESULT hr;
		D3D::CBPerFrame data;
		data.m_view = DirectX::XMMatrixTranspose(active_camera->GetViewMatrix());
		data.m_projection = DirectX::XMMatrixTranspose(active_camera->GetProjectionMatrix());
		data.camera_position = active_camera->GetPositionFloat4();
		data.ambient_color = ambient_color;

		D3D11_MAPPED_SUBRESOURCE mapped_resource;
		hr = device_context->Map(cb_per_frame.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);
		if (FAILED(hr)) {
			throw std::runtime_error("无法映射帧常量缓冲区");
			return;
		}
		memcpy(mapped_resource.pData, &data, sizeof(D3D::CBPerFrame));
		device_context->Unmap(cb_per_frame.Get(), 0);
		device_context->VSSetConstantBuffers(1, 1, cb_per_frame.GetAddressOf());
		device_context->PSSetConstantBuffers(1, 1, cb_per_frame.GetAddressOf());
	}

	void Renderer::UpdateCBPerObject(const DirectX::XMMATRIX& world_matrix, const DirectX::XMFLOAT4& mesh_color,
		const DirectX::XMFLOAT4& mix_data)
	{
		HRESULT hr;
		D3D::CBPerObject data;
		data.m_world = DirectX::XMMatrixTranspose(world_matrix);
		data.mesh_color = mesh_color;
		data.mix = mix_data;
		D3D11_MAPPED_SUBRESOURCE mapped_resource;
		hr = device_context->Map(cb_per_object.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);
		if (FAILED(hr)) {
			throw std::runtime_error("无法映射物体常量缓冲区");
			return;
		}
		memcpy(mapped_resource.pData, &data, sizeof(D3D::CBPerObject));
		device_context->Unmap(cb_per_object.Get(), 0);
		device_context->VSSetConstantBuffers(2, 1, cb_per_object.GetAddressOf());
		device_context->PSSetConstantBuffers(2, 1, cb_per_object.GetAddressOf());
	}

	void Renderer::UpdateCBLight(const D3D::CBLight& light_data)
	{
		HRESULT hr;
		D3D11_MAPPED_SUBRESOURCE mapped_resource;

		hr = device_context->Map(cb_light.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);
		if (FAILED(hr)) {
			throw std::runtime_error("无法映射光照常量缓冲区");
			return;
		}
		memcpy(mapped_resource.pData, &light_data, sizeof(CBLight));
		device_context->Unmap(cb_light.Get(), 0);
		
		device_context->PSSetConstantBuffers(3, 1, cb_light.GetAddressOf());
	}

	void Renderer::UpdateCBFog(const D3D::CBFog& fog_data)
	{
		HRESULT hr;
		D3D11_MAPPED_SUBRESOURCE mapped_resource;
		hr = device_context->Map(cb_fog.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);
		if (FAILED(hr)) {
			throw std::runtime_error("无法映射雾效常量缓冲区");
			return;
		}

		memcpy(mapped_resource.pData, &fog_data, sizeof(D3D::CBFog));
		device_context->Unmap(cb_fog.Get(), 0);

		device_context->PSSetConstantBuffers(4, 1, cb_fog.GetAddressOf());
	}

	void Renderer::UpdateCBFog()
	{
		if (!cm || !cm->GetActiveCamera()) return;

		const D3D::Fog& cam_fog = cm->GetActiveCamera()->GetFog();

		D3D::CBFog data;
		data.fog_start = cam_fog.fog_start;
		data.fog_end = cam_fog.fog_end;
		data.fog_color = cam_fog.fog_color;
		data.fog_enable = cam_fog.fog_enable;

		HRESULT hr;
		D3D11_MAPPED_SUBRESOURCE mapped_resource;
		hr = device_context->Map(cb_fog.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);
		if (FAILED(hr)) {
			throw std::runtime_error("无法映射雾效常量缓冲区");
			return;
		}

		memcpy(mapped_resource.pData, &data, sizeof(D3D::CBFog));
		device_context->Unmap(cb_fog.Get(), 0);

		device_context->PSSetConstantBuffers(4, 1, cb_fog.GetAddressOf());
	}

	void Renderer::UpdateAspect(float tlx, float tly, float w, float h)
	{
		m_projection_2d = XMMatrixOrthographicOffCenterLH(
			tlx, w,
			h, tly,
			0.0f, 1.0f
		);
	}

	void Renderer::Begin2D() {
		using namespace DirectX;
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

		device_context->IASetInputLayout(input_layout_3d.Get());
		device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		device_context->RSSetState(rasterizer_state_3d.Get());
		device_context->OMSetDepthStencilState(depthState_3d.Get(), 1);
		device_context->OMSetRenderTargets(1, rtv.GetAddressOf(), dsv.Get());
		device_context->PSSetSamplers(0, 1, sampler_state.GetAddressOf());
		UpdateCBPerFrame();
	}


	void Renderer::SetCameraParameter(const std::string& key, float x, float y, float z) {
		if (!cm) return;
		const XMFLOAT3 param = { x, y, z };

		if (key == "eye") {
			cm->UpdateActiveCameraPosition(param);
		}
		else if (key == "at") {
			cm->UpdateActiveCameraTarget(param);
		}
		else if (key == "up") {
			cm->UpdateActiveCameraUp(param);
		}
		UpdateCBPerFrame();
	}

	void Renderer::SetCameraParameterFov(float fov) {
		if (!cm) return;
		cm->UpdateActiveCameraFov(fov);
		UpdateCBPerFrame();
	}

	void Renderer::SetCameraParameterZ(float near_z, float far_z) {
		if (!cm) return;
		cm->UpdateActiveCameraZ(near_z, far_z);
		UpdateCBPerFrame();
	}
	void Renderer::SetCameraParameterFog(float start, float end, const XMFLOAT3& color, bool enabled) {
		if (!cm) return;
		cm->UpdateActiveCameraFog(start, end, color, enabled);
		UpdateCBFog();
	}

	void Renderer::SetQuadRenderState(const std::string& texture_id,
		const std::string& mode,
		const DirectX::XMFLOAT4& color)
	{
		current_quad_color = color;

		float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		if (mode == "mul+add")
		{
			device_context->OMSetBlendState(blend_state_additive.Get(), blendFactor, 0xffffffff);
			device_context->PSSetShader(pixel_shader_3d.Get(), nullptr, 0);
			mode_toggle = 1.0f;

		}
		else
		{
			device_context->OMSetBlendState(blend_state.Get(), blendFactor, 0xffffffff);
			device_context->PSSetShader(pixel_shader_3d.Get(), nullptr, 0);
			mode_toggle = 0.0f;
		}
	}

	void Renderer::Draw3DQuad(const std::string& base_texture_id,
		const std::string& transition_texture_id,
		float transition_factor,
		const DirectX::XMFLOAT3& p1, const DirectX::XMFLOAT2& uv1, // TL
		const DirectX::XMFLOAT3& p2, const DirectX::XMFLOAT2& uv2, // TR
		const DirectX::XMFLOAT3& p3, const DirectX::XMFLOAT2& uv3, // BR
		const DirectX::XMFLOAT3& p4, const DirectX::XMFLOAT2& uv4) // BL
	{
		if (!device_context) {
			return;
		}

		Resource::TextureResource* tex_base = res->GetTexture(base_texture_id);

		Resource::TextureResource* tex_transition = nullptr;
		if (!transition_texture_id.empty()) {
			tex_transition = res->GetTexture(transition_texture_id);
		}

		ID3D11ShaderResourceView* srvs[2] = { nullptr, nullptr };

		if (tex_base && tex_base->d3d_srv) {
			srvs[0] = tex_base->d3d_srv.Get();
		}
		else {
			return;
		}

		if (tex_transition && tex_transition->d3d_srv && transition_factor > 0.001f) {
			srvs[1] = tex_transition->d3d_srv.Get();
		}
		else {
			srvs[1] = srvs[0];
		}

		device_context->PSSetShaderResources(0, 2, srvs);

		DirectX::XMVECTOR v_tl = DirectX::XMLoadFloat3(&p1);
		DirectX::XMVECTOR v_tr = DirectX::XMLoadFloat3(&p2);
		DirectX::XMVECTOR v_bl = DirectX::XMLoadFloat3(&p4);
		DirectX::XMVECTOR edge1 = DirectX::XMVectorSubtract(v_tr, v_tl);
		DirectX::XMVECTOR edge2 = DirectX::XMVectorSubtract(v_bl, v_tl);
		DirectX::XMVECTOR normalVec = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(edge1, edge2));
		DirectX::XMFLOAT3 quad_normal;
		DirectX::XMStoreFloat3(&quad_normal, normalVec);

		DirectX::XMFLOAT4 use_color = current_quad_color;

		Vertex1 corners[4] = {
			{p1, quad_normal, uv1, use_color},
			{p2, quad_normal, uv2, use_color},
			{p3, quad_normal, uv3, use_color},
			{p4, quad_normal, uv4, use_color}
		};

		Vertex1 list_data[6] = {
			corners[0], corners[1], corners[3],
			corners[1], corners[2], corners[3]
		};

		D3D11_MAPPED_SUBRESOURCE mapped{};
		if (FAILED(device_context->Map(vb.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) return;
		memcpy(mapped.pData, list_data, sizeof(list_data));
		device_context->Unmap(vb.Get(), 0);

		UINT stride = sizeof(Vertex1);
		UINT offset = 0;
		device_context->IASetVertexBuffers(0, 1, vb.GetAddressOf(), &stride, &offset);

		DirectX::XMMATRIX world_matrix = DirectX::XMMatrixIdentity();
		DirectX::XMFLOAT4 mix_data = { transition_factor, mode_toggle, 0.0f, 0.0f };
		UpdateCBPerObject(world_matrix, current_quad_color, mix_data);

		device_context->Draw(6, 0);
	}
}
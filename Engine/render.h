#pragma once
#include "../prefix.h"
namespace Engine::Resource {
	struct TextureResource;
	class ResourceManager;
}
namespace Engine::Maths {
	struct SpriteRect;
	struct Vec2;
}
using namespace Engine::Resource;
using namespace Engine::Maths;
using Microsoft::WRL::ComPtr;
namespace Engine::Render {
	class Sprite;
	class Renderer final{
	private:
		SDL_Renderer* renderer = nullptr;
		ID3D11Device* device = nullptr;
		ID3D11DeviceContext* device_context = nullptr;
		ResourceManager* res = nullptr;

	private:
		float screen_width;
		float screen_height;

	public:
		DirectX::XMMATRIX m_projection;
		DirectX::XMMATRIX m_projection_2d;
	public:
		float f[4];
		ComPtr<IDXGISwapChain1> swap_chain;
		ComPtr<ID3D11Buffer> cb;
		//这个vb仅用于2d
		ComPtr<ID3D11Buffer> vb;
		ComPtr<ID3D11InputLayout> input_layout;
		ComPtr<ID3D11VertexShader> vertex_shader;
		ComPtr<ID3D11PixelShader> pixel_shader;
		ComPtr<ID3D11SamplerState> sampler_state;
		ComPtr<ID3D11BlendState> blend_state;
		ComPtr<ID3D11RenderTargetView> rtv;
		ComPtr<ID3D11RasterizerState> rasterizer_state_2d;
		ComPtr<ID3D11DepthStencilState> depthState_2d;
		ComPtr<ID3D11Texture2D> depth_stencil_buffer;
		ComPtr<ID3D11DepthStencilView> dsv;
		ComPtr<ID3D11DepthStencilState> depthState_3d;
		ComPtr<ID3D11RasterizerState> rasterizer_state_3d;
		ComPtr<ID3D11VertexShader> vertex_shader_3d;
		ComPtr<ID3D11PixelShader> pixel_shader_3d;

#ifdef _DEBUG
		ComPtr<ID3D11Debug> d3d_debug;
#endif
	public:
		Renderer(SDL_Renderer* s_r, ResourceManager* r_s, 
			ID3D11Device* device, ID3D11DeviceContext* device_ctx,
			float _width, float _height);
		~Renderer() = default;
		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer(Renderer&&) = delete;
		Renderer& operator=(Renderer&&) = delete;

	public:
		SDL_Renderer* getSDLRenderer() const { return renderer; }

	public:
		std::optional<SDL_FRect> GetSpriteSrcRect(const Sprite& sprite);
		void SetDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
		void SetDrawColorFloat(float r, float g, float b, float a);
		void ClearScreen();
		void Present();
		void DrawSprite(const Sprite& sprite, const Vec2& pos, 
			const Vec2& scale, const std::optional<SDL_FRect>& s_rc, 
			const std::optional<SDL_Color>& color, double angle = 0.0f);
		void Begin2D();
		void Begin3D();
		void SetViewPort(float tlx, float tly, float w, float h);
	private:
		bool Draw(Resource::TextureResource* res, 
			const SDL_FRect* srcrect,
			const SDL_FRect* dstrect,
			double angle,
			const SDL_FPoint* center,
			const SDL_Color* _color,
			int flip = 0);
		void UpdateProjection();
		
	};

}
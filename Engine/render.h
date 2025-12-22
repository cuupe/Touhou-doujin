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
	class CameraManager;
	class Camera;
	class Sprite;
	namespace D3D {
		struct Vertex;
		struct CBLight;
		struct CBFog;
	}

	class Renderer final{
	private:
		SDL_Renderer* renderer = nullptr;
		ComPtr<ID3D11Device> device = nullptr;
		ComPtr<ID3D11DeviceContext> device_context = nullptr;
		ResourceManager* res = nullptr;
		CameraManager* cm = nullptr;
		

	private:
		float screen_width;
		float screen_height;

	private:
		DirectX::XMMATRIX m_projection;
		DirectX::XMMATRIX m_projection_2d;
		D3D11_TEXTURE2D_DESC tex2d_desc;
		DirectX::XMFLOAT4 current_quad_color = { 1.0f, 1.0f, 1.0f, 1.0f };
		float transition_factor = 0.0f;
		float mode_toggle = 0.0f;


	private:
		float f[4];
		ComPtr<IDXGISwapChain1> swap_chain;
		ComPtr<ID3D11Buffer> cb;
		ComPtr<ID3D11Buffer> cb_per_frame;
		ComPtr<ID3D11Buffer> cb_per_object;
		ComPtr<ID3D11Buffer> cb_light;
		ComPtr<ID3D11Buffer> cb_fog;
		ComPtr<ID3D11Buffer> vb;		//这个vb仅用于2d
		ComPtr<ID3D11InputLayout> input_layout;
		ComPtr<ID3D11InputLayout> input_layout_3d;
		ComPtr<ID3D11VertexShader> vertex_shader;
		ComPtr<ID3D11PixelShader> pixel_shader;
		ComPtr<ID3D11SamplerState> sampler_state;
		ComPtr<ID3D11BlendState> blend_state;
		ComPtr<ID3D11BlendState> blend_state_additive;
		ComPtr<ID3D11RenderTargetView> rtv;
		ComPtr<ID3D11RasterizerState> rasterizer_state_2d;
		ComPtr<ID3D11DepthStencilState> depthState_2d;
		ComPtr<ID3D11Texture2D> depth_stencil_buffer;
		ComPtr<ID3D11DepthStencilView> dsv;
		ComPtr<ID3D11DepthStencilState> depthState_3d;
		ComPtr<ID3D11RasterizerState> rasterizer_state_3d;
		ComPtr<ID3D11VertexShader> vertex_shader_3d;
		ComPtr<ID3D11PixelShader> pixel_shader_3d;

	public:
		Renderer(SDL_Renderer* s_r, ResourceManager* r_s, CameraManager* c_m,
			ID3D11Device* device, ID3D11DeviceContext* device_ctx,
			float _width, float _height);
		~Renderer() = default;
		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer(Renderer&&) = delete;
		Renderer& operator=(Renderer&&) = delete;

	public:
		SDL_Renderer* GetSDLRenderer() const { return renderer; }

	public:
		std::optional<SDL_FRect> GetSpriteSrcRect(const Sprite& sprite);
		const D3D11_TEXTURE2D_DESC& GetTexDesc() const { return tex2d_desc; }
		void SetDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
		void SetDrawColorFloat(float r, float g, float b, float a);
		void ClearScreen();
		void Present() { swap_chain->Present(0, 0); }
		void DrawSprite(const Sprite& sprite, const Vec2& pos,
			const Vec2& scale, const std::optional<SDL_FRect>& s_rc,
			const std::optional<SDL_Color>& color, double angle = 0.0f);
		void Begin2D();
		void Begin3D();
		void SetViewPort(float tlx, float tly, float w, float h);
		void UpdateCBPerFrame();
		void UpdateCBPerObject(const DirectX::XMMATRIX& world_matrix, const DirectX::XMFLOAT4& mesh_color, 
			const DirectX::XMFLOAT4& mix_data);
		void UpdateCBLight(const D3D::CBLight& light_data);
		void UpdateCBFog(const D3D::CBFog& fog_data);
		void UpdateCBFog();
		void UpdateAspect(float tlx, float tly, float w, float h);
		bool Draw(const Resource::TextureResource* res,
			const SDL_FRect* srcrect,
			const SDL_FRect* dstrect,
			float angle,
			const SDL_FPoint* center,
			const SDL_Color* _color,
			int flip = 0);
		void DrawVertices(ID3D11ShaderResourceView* texture,
			const std::vector<D3D::Vertex>& vertices);

	public:
		void SetCameraParameter(const std::string& key, float x, float y, float z);
		void SetCameraParameterFov(float fov);
		void SetCameraParameterZ(float near_z, float far_z);
		void SetCameraParameterFog(float start, float end, const DirectX::XMFLOAT3& color, bool enabled);
		void SetQuadRenderState(const std::string& texture_id,
			const std::string& mode,
			const DirectX::XMFLOAT4& color);
		void Draw3DQuad(const std::string& base_texture_id,
			const std::string& transition_texture_id,
			float transition_factor, 
			const DirectX::XMFLOAT3& p1, const DirectX::XMFLOAT2& uv1,
			const DirectX::XMFLOAT3& p2, const DirectX::XMFLOAT2& uv2,
			const DirectX::XMFLOAT3& p3, const DirectX::XMFLOAT2& uv3,
			const DirectX::XMFLOAT3& p4, const DirectX::XMFLOAT2& uv4);
		void SetCameraManager(CameraManager* new_cm) { cm = new_cm; }
		CameraManager* GetCameraManager() const { return cm; }

	private:
		void UpdateProjection();
		
	};

}
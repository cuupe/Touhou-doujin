#include "Game.h"
#include "Scene/Stage.h"
#include "Scene/TitleScene.h"
#include "../Engine/schedule.h"
#include <thread>
#include "../Engine/D3D/model.h"

namespace Game {
    using namespace Engine;
    MGame::MGame(const char* win_name, int width, int height, int flag, int fps)
        :Engine::engine(win_name, width, height, flag, fps)
    {
        try {
            res = std::make_unique<Engine::Resource::ResourceManager>(device.Get());
            sch = std::make_unique<Engine::Time::Schedule>();
            r = std::make_unique<Engine::Render::Renderer>(renderer, res.get(), 
                device.Get(), device_context.Get(), width, height);
            input = std::make_unique<Engine::Input::InputManager>(r.get());
            ctx = std::make_unique<Engine::Core::Context>(*r, *res, *input, *sch);
            sc = std::make_unique<Engine::Scene::SceneManager>(*ctx);
            audio = std::make_unique<Engine::Audio::AudioManager>(*ctx);
            ui = std::make_unique<Engine::UISystem::UIManager>(*ctx, *audio, *sc);
        }
        catch (const std::exception& e) {
            spdlog::error("初始化失败", e.what());
            return;
        }

        //test load(后续将使用多线程)
        {
            res->LoadTexture(renderer, device.Get(), "resources/textures/player/pl00/pl00.png");
            res->LoadTexture(renderer, device.Get(), "resources/textures/UI/rank00.png");
            res->LoadTexture(renderer, device.Get(), "resources/textures/enemy/enemy5.png");
            res->LoadTexture(renderer, device.Get(), "resources/textures/UI/title_ver.png");
            res->LoadTexture(renderer, device.Get(), "resources/textures/UI/title_item00.png");
            res->LoadTexture(renderer, device.Get(), "resources/textures/UI/title_pattern1.png");
            res->LoadTexture(renderer, device.Get(), "resources/textures/UI/title_bk00.png");
            res->LoadTexture(renderer, device.Get(), "resources/textures/UI/title.png");
            res->LoadAudio(audio->GetMixer(), "resources/audios/bgm/menu.wav");
            res->LoadAudio(audio->GetMixer(), "resources/audios/sfx/se_tan01.wav");
            res->LoadAudio(audio->GetMixer(), "resources/audios/sfx/se_pldead00.wav");
            res->LoadAudio(audio->GetMixer(), "resources/audios/sfx/se_select00.wav");
            res->LoadAudio(audio->GetMixer(), "resources/audios/sfx/se_ok00.wav");
            res->LoadAudio(audio->GetMixer(), "resources/audios/sfx/se_cancel00.wav");
            res->LoadModel(renderer, device.Get(), "C:/Users/Lenovo/Desktop/srd/liuy.fbx");


            for (int i = 1; i <= 5; i++) {
                std::string po = "resources/textures/bullet/bullet" + std::to_string(i) + ".png";
                res->LoadTexture(renderer, device.Get(), po);
            }
        }

        {
            auto ss = std::make_unique<Scene::TitleScene>("fuck", *ctx, *sc, *audio, *ui);
            audio->PlayBGM("menu");
            sc->RequestPushScene(std::move(ss));
        }
        {
        }

        initialized = true;
    }

    MGame::~MGame()
    { }

    inline void MGame::HandleInput()
    {
        sc->HandleInput();
    }

    inline void MGame::Render()
    {
        using namespace DirectX;
        r->SetDrawColor(0, 0, 255, 255);
        r->ClearScreen();

        
        r->Begin3D();
        static float angle = 0.0f;
        angle += 0.1f;

        XMMATRIX world = XMMatrixRotationX(_1_2_PI) * XMMatrixRotationY(angle);
        XMMATRIX view = XMMatrixLookAtLH(XMVectorSet(0, 1 , -5, 0), XMVectorSet(0, 0, 0, 0), XMVectorSet(0, 1, 0, 0));

        D3D11_MAPPED_SUBRESOURCE mapped;
        if (SUCCEEDED(device_context->Map(r->cb.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) {
            Engine::Render::D3D::ConstantBuffer* buffer = (Engine::Render::D3D::ConstantBuffer*)mapped.pData;
            buffer->m_world = XMMatrixTranspose(world);
            buffer->m_view = XMMatrixTranspose(view);
            buffer->m_projection = XMMatrixTranspose(r->m_projection);
            device_context->Unmap(r->cb.Get(), 0);
        }
        device_context->VSSetConstantBuffers(0, 1, r->cb.GetAddressOf());

        auto model = res->GetModel("liuy");
        if (model) {
            for (auto& mesh : model->meshes) {
                UINT stride = sizeof(Engine::Render::D3D::Vertex);
                UINT offset = 0;
                ID3D11Buffer* vb = mesh.vertex_buffer.Get();
                device_context->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
                if (mesh.index_buffer) {
                    device_context->IASetIndexBuffer(mesh.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
                }

                device_context->PSSetShaderResources(0, 1, mesh.diffuseSRV.GetAddressOf());

                if (mesh.index_count) {
                    device_context->DrawIndexed(mesh.index_count, 0, 0);
                }
                else {
                    device_context->Draw(mesh.vertex_count, 0);
                }
            }
        }
        

        r->Begin2D();
        sc->Render();
        r->Present();
    }

    inline void MGame::Update()
    {
        input->Update();
        t.Update();
        sch->Update(t.DeltaTime());
        sc->Update(t.DeltaTime());

        if (input->ShouldQuit()) {
            running = false;
        }
    }


    void MGame::Run()
    {
        while (running) {
            HandleInput();
            Update();
            Render();
        }
    }
}




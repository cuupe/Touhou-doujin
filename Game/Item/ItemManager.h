#pragma once
#include "../../Engine/context.h"
#include "../../Engine/AudioManager.h"
#include "../GameData.h"
#include "Item.h"
#include <map>
#include <vector>
#include <utility>

namespace Game::Entity {
    class Player;
}

namespace Game::Manager {
    class ItemManager final {
    private:
        std::map<Items::ItemType, Items::ItemConfig> item_configs;
        std::string source;
        std::vector<Items::ItemData> items;
        Engine::Core::Context& ctx;
        Engine::Audio::AudioManager& audio;

        int index = 0;

    public:
        ItemManager(Engine::Core::Context& _ctx, Engine::Audio::AudioManager& _au);
        ~ItemManager() = default;

    public:
        void Update(float dt);
        void Render();
        void SpawnItem(Items::ItemType type, const Engine::Maths::Vec2& pos);
        void SpawnItems(const Engine::Maths::Vec2& pos, const std::vector<std::pair<Items::ItemType, int>>& items_to_spawn);
        void RemoveAllItems();
        void CheckCollision(Game::Entity::Player* player);

    private:
        Items::ItemData& GetAvailableItem();
        Items::ItemType StringToItemType(const std::string& str);
    };
}
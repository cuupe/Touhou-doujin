#pragma once
#include "../prefix.h"
namespace Engine::Core {
    class Config final {
    public:
        std::string window_title = "Test";
        int window_width = 1280;
        int window_height = 960;
        bool window_resizable = true;
        bool vsync_enabled = false;
        int target_fps = 60;
        float music_volume = 1.0f;
        float sound_volume = 1.0f;

        // 存储动作名称到 SDL Scancode 名称列表的映射
        std::unordered_map<std::string, std::vector<std::string>> input_mappings = {
            // 提供一些合理的默认值，以防配置文件加载失败或缺少此部分
            {"move_left", {"A", "Left"}},
            {"move_right", {"D", "Right"}},
            {"move_up", {"W", "Up"}},
            {"move_down", {"S", "Down"}},
            {"jump", {"J", "Space"}},
            {"attack", {"K", "MouseLeft"}},
            {"pause", {"P", "Escape"}},
        };

        explicit Config(const std::string& filepath);

        // 删除拷贝和移动语义
        Config(const Config&) = delete;
        Config& operator=(const Config&) = delete;
        Config(Config&&) = delete;
        Config& operator=(Config&&) = delete;

        bool LoadFromFile(const std::string& filepath);
        [[nodiscard]] bool SaveToFile(const std::string& filepath);
    private:
        void FromJson(const nlohmann::json& j);
        nlohmann::ordered_json ToJson() const;
    };
}
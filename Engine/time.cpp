#include "time.h"
#include "../prefix.h"

namespace Engine::Time {
    time::time(int limit)
        :limit_fps(limit), start(std::chrono::high_resolution_clock::now()),
        end(std::chrono::high_resolution_clock::now()), during(0), sum(0),
        count(0), fps(0)
    {
        limit_time = 1000 / limit_fps;
    }

    time::~time()
    { }

    void time::ChangeLimitFps(int newfps)
    {
        limit_fps = newfps;
        limit_time = 1000 / limit_fps;
    }

    void time::Update() {
        end = std::chrono::high_resolution_clock::now();
        during = end - start;
        auto target_duration = std::chrono::milliseconds(limit_time);
        if (during < target_duration) {
            std::this_thread::sleep_for(target_duration - during);
            end = std::chrono::high_resolution_clock::now();
            during = end - start;
        }

        sum += std::chrono::duration_cast<std::chrono::milliseconds>(during).count();
        start = end;
        count++;
        if (sum >= 1000) {
            fps = static_cast<double>(count) / sum * 1000.0;
            sum = 0;
            count = 0;
            spdlog::info("fps: {}", fps);
        }
    }

    long long time::GetDeltaTime() const
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(during).count();
    }

    float time::DeltaTime() const
    {
        return static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(during).count()) / 1000;
    }

    long long time::GetCount() const
    {
        return count;
    }

}


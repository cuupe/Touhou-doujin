#pragma once
#include "../prefix.h"
namespace Engine::Time {
    class Schedule {
    public:
        struct Task {
            float timer;
            std::function<void()> func;
            std::function<void(float t)> update_func;
            float duration = 0.0f;
            float elapsed = 0.0f;
            bool is_tween = false;

            bool done;
            Task(float t, std::function<void()> f) :
                timer(t), func(f), done(false), is_tween(false) {
            }
            Task(float dur, std::function<void(float t)> uf) :
                timer(0.0f), func(nullptr), done(false),
                update_func(uf), duration(dur), elapsed(0.0f), is_tween(true) {
            }
        };

        struct TaskSequence {
            std::vector<Task> tasks;
            size_t index = 0;
            TaskSequence& Delay(float t, std::function<void()> f) {
                tasks.push_back(Task(t, f));
                return *this;
            }

            TaskSequence& Call(std::function<void()> f) {
                tasks.push_back(Task(0.0f, f));
                return *this;
            }
            TaskSequence& Tween(float duration, std::function<void(float t)> update_func) {
                tasks.push_back(Task(duration, update_func));
                return *this;
            }

            bool Update(float dt) {
                if (index >= tasks.size()) {
                    return true;
                }
                Task& task = tasks[index];

                if (task.is_tween) {
                    if (task.done) {
                        index++;
                        return index >= tasks.size();
                    }

                    task.elapsed += dt;
                    float t = std::min(1.0f, task.elapsed / task.duration);

                    task.update_func(t);

                    if (task.elapsed >= task.duration) {
                        task.update_func(1.0f);
                        task.done = true;
                    }

                }
                else {
                    task.timer -= dt;
                    if (task.timer <= 0.0f && !task.done) {
                        task.func();
                        task.done = true;
                    }
                }

                if (task.done) index++;
                return index >= tasks.size();
            }
        };

        TaskSequence& NewSequence() {
            sequences.push_back(std::make_shared<TaskSequence>());
            return *sequences.back();
        }

        void Update(float dt) {
            for (int i = static_cast<int>(sequences.size()) - 1; i >= 0; --i) {
                if (sequences[i]->Update(dt)) {
                    sequences.erase(sequences.begin() + i);
                }
            }
        }

    private:
        std::vector<std::shared_ptr<TaskSequence>> sequences;
    };
}
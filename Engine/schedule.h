#pragma once
#include <functional>
#include <vector>
#include <algorithm>
namespace Engine::Time {
    class Schedule {
    public:
        struct Task {
            float timer;
            std::function<void()> func;
            bool done;
            Task(float t, std::function<void()> f) :
                timer(t), func(f), done(false) {
            }
        };

        // 每条链
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

            bool Update(float dt) {
                if (index >= tasks.size()) {
                    return true;
                }
                Task& task = tasks[index];
                task.timer -= dt;
                if (task.timer <= 0.0f && !task.done) {
                    task.func();
                    task.done = true;
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
            for (int i = (int)sequences.size() - 1; i >= 0; --i) {
                if (sequences[i]->Update(dt)) {
                    sequences.erase(sequences.begin() + i);
                }
            }
        }

    private:
        std::vector<std::shared_ptr<TaskSequence>> sequences;
    };
}
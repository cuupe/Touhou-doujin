#pragma once
#include <thread>
#include <chrono>

namespace Engine::Time {
	class time {
	private:
		std::chrono::high_resolution_clock::time_point start;
		std::chrono::high_resolution_clock::time_point end;
		std::chrono::high_resolution_clock::duration during;
		long long sum;
		long long count;
		long long limit_time;
		int limit_fps;
		double fps;

	public:
		time(int limit);
		time& operator=(const time&) = delete;
		time(const time&) = delete;
		time& operator=(time&&) = delete;
		time(time&&) = delete;
		~time();

	public:
		void ChangeLimitFps(int newfps);
		void Update();
		long long GetDeltaTime() const;
		float DeltaTime() const;
		long long GetCount() const;
	};
}
#include "maths.h"
#include "Components/TransformComponent.h"
#include "Components/ColliderComponent.h"
using namespace Engine::Core::Collider;
namespace Engine::Maths {
	Vec2 Clamp(const Vec2& t, const Vec2& mini, const Vec2& maxi) {
		Vec2 temp = t;

		if (temp.x < static_cast<float>(mini.x)) {
			temp.x = static_cast<float>(mini.x);
		}
		else if (temp.x > static_cast<float>(maxi.x)) {
			temp.x = maxi.x;
		}
		if (temp.y < static_cast<float>(mini.y)) {
			temp.y = mini.y;
		}
		else if (temp.y > static_cast<float>(maxi.y)) {
			temp.y = maxi.y;
		}

		return temp;
	}

	Vec2 Clamp(const Vec2& t, float mini, float maxi) {
		float dk = Vec2_SquareLength(t);
		if (dk == 0) {
			return t;
		}

		if (dk <= mini * mini)
			return t * (mini / sqrtf(dk));
		if (dk >= maxi * maxi)
			return t * (maxi / sqrtf(dk));
		return t;
	}

	float GetRandomFloat(float min, float max)
	{
		using namespace std;
		random_device seed;
		ranlux48 engine(seed());
		uniform_real_distribution<float> distrib(min, max);
		return distrib(engine);
	}

	int GetRandomInt(int min, int max) {
		using namespace std;
		random_device seed;
		ranlux48 engine(seed());
		uniform_int_distribution<int> dis(min, max);
		return dis(engine);
	}
	
	float NormalizeAngle(float aim, float target)
	{
		aim += target;
		unsigned char count = 0;
		while (aim > _PI) {
			aim -= _2PI;
			if (++count > 16) {
				break;
			}
		}

		while (aim < -_PI) {
			aim += _2PI;
			if (++count > 16) {
				break;
			}
		}
		return aim;
	}

	Vec2 SpeedAngleToVec2(float speed, float angle)
	{
		Vec2 vel;
		vel.x = speed * cos(angle);
		vel.y = speed * sin(angle);
		return vel;
	}
}

#include "maths.h"
#include "Components/TransformComponent.h"
#include "Components/ColliderComponent.h"
using namespace Engine::Core::Collider;
namespace Engine::Maths {
	Vec2 AngleToVec2(float angle)
	{
		return Vec2{ cos(angle), sin(angle) };
	}

	float Vec2ToAngle(const Vec2& a)
	{
		return acos(a.x / Vec2_Length(a));
	}

	inline float Vec2_Length(const Vec2& a) {
		return sqrtf(a.x * a.x + a.y * a.y);
	}

	inline float Vec2_SquareLength(const Vec2& a)
	{
		return a.x * a.x + a.y * a.y;
	}

	float RadToDeg(float r)
	{
		return fmod(360.0f * r / _2PI, 360.0f);
	}

	float DegToRad(float d)
	{
		return fmod(d / 360.0f * _2PI, _2PI);
	}

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


	template <typename T>
	T Clamp(T v, T mini, T maxi) {
		if (v < mini) {
			return mini;
		}
		else if (v > maxi) {
			return maxi;
		}
		else {
			return v;
		}
	}




	bool CheckCollision(const ColliderComponent& a, const ColliderComponent& b)
	{
		auto a_collider = a.GetCollider();
		auto b_collider = b.GetCollider();
		auto a_trans = a.GetTransform();
		auto b_trans = b.GetTransform();

		//粗略计算最小包围圈的碰撞
		auto a_size = a_collider->Get_AABB_Size() * a_trans->GetScale();
		auto b_size = b_collider->Get_AABB_Size() * b_trans->GetScale();
		auto a_pos = a_trans->GetPosition() + a.GetOffset();
		auto b_pos = b_trans->GetPosition() + b.GetOffset();
		

		if (!CheckAABBOverlap(a_pos, a_size, b_pos, b_size)) {
			return false;
		}

		if (a_collider->GetType() == ColliderType::AABB &&
			b_collider->GetType() == ColliderType::AABB) {
			return true;
		}
		else if (a_collider->GetType() == ColliderType::CIRCLE &&
			b_collider->GetType() == ColliderType::CIRCLE) {
			auto a_center = a_pos + a_size * 0.5f;
			auto b_center = b_pos + b_size * 0.5f;
			auto a_radius = 0.5f * a_size.x;
			auto b_radius = 0.5f * b_size.x;
			return CheckCircleOverlap(a_center, a_radius, b_center, b_radius);
		}
		else if (a_collider->GetType() == ColliderType::AABB &&
			b_collider->GetType() == ColliderType::CIRCLE) {
			auto b_center = b_pos + 0.5f * b_size;
			auto b_radius = 0.5f * b_size.x;
			auto nearest_point = Clamp(b_center, b_pos, b_pos + b_size);
			return CheckPointInCircle(nearest_point, b_center, b_radius);
		}
		else if (a_collider->GetType() == ColliderType::CIRCLE &&
			b_collider->GetType() == ColliderType::AABB) {
			auto a_center = a_pos + 0.5f * a_size;
			auto a_radius = 0.5f * a_size.x;
			auto nearest_point = Clamp(a_center, a_pos, a_pos + a_size);
			return CheckPointInCircle(nearest_point, a_center, a_radius);
		}
		return false;
	}

	//检查圆的重叠，直接检查两圆圆心的距离是否大于两圆半径的和
	bool CheckCircleOverlap(const Vec2& a_center, float a_radius, const Vec2& b_center, float b_radius)
	{
		return (Vec2_Length(a_center - b_center) < a_radius + b_radius);
	}

	//检查矩形的重叠直接检测四个顶点是否处于另一个矩形内（基于分离轴定理）
	bool CheckAABBOverlap(const Vec2& a_pos, const Vec2& a_size, const Vec2& b_pos, const Vec2& b_size)
	{
		if (a_pos.x + a_size.x <= b_pos.x || a_pos.x >= b_pos.x + b_size.x ||
			a_pos.y + a_size.y <= b_pos.y || a_pos.y >= b_pos.y + b_size.y) {
			return false;
		}
		return true;
	}
	
	bool CheckRectOverlap(const Rect& a, const Rect& b)
	{
		return CheckAABBOverlap(Vec2{ a.x, a.y }, Vec2{ a.w, a.h },
			Vec2{ b.x, b.y }, Vec2{ b.w, b.h });
	}

	bool CheckPointInCircle(const Vec2& point, const Vec2& center, float radius)
	{
		return (Vec2_Length(point - center) < radius);
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

#pragma once
#include <cmath>
#include <random>
constexpr float _PI = 3.141592653f;
constexpr float _2PI = 6.283185306f;
constexpr float _1_2_PI = 1.5707963265f;

namespace Engine::Core::Components {
	class ColliderComponent;
}
using namespace Engine::Core::Components;
namespace Engine::Maths {
	template <typename T>
	using allowed = std::integral_constant<bool,
		std::is_same<T, short>::value ||
		std::is_same<T, int>::value ||
		std::is_same<T, long>::value ||
		std::is_same<T, long long>::value ||
		std::is_same<T, float>::value ||
		std::is_same<T, double>::value ||
		std::is_same<T, long double>::value>;

	//泛用二维向量
	struct Vec2 {
		float x;
		float y;

		Vec2() :
			x(0), y(0) {
		}
		explicit Vec2(float x) :
			x(x), y(0) {
		}
		Vec2(float x, float y) :
			x(x), y(y) {
		}
		Vec2(const Vec2& o) :
			x(o.x), y(o.y) {
		}
		Vec2(Vec2&& o) noexcept :
			x(o.x), y(o.y) {
		}
		Vec2& operator=(const Vec2& o) 
	    {
			x = o.x;
			y = o.y;
			return *this;
		}
		Vec2& operator=(Vec2&& o) noexcept {
			x = o.x;
			y = o.y;
			return *this;
		}
		Vec2 operator+(const Vec2& o) const {
			return Vec2(this->x + o.x, this->y + o.y);
		}
		Vec2 operator-(const Vec2& o) const {
			return Vec2(this->x - o.x, this->y - o.y);
		}
		Vec2 operator*(const Vec2& o) const {
			return Vec2(this->x * o.x, this->y * o.y);
		}
		bool operator==(const Vec2& o) const {
			return (this->x == o.x) && (this->y == o.y);
		}
		bool operator!() const {
			return (this->x == 0) && (this->y == 0);
		}
		Vec2& operator+=(const Vec2& o) {
			x += o.x;
			y += o.y;
			return *this;
		}
		Vec2& operator-=(const Vec2 & o) {
			x -= o.x;
			y -= o.y;
			return *this;
		}
		Vec2& operator*=(const Vec2& o) {
			x *= o.x;
			y *= o.y;
			return *this;
		}
		Vec2 operator*(float s) const {
			return Vec2(this->x * s, this->y * s);
		}
		friend Vec2 operator*(float s, const Vec2& v) {
			return Vec2(v.x * s, v.y * s);
		}
		Vec2& operator*=(float s) {
			x *= s;
			y *= s;
			return *this;
		}
		float Length() const {
			return std::sqrtf(x * x + y * y);
		}
		void Normalized() {
			float length = Length();
			if (length > 0.0f) {
				x /= length;
				y /= length;
			}
		}
		void Reserve() {
			x *= -1;
			y *= -1;
		}
	};
	inline Vec2 AngleToVec2(float angle);
	inline float Vec2_Length(const Vec2& a);
	inline float Vec2_SquareLength(const Vec2& a);

	//Vec2特供
	Vec2 Clamp(const Vec2& t, const Vec2& mini, const Vec2& maxi);
	Vec2 Clamp(const Vec2& t, float mini, float maxi);

	//视口矩形
	struct Rect {
		float x;
		float y;
		float w;
		float h;

		Rect() :
			x(0), y(0), w(0), h(0) {
		}
		explicit Rect(float x) :
			x(x), y(0), w(0), h(0) {
		}
		Rect(float x, float y) :
			x(x), y(y), w(0), h(0) {
		}
		Rect(float x, float y, float w, float h) :
			x(x), y(y), w(w), h(h) {
		}
		Rect(const Vec2& p, const Vec2& s) :
			x(p.x), y(p.y), w(s.x), h(s.y) {
		}
		explicit Rect(const Vec2& s) :
			x(0), y(0), w(s.x), h(s.y) {
		}
		Rect(const Rect& o) :
			x(o.x), y(o.y), w(o.w), h(o.h) {
		}
		Rect(Rect&& o) noexcept:
			x(o.x), y(o.y), w(o.w), h(o.h) {
		}
		Rect& operator=(const Rect& o)
		{
			x = o.x;
			y = o.y;
			w = o.w;
			h = o.h;
			return *this;
		}
		Rect& operator=(Rect&& o) noexcept {
			x = o.x;
			y = o.y;
			w = o.w;
			h = o.h;
			return *this;
		}
		Rect operator+(const Vec2& v) const {
			return Rect(this->x + v.x, this->y + v.y, w, h);
		}
		Rect operator-(const Vec2& v) const {
			return Rect(this->x - v.x, this->y - v.y, w, h);
		}
		Rect operator*(const Vec2& v) const {
			return Rect(this->x * v.x, this->y * v.y, w * v.x, h * v.y);
		}
		bool operator==(const Rect& o) const {
			return (this->x == o.x) && (this->y == o.y) && (this->w == o.w) && (this->h == o.h);
		}
		bool operator!() const {
			return (this->w == 0) && (this->h == 0);
		}
		Rect& operator+=(const Vec2& v) {
			x += v.x;
			y += v.y;
			return *this;
		}
		Rect& operator-=(const Vec2& v) {
			x -= v.x;
			y -= v.y;
			return *this;
		}
		Rect& operator*=(const Vec2& v) {
			x *= v.x;
			y *= v.y;
			w *= v.x;
			h *= v.y;
			return *this;
		}
		Rect operator*(float s) const {
			return Rect(this->x * s, this->y * s, this->w * s, this->h * s);
		}
		Rect& operator*=(float s) {
			x *= s;
			y *= s;
			w *= s;
			h *= s;
			return *this;
		}
		float Square() const {
			return std::sqrtf(this->w * this->w + this->h * this->h);
		}
		void Reserve() {
			this->x *= -1;
			this->y *= -1;
		}
	};
	//对齐方式
	enum class Align {
		NONE,           
		TOP_LEFT,       
		TOP_CENTER,     
		TOP_RIGHT,      
		CENTER_LEFT,    
		CENTER,         
		CENTER_RIGHT,   
		BOTTOM_LEFT,    
		BOTTOM_CENTER,  
		BOTTOM_RIGHT    
	};


	//基本类型
	template <typename T,
		typename = typename std::enable_if<allowed<T>::value>::type>
	T Clamp(T v, T mini, T maxi);

	bool CheckCollision(
		const ColliderComponent& a,
		const ColliderComponent& b);

	bool CheckCircleOverlap(
		const Vec2& a_center,
		float a_redius,
		const Vec2& b_center,
		float b_redius);

	bool CheckAABBOverlap(
		const Vec2& a_pos,
		const Vec2& a_size,
		const Vec2& b_pos,
		const Vec2& b_size);

	bool CheckRectOverlap(
		const Rect& a,
		const Rect& b);

	bool CheckPointInCircle(
		const Vec2& point,
		const Vec2& center,
		float radius);


	float GetRandomFloat(float min, float max);
	float NormalizeAngle(float aim, float target);
	Vec2 SpeedAngleToVec2(float speed, float angle);
}
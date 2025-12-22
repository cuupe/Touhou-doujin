#pragma once
#include "../prefix.h"
namespace Engine::Core::Components {
	class ColliderComponent;
}
using namespace Engine::Core::Components;
namespace Engine::Maths {
	constexpr float _PI = 3.141592653f;
	constexpr float _2PI = 6.283185306f;
	constexpr float _1_2_PI = 1.5707963265f;
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
	inline float Vec2ToAngle(const Vec2& a) {
		return std::atan2(a.y, a.x);
	}
	inline Vec2 AngleToVec2(float angle) {
		return Vec2{ cos(angle), sin(angle) };
	}
	inline float Vec2_Length(const Vec2& a) {
		return sqrtf(a.x * a.x + a.y * a.y);
	}
	inline float Vec2_SquareLength(const Vec2& a)
	{
		return a.x * a.x + a.y * a.y;
	}
	inline Vec2 LerpVec2(const Vec2& a, const Vec2& b, float t) {
		t = std::clamp(t, 0.0f, 1.0f);

		return Engine::Maths::Vec2{
			a.x + (b.x - a.x) * t,
			a.y + (b.y - a.y) * t
		};
	}
	inline float RadToDeg(float r) {
		return fmod(360.0f * r / _2PI, 360.0f);
	}
	inline float DegToRad(float d) {
		return fmod(d / 360.0f * _2PI, _2PI);
	}

	//Vec2特供
	Vec2 Clamp(const Vec2& t, const Vec2& mini, const Vec2& maxi);
	Vec2 Clamp(const Vec2& t, float mini, float maxi);
	inline float Lerp(float a, float b, float t) {
		return a + (b - a) * t;
	}
	inline Vec2 Lerp(const Maths::Vec2& a, const Maths::Vec2& b, float t) {
		return { Lerp(a.x, b.x, t), Lerp(a.y, b.y, t) };
	}
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

	inline aiMatrix4x4 get_global_transformation(const aiNode* node) {
		aiMatrix4x4 transform = node->mTransformation;
		const aiNode* current = node->mParent;

		while (current != nullptr) {
			transform = current->mTransformation * transform;
			current = current->mParent;
		}
		return transform;
	}
	inline DirectX::XMMATRIX ai_mat_to_dx_mat(const aiMatrix4x4& mat) {
		DirectX::XMMATRIX dx_mat = DirectX::XMLoadFloat4x4(reinterpret_cast<const DirectX::XMFLOAT4X4*>(&mat));
		return DirectX::XMMatrixTranspose(dx_mat);
	}


	float GetRandomFloat(float min, float max);
	int GetRandomInt(int min, int max);
	float NormalizeAngle(float aim, float target);
	Vec2 SpeedAngleToVec2(float speed, float angle);
}
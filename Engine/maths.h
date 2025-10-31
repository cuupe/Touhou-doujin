#pragma once
namespace Engine::Maths {
	//泛用二维向量
	struct Vec2 {
		float x;
		float y;

		Vec2() :
			x(0), y(0) {
		}
		Vec2(float x) :
			x(x), y(0) {
		}
		Vec2(float x, float y) :
			x(x), y(y) {
		}
	};

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
}
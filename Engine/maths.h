#pragma once
namespace Engine::Maths {
	//精灵图的绘图区域(主要针对图像本身)
	struct SpriteRect {
		float x;
		float y;
		float h;
		float w;
		float scale_x;
		float scale_y;
		SpriteRect(float x, float y, float w, float h) :
			x(x), y(y),
			h(h), w(w),
			scale_x(1),
			scale_y(1) {
		}

		SpriteRect(float x, float y, float w, float h, float scale_x, float scale_y) :
			x(x), y(y),
			h(h), w(w),
			scale_x(scale_x),
			scale_y(scale_y) {
		}
	};

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
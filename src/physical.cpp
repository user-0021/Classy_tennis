#include "physical.hpp"
#include <math.h>

namespace physical
{
	
	template<typename T>
	Vec2D<T>::Vec2D()
	:_x((T)0),_y((T)0){
		
	}

	template<typename T>
	Vec2D<T>::Vec2D(const T &x,const T &y)
	:_x(x),_y(y){

	}

	template<typename T>
	T Vec2D<T>::length(){
		float x = (float)_x,y = (float)_y;
		
		return (T)sqrtf(x*x + y*y);
	}

	template<typename T>
	Vec2D<T> Vec2D<T>::operator+(const Vec2D &right){
		return Vec2D(_x + right._x,_y + right._y);
	}

	template<typename T>
	Vec2D<T> Vec2D<T>::operator-(const Vec2D &right){
		return Vec2D(_x - right._x,_y - right._y);		
	}

	template<typename T>
	Vec2D<T> Vec2D<T>::operator*(const T &right){
		return Vec2D(_x * right,_y * right);	
	}

	template<typename T>
	Vec2D<T> Vec2D<T>::operator/(const T &right){
		return Vec2D(_x / right,_y / right);			
	}

	template<typename T>
	T Vec2D<T>::operator*(const Vec2D &right){
		return (_x * right._x) + (_y * right._y);
	}

	template<typename T>
	T Vec2D<T>::operator/(const Vec2D &right){
		return (_x * right._y) - (right._x * _y);
	}

	Rigitbody::Rigitbody()
	:_bounce(0.0),_dont_move(false),_speed(0.0,0.0),_max_radius(0){
		
	}
	
	void Rigitbody::add_polygon(const block_polygon &polygon){
		_polygons.push_back(polygon);
		
		std::vector<block_polygon>::iterator iter = _polygons.begin();

		while (iter != _polygons.end())
		{
			uint8_t radius = (iter->local_pos).length() + 1;
			if(radius > _max_radius){
				_max_radius = radius;
			}
		}
	}
	Physical::Physical(const uint8_t &width,const uint8_t &height)
	:_width(width),_height(height){
		
	}

	Physical::~Physical(){
		
	}
	
	uint32_t Physical::add_rigidbody(const Rigitbody &rigit){
		_rigits.push_back(rigit);
		return (uint32_t)_rigits.size();
	}
} 

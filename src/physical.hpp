/// @file
/// @brief 物理演算を行うためのクラスを含みます
/// @author user-0021
/// @date 2024/01/12

#pragma once

#include<stdint.h>
#include<vector>

//簡単な物理演算を行うライブラリを提供します
namespace physical
{
	/// @brief 2次元ベクトル用クラスです
	/// @note 内積と外積はそれぞれベクトル同士の乗算と徐算で定義されます
	template<typename T>
	class Vec2D
	{
	public:
		/// @brief ベクトルのX成分
		T _x;
		/// @brief ベクトルのY成分
		T _y;

		/// @brief ゼロベクトルを生成します
		Vec2D();
		/// @brief x,y成分を指定してベクトルを生成します。
		/// @param x[in] x成分
		/// @param y[in] y成分
		Vec2D(const T &x,const T &y);
		/// @brief ベクトルの長さを取得します
		T length();

		Vec2D operator+(const Vec2D &right);
		Vec2D operator-(const Vec2D &right);
		Vec2D operator*(const T &right);
		Vec2D operator/(const T &right);
		T operator*(const Vec2D &right);
		T operator/(const Vec2D &right);
	private:
	};		

	/// @brief 正方形のポリゴンです
	typedef struct
	{
		/// @brief 法線ベクトルです
		Vec2D<float> normal;
		/// @brief ローカル座標です
		Vec2D<int8_t> local_pos;
	}block_polygon;

	//物理オブジェクトを管理します
	class Rigitbody{
		public:
		/// @brief オブジェクトの反発係数です。
		float _bounce;
		/// @brief 固定フラグです。trueだと座標が固定されます。 
		bool _dont_move;

		/// @brief ポリゴンを持たない物理オブジェクトを生成します
		Rigitbody();
		/// @brief ポリゴンを追加します
		/// @param polygon[in] 追加するポリゴン
		void add_polygon(const block_polygon &polygon);

		private:
		/// @brief ポリゴンリストです
		std::vector<block_polygon> _polygons;
		/// @brief グローバル座標です
		Vec2D<uint8_t> global_pos;
		/// @brief オブジェクトの現在の運動量のベクトル
		Vec2D<float> _speed;
		/// @brief　オブジェクトの接触判定を行うための最大半径
		uint8_t _max_radius;
	};

	/// @brief 物理演算を行います
	class Physical
	{
	public:
		/// @brief 指定サイズのベクトル空間を生成します
		/// @note 指定サイズの外に出たオブジェクトの演算はスキップされます。
		/// @param width[in] 横幅
		/// @param height[in] 縦幅
		Physical(const uint8_t &width,const uint8_t &height);
		/// @brief 物理オブジェクトなどを破棄します
		~Physical();

		/// @brief　物理オブジェクトを追加します
		/// @param rigit[in] 追加するオブジェクト
		/// @return uint32_t 追加したオブジェクトのID
		uint32_t add_rigidbody(const Rigitbody &rigit);
	private:
		/// @brief ベクトル空間の横幅
		const uint8_t _width;
		/// @brief ベクトル空間の縦幅
		const uint8_t _height;
		/// @brief 物理オブジェクトのリストです
		std::vector<Rigitbody> _rigits;
	};	
}

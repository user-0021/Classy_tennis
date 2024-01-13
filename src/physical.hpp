/// @file
/// @brief 物理演算を行うためのクラスを含みます
/// @author user-0021
/// @date 2024/01/12

#pragma once

//簡単な物理演算を行うライブラリを提供します
namespace physical
{
	/// @brief 2次元ベクトル用クラスです
	class Vec2D
	{
	public:
		/// @brief ゼロベクトルを生成します
		Vec2D();
	private:
		/// @brief ベクトルのX成分
		float x;
		/// @brief ベクトルのY成分
		float y;
	};		
}

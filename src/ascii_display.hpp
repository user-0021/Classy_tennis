/// @file
/// @brief コンソール上で文字で描画するためのクラスを含みます
/// @author user-0021
/// @date 2024/01/12

#pragma once
#include <stdint.h>
#include <vector>

#define bit_set(n) (1<<n)
#define bit_shift(d,n) (d<<n)

#define ASCII_FLAG_BOLD		bit_set(0)	//ボールド体
#define ASCII_FLAG_THIN		bit_set(1)	//薄く表示
#define ASCII_FLAG_ITALIC	bit_set(2)	//イタリック体
#define ASCII_FLAG_UNDER	bit_set(3)	//アンダーライン
#define ASCII_FLAG_HIDE		bit_set(4)	//非表示
#define ASCII_FLAG_STRIKE	bit_set(5)	//取り消し線

//固有の値としてヒープのアドレスを使うためのtypedef
//現実問題として32bitでも十分固有な値になると思うが、一様アドレスを切り詰めずに使っている。
#if UINTPTR_MAX == UINT32_MAX
	typedef uint32_t ASCII_HANDLE;
#elif UINTPTR_MAX == UINT64_MAX
	typedef uint64_t ASCII_HANDLE;
#endif

/// @brief コンソール上でグラフィックを扱うためのクラスなどを提供します
namespace ascii_display {

	/// @brief ascii display上のオブジェクトの座標を管理します
	typedef struct
	{
		/// @brief x座標
		uint8_t x;
		/// @brief y座標
		uint8_t y;
	} ascii_pos;
	
	
	/// @brief ascii_bitmapのビットデータを管理します
	typedef struct
	{
		/// @brief 描画するAsciiコードです
		char ascii;
		/// @brief フラグビット
		uint8_t flag;
		/// @brief フォントのRGBカラー
		uint32_t font_color:24;
		/// @brief 背景のRGBカラー
		uint32_t back_color:24;
	} ascii_bit;

	/// @brief 描画オブジェクトのビットマップを管理します
	typedef struct
	{
		/// @brief 描画エリアの横幅
		uint8_t width;
		/// @brief 描画エリアの縦幅
		uint8_t height;
		/// @brief ビット配列
		ascii_bit* bit_array;
	} ascii_bitmap;

	/// @brief 描画オブジェクトを管理します
	typedef struct
	{
		/// @brief 描画オブジェクトの横幅
		uint8_t width;
		/// @brief 描画オブジェクトの縦幅
		uint8_t height;
		/// @brief 描画オブジェクトの座標(座標の点はビットマップの左上です)
		ascii_pos pos;
		/// @brief 描画オブジェクトのansiコード
		char* ansi_code;
	} ascii_object;	


	/// @brief 描画エリアを管理するためのクラスです
	/// @note 現時点では同時に2つ以上のDisplayオブジェクトが存在することは許可されていません。
	/// @note 開発時間短縮などの観点から一部でも描画領域の外に出た描画オブジェクトは描画されません。
	/// @note 管理できる描画オブジェクトの最大数は255です。
	class Display
	{
	public:
		/// @brief 描画エリアを生成します。
		/// @param width[in] 描画エリアの横幅
		/// @param height[in] 描画エリアの縦幅
		Display(const uint8_t &width,const uint8_t &height);
		/// @brief 生成された描画オブジェクト等を破棄します
		~Display();
		/// @brief 描画オブジェクトを追加します
		/// @param bitmap[in] 追加するオブジェクトのビットマップ
		/// @param pos[in] 追加するオブジェクトの座標
		/// @return uint8_t 追加したオブジェクトのハンドルを返します。0が帰るとオブジェクトの追加の失敗を意味します
		ASCII_HANDLE add_object(const ascii_bitmap &bitmap,const ascii_pos &pos);

	private:
		/// @brief 描画エリアの横幅
		const uint8_t _width;
		/// @brief 描画エリアの縦幅
		const uint8_t _height;
		/// @brief 描画オブジェクトのリスト
		std::vector<ascii_object> _object_list;
	};
}
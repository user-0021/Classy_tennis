#include "ascii_display.hpp"
#include <string>
#include <iostream>

namespace ascii_display {
	
	/// @brief ビットマップデータをANSIエスケープコードにエンコードします。
	/// @param bitmap[in] エンコードするビットマップデータ
	/// @return char* エンコード済み文字列
	char* encode_ascii_bitmap(const ascii_bitmap &bitmap);
	/// @brief ascii_bitのflagからansiコードを生成します。
	/// @param flag[in] ansiコードを生成したいフラグ
	/// @return string エスケープコード,[,シーケンス識別文字は含まれません。
	std::string ansi_from_flag(const uint8_t &flag);
	/// @brief 24bitのRGBデータからansiコードを生成します
	/// @param color[in] ansiコードにしたいカラーコード
	/// @return string エスケープコード,[,シーケンス識別文字は含まれません。
	std::string ansi_from_color(const uint32_t &color);

	Display::Display(const uint8_t &width,const uint8_t &height)
	:_width(width),_height(height),_object_list(){
	}

	Display::~Display(){
	}
	
	ASCII_HANDLE Display::add_object(const ascii_bitmap &bitmap,const ascii_pos &pos){
		ascii_object object;
		object.pos = pos;
		object.width = bitmap.width;
		object.height = bitmap.height;
		object.ansi_code = encode_ascii_bitmap(bitmap);

		_object_list.push_back(object);

		return (ASCII_HANDLE)object.ansi_code;
	}

	char* encode_ascii_bitmap(const ascii_bitmap &bitmap){
		//エンコード用バッファ
		std::string ansi_code("");

		uint8_t last_flag = ~bitmap.bit_array[0].flag;//エンコードの関係でこうする
		uint32_t last_font_color = 0;
		uint32_t last_back_color = 0;
		//エンコード
		uint8_t i,j;
		for(i = 0;i < bitmap.height;i++){
			for(j = 0;j < bitmap.width;j++){
				ascii_bit bit = bitmap.bit_array[i*bitmap.width + j];

				if(last_flag & ~bit.flag){
					//最後に設定されたフラグに削除必要な要素があるなら
					ansi_code += "\033[0m";
					ansi_code += "\033["  + ansi_from_flag(bit.flag);
					ansi_code += ";38;2;" + ansi_from_color(bit.font_color);
					ansi_code += ";48;2;" + ansi_from_color(bit.back_color);
					ansi_code += "m";
				}
				else{
					//削除せずにフラグの更新が可能なら
					ansi_code += "\033["  + ansi_from_flag(bit.flag & ~last_flag);	
					if(bit.font_color != last_font_color)
						ansi_code += ";38;2;" + ansi_from_color(bit.font_color);
					if(bit.back_color != last_back_color)
						ansi_code += ";48;2;" + ansi_from_color(bit.back_color);
					ansi_code += "m";
				}

				//各種パラメータの更新
				last_flag = bit.flag;
				last_font_color = bit.font_color;
				last_back_color = bit.back_color;

				ansi_code.push_back(bit.ascii);
			}
		}

		std::cout << ansi_code << std::endl;

		//この後エンコード文字列は特に編集されないのでHeapにChar配列で置く
		std::size_t encoded_size = ansi_code.size()+1;
		char* encoded = new char[encoded_size];
		memcpy_s(encoded,encoded_size,ansi_code.c_str(),encoded_size);
		return encoded;
	}
	
	#define IF_ADD(b,l,r) if(b) l += r;
	#define STR_FROM_ASCII(v) std::string({(char)(v),'\0'})
	#define DEC_STR_FROM_8BIT(v) (STR_FROM_ASCII('0' + (v)/100) + STR_FROM_ASCII('0'+ (((v)/10)%10)) + STR_FROM_ASCII('0' + ((v)%10)))

	std::string ansi_from_flag(const uint8_t &flag){
		std::string ansi_code;
		
		//雑にifで生成
		IF_ADD(flag&bit_set(0),ansi_code,"1;")
		IF_ADD(flag&bit_set(1),ansi_code,"2;")
		IF_ADD(flag&bit_set(2),ansi_code,"3;")
		IF_ADD(flag&bit_set(3),ansi_code,"4;")
		IF_ADD(flag&bit_set(4),ansi_code,"8;")
		IF_ADD(flag&bit_set(5),ansi_code,"9;")

		if(!ansi_code.empty())
			ansi_code.pop_back();
		return ansi_code;
	}
	
	std::string ansi_from_color(const uint32_t &color){
		
		std::string ansi_code;
		
		ansi_code += DEC_STR_FROM_8BIT((color >> 16) & 0xFF) + ";";
		ansi_code += DEC_STR_FROM_8BIT((color >>  8) & 0xFF) + ";";
		ansi_code += DEC_STR_FROM_8BIT((color >>  0) & 0xFF);

		return ansi_code;
	}
}
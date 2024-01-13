#include "ascii_display.hpp"

#include <string>
#include <fstream>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>

#define IF_ADD(b,l,r) if(b) l += r;
#define STR_FROM_ASCII(v) std::string({(char)(v),'\0'})
#define DEC_STR_FROM_8BIT(v) (STR_FROM_ASCII('0' + (v)/100) + STR_FROM_ASCII('0'+ (((v)/10)%10)) + STR_FROM_ASCII('0' + ((v)%10)))

namespace ascii_display {
	
	/// @brief ビットマップデータをANSIエスケープコードにエンコードします。
	/// @param bitmap[in] エンコードするビットマップデータ
	/// @return std::string* エンコード済みstringへのポインタ
	std::string* encode_ascii_bitmap(const ascii_bitmap &bitmap);
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
		this->clear();
	}

	Display::~Display(){
		std::vector<ascii_object>::iterator iter = _object_list.begin();

		while (iter != _object_list.end())
		{
			delete iter->ansi_code;
			iter++;
		}

		std::cout << "\033[" << (int)_height << "B"
			<< "\033[" << (int)(_width) << "C" << std::endl;
	}
	
	void Display::clear(){			
		//ディスプレイのサイズを塗りつぶす
		uint8_t i,j;
		for(i = 0;i < _height + 1;i++){//カーソル捜査の関係で高さ+1しておくと便利
			for(j = 0;j < _width;j++){
				std::cout << " ";
			}
			std::cout << std::endl;
		}

		//カーソルを初期位置にセット
		std::cout << "\033[" << (int)(_height-1) << "A\033[" << (int)(_width-1) << "D";
	}

	void Display::update(){
		std::vector<ascii_object>::iterator iter = _object_list.begin();

		while (iter != _object_list.end())
		{
			if((iter->width + iter->pos.x) <= _width
			 && (iter->height + iter->pos.y) <= _height){
				//位置合わせ
				if(iter->pos.y)
					std::cout << "\033[" << (int)(iter->pos.y) << "B";
				if(iter->pos.x)
					std::cout << "\033[" << (int)(iter->pos.x) << "C";
				//描画
				std::cout << *iter->ansi_code << "\033[0m";
				//初期位置に戻る
				std::cout << "\033[" << (int)(iter->height + iter->pos.y) << "A"
					<< "\033[" << (int)(iter->pos.x) << "D";
			}
			iter++;
		}
		
	}
	
	void Display::fill(const ascii_pos &begine,const ascii_pos &end,const ascii_bit &bit)
	{
		if(end.x > _width || end.y > _height || begine.x > end.x || begine.y > end.y)
			return;//無効な範囲は描画スキップ

		//必要な値の計算
		uint8_t width = (end.x - begine.x) + 1;
		uint8_t height = (end.y - begine.y) + 1;
		
		std::string ansi_data("\033[");
		if(bit.flag)
			ansi_data += ansi_from_flag(bit.flag) + ";";
		ansi_data += "38;2;" + ansi_from_color(bit.font_color);
		ansi_data += ";48;2;" + ansi_from_color(bit.back_color) + "m";
	
		//位置合わせ
		if(begine.y)
			std::cout << "\033[" << (int)(begine.y) << "B";
		if(begine.x)
			std::cout << "\033[" << (int)(begine.x) << "C";

		//描画
		std::cout << ansi_data;
		uint8_t i,j;
		for(i = 0;i < height;i++){
			for(j = 0;j < width;j++){
				std::cout << bit.ascii;
			}
			std::cout << "\033[1B\033[" << (int)width << "D";
		}		
		//初期化
		std::cout << "\033[0m";
		
		//初期位置に戻る
		std::cout << "\033[" << (int)(end.y + 1) << "A"
			<< "\033[" << (int)(begine.x) << "D";
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
	
	void Display::remove_object(const ASCII_HANDLE &handle){
		std::vector<ascii_object>::iterator iter = _object_list.begin();

		while (iter != _object_list.end())
		{
			if(((ASCII_HANDLE)iter->ansi_code) == handle){
				delete iter->ansi_code;
				_object_list.erase(iter);
				return;
			}
			iter++;
		}
	}
	
	void Display::object_move_front(const ASCII_HANDLE &handle){
		std::vector<ascii_object>::iterator iter = _object_list.begin();

		while (iter != _object_list.end())
		{
			if(((ASCII_HANDLE)iter->ansi_code) == handle){
				ascii_object object = *iter;
				_object_list.erase(iter);
				_object_list.push_back(object);
				return;
			}
			iter++;
		}
	}

	void Display::object_swap_pos(const ASCII_HANDLE &handle_1,const ASCII_HANDLE &handle_2){
		std::vector<ascii_object>::iterator iter = _object_list.begin();
		std::vector<ascii_object>::iterator target = _object_list.end();
		
		while (iter != _object_list.end())
		{
			if(((ASCII_HANDLE)iter->ansi_code) == handle_1 || ((ASCII_HANDLE)iter->ansi_code) == handle_2){
				if(target != _object_list.end()){
					ascii_pos tmp = iter->pos;
					iter->pos = target->pos;
					target->pos = tmp;
					return;
				}

				target = iter;
			}
			iter++;
		}
	}
	
	void Display::object_update_pos(const ASCII_HANDLE &handle,const ascii_pos &pos){
		std::vector<ascii_object>::iterator iter = _object_list.begin();

		while (iter != _object_list.end())
		{
			if(((ASCII_HANDLE)iter->ansi_code) == handle){
				iter->pos = pos;
				return;
			}
			iter++;
		}
	}

	std::string* encode_ascii_bitmap(const ascii_bitmap &bitmap){
		//エンコード用バッファ
		std::string* ansi_code = new std::string;

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
					*ansi_code += "\033[0m";
					*ansi_code += "\033["  + ansi_from_flag(bit.flag);
					*ansi_code += ";38;2;" + ansi_from_color(bit.font_color);
					*ansi_code += ";48;2;" + ansi_from_color(bit.back_color);
					*ansi_code += "m";
				}
				else{
					//削除せずにフラグの更新が可能なら
					std::string work_buffer;

					if(bit.flag != last_flag)
						work_buffer +=  ansi_from_flag(bit.flag & ~last_flag);
					if(bit.font_color != last_font_color){
						if(!work_buffer.empty())
							work_buffer.push_back(';');
						work_buffer += "38;2;" + ansi_from_color(bit.font_color);
					}
					if(bit.back_color != last_back_color){
						if(!work_buffer.empty())
							work_buffer.push_back(';');
						work_buffer += "48;2;" + ansi_from_color(bit.back_color);
					}
					
					if(!work_buffer.empty()){

						*ansi_code += "\033[" + work_buffer + "m";
					}
				}

				//各種パラメータの更新
				last_flag = bit.flag;
				last_font_color = bit.font_color;
				last_back_color = bit.back_color;

				ansi_code->push_back(bit.ascii);
			}
			*ansi_code += "\033[1B\033[" + DEC_STR_FROM_8BIT(bitmap.width) + "D";
		}
		
		return ansi_code;
	}
	
	void write_ascii_bitmap(const std::string &path,const ascii_bitmap &bitmap){
		std::ofstream ascii_binary;

		ascii_binary.open(path,std::ios_base::out | std::ios_base::binary);
		
		if(!ascii_binary.is_open()){
			throw "in write_ascii_bitmap: failed open "+path;
		}

		ascii_binary << bitmap.width << bitmap.height;
		
		uint16_t i;
		uint16_t bitmap_size = bitmap.width * bitmap.height;
		for(i = 0;i < bitmap_size;i++){
			ascii_binary 
				<< bitmap.bit_array[i].ascii 
				<< bitmap.bit_array[i].flag
				<< (uint8_t)((bitmap.bit_array[i].font_color >> 16)&0xFF)
				<< (uint8_t)((bitmap.bit_array[i].font_color >>  8)&0xFF)
				<< (uint8_t)((bitmap.bit_array[i].font_color >>  0)&0xFF)
				<< (uint8_t)((bitmap.bit_array[i].back_color >> 16)&0xFF)
				<< (uint8_t)((bitmap.bit_array[i].back_color >>  8)&0xFF)
				<< (uint8_t)((bitmap.bit_array[i].back_color >>  0)&0xFF);
		}
	}
	
	void read_ascii_bitmap(const std::string &path,ascii_bitmap *bitmap){
		std::ifstream ascii_binary;

		ascii_binary.open(path,std::ios_base::in | std::ios_base::binary);
		
		if(!ascii_binary.is_open()){
			throw "in write_ascii_bitmap: failed open "+path;
		}

		ascii_binary.read((char*)&bitmap->width,1);
		ascii_binary.read((char*)&bitmap->height,1);
		
		uint16_t i;
		uint16_t bitmap_size = bitmap->width * bitmap->height;
		for(i = 0;i < bitmap_size;i++){
			char work_buff[8];
			ascii_binary.read(work_buff,8);
			
			bitmap->bit_array[i].ascii = work_buff[0]; 
			bitmap->bit_array[i].flag  = work_buff[1];
			
			bitmap->bit_array[i].font_color = ((work_buff[2]&0xFF)<<16) | ((work_buff[3]&0xFF)<<8) | ((work_buff[4]&0xFF)<<0);
			bitmap->bit_array[i].back_color = ((work_buff[5]&0xFF)<<16) | ((work_buff[6]&0xFF)<<8) | ((work_buff[7]&0xFF)<<0);
		}
	}
	
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
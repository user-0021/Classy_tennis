#include "ascii_display.hpp"
#include <iostream>
#include <windows.h>
#include <signal.h>

#define SET_ASCII_BIT(bit,a,f,f_c,f_b) bit.ascii=a;bit.flag=f;bit.font_color=f_c;bit.back_color=f_b

bool End_Game = false;//終了フラグ
void sigint_handler(int signal);//SIGINT用ハンドラー

int main()
{
	//SIGINT用ハンドラーの登録
	signal(SIGINT,sigint_handler);

	ascii_display::Display display(60,30);

	ascii_display::ascii_bitmap* bitmap = new ascii_display::ascii_bitmap;
	bitmap->bit_array = new ascii_display::ascii_bit[30*60];
	ascii_display::read_ascii_bitmap("C:\\Users\\nabey\\source\\repos\\Combo_Combo_Combo\\models\\stage.ascii",bitmap);
	display.add_object(*bitmap,{0,0});

	bitmap->width = 2;
	bitmap->height = 1;
	SET_ASCII_BIT(bitmap->bit_array[0],' ',0,0xFFFFFF,0x00CC00);
	SET_ASCII_BIT(bitmap->bit_array[1],' ',0,0xFFFFFF,0x00CC00);
	ASCII_HANDLE handle = display.add_object(*bitmap,{10,10});

	

	delete[] bitmap->bit_array;
	delete bitmap;


	display.update();
	Sleep(1000);
	display.object_update_pos(handle,{12,11});
	display.update();
	Sleep(1000);
	display.object_update_pos(handle,{14,12});
	display.update();
	Sleep(1000);

	while (1)
	{
		if(End_Game)
			break;
	}
	
	
	return 0;
}


void sigint_handler(int signal){
	End_Game = true;
}
#include "ascii_display.hpp"
#include "physical.hpp"
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

	ascii_display::Display display(60,30);//半角文字の縦横比は1:2なので

	ascii_display::ascii_bitmap* bitmap = new ascii_display::ascii_bitmap;
	bitmap->bit_array = new ascii_display::ascii_bit[30*60];
	ascii_display::read_ascii_bitmap("C:\\Users\\nabey\\source\\repos\\Combo_Combo_Combo\\models\\stage.ascii",bitmap);
	display.add_object(*bitmap,{0,0});
	

	bitmap->width = 2;
	bitmap->height = 1;
	SET_ASCII_BIT(bitmap->bit_array[0],' ',0,0xFFFFFF,0x00CC00);
	SET_ASCII_BIT(bitmap->bit_array[1],' ',0,0xFFFFFF,0x00CC00);
	ASCII_HANDLE handle = display.add_object(*bitmap,{1,28});

	

	delete[] bitmap->bit_array;
	delete bitmap;


	//物理演算系
	physical::Physical phys(30,30);
	
	physical::Rigitbody floar;
	int8_t i = -14;
	while(i != 16){
		physical::block_polygon poly;
		poly.normal._x = 0.0;
		poly.normal._y = -1.0;
		poly.local_pos._x = i;
		poly.local_pos._y = 0;
		floar.add_polygon(poly);
		i++;
	}

	phys.add_rigidbody(floar);

	display.update();
	Sleep(100);
	// display.object_update_pos(handle,{12,28});
	// display.update();
	// Sleep(100);
	// display.object_update_pos(handle,{14,28});
	// display.update();
	// Sleep(1000);
	uint8_t x = 1;
	bool count_up = true;
	while (1)
	{
		Sleep(100);
		if(count_up)
			x++;
		else
			x--;
		
		if(x == 56){
			count_up = false;
		}else if(x == 2){
			count_up = true;
		}
		
		display.object_update_pos(handle,{x,28});
		display.update();
		Sleep(50);

		if(End_Game)
			break;
	}
	
	
	return 0;
}


void sigint_handler(int signal){
	End_Game = true;
}
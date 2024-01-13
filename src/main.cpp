#include "ascii_display.hpp"

#define SET_ASCII_BIT(bit,a,f,f_c,f_b) bit.ascii=a;bit.flag=f;bit.font_color=f_c;bit.back_color=f_b

int main()
{
	ascii_display::Display display(0,0);

	ascii_display::ascii_bitmap bitmap;
	bitmap.height = 1;
	bitmap.width = 5;
	bitmap.bit_array = new ascii_display::ascii_bit[5];
	SET_ASCII_BIT(bitmap.bit_array[0],'A',ASCII_FLAG_BOLD,0x111111,0x000000);
	SET_ASCII_BIT(bitmap.bit_array[1],'A',ASCII_FLAG_BOLD,0x111111,0x000000);
	SET_ASCII_BIT(bitmap.bit_array[2],'A',ASCII_FLAG_BOLD,0x111111,0x000000);
	SET_ASCII_BIT(bitmap.bit_array[3],'A',ASCII_FLAG_BOLD,0x111111,0x000000);
	SET_ASCII_BIT(bitmap.bit_array[4],'A',ASCII_FLAG_BOLD,0x111111,0x000000);

	display.add_object(bitmap,{0,0});

	return 0;
}

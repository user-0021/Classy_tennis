#include "ascii_display.hpp"
#include <iostream>
#include <signal.h>
#include <math.h>

#ifdef _WIN32
	#include <windows.h>
	#define SLEEP(MS) Sleep(MS)
#endif


#define MODEL_0 "0.ascii"
#define MODEL_1 "1.ascii"
#define MODEL_2 "2.ascii"
#define MODEL_BAR "bar.ascii"
#define MODEL_BALL "ball.ascii"
#define MODEL_STAGE "stage.ascii"
#define MODEL_RACKET "racket.ascii"
#define MODEL_RESULT_WIN "youwin.ascii"
#define MODEL_RESULT_LOOSE "youloose.ascii"

//移動可能な最大範囲(1次元)
typedef struct{
	uint8_t max;
	uint8_t min; 
}move_limit_1d;

//移動可能な最大範囲(2次元)
typedef struct{
	uint8_t left;
	uint8_t right;
	uint8_t top;
	uint8_t middle; 
}move_limit_2d;

//ベクトル
typedef struct{
	float x;
	float y;
}vectol;

//ラケット用構造体
typedef struct{
	uint8_t x;
	uint8_t y;
	uint8_t length;
	move_limit_1d limit;
	ASCII_HANDLE handle;
}racket;

//物理オブジェクト用構造体
typedef struct{
	float x;
	float y;
	vectol speed;
	move_limit_2d limit;
	ASCII_HANDLE handle;
}physics_object;

bool End_Game = false;//終了フラグ
void sigint_handler(int signal);//SIGINT用ハンドラ

ASCII_HANDLE Player_Score_Bord[3];//プレーヤーの得点文字用バッファ
ASCII_HANDLE Cpu_Score_Bord[3];//CPUの得点文字用バッファ

void score_system_init(ascii_display::Display &display);//得点システム初期化関数
void cpu_get_point(ascii_display::Display &display);//CPUの得点のカウントアップ
void player_get_point(ascii_display::Display &display);//Playerの得点のカウントアップ
void print_result(ascii_display::Display & display,const bool &win);//勝利画面

physics_object ball;
racket cpu;
racket player;

void physics_engine_init(ascii_display::Display &display);
void update_pos();//座標計算
void check_collision(ascii_display::Display &display);


int main()
{
	//SIGINT用ハンドラーの登録
	signal(SIGINT,sigint_handler);

	ascii_display::Display display(120,30);//半角文字の縦横比は1:2なので
	
	//マップ生成
	ascii_display::ascii_bitmap* bitmap = new ascii_display::ascii_bitmap;
	bitmap->bit_array = new ascii_display::ascii_bit[120 * 30];
	ascii_display::read_ascii_bitmap(MODEL_STAGE,*bitmap);
	display.add_object(*bitmap,{0,0});

	delete[] bitmap->bit_array;
	delete bitmap;

	//得点システムの初期化
	score_system_init(display);
	
	//物理エンジンの初期化
	physics_engine_init(display);
	

	display.update();
	SLEEP(1000);
	while (1)
	{		
		//座標の計算
		update_pos();
		//衝突判定
		check_collision(display);
		//座標更新
		display.object_update_pos(cpu.handle,{cpu.x,cpu.y});
		display.object_update_pos(player.handle,{player.x,player.y});
		display.object_update_pos(ball.handle,{(uint8_t)ball.x,(uint8_t)ball.y});

		display.update();
		SLEEP(30);

		if(End_Game)
			break;
	}
	
	
	return 0;
}


void sigint_handler(int signal){
	End_Game = true;
}

void score_system_init(ascii_display::Display &display){
	//asciiビットマップ読み込み用バッファ
	ascii_display::ascii_bitmap bitmap;
	bitmap.bit_array = new ascii_display::ascii_bit[8 * 7];

	//0点
	ascii_display::read_ascii_bitmap(MODEL_0,bitmap);
	Player_Score_Bord[0] = display.add_object(bitmap,{48,2});
	Cpu_Score_Bord[0] = display.add_object(bitmap,{64,2});
	//1点
	ascii_display::read_ascii_bitmap(MODEL_1,bitmap);
	Player_Score_Bord[1] = display.add_object(bitmap,{0,100});
	Cpu_Score_Bord[1] = display.add_object(bitmap,{0,100});
	//2点
	ascii_display::read_ascii_bitmap(MODEL_2,bitmap);
	Player_Score_Bord[2] = display.add_object(bitmap,{0,100});
	Cpu_Score_Bord[2] = display.add_object(bitmap,{0,100});
	//バー
	ascii_display::read_ascii_bitmap(MODEL_BAR,bitmap);
	display.add_object(bitmap,{56,2});
	
	//不要になったオブジェクトの解放
	delete[] bitmap.bit_array;
}

void cpu_get_point(ascii_display::Display & display){
	static uint8_t cpu_score = 0;

	cpu_score++;

	if(cpu_score == 3){//3点でゲーム終了
		print_result(display,false);
		getchar();
		_exit(0);
	}

	//得点のスワップ
	display.object_swap_pos(Cpu_Score_Bord[cpu_score - 1],Cpu_Score_Bord[cpu_score]);
	SLEEP(1000);
}

void player_get_point(ascii_display::Display & display){
	static uint8_t player_score = 0;

	player_score++;

	if(player_score == 3){//3点でゲーム終了
		print_result(display,true);
		getchar();
		_exit(0);
	}

	//得点のスワップ
	display.object_swap_pos(Player_Score_Bord[player_score - 1],Player_Score_Bord[player_score]);
	SLEEP(1000);
}

void print_result(ascii_display::Display & display,const bool &win){
	ascii_display::ascii_bitmap bitmap;
	bitmap.bit_array = new ascii_display::ascii_bit[120 * 30];

	if(win)
		ascii_display::read_ascii_bitmap(MODEL_RESULT_WIN,bitmap);
	else
		ascii_display::read_ascii_bitmap(MODEL_RESULT_LOOSE,bitmap);

	display.add_object(bitmap,{0,0});
	display.update();
}

void physics_engine_init(ascii_display::Display &display){
	//asciiビットマップ読み込み用バッファ
	ascii_display::ascii_bitmap bitmap;
	bitmap.bit_array = new ascii_display::ascii_bit[5];

	//ボール生成
	ascii_display::read_ascii_bitmap(MODEL_BALL,bitmap);
	ball.x = 10.0;
	ball.y = 10.0;
	ball.speed = {1.0,1.0};
	ball.limit = {2, 116, 1, 28};
	ball.handle = display.add_object(bitmap,{(uint8_t)ball.x,(uint8_t)ball.y});

	
	//ラケットの生成
	player.x = 8;
	player.y = 4;
	player.length = 5;
	player.limit = {24,1};	

	cpu.x = 112;
	cpu.y = 4;
	cpu.length = 5;
	cpu.limit = {24,1};	

	ascii_display::read_ascii_bitmap(MODEL_RACKET,bitmap);
	cpu.handle = display.add_object(bitmap,{cpu.x,cpu.y});
	player.handle = display.add_object(bitmap,{player.x,player.y});
}

void update_pos(){
	//ボールの位置計算
	ball.x += ball.speed.x;
	ball.y += ball.speed.y;	

	//cpuの座標計算
	if(((uint8_t)ball.y) < cpu.y)
		cpu.y--;
	else if(((uint8_t)ball.y) > cpu.y + 5)
		cpu.y++;
		
	//プレーヤーの座標計算
	#ifdef _WIN32
		if((GetKeyState(VK_UP) & 0x8000) && player.y > player.limit.min)
			player.y--;
		else if((GetKeyState(VK_DOWN) & 0x8000) && player.y < player.limit.max)
			player.y++;
	#endif
}

void check_collision(ascii_display::Display &display){
	uint8_t ball_x = (uint8_t)ball.x;
	uint8_t ball_y = (uint8_t)ball.y;

	//得点チェック
	if(ball_x < ball.limit.left){//CPUの得点
		ball_x = ball.x = 20.0;
		ball.speed.x *= -1;
		cpu_get_point(display);
	}else if(ball_x > ball.limit.right){//プレイヤーの得点
		ball_x = ball.x = 100;
		ball.speed.x *= -1;
		player_get_point(display);
	}else if(ball_y < ball.limit.top || ball_y > ball.limit.middle){//壁衝突
		ball_y = ball.y = ball.y - ball.speed.y;
		ball.speed.y *= -1;
	}

	//cpuラケット判定
	if(ball_x + 1 == cpu.x && ball_y >= cpu.y && ball_y < cpu.y + cpu.length){
		ball.x -= ball.speed.x;
		ball.speed.x *= -1;
	}

	//プレーヤーラケット判定
	if(ball_x == player.x && ball_y >= player.y && ball_y < player.y + player.length){
		ball.x -= ball.speed.x;
		ball.speed.x *= -1;
	}
}
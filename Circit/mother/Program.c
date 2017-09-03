/*	Program.c
 *	高専ロボコン2017 Aチームプログラム
 *	ver. 0.50
 */

#include "includes.h"

/* Funcs Predefine */
void main(void);
void setup(void);
void motor_emit(char channnel, int power);
void motor_reset(void);
void PS2_read(void);
void led_reset(void);
void led_flash(void);
void led_on(char pin);
void led_off(char pin);

/* Global Vars */
/*	uchar PS2_Data[PS2_DATA_BUFFER_SIZE]
 *	PSコントローラからのデータを格納するバッファ
 */	unsigned char PS2_Data[PS2_DATA_BUFFER_SIZE];
/*	int PS2_offset
 *コントローラのオフセット
 */	int PS2_offset = 0;
const int pwt[] = PWR_MOV_TABLE;

void main()
{
	/* 変数宣言 */
	/*	int i
	 *	Index変数
	 */	int i;
	/*	int* f
	 *	Frame変数
	 */	int f[1];
	/*	int rcv
	 *	コントローラの受信状態
	 */	int rcv = 0;
	#if Program != 0
	signed int level[MOTOR_NUM];
	/*	int* pwr
	 *	モータ出力レベル
	 */	signed int pwr[MOTOR_NUM];
	#endif
	/*	uchar motor_buf
	 *	モータ出力状態バッファ
	 */	unsigned char motor_buf = 0;
	/*	long rcv_err
	 *	PSコントローラからの通信が途絶えたときに通信断絶状態と判定するための変数
	 *	閾値はRCV_THRESHOLDとする
	 */	long int rcv_err = RCV_THRESHOLD;
	#if ANALOG_ENABLE != 0
	Analogs Stick;						/* PSコントローラのアナログスティック情報 */
	#endif
	/* 初期化 */
	RESET:								/* ソフトリセット */
	setup();							/* セットアップ */
	motor_reset();						/* モータードライバに0(出力0%)を送信(暴走を防ぐため) */
	/* LED試験点灯 */
	led_flash();
	led_reset();
	delay_ms(500);
	/* メインループ */
	for(;;)								/* 無限ループ */
	{
		/* 各変数をリフレッシュ */
		#if Program != 0
		for(i = 0; i < MOTOR_NUM; i++)
		{
			level[i] = 0;
			pwr[i] = 0;
		}
		#endif
		PS2_offset = 0;
		f[0]++;	if (f[0] >= 2) f[0] = 0;
		/* PSコントローラからデータ受信 */
		PS2_read();
		/* PSコントローラのデータ処理 */
		if(PS2_Data[1 + PS2_offset]=='Z')
		{
			/* 信号を受信できた場合の処理 */
			/* 再試行回数のカウントをリセット、通信状態のフラグを1にする */
			rcv_err=0;
			rcv = 1;
			if(PS2_Data[2 + PS2_offset]&1) goto RESET;/* SELECTキーが押されたらソフトリセット */
			/* データ処理 */
			if (PS2_PUSH_R1)
			{
				/* ロボット1(釣り竿展開用ロボ) */
				#if (Program == 1)
				level[0] = (PS2_PUSH_UP ? (PS2_PUSH_L1 ? 2 : 1) : 0) - (PS2_PUSH_DN ? (PS2_PUSH_L1 ? 2 : 1) : 0);
				level[1] = (PS2_PUSH_UP ? (PS2_PUSH_L1 ? 2 : 1) : 0) - (PS2_PUSH_DN ? (PS2_PUSH_L1 ? 2 : 1) : 0);
				if (PS2_PUSH_LE)
				{
					level[0] = (PS2_PUSH_L1 ? 2 : 1);
					level[1] -= (PS2_PUSH_L1 ? 2 : 1);
				}
				if (PS2_PUSH_RI)
				{
					level[0] -= (PS2_PUSH_L1 ? 2 : 1);
					level[1] = (PS2_PUSH_L1 ? 2 : 1);
				}
				pwr[2] = PS2_PUSH_L2 ? PWR_AIR : 0;
				pwr[3] = PS2_PUSH_R2 ? PWR_AIR : 0;
				pwr[4] = (PS2_PUSH_TRI ? PWR_ARM : 0) - (PS2_PUSH_SQU ? PWR_ARM : 0);
				#endif
				/* ロボット2(風船割ロボ) */
				#if (Program == 2)
				#endif
			}
		}
		else
		{
		//PSコントローラから信号を受信できなかった時の処理
			if(rcv_err>=RCV_THRESHOLD)
			{
			//再試行回数が閾値を超えた場合通信断絶と判断する
				rcv=0;
			}
			//再試行回数のカウント
			else rcv_err++;
		}
	/* 入力内容からデータを処理 */
		#if Program != 0
		pwr[0] = pwt[2 + level[0]];
		pwr[1] = pwt[2 + level[1]];
		#endif
	/* LED点灯制御 */
		if(f[0])
			led_on(LED_OPR);
		else
			led_off(LED_OPR);
		if(EMITRULE_LED_F1)
			led_on(LED_F1);
		else
			led_off(LED_F1);
	/* モータードライバの出力内容を決定、信号を出力する */
		if (rcv && PS2_PUSH_R1)
		{
			#if Program == 1
			if (pwr[0] || pwr[1] || (motor_buf&0x01))
			{
				motor_emit(MOTOR_MOVEL, pwr[0]);
				motor_emit(MOTOR_MOVER, pwr[1]);
				motor_buf = motor_buf & (!0x01);
				motor_buf = motor_buf | ((pwr[0] || pwr[1]) ? 0x01 : 0x00);
			}
			if (pwr[2] || (motor_buf&0x02))
			{
				motor_emit(MOTOR_AIRa, pwr[2]);
				motor_buf = motor_buf & (!0x02);
				motor_buf = motor_buf | ((pwr[2]) ? 0x02 : 0x00);
			}
			if (pwr[3] || (motor_buf&0x04))
			{
				motor_emit(MOTOR_AIRb, pwr[3]);
				motor_buf = motor_buf & (!0x04);
				motor_buf = motor_buf | ((pwr[3]) ? 0x04 : 0x00);
			}
			if (pwr[4] || (motor_buf&0x08))
			{
				motor_emit(MOTOR_ARM, pwr[4]);
				motor_buf = motor_buf & (!0x08);
				motor_buf = motor_buf | ((pwr[4]) ? 0x08 : 0x00);
			}
			#endif
		}
		else
		{
			#if Program == 1
			motor_emit(MOTOR_MOVEL, 0);
			motor_emit(MOTOR_MOVER, 0);
			motor_emit(MOTOR_AIRa, 0);
			motor_emit(MOTOR_AIRb, 0);
			motor_emit(MOTOR_ARM, 0);
			#endif
		}
		#if LOOP_DELAYUNIT_US
		delay_us(LOOP_DELAY);
		#else
		delay_ms(LOOP_DELAY);
		#endif
	}	/* メインループ内ここより先到達できないコード */
}

void setup(void)
{
	/* 入力ピンのセット */
	set_tris_a(0xC0);
	set_tris_b(0xFF);
	/*	portC入力ピン設定
		C7は受信に使用するため該当ビットを1にする
		XMIT=6,RCV=7
	 */	set_tris_c(0x8D);
	/* オシレータの初期化 */
	setup_oscillator(OSC_8MHZ);
}

void motor_emit(char channnel, int power)
{
	printf("%c%U*\r", channnel, power);
	delay_us(TIME_MOTOR_MARGIN);
}

void motor_reset(void)
{
	printf("A%U*\r",0);   delay_us(TIME_MOTOR_MARGIN);
	printf("B%U*\r",0);   delay_us(TIME_MOTOR_MARGIN);
	printf("C%U*\r",0);   delay_us(TIME_MOTOR_MARGIN);
	printf("D%U*\r",0);   delay_us(TIME_MOTOR_MARGIN);
	printf("E%U*\r",0);   delay_us(TIME_MOTOR_MARGIN);
	printf("F%U*\r",0);   delay_us(TIME_MOTOR_MARGIN);
	printf("L%U*\r",0);   delay_us(TIME_MOTOR_MARGIN);
	printf("R%U*\r",0);   delay_us(TIME_MOTOR_MARGIN);
}

void PS2_read()
{
	int i;
	for(i = 0; i < PS2_DATA_BUFFER_SIZE; i++)
		PS2_Data[i]=0;
	gets(PS2_Data);
	for (i = 0; i < PS2_DATA_BUFFER_SIZE - 1; i++)
	{
		//PSコントローラの識別子0x5a('Z')を捜索、オフセットを決定
		if (PS2_Data[i] == 'Z')
		{
			PS2_offset = i - 1;
			break;
		}
	}
}

void led_reset(void)
{
	led_off(LED_OPR);
	led_off(LED_F1);
}

void led_flash(void)
{
	led_on(LED_OPR);
	int i;
	for(i= 5; i; i--){
		led_on(LED_F1);
		delay_ms(200);
		led_off(LED_F1);
		delay_ms(200);
	}
}

void led_on(char pin)
{
	if (LED_PULLUP)
		output_low(pin);
	else
		output_high(pin);
}

void led_off(char pin)
{
	if (LED_PULLUP)
		output_high(pin);
	else
		output_low(pin);
}

/* Program.c EOF */
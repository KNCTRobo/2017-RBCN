/*	Program.c
 *	高専ロボコン2017 Aチームプログラム
 *	ver. 1.02
 */

#include "includes.h"

/* Funcs Predefine */
/*	main()
 *	プログラムのメインエントリポイントです */
 void main(void);
/*	setup()
 *	プログラムのセットアップセッションです */
 void setup(void);
/*	motor_emit(char channnel,int power)
 *	モータードライバにデータを送信します */
 void motor_emit(char channnel, int power);
/*	motor_reset()
 *	モータードライバに出力0を送信します */
 void motor_reset(void);
/*	PS2_read()
 *	コントローラからの信号を受信します
 *	受信したデータはグローバル変数PS2_Dataに格納されます */
 void PS2_read(void);
/*	Data_Proc(int*)
 *	データ処理 */
 void Data_Proc(signed int* stat);
/*	Set_pwr(int*, int*, int)
 *	モータードライバ出力レベル決定 */
 void Set_pwr(signed int* stat, signed int* power);
/*	motor_senddata(int* power, int* buffer, int index)
 *	モーターへの出力情報をまとめて送信します */
 void motor_senddata(int* power, int* buffer, int index);
/*	int = motor_isemit(int* power)
 *	モーターへの出力状態を返します
 *	1つ以上出力状態が0でない場合1を返します */
 int motor_isemit(int* power);
/*	led_reset()
 *	LEDを全消灯します */
 void led_reset(void);
/*	led_flash()
 *	LEDを点滅させます */
 void led_flash(void);
/*	led_reflesh(int LEDlogic_OPR, int LEDlogic_F1, int LEDlogic_F2, int LEDlogic_F3)
 *	LEDの点灯状態を更新します */
 void led_reflesh(int LEDlogic_OPR, int LEDlogic_F1, int LEDlogic_F2, int LEDlogic_F3);
/*	led_on(char pin)
 *	指定されたピンのLEDをONにします */
 void led_on(char pin);
/*	led_off(char pin)
 *	指定されたピンのLEDをOFFにします */
 void led_off(char pin);

/* Global Vars */
/*	uchar PS2_Data[PS2_DATA_BUFFER_SIZE]
 *	PSコントローラからのデータを格納するバッファ */
 unsigned char PS2_Data[PS2_DATA_BUFFER_SIZE];
/*	int PS2_offset
 *コントローラのオフセット */
 int PS2_offset = 0;
/* 	const int pwt[]
 *	移動モーターに出力するレベルのテーブル
 *	内容はdefines.h内のPWR_MOV_TABLEで定義されています */
 const int pwt[] = PWR_MOV_TABLE;
/*	int rcv
 *	コントローラの受信状態 */
 int rcv = 0;
#if ANALOG_ENABLE != 0
/*	Analogs stick
 *	PSコントローラのアナログスティック情報 */
 Analogs Stick;
#endif

void main()
{
	/* 変数宣言 */
	/*	int i
	 *	Index変数 */
	 int i;
	/*	int* f
	 *	Frame変数 */
	 int f[2];
	/*	long rcv_err
	 *	PSコントローラからの通信が途絶えたときに通信断絶状態と判定するための変数
	 *	閾値はRCV_THRESHOLDとする */
	 long int rcv_err = RCV_THRESHOLD;
	#if Program != 0
	signed int level[MOTOR_NUM];
	/*	int* pwr
	 *	モータ出力レベル */
	 signed int pwr[MOTOR_NUM];
	/*	int* motor_buf
	 *	モータ出力レベル */
	 signed int motor_buf[MOTOR_NUM];
	#endif

	/* 初期化
	 * ソフトリセット時の復帰ポイント */
	RESET:
	/* セットアップ */
	setup();
	/* モータードライバに0(出力0%)を送信(暴走を防ぐため) */
	motor_reset();

	/* LED点滅 */
	led_flash();
	led_reset();
	delay_ms(500);

	/* メインループ */
	for(;;)								/* 無限ループ */
	{
		/* 各変数をリフレッシュ */
		#if Program != 0
		/* 接続がタイムアウトしている場合ドライバ出力を0にする */
		if (rcv == 0)
		{
			for(i = 0; i < MOTOR_NUM; i++)
			{
				level[i] = 0;
				pwr[i] = 0;
			}
		}
		#endif
		f[0]++;	if (f[0] >= 2) f[0] = 0;
		f[1]++;	if (f[1] >= MOTOR_NUM) f[1] = 0;

		/* PSコントローラからデータ受信 */
		PS2_read();

		/* PSコントローラのデータ処理 */
		if(PS2_Data[1 + PS2_offset]=='Z')
		{
			/* 信号を受信できた場合の処理 */
			/* 再試行回数のカウントをリセット、通信状態のフラグを1にする */
			rcv_err=0;
			rcv = 1;
			/* SELECTキーが押されたらソフトリセット */
			if(PS2_Data[2 + PS2_offset]&1) goto RESET;
			/* ANALOGスティックが有効な場合スティック情報を取得 */
			#if ANALOG_ENABLE != 0
			Stick = gen_Analog(PS2_Data, PS2_offset);
			#endif
			/* データを処理し、ドライバ出力を決定 */
			Data_Proc(level);
			Set_pwr(level, pwr);
		}
		else
		{
			/* PSコントローラから信号を受信できなかった時の処理 */
			if(rcv_err >= RCV_THRESHOLD)
			{
				/* 再試行回数が閾値を超えた場合通信断絶と判断する */
				rcv = 0;
			}
			else
			{
				/* 再試行回数のカウント */
				rcv_err++;
			}
		}
		/* LED点灯制御 */
		led_reflesh(f[0], EMITRULE_LED_F1, EMITRULE_LED_F2, EMITRULE_LED_F3);

		/* モータードライバの出力内容を決定、信号を出力する */
		motor_senddata(pwr, motor_buf, f[1]);

		/* ループ毎のディレイ */
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
		XMIT=6,RCV=7 */
	set_tris_c(0x80);
	/* オシレータの初期化 */
	setup_oscillator(OSC_8MHZ);
}

void motor_emit(char channnel, int power)
{
	led_on(LED_F3);
	printf("%c%U*\r", channnel, power);	/* データ送信 */
	led_off(LED_F3);
	delay_us(TIME_MOTOR_MARGIN);		/* データ送信後の待ち時間 */
}

void motor_reset(void)
{
	/* 全てのドライバに出力0を送信 */
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
	PS2_offset = 0;
	/* データを受信(データが来ない場合ここでプログラムが止まります) */
	gets(PS2_Data);
	for (i = 0; i < PS2_DATA_BUFFER_SIZE - 1; i++)
	{
		/* PSコントローラの識別子0x5a('Z')を捜索、オフセットを決定 */
		if (PS2_Data[i] == 'Z')
		{
			PS2_offset = i - 1;
			break;
		}
	}
}

void Data_Proc(signed int* stat)
/* ロボット1(釣り竿展開用ロボ) */
#if (Program == 1)
{
	/* R1キーが押されている場合に処理を実行 */
	if (PS2_PUSH_R1)
	{
		/* 前後移動 */
		stat[0] = (PS2_PUSH_DN ? (PS2_PUSH_L1 ? 2 : 1) : 0) - (PS2_PUSH_UP ? (PS2_PUSH_L1 ? 2 : 1) : 0);
		stat[1] = (PS2_PUSH_DN ? (PS2_PUSH_L1 ? 2 : 1) : 0) - (PS2_PUSH_UP ? (PS2_PUSH_L1 ? 2 : 1) : 0);
		/* 左右旋回 */
		if (PS2_PUSH_LE)
		{
			stat[0] += (PS2_PUSH_L1 ? 2 : 1);
			stat[1] -= (PS2_PUSH_L1 ? 2 : 1);
		}
		if (PS2_PUSH_RI)
		{
			stat[0] -= (PS2_PUSH_L1 ? 2 : 1);
			stat[1] += (PS2_PUSH_L1 ? 2 : 1);
		}
		/* 配列のアウトレンジ回避 */
		stat[0] = stat[0] > 2 ? 2 : stat[0];
		stat[1] = stat[1] > 2 ? 2 : stat[1];
		stat[0] = stat[0] < -2 ? -2 : stat[0];
		stat[1] = stat[1] < -2 ? -2 : stat[1];
	}
}
/* ロボット2(風船割ロボ) */
#elif (Program == 2)
{
	if (PS2_PUSH_R1)
	{
		/* 前後移動 */
		stat[0] = (PS2_STICK_LUP(Stick) ? 1 : 0) - (PS2_STICK_LDN(Stick) ? 1 : 0);
		stat[1] = (PS2_STICK_LUP(Stick) ? 1 : 0) - (PS2_STICK_LDN(Stick) ? 1 : 0);
		stat[2] = (PS2_STICK_LUP(Stick) ? 1 : 0) - (PS2_STICK_LDN(Stick) ? 1 : 0);
		stat[3] = (PS2_STICK_LUP(Stick) ? 1 : 0) - (PS2_STICK_LDN(Stick) ? 1 : 0);
		/* 左右スライド移動 */
		if (PS2_STICK_LRI(Stick))
		{
			stat[0] += 1;
			stat[1] -= 1;
			stat[2] -= 1;
			stat[3] += 1;
		}
		if (PS2_STICK_LLE(Stick))
		{
			stat[0] -= 1;
			stat[1] += 1;
			stat[2] += 1;
			stat[3] -= 1;
		}
		/* 左右旋回 */
		if (PS2_PUSH_LE)
		{
			stat[0] = -1;
			stat[1] = 1;
			stat[2] = -1;
			stat[3] = 1;
		}
		if (PS2_PUSH_RI)
		{
			stat[0] = 1;
			stat[1] = -1;
			stat[2] = 1;
			stat[3] = -1;
		}
		/* 配列のアウトレンジ回避 */
		stat[0] = stat[0]>1 ? 1 : stat[0];
		stat[1] = stat[1]>1 ? 1 : stat[1];
		stat[2] = stat[2]>1 ? 1 : stat[2];
		stat[3] = stat[3]>1 ? 1 : stat[3];
		stat[0] = stat[0]<-1 ? -1 : stat[0];
		stat[1] = stat[1]<-1 ? -1 : stat[1];
		stat[2] = stat[2]<-1 ? -1 : stat[2];
		stat[3] = stat[3]<-1 ? -1 : stat[3];
		/* L1での加速処理 */
		if (PS2_PUSH_L1)
		{
			stat[0] *= 2;
			stat[1] *= 2;
			stat[2] *= 2;
			stat[3] *= 2;
		}
	}
}
#else
{}
#endif

void Set_pwr(signed int* stat, signed int* power)
/* ロボット1(釣り竿展開用ロボ) */
#if (Program == 1)
{
	power[0] = pwt[2 + stat[0]];
	power[1] = pwt[2 + stat[1]];
	power[2] = PS2_PUSH_L2 ? PWR_AIR : 0;
	power[3] = PS2_PUSH_R2 ? PWR_AIR : 0;
	power[4] = (PS2_PUSH_CIR ? PWR_ARM : 0) - (PS2_PUSH_SQU ? PWR_ARM : 0);
}
/* ロボット2(風船割ロボ) */
#elif (Program == 2)
{
	power[0] = pwt[2 + stat[0]];
	power[1] = pwt[2 + stat[1]];
	power[2] = pwt[2 + stat[2]];
	power[3] = pwt[2 + stat[3]];
	power[4] = PS2_PUSH_TRI ? PWR_AIR : 0;
	power[5] = (PS2_PUSH_CIR ? PWR_ARM : 0) - (PS2_PUSH_SQU ? PWR_ARM : 0);
	if (PWR_MOVEA_INVERT) power[0] = -power[0];
	if (PWR_MOVEB_INVERT) power[1] = -power[1];
	if (PWR_MOVEC_INVERT) power[2] = -power[2];
	if (PWR_MOVED_INVERT) power[3] = -power[3];
}
#else
{}
#endif

void motor_senddata(int* power, int* buffer, int index)
/* ロボット1(釣り竿展開用ロボ) */
#if Program == 1
{
	if (rcv && PS2_PUSH_R1)
	{
		if (power[0] != buffer[0] || index == 0)
		{
			motor_emit(MOTOR_MOVEL, power[0]);
			buffer[0] = power[0];
		}
		if (power[0] != buffer[1] || index == 1)
		{
			motor_emit(MOTOR_MOVER, power[1]);
			buffer[1] = power[1];
		}
		if (power[2] != buffer[2] || index == 2)
		{
			motor_emit(MOTOR_AIRa, power[2]);
			buffer[2] = power[2];
		}
		if (power[3] != buffer[3] || index == 3)
		{
			motor_emit(MOTOR_AIRb, power[3]);
			buffer[3] = power[3];
		}
		if (power[4] != buffer[4] || index == 4)
		{
			motor_emit(MOTOR_ARM, power[4]);
			buffer[4] = power[4];
		}
	}
	else
	{
		motor_emit(MOTOR_MOVEL, 0);
		motor_emit(MOTOR_MOVER, 0);
		motor_emit(MOTOR_AIRa, 0);
		motor_emit(MOTOR_AIRb, 0);
		motor_emit(MOTOR_ARM, 0);
		buffer[0] = power[0];
		buffer[1] = power[1];
		buffer[2] = power[2];
		buffer[3] = power[3];
		buffer[4] = power[4];
	}
}
/* ロボット2(風船割ロボ) */
#elif Program == 2
{
	if (rcv && PS2_PUSH_R1)
	{
		if (power[0] != buffer[0] || index == 0)
		{
			motor_emit(MOTOR_MOVEA, power[0]);
			buffer[0] = power[0];
		}
		if (power[1] != buffer[1] || index == 1)
		{
			motor_emit(MOTOR_MOVEB, power[1]);
			buffer[1] = power[1];
		}
		if (power[2] != buffer[2] || index == 2)
		{
			motor_emit(MOTOR_MOVEC, power[2]);
			buffer[2] = power[2];
		}
		if (power[3] != buffer[3] || index == 3)
		{
			motor_emit(MOTOR_MOVED, power[3]);
			buffer[3] = power[3];
		}
		if (power[4] != buffer[4] || index == 4)
		{
			motor_emit(MOTOR_AIR, power[4]);
			buffer[4] = power[4];
		}
		if (power[5] != buffer[5] || index == 5)
		{
			motor_emit(MOTOR_ARM, power[5]);
			buffer[5] = power[5];
		}
	}
	else
	{
		motor_emit(MOTOR_MOVEA, 0);
		motor_emit(MOTOR_MOVEB, 0);
		motor_emit(MOTOR_MOVEC, 0);
		motor_emit(MOTOR_MOVED, 0);
		motor_emit(MOTOR_AIR, 0);
		motor_emit(MOTOR_ARM, 0);
		buffer[0] = power[0];
		buffer[1] = power[1];
		buffer[2] = power[2];
		buffer[3] = power[3];
		buffer[4] = power[4];
		buffer[5] = power[5];
	}
}
#else
{}
#endif

int motor_isemit(int* power)
{
	int i, y = 0;
	for (i = 0; i < MOTOR_NUM; i++)
	{
		y |= power[i];
	}
	return y;
}

void led_reset(void)
{
	led_off(LED_OPR);
	led_off(LED_F1);
	led_off(LED_F2);
	led_off(LED_F3);
}

void led_flash(void)
{
	led_on(LED_OPR);
	int i;
	for(i= 5; i; i--){
		led_on(LED_F1);
		led_on(LED_F2);
		led_on(LED_F3);
		delay_ms(200);
		led_off(LED_F1);
		led_off(LED_F2);
		led_off(LED_F3);
		delay_ms(200);
	}
}

void led_reflesh(int LEDlogic_OPR, int LEDlogic_F1, int LEDlogic_F2, int LEDlogic_F3)
{
	if (LEDlogic_OPR)
		led_on(LED_OPR);
	else
		led_off(LED_OPR);
	if (LEDlogic_F1)
		led_on(LED_F1);
	else
		led_off(LED_F1);
	if (LEDlogic_F2)
		led_on(LED_F2);
	else
		led_off(LED_F2);
	if (LEDlogic_F3)
		led_on(LED_F3);
	else
		led_off(LED_F3);
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
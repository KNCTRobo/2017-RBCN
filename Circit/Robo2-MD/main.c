/* リレー&FETのモータドライバ（ハーフブリッジ？）用プログラム
 * ±100のデータにしたがって速度を100段階制御
 * データの先頭文字（ヘッダ）がLかRかで左右の識別を行う
 * コメント：2011年9月17日　電子工学科5年　佐賀
 * comment modified: 2017.10.15 電子工学科5年 福澤 */

#include "defines.h"					/* PICおよびプログラムの基本設定項目 */
#include <stdlib.h>						/* atoi()を使うためのヘッダ */

/* 使用関数プロトタイプ宣言 */
/*	main()
 *	プログラムのメインエントリポイントです */
 void main();
/*	setup()
 *	プログラムのセットアップセッションです */
 void setup();
/*	rtcc_isr()
 *	タイマ0割り込みセッションです */
 #INT_RTCC
 void rtcc_isr();
/*	t1_isr()
 *	タイマ1割り込みセッションです */
 #INT_TIMER1
 void t1_isr();
/*	data_read()
 *	データ読み出しルーチンです
 *	*無限ループを行うため通常脱出しません* */
 void data_read();
/*	LED_flasher()
 *	LEDを点滅させます */
 void LED_flasher();
/*	l_...()
 *	Lドライバのリレー制御を行います */
 void l_adv();
 void l_back();
 void l_stop();
/*	r_...()
 *	Rドライバのリレー制御を行います */
 void r_adv();
 void r_back();
 void r_stop();
/*	d_stop()
 *	両ドライバを停止するようリレーを制御します */
 void d_stop();

/* 定数 */
/* LED_PIN
 * LEDのピンの位置 */
 #define LED_PIN PIN_A4
#if MOTOR_TYPE == 1
#define MOTOR_HEAD_A 'A'
#define MOTOR_HEAD_B 'B'
#elif MOTOR_TYPE == 2
#define MOTOR_HEAD_A 'C'
#define MOTOR_HEAD_B 'D'
#elif MOTOR_TYPE == 3
#define MOTOR_HEAD_A 'E'
#define MOTOR_HEAD_B 'F'
#elif MOTOR_TYPE == 4
#define MOTOR_HEAD_A 'L'
#define MOTOR_HEAD_B 'R'
#endif

/* グローバル変数 */
/*	uchar cnt
 *	PWMカウント用変数 */
 unsigned char cnt;
/*	uchar duty
 *	設定されているデューティ比 */
 unsigned char duty1, duty2;
/*	char pwm
 *	パワーレベル保持変数 */
 signed char pwml, pwmr;
/*	uint MWT
 *	暴走防止用タイマ
 *	閾値は MWT_THRESHOLD で設定 */
 unsigned int MWT = MWT_THRESHOLD;
/*	uchar* Buff
 *	受信データ処理用バッファ */
 unsigned char Buff[BUFFER_SIZE];
/*	uchar* string
 *	受信データ保持用バッファ */
 unsigned char string[BUFFER_SIZE];

void main()
{
	/* mainが長すぎると割り込みがうまくいかなくなるよ */
	/* スタック変数宣言 */
	int i;
	/* PIC初期化 */
	setup();
	/* 変数等初期化 */
	d_stop();
	cnt = 0;
	pwml = pwmr = 0;
	duty1 = abs(pwml);
	duty2 = abs(pwmr);
	for (i = 0; i < BUFFER_SIZE; i++) string[i] = 0;
	/* LED点滅 */
	LED_flasher();
	/* 処理開始 */
	enable_interrupts(INT_RTCC);					//タイマー0の割り込み許可
	enable_interrupts(INT_TIMER1);
	enable_interrupts(GLOBAL);
	/* 高専ロボコン2017 朱雀モータ使用時に無限ループを脱出問題が発生
	 * 対策のためデータ読み出し(無限ループ)部分をサブルーチン化、二重で無限ループをかけることで回避
	 * @福澤 */
	for (;;)
	{
		data_read();
	}
}

void setup()
{
	/* 入出力ピンの設定
	 * PORTA: 0x00(0b00000000)
	 * PORTB: 0x02(0b00000010) */
	 set_tris_a(0x00);
	 set_tris_b(0x02);
	/* タイマ0の設定
	 * 内部オシレータ20MHz -> 0.2us/クロック
	 * プリスケーラ伸長1:4 -> 0.8us/カウント
	 * タイマ初期値131 -> 124カウント
	 * オーバーフローまで -> 99.2us */
	 setup_timer_0(RTCC_INTERNAL | RTCC_DIV_4);		//割り込み用カウンターの設定詳しくはp155参照
	 set_timer0(131);								//131から255まで（124）カウントする
	/* タイマ1の設定
	 * 使用オシレータ20MHz -> 0.2us/クロック
	 * プリスケーラ伸長1:8 -> 1.6us/カウント
	 * タイマ初期値3036 -> 62500カウント
	 * オーバーフローまで -> 100.0ms */
	 setup_timer_1(T1_INTERNAL | T1_DIV_BY_8);
	 set_timer1(3036);
}

#INT_RTCC
void rtcc_isr()
{
	/* cntを１ずつ増加(プリスケーラでf/4Hzに設定してT=0.8us、さらに
	 * 124カウント周期で割り込みより6.4us * 124 = 0.1msおきにcnt++ */
	/* cnt<dutyならﾎﾟｰﾄB5（FET）をhigh */
	if (cnt == 0)
		output_high(PIN_B5);
	if (cnt == duty1)
		output_low(PIN_B5);
	/* cnt<dutyならﾎﾟｰﾄA0（FET）をhigh */
	if (cnt == 0)
		output_high(PIN_A0);
	if (cnt == duty2)
		output_low(PIN_A0);
	/* cntが100(10msかかる)になったら初期化 */
	cnt++;
	if (cnt == 100)	cnt=0;
	#if LED_PATTERN==1
	if (cnt == 0) output_low(LED_PIN);
	else output_high(LED_PIN);
	#endif
	#if LED_PATTERN==4
	output_high(LED_PIN);
	#endif
	/* 131から255まで（124）カウントする */
	set_timer0(131);
}

#INT_TIMER1
t1_isr()
{
	/* カウンタ再セット */
	set_timer1(3036);
	/* 暴走防止用タイマデクリメント */
	MWT--;
	/* 0になった場合両ドライバにストップ送信 */
	if (MWT == 0)
	{
		d_stop();
		duty1 = duty2 = 0;
		/* メインルーチン再起動 */
		main();
	}
}

void data_read()
{
	int i;
	while(1)
	{
		MWT = MWT_THRESHOLD;
		if (kbhit())
		{	//受信回路から信号があったとき
			#if LED_PATTERN==2
			output_low(LED_PIN);			//処理をしている間LED点灯
			#endif
			#if LED_PATTERN==3
			output_low(LED_PIN);
			#endif
			disable_interrupts(GLOBAL);	//全ての割り込み処理を禁止
			gets(string);
			#if LED_PATTERN==0
			output_low(LED_PIN);			//処理をしている間LED点灯
			#endif
			i = 0;
			while((string[i+1] != '*') && (i < BUFFER_SIZE - 1))
			{	//stringから速度のデータを抽出
				Buff[i] = string[i + 1];
				Buff[i + 1] = '\0';
				i++;
			}
			switch(string[0])
			{
			case MOTOR_HEAD_A:
				pwml = atoi(Buff);	//文字から整数へ変換
				duty1 = abs(pwml);	//絶対値をとり、速度をセット
				if (pwml > 0) l_adv();	//pwmlの正負で回転を制御
				else if (pwml < 0) l_back();
				else l_stop();
				break;
			case MOTOR_HEAD_B:
				pwmr = atoi(Buff);	//文字から整数へ変換
				duty2 = abs(pwmr);	//絶対値をとり、速度をセット
				if (pwmr > 0) r_adv();	//pwmrの正負で回転を制御
				else if (pwmr < 0) r_back();
				else r_stop();
				break;
			default:
				break;
			}
			#if LED_PATTERN==0
			output_high(LED_PIN);			//処理完了につきLED消灯
			#endif
			#if LED_PATTERN==2
			output_high(LED_PIN);			//処理完了につきLED消灯
			#endif
			enable_interrupts(GLOBAL);	//全ての割り込みを許可
		}
		else
		{
			#if 0
			/* データが来ていないときは両方停止 */
			#if LED_PATTERN==0
			output_high(LED_PIN);
			#endif
			pwml = pwmr = 0;
			#endif
			#if LED_PATTERN==3
			output_high(LED_PIN);
			#endif
			#if LED_PATTERN==4
			output_low(LED_PIN);
			#endif
		}
	}
}

void LED_flasher()
{
	int i;
	for(i = 0; i < LED_FLASH_DIV; i++)
	{
		output_low(LED_PIN);
		delay_ms(LED_FLASH_TIME/2/LED_FLASH_DIV);
		output_high(LED_PIN);
		delay_ms(LED_FLASH_TIME/2/LED_FLASH_DIV);
	}
}

void l_adv()	//左前進
{
	output_low(PIN_B6);
	output_low(PIN_B7);
}

void l_back()	//左後退
{
	output_high(PIN_B6);
	output_low(PIN_B7);
}

void r_adv()	//右前進
{
	output_low(PIN_A2);
	output_low(PIN_A1);
}

void r_back()	//右後退
{
	output_high(PIN_A2);
	output_low(PIN_A1);
}

void l_stop()	//左停止
{
	output_high(PIN_B7);
	output_low(PIN_B6);
}

void r_stop()	//右停止
{
	output_high(PIN_A1);
	output_low(PIN_A2);
}

void d_stop()
{
	l_stop();
	r_stop();
}

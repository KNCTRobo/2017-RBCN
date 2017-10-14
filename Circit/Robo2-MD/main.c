//リレー&FETのモータドライバ（ハーフブリッジ？）用プログラム
//±100のデータにしたがって速度を100段階制御
//データの先頭文字（ヘッダ）がLかRかで左右の識別を行う
//コメント：2011年9月17日　電子工学科5年　佐賀

#include <16f648a.h>
#include <stdlib.h>				//atoi()を使うためのヘッダ
#include "defines.h"

void main();
#INT_RTCC
void rtcc_isr();
#INT_TIMER1
void t1_isr();
void data_read();
void LED_flasher();
void l_adv();
void l_back();
void l_stop();
void r_adv();
void r_back();
void r_stop();
void d_stop();

unsigned char cnt,duty1,duty2;
signed char pwml,pwmr;
unsigned int MWT = 10;
unsigned char Buff[BUFFER_SIZE];
unsigned char string[BUFFER_SIZE];

void main()		//mainが長すぎると割り込みがうまくいかなくなるよ
{
	int i;

	set_tris_a(0x00);
	set_tris_b(0x02);
	setup_timer_0(RTCC_INTERNAL | RTCC_DIV_4);		//割り込み用カウンターの設定詳しくはp155参照
	set_timer0(131);								//131から255まで（124）カウントする
	/*	タイマ1の設定
		使用オシレータ20MHz -> 0.2us/クロック
		プリスケーラ伸長1:8 -> 1.6us/カウント
		タイマ初期値3036 -> 62500カウント
		オーバーフローまで -> 100.0ms */
	 setup_timer_1(T1_INTERNAL | T1_DIV_BY_1);
	 set_timer1(3036);

	d_stop();
	cnt = 0;
	pwml = pwmr = 0;
	duty1 = abs(pwml);
	duty2 = abs(pwmr);
	for (i = 0; i < BUFFER_SIZE; i++) string[i] = 0;
	LED_flasher();

	enable_interrupts(INT_RTCC);					//タイマー0の割り込み許可
	enable_interrupts(INT_TIMER1);
	enable_interrupts(GLOBAL);

	for (;;)
	{
		data_read();
	}
}

#INT_RTCC
void rtcc_isr()		//cntを１ずつ増加(プリスケーラでf/4Hzに設定してT=0.8us、さらに
{				//124カウント周期で割り込みより6.4us * 124 = 0.1msおきにcnt++
	if (cnt == 0)
		output_high(PIN_B5);	//cnt<dutyならﾎﾟｰﾄB5（FET）をhigh
	if (cnt == duty1)
		output_low(PIN_B5);

	if (cnt == 0)
		output_high(PIN_A0);	//cnt<dutyならﾎﾟｰﾄA0（FET）をhigh
	if (cnt == duty2)
		output_low(PIN_A0);

	cnt++;
	if (cnt == 100)	cnt=0;		//cntが100(10msかかる)になったら初期化
	#if LED_PATTERN==2
	if (cnt == 0) output_low(LED_PIN);
	else output_high(LED_PIN);
	#endif
	#if LED_PATTERN==5
	output_high(LED_PIN);
	#endif

	set_timer0(131);			//131から255まで（124）カウントする
}

#INT_TIMER1
t1_isr()
{
	set_timer1(3036);
	MWT--;
	if (MWT == 0)
	{
		d_stop();
		duty1 = duty2 = 0;
	}
}

void data_read()
{
	int i;
	while(1)
	{
		MWT = 10;
		if (kbhit())
		{	//受信回路から信号があったとき
			#if LED_PATTERN==3
			output_low(LED_PIN);			//処理をしている間LED点灯
			#endif
			#if LED_PATTERN==4
			output_low(LED_PIN);
			#endif
			/* バッファクリア */
			#if 0
			for (i = 0; i < BUFFER_SIZE; i++) string[i] = 0;
			#endif
			disable_interrupts(GLOBAL);	//全ての割り込み処理を禁止
			gets(string);
			#if LED_PATTERN==1
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
			#if LED_PATTERN==1
			output_high(LED_PIN);			//処理完了につきLED消灯
			#endif
			#if LED_PATTERN==3
			output_high(LED_PIN);			//処理完了につきLED消灯
			#endif
			enable_interrupts(GLOBAL);	//全ての割り込みを許可
		}
		else
		{						//データが来ていないときは両方停止
		#if 0
			#if LED_PATTERN==1
			output_high(LED_PIN);
			#endif
			pwml = pwmr = 0;
		#endif
			#if LED_PATTERN==4
			output_high(LED_PIN);
			#endif
			#if LED_PATTERN==5
			output_low(LED_PIN);
			#endif
		}
	}
}

void LED_flasher()
{
	int i;
	for(i = 0; i < LED_FLASH_DIV; i++)
	{		//LED_flasher
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

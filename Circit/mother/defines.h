/*	defines.h
 *	2017年度高専ロボコン 釧路高専Aチーム 定義領域
 */
#ifndef _predefine_
#define _predefine_

/*	Program
 *	コンパイル制御による制御プログラムの選択
 */
#define Program

/*	MOTOR_MOVER
 */
#define MOTOR_MOVER	'R'
/*	MOTOR_MOVEL
 */
#define MOTOR_MOVEL	'L'
/*	MOTOR_AIRa
 */
#define MOTOR_AIRa	'A'
/*	MOTOR_AIRb
 */
#define MOTOR_AIRb	'B'
/*	MOTOR_AIRc
 */
#define MOTOR_AIRc	'C'

/*	LOOP_DELAY
 *	プログラムループの待ち時間
 */
#define LOOP_DELAY 1000
/*	TIME_MOTOR_MARGIN
 *	モータードライバに信号を送信した直後の待ち時間
 */
#define TIME_MOTOR_MARGIN 6100
/*	F_TIME
 *	LED試験点灯のLED点灯時間
 */
#define F_TIME 200

/*	PWR_MOV
 *	ロボット1用移動に用いるパワー
 */
#define PWR_MOV		100
/*	PWR_MOV_H
 *	ロボット2用移動に用いるパワー
 */
#define PWR_MOV_H	100
/*	PWR_MOV_L
 *	ロボット2用移動に用いるパワー
 */
#define PWR_MOV_L	 30
/*	PWR_AIR
 *	エアシリンダ駆動に用いるパワー
 */
#define PWR_AIR 100

/*	RCV_THRESHOLD
 *	PS2の信号が途絶えた際に通信断絶状態と判定するための閾値
 */
#define RCV_THRESHOLD 3

/*	EMITRULE_LED_F1
 *	LED1の点灯条件
 */
#define EMITRULE_LED_F1 (((rcv) && (!PS2_PUSH_R1)) || ())
/*	LED_PULLUP
 *	LED1のプルアップ使用フラグ
 */
#define LED_PULLUP 1
/*	LED_OPR
 *	オペレーションLEDのピン設定
 */
#define LED_OPR	PIN_A0
/*	LED_F1
 *	LED1のピン設定
 */
#define LED_F1	PIN_A2

#endif
/* defines.h EOF */
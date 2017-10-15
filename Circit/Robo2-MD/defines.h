/* defines.h
 * プログラムの設定項目
 * comment: 2017.10.15 電子工学科5年 福澤 */

#include <16f648a.h>					/* PIC16F648Aを使用するためのヘッダ */
#fuses HS,NOWDT,NOPROTECT,PUT,NOMCLR,NOLVP,BROWNOUT
#use delay(CLOCK=20000000)
#use RS232(BAUD=9600,RCV=PIN_B1,stream=com0)
#byte port_a=5
#byte port_b=6
#use fast_io(a)
#use fast_io(b)

/* LED_FLASH_TIME
 * LED点滅時間 */
 #define LED_FLASH_TIME 500
/* LED_FLASH_DIV
 * LED点滅回数 */
 #define LED_FLASH_DIV 3
/* LED_PATTERN
 * LED点灯パターン
 * 指定できる値は0...4です
 * 通常は0を指定してください */
 #define LED_PATTERN 1
/* BUFFER_SIZE
 * データ受信時のバッファサイズ
 * *不用意に変更しないでください* */
 #define BUFFER_SIZE 20
/* MOTOR_TYPE
 * 受け付けるモータードライバ制御信号を選択します
 * 0: オリジナル(下部で定義可能)
 * 1: A/B
 * 2: C/D
 * 3: E/F
 * 4: L/R */
 #define MOTOR_TYPE 4
/* MOTOR_HEAD
 * モータードライバ制御信号のオリジナル設定
 * この項目を使用する際は MOTOR_TYPE を0にしてください */
 #if MOTOR_TYPE==0
 #define MOTOR_HEAD_A ''
 #define MOTOR_HEAD_B ''
 #endif
/* MWT_THRESHOLD
 * 暴走防止用タイマの閾値設定
 * ここで設定した値*100msの時間メインルーチンの動作を確認できなかった場合再起動を行います */
 #define MWT_THRESHOLD 10

/* defines.h EOF */
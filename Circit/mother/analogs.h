/*	analogs.h
 */
#ifndef _ANALOGS_DEFINED_
#define _ANALOGS_DEFINED_

#ifndef ANALOG_THRESHOLD
#define ANALOG_THRESHOLD 80
#endif

typedef struct {
	unsigned int X, Y;
} Point;
/*	Analogs
	Analogs の中身
		Analogs.sticks
		->	傾きの方向フラグ
			0x01 右スティック左
			0x02 右スティック右
			0x04 右スティック上
			0x08 右スティック下
			0x10 左スティック左
			0x20 左スティック右
			0x40 左スティック上
			0x80 左スティック下
		Analogs.depth_L
		->	左スティック傾き深度
		Analogs.depth_R
		->	右スティック傾き深度
 */
typedef struct {
	unsigned char sticks;
	Point depth_L;
	Point depth_R;
} Analogs;

Analogs gen_Analog(unsigned char source[],int offset);

#endif
/* analogs.h EOF */
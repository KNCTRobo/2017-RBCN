/*	analogs.h
 */
#ifndef _ANALOGS_DEFINED_
#define _ANALOGS_DEFINED_
#if ANALOG_ENABLE != 0

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

#define PS2_STICK_RLE(data) ((data.sticks & 0x01) != 0)
#define PS2_STICK_RRI(data) ((data.sticks & 0x02) != 0)
#define PS2_STICK_RUP(data) ((data.sticks & 0x04) != 0)
#define PS2_STICK_RDN(data) ((data.sticks & 0x08) != 0)
#define PS2_STICK_LLE(data) ((data.sticks & 0x10) != 0)
#define PS2_STICK_LRI(data) ((data.sticks & 0x20) != 0)
#define PS2_STICK_LUP(data) ((data.sticks & 0x40) != 0)
#define PS2_STICK_LDN(data) ((data.sticks & 0x80) != 0)

#endif
#endif
/* analogs.h EOF */
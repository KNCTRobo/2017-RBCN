/*	analogs.c
 */

#include "includes.h"
#if (ANALOG_ENABLE != 0)

/*	Analogs = gen_Analog(int* source, int offset)
	アナログスティックの計算
	@params source 基となるデータ
	@params offset データのオフセット情報
 */
Analogs gen_Analog(unsigned char source[],int offset)
{
	Analogs data;
	int StickRX;
	int StickRY;
	int StickLX;
	int StickLY;

	StickRX= source[4+offset];
	StickRY= source[5+offset];
	StickLX= source[6+offset];
	StickLY= source[7+offset];

	data.sticks= 0;
	data.depth_R.X= 0;
	data.depth_R.Y= 0;
	data.depth_L.X= 0;
	data.depth_L.Y= 0;

	//--右X-
	if (StickRX < (0x80 - ANALOG_THRESHOLD)) {	//右Stickが左
		data.sticks += 0x01;
		data.depth_R.X = 0x7F - StickRX;
	} else if (StickRX > (0x80 + ANALOG_THRESHOLD)) {	//右Stickが右
		data.sticks += 0x02;
		data.depth_R.X = StickRX - 0x80;
	}
	//--右Y-
	if (StickRY < (0x80 - ANALOG_THRESHOLD)) {	//右Stickが上
		data.sticks += 0x04;
		data.depth_R.Y = 0x7F - StickRY;
	} else if (StickRY > (0x80 + ANALOG_THRESHOLD)) {	//右Stickが下
		data.sticks += 0x08;
		data.depth_R.Y = StickRY - 0x80;
	}

	//--左X-
	if (StickLX < (0x80 - ANALOG_THRESHOLD)) {	//左Stickが左
		data.sticks += 0x10;
		data.depth_L.X = 0x7F - StickLX;
	} else if (StickLX > (0x80 + ANALOG_THRESHOLD)) {	//左Stickが右
		data.sticks += 0x20;
		data.depth_L.X = StickLX - 0x80;
	}
	//--左Y-
	if (StickLY < (0x80 - ANALOG_THRESHOLD)) {	//左Stickが上
		data.sticks += 0x40;
		data.depth_L.Y = 0x7F - StickLY;
	} else if (StickLY > (0x80 + ANALOG_THRESHOLD)) {	//左Stickが下
		data.sticks += 0x80;
		data.depth_L.Y = StickLY - 0x80;
	}
	return data;
}

#endif

/* analogs.c EOF */
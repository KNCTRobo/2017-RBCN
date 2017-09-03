/*	analogs.c
 */

#include "includes.h"
#if (ANALOG_ENABLE != 0)

/*	Analogs = gen_Analog(int* source, int offset)
	�A�i���O�X�e�B�b�N�̌v�Z
	@params source ��ƂȂ�f�[�^
	@params offset �f�[�^�̃I�t�Z�b�g���
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

	//--�EX-
	if (StickRX < (0x80 - ANALOG_THRESHOLD)) {	//�EStick����
		data.sticks += 0x01;
		data.depth_R.X = 0x7F - StickRX;
	} else if (StickRX > (0x80 + ANALOG_THRESHOLD)) {	//�EStick���E
		data.sticks += 0x02;
		data.depth_R.X = StickRX - 0x80;
	}
	//--�EY-
	if (StickRY < (0x80 - ANALOG_THRESHOLD)) {	//�EStick����
		data.sticks += 0x04;
		data.depth_R.Y = 0x7F - StickRY;
	} else if (StickRY > (0x80 + ANALOG_THRESHOLD)) {	//�EStick����
		data.sticks += 0x08;
		data.depth_R.Y = StickRY - 0x80;
	}

	//--��X-
	if (StickLX < (0x80 - ANALOG_THRESHOLD)) {	//��Stick����
		data.sticks += 0x10;
		data.depth_L.X = 0x7F - StickLX;
	} else if (StickLX > (0x80 + ANALOG_THRESHOLD)) {	//��Stick���E
		data.sticks += 0x20;
		data.depth_L.X = StickLX - 0x80;
	}
	//--��Y-
	if (StickLY < (0x80 - ANALOG_THRESHOLD)) {	//��Stick����
		data.sticks += 0x40;
		data.depth_L.Y = 0x7F - StickLY;
	} else if (StickLY > (0x80 + ANALOG_THRESHOLD)) {	//��Stick����
		data.sticks += 0x80;
		data.depth_L.Y = StickLY - 0x80;
	}
	return data;
}

#endif

/* analogs.c EOF */
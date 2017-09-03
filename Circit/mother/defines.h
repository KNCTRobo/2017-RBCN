/*	defines.h
 *	2017�N�x���ꃍ�{�R�� ���H����A�`�[�� ��`�̈�
 */
#ifndef _predefine_
#define _predefine_

#include <16F886.h>

#fuses INTRC_IO,NOWDT,NOPROTECT,PUT,NOMCLR,NOLVP,BROWNOUT
#use delay(CLOCK=8000000)
#use fast_io(a)//��ɐ錾����B
#use fast_io(b)
#use fast_io(c)
#use fast_io(e)
/*	RS232(USART)���W���[���g�p
 *	���x			9600Hz
 *	��M�s��		C7
 *	���M�s��		C6
 *	�G���[�L�^	�L��
 */	#use RS232(BAUD=9600,RCV=PIN_C7,XMIT=PIN_C6,ERRORS,FORCE_SW)
#byte port_a = 5
#byte port_b = 6
#byte port_c = 7
#byte port_e = 9
#byte INTCON = 0x0B

/*	Program
 *	�R���p�C������ɂ�鐧��v���O�����̑I��
 */	#define Program	1

#if Program == 0
#define PWR_MOV_TABLE	{0}
#endif
#if Program == 1
#define MOTOR_NUM	5
#define MOTOR_MOVER 'R'
#define MOTOR_MOVEL 'L'
#define MOTOR_AIRa 'A'
#define MOTOR_AIRb 'B'
#define MOTOR_ARM 'C'
/*	PWR_MOV_TABLE
 *	�ړ��ɗp����p���[
 */	#define PWR_MOV_TABLE	{-100, -30, 0, 30, 100}
/*	PWR_AIR
 *	�G�A�V�����_�쓮�ɗp����p���[
 */	#define PWR_AIR	100
/*	PWR_ARM
 *	�A�[���쓮�ɗp����p���[
 */	#define PWR_ARM	100
#endif
#if Program == 2
#define MOTOR_NUM	6
#define MOTOR_MOVER	'R'
#define MOTOR_MOVEL	'L'
#endif

/*	LOOP_DELAY
 *	�v���O�������[�v�̑҂�����
 */	#define LOOP_DELAY	0
/*	LOOP_DELAYUNIT_US
 *	�v���O�������[�v�̑҂����Ԃ̒P�ʂ�us�ɂ��邩�ǂ���
 */	#define LOOP_DELAYUNIT_US	1
/*	TIME_MOTOR_MARGIN
 *	���[�^�[�h���C�o�ɐM���𑗐M��������̑҂�����
 */	#define TIME_MOTOR_MARGIN	6100
/*	F_TIME
 *	LED�����_����LED�_������
 */	#define F_TIME	200

/*	DATA_BUFFER_SIZE
 *	��M�f�[�^�̃o�b�t�@�T�C�Y
 */	#define PS2_DATA_BUFFER_SIZE		  32
/*	RCV_THRESHOLD
 *	PS2�̐M�����r�₦���ۂɒʐM�f���ԂƔ��肷�邽�߂�臒l
 */	#define RCV_THRESHOLD		   3

#define ANALOG_ENABLE	0
#define ANALOG_THRESHOLD	80

/*	LED_PULLUP
 *	LED1�̃v���A�b�v�g�p�t���O
 */	#define LED_PULLUP			   1
/*	LED_OPR
 *	�I�y���[�V����LED�̃s���ݒ�
 */	#define LED_OPR				PIN_A0
/*	LED_F1
 *	LED1�̃s���ݒ�
 */	#define LED_F1				PIN_A2
/*	EMITRULE_LED_F1
 *	LED1�̓_������
 */	#define EMITRULE_LED_F1		((rcv && !PS2_PUSH_R1) || motor_buf)

#endif
/* defines.h EOF */
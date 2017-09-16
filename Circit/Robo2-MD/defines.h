

#use delay(CLOCK=20000000)
#use RS232(BAUD=9600,RCV=PIN_B1)
#byte port_a=5
#byte port_b=6
#fuses HS,NOWDT,NOPROTECT,PUT,NOMCLR,NOLVP,BROWNOUT
#use fast_io(a)
#use fast_io(b)
#define LED PIN_A4

#define LED_FLASH_TIME 500
#define LED_FLASH_DIV 3

#define MOTOR_TYPE 1

#if MOTOR_TYPE == 1
#define MOTOR_HEAD_A 'A'
#define MOTOR_HEAD_B 'B'
#elif MOTOR_TYPE == 2
#define MOTOR_HEAD_A 'C'
#define MOTOR_HEAD_B 'D'
#elif MOTOR_TYPE == 3
#define MOTOR_HEAD_A 'E'
#define MOTOR_HEAD_B 'F'
#endif


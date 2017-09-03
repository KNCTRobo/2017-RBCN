/*	ps2_keys.h
 */
#ifndef _PS2_KEYS_INCLUDED_
#define _PS2_KEYS_INCLUDED_

#define DIGITAL_MODE 0x41
#define ANALOG_MODE 0x73
#define PS2_MODE_DIGI	(PS2_Data[PS2_offset+0] == 0x41)
#define PS2_MODE_ANLG	(PS2_Data[PS2_offset+0] == 0x53)
#define PS2_PUSH_LE		((PS2_Data[PS2_offset+2] & 0x80) != 0)
#define PS2_PUSH_DN		((PS2_Data[PS2_offset+2] & 0x40) != 0)
#define PS2_PUSH_RI		((PS2_Data[PS2_offset+2] & 0x20) != 0)
#define PS2_PUSH_UP		((PS2_Data[PS2_offset+2] & 0x10) != 0)
#define PS2_PUSH_START	((PS2_Data[PS2_offset+2] & 0x08) != 0)
#define PS2_PUSH_R3		((PS2_Data[PS2_offset+2] & 0x04) != 0)
#define PS2_PUSH_L3		((PS2_Data[PS2_offset+2] & 0x02) != 0)
#define PS2_PUSH_SELECT	((PS2_Data[PS2_offset+2] & 0x01) != 0)
#define PS2_PUSH_SQU	((PS2_Data[PS2_offset+3] & 0x80) != 0)
#define PS2_PUSH_CRO	((PS2_Data[PS2_offset+3] & 0x40) != 0)
#define PS2_PUSH_CIR	((PS2_Data[PS2_offset+3] & 0x20) != 0)
#define PS2_PUSH_TRI	((PS2_Data[PS2_offset+3] & 0x10) != 0)
#define PS2_PUSH_R1		((PS2_Data[PS2_offset+3] & 0x08) != 0)
#define PS2_PUSH_L1		((PS2_Data[PS2_offset+3] & 0x04) != 0)
#define PS2_PUSH_R2		((PS2_Data[PS2_offset+3] & 0x02) != 0)
#define PS2_PUSH_L2		((PS2_Data[PS2_offset+3] & 0x01) != 0)

#endif
/* ps2_keys.h EOF */
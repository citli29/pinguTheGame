#ifndef __i8042_H
#define __i8042_H
#include <lcom/lcf.h>
/** @defgroup i8042 i8042
 * @{
 *
 * Functions for using the i8042
 */

/**
 * @brief  reads or write after introducing a command to the kbc
 * @param command the command to to write to the kbc
 * @param arg the argument that will be written or will be filled wit the information
 * @param write if the command is to write or read
 * @return 0 if success
 */
int(kbc_read_write)( uint8_t command,uint8_t *arg,bool write);
/**
 * @brief 
 * @param status the byte that will be filled with status register byte
 * @return 0 if success
 */
int (read_status_register)(uint8_t *status);
/**
 * @brief 
 * @return true if kbc is available
 */
bool(kbc_available)();

#define DELAY_US    10
#define MAX_TRIES 5

#define KEYBOARD_IRQ1 1
#define IN_BUF 0x60
#define OUT_BUF 0x60
#define STAT_REG 0x64
#define KBC 0x64
#define SR_PARITY BIT(7)
#define SR_TIMEOUT BIT(6)
#define SR_IBF BIT(1)
#define SR_OBF BIT(0)
#define SR_AUX BIT(5)
#define READ_CMD 0x20
#define WRITE_CMD 0x60
#define WRITE_MOUSE_CMD 0xD4
#define CHECK_KBC 0xAA
#define CHECK_KBC_OK 0x55
#define CHECK_KBC_ERR 0xFC
#define CHECK_KBDI 0xAB
#define KBC_CMD_DIS_M BIT(5)
#define KBC_CMD_DIS_K BIT(4)
#define KBC_CMD_INT_M BIT(1)
#define KBC_CMD_INT_K BIT(0)
#define SC_TWO_BYTES 0xE0
#define KBC_CB_INT BIT(0)
#define KBC_CB_INT2 BIT(1)
#define KBC_CB_DIS BIT(4)
#define KBC_CB_DIS2 BIT(5)
////////
#define MOUSE_IRQ12 12
//minix initial mouse configuration
//stream mode
#define KBC_F_SET_STREAM_MODE 0xEA
//data reporting desable
#define KBC_F_DISABLE_DR 0xF5
//what we want our program to do
//enabling data report
#define KBC_F_ENABLE_DR 0xF4
//setting remote mode
#define KBC_F_SET_REMOTE_MODE 0xF0
#define KBC_F_READ_DATA 0xEB
////////
#define M_RESPONSE_ACK 0xFA
#define M_RESPONSE_NACK 0xFE
#define M_RESPONSE_ERROR 0xFC

#define ESC_BREAK 0x81
#define W_MAKE 0x11
#define W_BREAK 0x91
#define A_MAKE 0x1e
#define A_BREAK 0x9e
#define S_MAKE 0x1f
#define S_BREAK 0x9f
#define D_MAKE 0x20
#define D_BREAK 0xa0
#endif

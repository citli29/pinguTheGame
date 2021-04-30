#ifndef __MOUSE_H
#define __MOUSE_H

#include <lcom/lcf.h>
#include "i8042.h"
/** @defgroup mouse mouse
 * @{
 *
 * Functions for using the mouse
 */
/**
 * @brief reads the out buf, checks and assign it to a packet 
 */
 void (mouse_ih)();
/**
 * @brief subscribe to mouse interrutps
 * @param bit_no byte that will be filled with the hook id 
 * @return 0 if success
 */int (mouse_subscribe_int)(uint8_t *bit_no);
/**
 * @brief unsubscribe to mouse interrupts
 * @return 0 if success
 */int (mouse_unsubscribe_int)();
/**
 * @brief write to mouse
 * @param command the command to write to the mouse
 * @param par if the command requires a parameter
 * @param par_value the value of the parameter
 * @return 0 if success
 */
 int (mouse_write_mouse_command_kbc)(uint8_t command, bool par, uint8_t par_value);
/**
 * @brief write a byte to the mouse
 * @param byte the byte to write to the mouse
 * @return 0 if success
 */
int (mouse_write_kbc)(uint8_t byte);
/**
 * @brief according to the byte return an int
 * @param byte 
 * @return 3 if not valid, 0 if ack, 1 if nack, 2 if error
 */
 int(mouse_acknowledgment)(uint8_t byte);
/**
 * @brief assigns the correct values to the struct packet according to the packets received
 * @param packet struct 
 */
void (mouse_assign_packet_struct)(struct packet *packetStruct);
/**
 * @brief assigns the byte to the correct position of the packet[]
 * @param byte
 */
void(mouse_assign_byte_packet)(uint8_t byte);

#endif


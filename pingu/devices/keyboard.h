#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include <lcom/lcf.h>
/** @defgroup keyboard keyboard
 * @{
 *
 * Functions for using the keyboard
 */
/**
 * @brief subscribes to keyboard interrupts
 * @param bit_no filled with the information about the number of the hook id
 * @return 0 if success
 */
int (keyboard_subscribe_int)(uint8_t *bit_no) ;
/**
 * @brief unsubscribe to the keyboard interrupts
 * @return 0 if success
 */
int (keyboard_unsubscribe_int)() ;
/**
 * @brief reads the byte from outbuf, checks and assign it to a scan code
 */
void (kbc_ih)();
/**
 * @brief assign the byte received to a scan code 
 */
 void(keyboard_assign_scancodes)(uint8_t out_buf);
/**
 * @brief enables interrupts from the keyboard
 * @return 0 if success
 */int (keyboard_enable_interrupts)();

#endif

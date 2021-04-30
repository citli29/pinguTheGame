#include <lcom/lcf.h>
#include <minix/sysutil.h>

#include "keyboard.h"
#include "i8042.h"

bool make;
uint8_t size;
uint8_t bytes[2];
uint32_t cnt;

int hook_id_keyboard = 1;

bool doubleByte;
bool validScancode;

void (kbc_ih)(){
  uint8_t status;
  uint8_t out_buf;
  //read the status from the register
  read_status_register(&status);
  //read a byte from out_buf
  util_sys_inb(OUT_BUF,&out_buf);
  //check if there was some error    
  if(!(status&(SR_PARITY | SR_TIMEOUT))){
    //if not, assign the out buf to the variables
    keyboard_assign_scancodes(out_buf);
  }
}

void(keyboard_assign_scancodes)(uint8_t out_buf){
  //if there as before the TWO BYTE byte
    if(doubleByte){
      bytes[1] = out_buf;
      validScancode = true;
      
    }//if the byte read is TWO BYTE byte
    else if(out_buf==SC_TWO_BYTES){
      bytes[0] = out_buf;
      doubleByte = true;
      validScancode = false;
    }//if its a regular 1 byte long 
    else{
      bytes[0] = out_buf;
      validScancode = true;
    }
    //if the scancode is valid
    if(validScancode){
      make = ! (bytes[doubleByte?1:0] & BIT(7));
      size = doubleByte?2:1;
      doubleByte = false;
    }
}

int (keyboard_subscribe_int)(uint8_t *bit_no) {
  *bit_no = (uint8_t)hook_id_keyboard;
  if(sys_irqsetpolicy(KEYBOARD_IRQ1,IRQ_REENABLE | IRQ_EXCLUSIVE,&hook_id_keyboard)!=OK)
    printf("Subscribed failed\n");
  return 0;
}

int (keyboard_unsubscribe_int)() {
  uint8_t out_buf;
  if(sys_irqrmpolicy(&hook_id_keyboard)!=OK)
    printf("Unsubscribed failed\n");
  for(uint16_t i = 0; i< 300; i++){
    util_sys_inb(OUT_BUF,&out_buf);
  }
  return 0;
}

int (keyboard_enable_interrupts)(){
  uint8_t commandByte;
  if(kbc_read_write(READ_CMD,&commandByte,false)==-1)return -1;
  commandByte |= KBC_CB_INT;
  if(kbc_read_write(WRITE_CMD,&commandByte,true)==-1)return -1;
  return 0;
}


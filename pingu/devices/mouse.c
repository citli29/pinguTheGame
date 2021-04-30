#include <minix/sysutil.h>

#include "mouse.h"
#include "i8042.h"

uint8_t packet[3];
uint8_t byte_counter=0;
int hook_id_mouse = 2;

void (mouse_ih)() {
  uint8_t status;
  uint8_t byte=0;

  //read the status register
  read_status_register(&status);
  //check if obf and aux are set
  if(status & (SR_OBF | SR_AUX))
    //read the value
    util_sys_inb(OUT_BUF,&byte);
  else
    return;
  mouse_assign_byte_packet(byte);
}

void(mouse_assign_byte_packet)(uint8_t byte){
  if(byte_counter == 3) byte_counter = 0;

  //according to the positioning of the byte on the packet
  if(byte_counter == 0){
    //if the bit 3 is set, then its valid
    if((byte & BIT(3))){
      packet[byte_counter] = byte;
      byte_counter++;
    }
  }else{
    packet[byte_counter] = byte;
    byte_counter++;
  }
}

int (mouse_write_mouse_command_kbc)(uint8_t command, bool par, uint8_t par_value){
  uint8_t tries =0;
  while(tries++ <= MAX_TRIES){
    //if the command receives an ack
    if(mouse_write_kbc(command)== 0){
        //if the command has parameters
        if(par){
          //if the parameter receives an ack
          if(mouse_write_kbc(par_value)== 0)
            return 0;
        }else{
          return 0;
        }
      }
  }
  return 1;
}

int (mouse_write_kbc)(uint8_t byte){
  
  uint8_t tries = 0;
  uint8_t out_buf;
  uint8_t mouse_ack = 3;
  
  //waiting ibf beeing empty
  while(tries++ <= MAX_TRIES){
    if(!kbc_available())
      tickdelay(micros_to_ticks(DELAY_US));  
    else
      break;
  }
  
  tries = 0;
  //writing the 0xD4 to the kbc) &&
  sys_outb(KBC,WRITE_MOUSE_CMD);
  //waiting ibf beeing empty
  while(tries++ <= MAX_TRIES){
    if(!kbc_available())
      tickdelay(micros_to_ticks(DELAY_US));  
    else
      break;
  }
  tries = 0;
  //writing the byte
  sys_outb(IN_BUF,byte);
  //getting the ack
  while(tries++ <= MAX_TRIES)
  {
    util_sys_inb(OUT_BUF,&out_buf);
    mouse_ack = mouse_acknowledgment(out_buf);
    //if the byte is not a response, wait and read again
    if(mouse_ack == 3)
      tickdelay(micros_to_ticks(DELAY_US));  
    else
      break;
  }
  return mouse_ack;
}

int(mouse_acknowledgment)(uint8_t byte){
  switch (byte)
  {
  case M_RESPONSE_ACK:
    return 0;
  case M_RESPONSE_NACK:
    return 1;
  case M_RESPONSE_ERROR:
    return 2;
  default:
    return 3;
  }
}

void (mouse_assign_packet_struct)(struct packet *packetStruct){
  (*packetStruct).bytes[0] = packet[0];
  (*packetStruct).bytes[1] = packet[1];
  (*packetStruct).bytes[2] = packet[2];
  (*packetStruct).lb =packet[0] & BIT(0);
  (*packetStruct).rb =packet[0] & BIT(1);
  (*packetStruct).mb =packet[0] & BIT(2);
  (*packetStruct).x_ov = packet[0] & BIT(6);
  (*packetStruct).y_ov = packet[0] & BIT(7);
  (*packetStruct).delta_x = packet[1] - ((packet[0] << 4) & BIT(8));
  (*packetStruct).delta_y = packet[2] - ((packet[0] << 3) & BIT(8));         
}


int (mouse_subscribe_int)(uint8_t *bit_no) {
  *bit_no = (uint8_t)hook_id_mouse;
  if(sys_irqsetpolicy(MOUSE_IRQ12,(IRQ_REENABLE | IRQ_EXCLUSIVE),&hook_id_mouse)!=OK)
    printf("Subscribed failed\n");
  return 0;
}

int (mouse_unsubscribe_int)() {
  uint8_t out_buf;
  if(sys_irqrmpolicy(&hook_id_mouse)!=OK)
    printf("Unsubscribed failed\n");
  
  for(uint16_t i = 0; i< 300; i++){
    util_sys_inb(OUT_BUF,&out_buf);
  }
  
  return 0;
}



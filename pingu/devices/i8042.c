#include "i8042.h"

bool(kbc_available)(){
  uint8_t status;
  //read the status from the register
  read_status_register(&status);
  return !(status & SR_IBF);
}

int(read_status_register)(uint8_t *status){
  return util_sys_inb(STAT_REG,status);
}

int(kbc_read_write)( uint8_t command,uint8_t *arg,bool write){
int tries =0;
//waits untill status register is available, if tries>5 it gives up
while(!kbc_available()){
  tickdelay(micros_to_ticks(DELAY_US));
  if(tries>MAX_TRIES) return -1;
  tries++;
}
if(write){
  if(sys_outb(STAT_REG,command) != OK) return -1;
  //wait to get a response from the stat reg
  tickdelay(micros_to_ticks(DELAY_US));
  //pass the argument
  if(sys_outb(IN_BUF,*arg) !=OK)return -1;
}else{
  if(sys_outb(STAT_REG,command) != OK) return -1;
  //wait to get a response from the stat reg
  tickdelay(micros_to_ticks(DELAY_US));
  //reads the return
  util_sys_inb(OUT_BUF,arg);
}
return 0;
}


#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>
#include <math.h>
#include "i8254.h"
#include "timer.h"

int hook_id = 0;
int timerCounter = 0;

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  
  //check if the timer is valid
  if(timer <0 || timer >2){
  printf("Selected Timer is invalid\n");
  return 1;
  }

  //getting the previous config
  uint8_t previousConfig;
  timer_get_conf(timer,&previousConfig);
  
  //getting the least 4 significant bytes of the config
  uint8_t cmd = previousConfig & 0x0F; 
  
  //selecting the timer
  uint32_t port = 0;;
  switch (timer)
  {
  case 0:
    cmd |= TIMER_SEL0;  
    port = TIMER_0; 
    break;
  case 1:
    cmd |= TIMER_SEL1;   
    port = TIMER_1; 
    break;
  case 2:
    cmd |= TIMER_SEL2;   
    port = TIMER_2; 
    break;
  default:
    return 1;
  }
  
  //defining LSB followed by MSB as counter  initialization
  cmd |= TIMER_LSB_MSB;

  //getting the divisor
  uint32_t divisor = TIMER_FREQ/freq;
  uint32_t divisorInBase=0;
  //checking if the frequency is accourding to the limits
  if((previousConfig & TIMER_BCD && divisor > 9999) || divisor > 65535 ){
    printf("Divisor = %d\n",divisor);
    printf("Invalid frequency!");
    return 1;
  }
  //converting to BCD if necessary
  if(previousConfig & 1){
    uint8_t digits = 0;
    while(divisor/10 !=0){
      divisorInBase += ((divisor%10)*(pow(16,digits++)));
      divisor /=10;
    }
    divisorInBase += ((divisor%10)*(pow(16,digits)));
  }else{
    divisorInBase = divisor;
  }
  divisorInBase = divisor;
  //writing the command to the control register
  if(sys_outb(TIMER_CTRL,cmd) != OK) printf("Outb failed on control register\n");
  
  //writing the frequency to the timer register
  uint8_t lsb=0;
  uint8_t msb=0;
  
  util_get_LSB(divisorInBase,&lsb);
  util_get_MSB(divisorInBase,&msb);

  if(sys_outb(port,lsb) != OK || sys_outb(port,msb) != OK){
    printf("Outb failed on writing the frequency\n");
    return 1;
  }
  return 0;
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  u32_t port=0;
  uint8_t cmd = TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer);
  switch (timer)
  {
  case 0:
    port = TIMER_0;
    break;
  case 1:
    port = TIMER_1;
    break;
  case 2:
    port = TIMER_2;
    break;
  default:
  return 1;
  }

  if(sys_outb(TIMER_CTRL,cmd) != OK) printf("Outb failed\n");
  if(util_sys_inb(port,st) != OK) printf("Util_Inb failed\n");
  return 0;
}

int (timer_display_conf)(uint8_t timer, uint8_t st,
                        enum timer_status_field field) {

  union timer_status_field_val conf;
  switch (field)
  {
  case tsf_all:
    // status byte obtained
    conf.byte = st;
    break;
  case tsf_initial:
    // masking the status to get the counter initialization
    conf.in_mode = st & 0x30;
    conf.in_mode >>=4;
  break;
  case tsf_mode:
    // masking the status to get the counting mode
    conf.count_mode = (st & 0xe);
    conf.count_mode >>=1;
    // masking the dont care bits if necessary
    if(conf.count_mode >5) conf.count_mode &= 0x3;
  break;
  case tsf_base:
    // masking to get the base
    conf.bcd = st & 0x1; 
  break;
  default:
    break;
  }  

  timer_print_config(timer,field,conf);

  return 1;
}

int (timer_subscribe_int)(uint8_t *bit_no) {
  *bit_no = (uint8_t)hook_id;
  if(sys_irqsetpolicy(TIMER0_IRQ,IRQ_REENABLE,&hook_id)!=OK)
    printf("Subscribed failed\n");
  return 0;
}

int (timer_unsubscribe_int)() {
  if(sys_irqrmpolicy(&hook_id)!=OK)
    printf("Unsubscribed failed\n");
  return 0;
}

void(timer_int_handler)() {
  timerCounter++;
}


#include "rtc.h"
#include <minix/sysutil.h>

int hook_id_rtc = 3;

int (rtc_subscribe_int)(uint8_t *bit_no) {
  *bit_no = (uint8_t)hook_id_rtc;
  if(sys_irqsetpolicy(RTC_IRQ8,(IRQ_REENABLE | IRQ_EXCLUSIVE),&hook_id_rtc)!=OK)
    printf("Subscribed failed\n");
  return 0;
}

int (rtc_unsubscribe_int)() {
  if(sys_irqrmpolicy(&hook_id_rtc)!=OK)
    printf("Unsubscribed failed\n");
  return 0;
}

int(rtc_set_periodic_rate)(uint8_t rate_selector){
    uint8_t byte;
    if(sys_outb(RTC_ADDR_REG, RTC_REGISTERA)!= OK){
        printf("rtc_writing register failed");
        return 1;
    }
    if(util_sys_inb(RTC_DATA_REG, &byte)!= OK){
            printf("rtc_reading register failed");
            return 1;
    }
    byte &=0xF0;
    byte |=rate_selector;
    if(sys_outb(RTC_ADDR_REG, RTC_REGISTERA)!= OK){
        printf("rtc_writing register failed");
        return 1;
    }
    if(sys_outb(RTC_DATA_REG, byte)!= OK){
        printf("rtc_writing register failed");
        return 1;
    }
    return 0;
}   

int(rtc_enable_periodic_interrupts)(){
    uint8_t byte;
    if(sys_outb(RTC_ADDR_REG, RTC_REGISTERC)!= OK){
        printf("rtc_writing register failed");
        return 1;
    }
    if(util_sys_inb(RTC_DATA_REG, &byte)!= OK){
            printf("rtc_reading register failed");
            return 1;
    }
    
    if(sys_outb(RTC_ADDR_REG, RTC_REGISTERB)!= OK){
        printf("rtc_writing register failed");
        return 1;
    }
    if(util_sys_inb(RTC_DATA_REG, &byte)!= OK){
            printf("rtc_reading register failed");
            return 1;
    }
    byte |= BIT(4);
    if(sys_outb(RTC_DATA_REG, byte)!= OK){
        printf("rtc_writing register failed");
        return 1;
    }
    return 0;
}

int(rtc_ih)(){
    uint8_t byte;   
    if(sys_outb(RTC_ADDR_REG, RTC_REGISTERC)!= OK){
        printf("rtc_writing register failed");
        return 1;
    }
    if(util_sys_inb(RTC_DATA_REG, &byte)!= OK){
            printf("rtc_reading register failed");
            return 1;
    }
    return 0;
}

int(rtc_disable_periodic_interrupts)(){
    uint8_t byte;   
    if(sys_outb(RTC_ADDR_REG, RTC_REGISTERB)!= OK){
        printf("rtc_writing register failed");
        return 1;
    }
    if(util_sys_inb(RTC_DATA_REG, &byte)!= OK){
            printf("rtc_reading register failed");
            return 1;
    }
    byte &= 0xEF;
    if(sys_outb(RTC_DATA_REG, byte)!= OK){
        printf("rtc_writing register failed");
        return 1;
    }
    return 0;
}


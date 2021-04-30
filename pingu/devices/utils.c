#include <lcom/lcf.h>
#include <stdint.h>
#include <stdio.h>

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  val &= 0xFF;
  *lsb = (uint8_t)val;
  return 1;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  val &= 0xFF00;
  val >>=8;
  *msb = (uint8_t)val;
  return 1;
}

int (util_sys_inb)(int port, uint8_t *value) {
    uint32_t byte=0;
    int success = sys_inb(port,&byte);
    *value = (uint8_t)byte;
  return success;
}

int (abs)(int number){
  if(number<0){
    return -number;
  }
  return number;
}


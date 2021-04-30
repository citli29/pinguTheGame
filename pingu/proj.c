// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>
#include <lcom/liblm.h>
#include <lcom/proj.h>

#include <stdbool.h>
#include <stdint.h>

// Any header files included below this line should have been created by you


#include "game.h"

extern uint8_t bit_no_timer;
extern uint8_t bit_no_keyboard;
extern uint8_t bit_no_mouse;
extern uint8_t bit_no_rtc;
extern enum game_state current_state;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  //lcf_trace_calls("/home/lcom/labs/proj/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  //lcf_log_output("/home/lcom/labs/proj/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}


int(proj_main_loop)(int argc, char *argv[]) {

  if(game_set_initial_config()!=0){
    printf("game set intial config failed\n");
    return 1;
  }
  //game loop
  int ipc_status;
  message msg;
  int r;

  while(current_state != exiting){
  
    //getting the device driver message
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
      printf("driver_receive failed with: %d\n",r);
      continue;
    }
    //checking the source of the message
    if(is_ipc_notify(ipc_status)){
      switch (_ENDPOINT_P(msg.m_source))
      {
      case HARDWARE:
        //checking if the interrupt message is the subscribed
        if(msg.m_notify.interrupts & BIT(bit_no_timer)){
          game_timer_handler();
        }
        if(msg.m_notify.interrupts & BIT(bit_no_keyboard)){
          game_keyboard_handler();
        }
        if(msg.m_notify.interrupts & BIT(bit_no_mouse)){
          game_mouse_handler();
        }
        if(msg.m_notify.interrupts & BIT(bit_no_rtc)){
          game_rtc_handler();
        }
        break;
      default:
        break;
      }
    }
  }
if(game_reset_config()!=0){
  printf("game reset config failed\n");
  return 1;
}
printf("exited\n");
  return 0;
}



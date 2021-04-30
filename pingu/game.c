#include "game.h"
#include "devices/timer.h"
#include "devices/keyboard.h"
#include "devices/mouse.h"
#include "devices/video.h"
#include "devices/rtc.h"
#include "pix.h"
#include <math.h>

//states: menu, in game,exiting

enum game_state current_state = menu;
uint8_t bit_no_timer;
uint8_t bit_no_keyboard;
uint8_t bit_no_mouse;
uint8_t bit_no_rtc;

//timer
extern int timerCounter;
//keyboard
extern bool validScancode;
extern uint8_t bytes[2];
extern uint8_t size;
//mouse
extern uint8_t byte_counter;
struct packet packetStruct;
//video
extern vbe_mode_info_t vmi;
extern unsigned char *frame_buffer;
extern int bytesPerPixel;
//game_objects
game_object mouse;
game_object character;
game_object background;
game_object base_enemy;
game_object base_bullet;

animation_object base_blink;
enemyStruct enemies;
bulletStruct bullets;
effectStruct effects;
uint8_t wasd[4];
//hud
game_object leave_button;
game_object singleplayer_button;
game_object how_to_button;
game_object leave_how_to_button;
game_object exit_button;
xpm_image_t numbers[12];
xpm_image_t pointframe;
xpm_image_t game_over_hud;
xpm_image_t wallpaper;
xpm_image_t howTo;


//game info
bool shoot_input = false;
uint8_t enemyCounter=0; 
uint8_t bulletCounter=0;
uint8_t effectCounter=0;
//game play info
uint8_t difficulty =1;
uint8_t enemy_reaction_time = 30;
uint64_t score = 0;
uint64_t ticks_in_game = 0;
uint8_t rtcCounter = 0;
int16_t health = 100;
uint8_t damage = 5;
uint64_t level = 1;
uint16_t charSpeed = CHAR_SPEED;
//boss
bool bossSpawned=false;
int16_t bossMaxHP = 100;
int16_t bossHP = 100;
game_object boss;
enum boss_state boss_state;
game_object hulk_shadow;
xpm_image_t ground_destroyed;

int(game_set_initial_config)(){

  //subscribe to mouse, keyboard, timer
  if(timer_subscribe_int(&bit_no_timer)){
    printf("timer_subs failed\n");
    return 1;
  }
  if(mouse_subscribe_int(&bit_no_mouse)){
    printf("mouse_subs failed\n");
    return 1;
  }  
  if(keyboard_subscribe_int(&bit_no_keyboard)){
    printf("keyboard_subs failed\n");
    return 1;
  }
  if(rtc_subscribe_int(&bit_no_rtc)){
    printf("rtc_subs failed\n");
    return 1;
  }
  //set time frequency
  timer_set_frequency(0,60);

  //set the graphics mode
  if( video_get_mode_info(GRAPHIC_MODE) != OK)
  {
      printf("vbe_get_mode_info(): failed \n");
      return 1;
  }

  if(video_allow_memory_mapping() != OK){
    printf("allow_memory_mapping failed\n");
    return 1;
  }


  if(video_set_mode(GRAPHIC_MODE) != OK) {
  printf("video_set_mode failed\n");
  return 1;
  }
  
  if(game_set_sprite_info() != OK) {
  printf("game_set_sprite_info failed\n");
  return 1;
  }

  return 0;
}

int(game_set_sprite_info)(){
  //numbers
  xpm_load(n0_map,XPM_8_8_8,&numbers[0]);
  xpm_load(n1_map,XPM_8_8_8,&numbers[1]);
  xpm_load(n2_map,XPM_8_8_8,&numbers[2]);
  xpm_load(n3_map,XPM_8_8_8,&numbers[3]);
  xpm_load(n4_map,XPM_8_8_8,&numbers[4]);
  xpm_load(n5_map,XPM_8_8_8,&numbers[5]);
  xpm_load(n6_map,XPM_8_8_8,&numbers[6]);
  xpm_load(n7_map,XPM_8_8_8,&numbers[7]);
  xpm_load(n8_map,XPM_8_8_8,&numbers[8]);
  xpm_load(n9_map,XPM_8_8_8,&numbers[9]);
  xpm_load(tp_map,XPM_8_8_8,&numbers[10]);
  xpm_load(l_map,XPM_8_8_8,&numbers[11]);

  xpm_load(pointframe_map, XPM_8_8_8, &pointframe);
  xpm_load(gameover_map, XPM_8_8_8, &game_over_hud);
  xpm_load(wallpaper_map, XPM_8_8_8, &wallpaper);
  xpm_load(howTo_map, XPM_8_8_8, &howTo);


//singleplayer button
  singleplayer_button.map = xpm_load(singleplayer_map, XPM_8_8_8, &singleplayer_button.img);
  singleplayer_button.pos_x = vmi.XResolution/2 - singleplayer_button.img.width/2 + 200;
  singleplayer_button.has_collider = true;
  singleplayer_button.collider_x = 0;
  singleplayer_button.collider_y =0;
  singleplayer_button.collider_width =  singleplayer_button.img.width;
  singleplayer_button.collider_height =  singleplayer_button.img.height;
  singleplayer_button.has_animation = false;

//how to play button
  how_to_button.map = xpm_load(howToPlay_map, XPM_8_8_8, &how_to_button.img);
  how_to_button.pos_x = vmi.XResolution/2 - how_to_button.img.width/2 + 200;
  how_to_button.has_collider = true;
  how_to_button.collider_x = 0;
  how_to_button.collider_y =0;
  how_to_button.collider_width =  how_to_button.img.width;
  how_to_button.collider_height =  how_to_button.img.height;
  how_to_button.has_animation = false;

//leave how to play button
  leave_how_to_button.map = xpm_load(leave_how_to_map, XPM_8_8_8, &leave_how_to_button.img);
  leave_how_to_button.pos_x = 50;
  leave_how_to_button.has_collider = true;
  leave_how_to_button.collider_x = 0;
  leave_how_to_button.collider_y =0;
  leave_how_to_button.collider_width =  leave_how_to_button.img.width;
  leave_how_to_button.collider_height =  leave_how_to_button.img.height;
  leave_how_to_button.has_animation = false;

//exit button
  exit_button.map = xpm_load(exit_map, XPM_8_8_8, &exit_button.img);
  exit_button.pos_x = vmi.XResolution/2 - exit_button.img.width/2 + 200;
  exit_button.has_collider = true;
  exit_button.collider_x = 0;
  exit_button.collider_y =0;
  exit_button.collider_width =  exit_button.img.width;
  exit_button.collider_height =  exit_button.img.height;
  exit_button.has_animation = false;

//leave button
  leave_button.map = xpm_load(leave_map, XPM_8_8_8, &leave_button.img);
  leave_button.pos_y = 0;
  leave_button.pos_x = vmi.XResolution/2 - leave_button.img.width/2;
  leave_button.has_collider = true;
  leave_button.collider_x = 0;
  leave_button.collider_y =0;
  leave_button.collider_width =  leave_button.img.width;
  leave_button.collider_height =  leave_button.img.height;
  leave_button.has_animation = false;

  //mouse
  mouse.pos_x = vmi.XResolution/2;
  mouse.pos_y = vmi.YResolution/2;
  mouse.map = xpm_load(aim_map, XPM_8_8_8, &mouse.img);
  mouse.has_collider = true;
  mouse.collider_x = mouse.img.width/2;
  mouse.collider_y = mouse.img.height/2;
  mouse.collider_width = 1;
  mouse.collider_height = 1;
  mouse.has_animation = false;

  //background
  background.pos_x =0;
  background.pos_y =0;
  background.map = xpm_load(background_map,  XPM_8_8_8, &background.img);
  background.has_collider = true;
  background.collider_x = 0;
  background.collider_y =0;
  background.collider_width = vmi.XResolution;
  background.collider_height = vmi.YResolution;
  background.has_animation = false;
  
  base_bullet.map = xpm_load(bullet_map, XPM_8_8_8,&base_bullet.img);

  //character
  character.pos_x = vmi.XResolution/2;
  character.pos_y = vmi.YResolution/2;
  character.map = xpm_load(pd1_map, XPM_8_8_8, &character.img);
  character.has_collider = true;
  character.collider_x = CHAR_COL_X;
  character.collider_y = CHAR_COL_Y;
  character.collider_width = CHAR_COL_W;
  character.collider_height = CHAR_COL_H;
  character.has_animation = true;
  character.animation_no = up;

  //penguin down anim
  animation_object pd;
  pd.maps[0] = xpm_load(pd1_map, XPM_8_8_8, &pd.imgs[0]);
  pd.maps[1] = xpm_load(pd2_map, XPM_8_8_8, &pd.imgs[1]);
  pd.no_images = 1;
  pd.animation_speed = ANIM_SPEED;
  pd.previous_frame = pd.no_images;
  pd.frame_interval_counter = pd.animation_speed;

  //penguin up anim
  animation_object pu;
  pu.maps[0] = xpm_load(pu1_map, XPM_8_8_8, &pu.imgs[0]);
  pu.maps[1] = xpm_load(pu2_map, XPM_8_8_8, &pu.imgs[1]);
  pu.no_images = 1;
  pu.animation_speed = ANIM_SPEED;
  pu.previous_frame = pu.no_images;
  pu.frame_interval_counter = pu.animation_speed;

  //penguin left anim
  animation_object pl;
  pl.maps[0] = xpm_load(pl1_map, XPM_8_8_8, &pl.imgs[0]);
  pl.maps[1] = xpm_load(pl2_map, XPM_8_8_8, &pl.imgs[1]);
  pl.no_images = 1;
  pl.animation_speed = ANIM_SPEED;
  pl.previous_frame = pl.no_images;
  pl.frame_interval_counter = pl.animation_speed;

  //penguin right anim
  animation_object pr;
  pr.maps[0] = xpm_load(pr1_map, XPM_8_8_8, &pr.imgs[0]);
  pr.maps[1] = xpm_load(pr2_map, XPM_8_8_8, &pr.imgs[1]);
  pr.no_images = 1;
  pr.animation_speed = ANIM_SPEED;
  pr.previous_frame = pr.no_images;
  pr.frame_interval_counter = pr.animation_speed;

  character.animations[down] = pd;
  character.animations[up] = pu;
  character.animations[left] = pl;
  character.animations[right] = pr;

  //base_enemy
  base_enemy.map = xpm_load(cd1_map,XPM_8_8_8,&base_enemy.img);
  enemies.reaction_time = enemy_reaction_time;

  //chuck down anim
  animation_object cd;
  cd.maps[0] = xpm_load(cd1_map, XPM_8_8_8, &cd.imgs[0]);
  cd.maps[1] = xpm_load(cd2_map, XPM_8_8_8, &cd.imgs[1]);
  cd.no_images = 1;
  cd.animation_speed = ANIM_SPEED;
  cd.previous_frame = cd.no_images;
  cd.frame_interval_counter = cd.animation_speed;

  //chuck up anim
  animation_object cu;
  cu.maps[0] = xpm_load(cu1_map, XPM_8_8_8, &cu.imgs[0]);
  cu.maps[1] = xpm_load(cu2_map, XPM_8_8_8, &cu.imgs[1]);
  cu.no_images = 1;
  cu.animation_speed = ANIM_SPEED;
  cu.previous_frame = cu.no_images;
  cu.frame_interval_counter = cu.animation_speed;

  //chuck left anim
  animation_object cl;
  cl.maps[0] = xpm_load(cl1_map, XPM_8_8_8, &cl.imgs[0]);
  cl.maps[1] = xpm_load(cl2_map, XPM_8_8_8, &cl.imgs[1]);
  cl.no_images = 1;
  cl.animation_speed = ANIM_SPEED;
  cl.previous_frame = cl.no_images;
  cl.frame_interval_counter = cl.animation_speed;

  //chuck right anim
  animation_object cr;
  cr.maps[0] = xpm_load(cr1_map, XPM_8_8_8, &cr.imgs[0]);
  cr.maps[1] = xpm_load(cr2_map, XPM_8_8_8, &cr.imgs[1]);
  cr.no_images = 1;
  cr.animation_speed = ANIM_SPEED;
  cr.previous_frame = cr.no_images;
  cr.frame_interval_counter = cr.animation_speed;

  base_enemy.animations[down] = cd;
  base_enemy.animations[up] = cu;
  base_enemy.animations[left] = cl;
  base_enemy.animations[right] = cr;

  //blink aimation
  base_blink.maps[1] = xpm_load(bb1_map, XPM_8_8_8, &base_blink.imgs[1]);
  base_blink.maps[2] = xpm_load(bb2_map, XPM_8_8_8, &base_blink.imgs[2]);
  base_blink.maps[3] = xpm_load(bb3_map, XPM_8_8_8, &base_blink.imgs[3]);
  base_blink.maps[4] = xpm_load(bb4_map, XPM_8_8_8, &base_blink.imgs[4]);
  base_blink.no_images = 4;
  base_blink.animation_speed = BLINK_ANIM_SPEED;
  base_blink.previous_frame = 0;
  base_blink.frame_interval_counter = base_blink.animation_speed;



  //boss

  xpm_load(ground_destructed_map, XPM_8_8_8, &ground_destroyed);

  boss.map = xpm_load(hulk_punch_map, XPM_8_8_8, &boss.img);
  boss.has_collider = false;
  boss.collider_x = 50;
  boss.collider_y = 50;
  boss.collider_width = boss.img.width-100;
  boss.collider_height = boss.img.height-100;
  boss.has_animation = false;
  boss.animation_no = falling;

  animation_object fall;

  fall.maps[3] = xpm_load(hulk_higher_map, XPM_8_8_8, &fall.imgs[3]);
  fall.maps[4] = xpm_load(hulk_lower_map, XPM_8_8_8, &fall.imgs[4]);
  fall.maps[5] = xpm_load(hulk_punch_map, XPM_8_8_8, &fall.imgs[5]);

  fall.no_images = 5;
  fall.animation_speed = 5;
  fall.previous_frame = 0;
  fall.frame_interval_counter = fall.animation_speed;

  animation_object jump;

  jump.maps[1] = xpm_load(hulk_punch_map, XPM_8_8_8, &jump.imgs[1]);
  jump.maps[2] = xpm_load(hulk_wide_map, XPM_8_8_8, &jump.imgs[2]);
  jump.maps[3] = xpm_load(hulk_lower_map, XPM_8_8_8, &jump.imgs[3]);
  jump.maps[4] = xpm_load(hulk_higher_map, XPM_8_8_8, &jump.imgs[4]);

  jump.no_images = 4;
  jump.animation_speed = 5;
  jump.previous_frame = 0;
  jump.frame_interval_counter = jump.animation_speed;

  boss.animations[falling] = fall;
  boss.animations[jumping] = jump;

  hulk_shadow.animation_no = 0;
  hulk_shadow.has_animation = true;

  //hulk_shadow.animations[0].maps[1] = xpm_load(hulk_shadow1_map, XPM_8_8_8, &hulk_shadow.animations[0].imgs[1]);
  hulk_shadow.animations[0].maps[2] = xpm_load(hulk_shadow2_map, XPM_8_8_8, &hulk_shadow.animations[0].imgs[2]);
  hulk_shadow.animations[0].maps[3] = xpm_load(hulk_shadow3_map, XPM_8_8_8, &hulk_shadow.animations[0].imgs[3]);
  hulk_shadow.animations[0].maps[4] = xpm_load(hulk_shadow4_map, XPM_8_8_8, &hulk_shadow.animations[0].imgs[4]);

  hulk_shadow.animations[0].no_images = 5;
  hulk_shadow.animations[0].animation_speed = boss.animations[falling].animation_speed;
  hulk_shadow.animations[0].previous_frame = 0;
  hulk_shadow.animations[0].frame_interval_counter = hulk_shadow.animations[0].animation_speed;
  return 0;
}

int(game_controller)(){
    switch(current_state){
        case menu:
          game_display_menu();
          game_display_game_object(&mouse);
          video_buffer_to_vg();
        break;
        case in_game:
          //move objects
          game_move_game_objects();
          game_update_boss();
          //level controller
          game_level_controller();
          //add all the pixels to a buffer        
          game_display_level();
          character.animation_no = game_get_direction(character.pos_x,character.pos_y,mouse.pos_x+mouse.collider_x, mouse.pos_y + mouse.collider_y);
          game_display_game_object(&character);
          if(bossSpawned)
            game_display_game_object(&boss);
          game_display_game_object(&mouse);
          //pass the info to the vg
          video_buffer_to_vg();
          if(bytes[0]==ESC_BREAK && validScancode) current_state = exiting;
        break;
        case game_over:
          game_display_level();
          game_display_game_over();
          game_display_game_object(&mouse);
          video_buffer_to_vg();
          break;
        case how_to:
          leave_how_to_button.pos_y = 50;
  
          if(game_has_colided_obj(mouse,leave_how_to_button))
          leave_how_to_button.pos_y -=5;

          video_frame_to_buffer(howTo.bytes,howTo,0,0);
          game_display_game_object(&leave_how_to_button);
          game_display_game_object(&mouse);
          video_buffer_to_vg();
        break; 
        case exiting:
        break;
    }
    return 0;
}

//handlers
int(game_timer_handler)(){
    timer_int_handler();
    ticks_in_game++;
    
    if(timerCounter%(60/FRAME_RATE) == 0){
        timerCounter = 0;
        game_controller();
    }
    
    return 0;
}

int(game_mouse_handler)(){
    mouse_ih(); 

    if(byte_counter ==3) {
      mouse_assign_packet_struct(&packetStruct);
      mouse.pos_x += packetStruct.delta_x;
      mouse.pos_y -= packetStruct.delta_y;
      if(mouse.pos_x > vmi.XResolution-mouse.img.width) mouse.pos_x = vmi.XResolution-mouse.img.width;
      else if(mouse.pos_x < 0) mouse.pos_x = 0;
      if(mouse.pos_y > vmi.YResolution-mouse.img.height) mouse.pos_y = vmi.YResolution-mouse.img.height;
      else if(mouse.pos_y < 0) mouse.pos_y = 0;
      if(packetStruct.lb && !shoot_input) {
        shoot_input = true;
        game_click_handler();
      }
      else if(!packetStruct.lb) shoot_input = false;
    }
    return 0;
}

int(game_keyboard_handler)(){
    kbc_ih();
    if(validScancode && size ==1){
      switch(bytes[0]){
        case W_MAKE:
          wasd[up] = 1;
          break;
        case W_BREAK:
          wasd[up] = 0;
          break;
        case A_MAKE:
          wasd[left] = 1;
          break;
        case A_BREAK:
          wasd[left] = 0;
          break;
        case S_MAKE:
          wasd[down] = 1;
          break;
        case S_BREAK:
          wasd[down] = 0;
          break;
        case D_MAKE:
          wasd[right] = 1;
          break;
        case D_BREAK:
          wasd[right] = 0;
          break;
        default:
          break;
      }
    }
    return 0;
}

int(game_rtc_handler)(){
  rtc_ih();
  rtcCounter++;
  if(enemyCounter <MAX_ENEMIES)
    enemies.to_spawn = true;
  return 0;
}

int(game_click_handler)(){
  switch(current_state){
    case in_game:
      game_shoot();
    break;
    case game_over:
      if(game_has_colided_obj(mouse,leave_button))
        current_state=menu;
    break;
    case menu:
      if(game_has_colided_obj(mouse,exit_button))
        current_state = exiting;
      if(game_has_colided_obj(mouse,singleplayer_button)){
        game_reset_level();
        current_state = in_game;
        }
      if(game_has_colided_obj(mouse,how_to_button))
        current_state = how_to;
    case how_to:
    if(game_has_colided_obj(mouse,leave_how_to_button))
        current_state = menu;
    break;
    default:
    break;
  }
  return 0;
}

//in game logic
int(game_level_controller)(){
    if(rtcCounter >= 20 && !bossSpawned)
      game_spawn_boss();
    if(health<=0)
      game_set_game_over();
    if(enemies.to_spawn)
      game_generate_enemy();
    if(score>level*100){
      level++;
      difficulty++;
      if(damage<=100) damage ++;
      if(enemies.reaction_time >= 0)enemies.reaction_time--;
    }
    return 0;
}

int (game_move_character)(){
  int8_t x_movement=0;
  int8_t y_movement=0;

  if(wasd[up])
    y_movement = -1;
  if(wasd[left])
    x_movement = -1;
  if(wasd[down])
    y_movement = 1;
  if(wasd[right])
    x_movement = 1; 
  if(!wasd[up] && !wasd[down])
    y_movement = 0;
  if(!wasd[left] && !wasd[right])
    x_movement = 0;
  
  character.speed_x = x_movement * charSpeed;
  character.speed_y = y_movement * charSpeed;

  character.pos_x += character.speed_x;
  if(!game_check_if_in_bounds(character)){
    character.pos_x -= character.speed_x;
  }
  character.pos_y += character.speed_y;
  if(!game_check_if_in_bounds(character)){
    character.pos_y -= character.speed_y;
  }

  if(bossSpawned && game_has_colided_obj(boss,character))
    game_set_game_over();

  for(uint8_t i = 0; i<MAX_ENEMIES; i++){
    if(enemies.enemies_created[i])
      if(game_has_colided_obj(character, enemies.enemies[i]))
        {
          health -= damage;
          game_kill_enemy(i);
        }
  }
  return 0;
}

int(game_move_game_objects)(){
  //order will be 1st character moves, then enemies by position
  game_move_character();
  game_update_bullets();
  game_update_enemies_speed();
  for(uint8_t i = 0; i<MAX_ENEMIES; i++){
    if(enemies.enemies_created[i]) {
      game_move_enemy(i);
     if(bossSpawned && game_has_colided_obj(enemies.enemies[i],boss))
        game_kill_enemy(i);
    }
  }
  return 0;
}

int(game_move_enemy)(uint8_t pos){

  bool can_x=true;
  bool can_y=true;
  //check x
  enemies.enemies[pos].pos_x += enemies.enemies[pos].speed_x;

  //to not get out of map
  can_x = game_check_if_in_bounds(enemies.enemies[pos]);
  
  if(can_x){
    for(uint8_t i = 0; i<MAX_ENEMIES; i++){
        if(enemies.enemies_created[i] && i!=pos){
          if(game_has_colided_obj(enemies.enemies[i],enemies.enemies[pos])){
            can_x = false;
            break;
          }
        }
    }
  }
  
  if(!can_x) enemies.enemies[pos].pos_x -= enemies.enemies[pos].speed_x;

  //check y
  enemies.enemies[pos].pos_y += enemies.enemies[pos].speed_y;

  //to not get out of map
  can_y = game_check_if_in_bounds(enemies.enemies[pos]);
  
  if(can_y){
    for(uint8_t i = 0; i<MAX_ENEMIES; i++){
        if(enemies.enemies_created[i] && i!=pos){
          if(game_has_colided_obj(enemies.enemies[i],enemies.enemies[pos])){
            can_y = false;
            break;
          }
        }
    }
  }
  
  if(!can_y) enemies.enemies[pos].pos_y -= enemies.enemies[pos].speed_y;
  
  

  return 0;
}

int(game_update_enemies_speed)(){
  for(uint8_t i = 0; i<MAX_ENEMIES; i++){
    if(enemies.enemies_created[i]){
      if(enemies.enemies_delay[i] >= enemies.reaction_time)
        game_calculate_trajectory_at_speed(enemies.enemies[i].pos_x,enemies.enemies[i].pos_y,character.pos_x,character.pos_y,ENEMY_SPEED,&enemies.enemies[i].speed_x,&enemies.enemies[i].speed_y);
      else
        enemies.enemies_delay[i]++;
    }
  }
  return 0;
}

int(game_shoot)(){
  if(bulletCounter >= MAX_BULLETS) return 0;
  uint8_t pos =0;
  for(uint8_t i = 0; i< MAX_BULLETS; i++){
    if(!bullets.bullets_created[i]){
      pos = i;
      break;
    }
  }
  game_object bullet;
  bullet.pos_x = character.pos_x + character.collider_width/2;
  bullet.pos_y = character.pos_y + character.collider_height/2;
  game_calculate_trajectory_at_speed(bullet.pos_x,bullet.pos_y,mouse.pos_x+mouse.collider_x,mouse.pos_y+mouse.collider_y, BULLET_SPEED, &bullet.speed_x,&bullet.speed_y);
  bullet.img = base_bullet.img;
  bullet.map = base_bullet.map;
  bullet.has_collider = true;
  bullet.collider_x = BULLET_COL_X;
  bullet.collider_y = BULLET_COL_Y;
  bullet.collider_width = BULLET_COL_W;
  bullet.collider_height = BULLET_COL_H;
  bullet.has_animation = false;

  bullets.bullets[pos] = bullet;
  bullets.bullets_created[pos] = true;
  bulletCounter++;

  return 0;
}

int(game_kill_enemy)(uint8_t pos){
  enemies.enemies_created[pos] = false;
  enemyCounter--;
  score +=10;
  return 0;
}

int(game_update_bullets)(){
  for(uint8_t i = 0; i<MAX_BULLETS; i++){
    if(bullets.bullets_created[i]){
      bullets.bullets[i].pos_x += bullets.bullets[i].speed_x;
      bullets.bullets[i].pos_y += bullets.bullets[i].speed_y;
      if(!game_check_if_in_bounds(bullets.bullets[i])){
        bullets.bullets_created[i]=false;
        bulletCounter--;
      } else if (bossSpawned && game_has_colided_obj(bullets.bullets[i],boss)){
        bullets.bullets_created[i]=false;
        bulletCounter--;
        bossHP -= 5;
        game_create_blink(bullets.bullets[i].pos_x,bullets.bullets[i].pos_y);
      }else{
        for(uint8_t j = 0; j<MAX_ENEMIES; j++){
          if(enemies.enemies_created[j])
            if(game_has_colided_obj(enemies.enemies[j],bullets.bullets[i])){
              game_create_blink(enemies.enemies[j].pos_x,enemies.enemies[j].pos_y);
              game_kill_enemy(j);
              bullets.bullets_created[i]=false;
              bulletCounter--;
              break;
            }
        }
      }
    }
  }
  return 0;
}


int(game_generate_enemy)(){
  if(enemyCounter < MAX_ENEMIES){
    bool success = false;
    uint8_t pos = 0;
    game_object enemy;
    time_t t;
    
    enemy.map = base_enemy.map;
    enemy.img = base_enemy.img;
    enemy.has_collider = true;
    enemy.collider_x = ENEMY_COL_X;
    enemy.collider_y = ENEMY_COL_Y;
    enemy.collider_width = ENEMY_COL_W;
    enemy.collider_height = ENEMY_COL_H;
    enemy.speed_x = 0;
    enemy.speed_y = 0;
    while(!success){
      srand((unsigned) time(&t));
      enemy.pos_x = background.collider_x + rand()% (background.collider_width-base_enemy.img.width-1);
      enemy.pos_y = background.collider_y + rand() % (background.collider_height-base_enemy.img.height-1);
      success = true;
      for(uint8_t i = 0; i<MAX_ENEMIES; i++){
        if(enemies.enemies_created[i]){
          if(game_has_colided_obj(enemy,enemies.enemies[i]) || game_has_colided_circle(base_enemy.img.width/2,enemy.pos_x,enemy.pos_y,100, character.pos_x, character.pos_y) || (bossSpawned && game_has_colided_obj(enemy,boss))){
            success = false;
            break;
          }
        }
      }
    }

    while(enemies.enemies_created[pos]){
      pos++;
    }
    
    enemy.has_animation = true;
    enemy.animation_no = left;
    enemy.animations[up] = base_enemy.animations[up];
    enemy.animations[right] = base_enemy.animations[right];
    enemy.animations[left] = base_enemy.animations[left];
    enemy.animations[down] = base_enemy.animations[down];

    enemies.enemies[pos] = enemy;
    enemies.enemies_created[pos] = true;
    enemies.enemies_delay[pos] = enemies.reaction_time;;
    enemies.to_spawn = false;
    enemyCounter++;
  }
  return 0;
}

int(game_create_blink)(int16_t x, int16_t y){
  if(effectCounter == MAX_EFFECTS) return 0;
  uint8_t pos =0;
  game_object blink;
  blink.pos_x = x;
  blink.pos_y = y;
  blink.has_animation = true;
  blink.animation_no = 0;
  blink.animations[0] = base_blink;

  for(uint8_t i = 0; i<MAX_EFFECTS; i++){
    if(!effects.effects_created[i]){
      pos=i;
      break;
    }
  }

  effects.effects[pos] = blink;
  effects.effects_created[pos] = true;
  effectCounter++;

  return 0;
}

int(game_update_boss)(){
  if(bossSpawned){
    if(bossHP <= 0){
      level++;
      bossSpawned = false;
      rtcCounter = 0;
      score += 100;
      bossMaxHP += 25 ;

      effects.bossPuff.animations[0].previous_frame = 0;
      effects.bossPuff.animations[0].frame_interval_counter = effects.bossPuff.animations[0].animation_speed;
      effects.bossPuff.pos_x = boss.pos_x;
      effects.bossPuff.pos_y = boss.pos_y;

      effects.bossPuffToShow = true;
    }
  switch(boss_state){
    case falling:
        if(boss.animations[falling].previous_frame == 1 && boss.animations[falling].frame_interval_counter ==0){
          boss.animations[falling].animation_speed = 5;
          hulk_shadow.animations[0].animation_speed = 5;
        }
        if(boss.animations[falling].frame_interval_counter == boss.animations[falling].animation_speed){
        if(boss.animations[falling].previous_frame == 1){
          int16_t x = character.pos_x - (boss.img.width/2 - character.img.width/2);
          int16_t y = character.pos_y - (boss.img.height/2 - character.img.height/2);
          if(x<0) x= 0;
          if(y<0) y = 0;
          if(x>vmi.XResolution - boss.img.width) x = vmi.XResolution - boss.img.width;
          if(y>vmi.YResolution - boss.img.height) y = vmi.YResolution - boss.img.height;

          boss.pos_x = x;
          boss.pos_y = y;

          hulk_shadow.pos_x = x;
          hulk_shadow.pos_y = y;

        } 
        if(boss.animations[falling].previous_frame == boss.animations[falling].no_images -1)
          game_print_to_background(boss.pos_x,boss.pos_y, ground_destroyed);
        if(boss.animations[falling].previous_frame == boss.animations[falling].no_images){
          boss.has_collider = true;
          boss.has_animation = false;
          boss_state = idle;
          rtcCounter = 0;
        }
      }
    break;
    case idle:
      if(rtcCounter == 2){
        boss.animations[jumping].previous_frame = 0;
        boss_state = jumping;
        boss.has_animation = true;
        boss.animation_no = jumping;
        boss.animations[jumping].frame_interval_counter = boss.animations[jumping].animation_speed;
      }
    break;
    case jumping:
      if(boss.animations[jumping].frame_interval_counter == boss.animations[jumping].animation_speed){
        if(boss.animations[jumping].previous_frame == boss.animations[jumping].no_images -1)
            boss.has_collider = false;
        if(boss.animations[jumping].previous_frame == boss.animations[jumping].no_images){
          boss_state = falling;
          boss.animations[falling].previous_frame = 0;
          hulk_shadow.animations[0].previous_frame = 0;
          boss.animations[falling].animation_speed = 60;
          hulk_shadow.animations[0].animation_speed = 60;
          boss.animation_no = falling;

        }
      }
    break;
  }
  }
  return 0;
}
int(game_spawn_boss)(){
  bossSpawned = true;
  boss_state = falling;
  bossHP = bossMaxHP;
  boss.has_collider = false;
  boss.has_animation = true;
  boss.animation_no = falling;
  boss.animations[falling].previous_frame = 0;
  hulk_shadow.animations[0].previous_frame =0;
  
  return 0;
}


//maths

bool(game_check_if_in_bounds)(game_object go){
  if(!go.has_collider) return false;
  return game_has_colided(background.pos_x + background.collider_x + go.img.width, background.pos_y + background.collider_y + go.img.height, 
    background.collider_width - (go.img.width * 2), background.collider_height - (go.img.height * 2), go.pos_x,go.pos_y, go.img.width, go.img.height);
}

bool(game_has_colided)(int16_t x1, int16_t y1, uint16_t width1, uint16_t height1,
      int16_t x2, int16_t y2, uint16_t width2, uint16_t height2){
  if(x1<x2+width2 && x1 + width1 > x2 && y1<y2+height2 && y1 + height1 > y2){
          return true;
    }
  return false;
}

bool(game_has_colided_obj)(game_object g1, game_object g2){
  if(!g1.has_collider || !g2.has_collider) return false;
  return game_has_colided(g1.pos_x + g1.collider_x,g1.pos_y + g1.collider_y,g1.collider_width,g1.collider_height,
  g2.pos_x + g2.collider_x,g2.pos_y + g2.collider_y,g2.collider_width,g2.collider_height);
}

bool(game_has_colided_circle)(uint16_t radius1, uint16_t x1, uint16_t y1, uint16_t radius2, uint16_t x2, uint16_t y2){
  int16_t x = x1-x2;
  int16_t y = y1-y2;

  float d = sqrt(x*x + y*y);

  return d<radius1+radius2;

}

int(game_calculate_trajectory_at_speed)(int16_t x1, int16_t y1, int16_t x2,int16_t y2,int16_t speed,int16_t *vx, int16_t *vy){
  float x = x2-x1;
  float y = y2-y1;
  float alpha = atan(y/x);
  if(x<0) alpha+=M_PI;
  *vx = (int16_t) (speed*cos(alpha));
  *vy = (int16_t) (speed*sin(alpha));

  return 0;
}

enum frame_direction (game_get_direction)(int16_t x1, int16_t y1, int16_t x2,int16_t y2){
  float x = x2-x1;
  float y = y2-y1;
  float alpha = atan(y/x);
  if(x<0) alpha+=M_PI;
  if(alpha>= -M_PI/4 && alpha <= M_PI/4) return right;
  if(alpha>= 3*M_PI/4 && alpha <= 5*M_PI/4) return left;
  if(alpha> M_PI/4 && alpha < 3*M_PI/4) return down;
  return up;
}
//menu
int(game_display_menu)(){
  singleplayer_button.pos_y = vmi.YResolution/2 -25;
  how_to_button.pos_y = vmi.YResolution/2 +singleplayer_button.img.height;
  exit_button.pos_y = vmi.YResolution/2 +  singleplayer_button.img.height + how_to_button.img.height + 25;
  
  if(game_has_colided_obj(mouse,singleplayer_button))
    singleplayer_button.pos_y -=5;
  else if(game_has_colided_obj(mouse,exit_button))
    exit_button.pos_y -=5;
  else if(game_has_colided_obj(mouse,how_to_button))
    how_to_button.pos_y -= 5;

  video_frame_to_buffer(wallpaper.bytes,wallpaper,0,0);
  game_display_game_object(&singleplayer_button);
  game_display_game_object(&how_to_button);
  game_display_game_object(&exit_button);
  return 0;
}
//game_over
int(game_set_game_over)(){
  rtc_disable_periodic_interrupts();
  current_state = game_over;
  return 0;
}
int(game_display_game_over)(){
  uint16_t x_go = vmi.XResolution/2 - game_over_hud.width/2;
  uint16_t y_go = vmi.YResolution/2 - game_over_hud.height/2;
  video_frame_to_buffer(game_over_hud.bytes,game_over_hud,x_go,y_go);
  leave_button.pos_y = y_go + game_over_hud.height/2;
  if(game_has_colided_obj(mouse,leave_button))
    leave_button.pos_y -=5;
  game_display_game_object(&leave_button);
  return 0;
}

//displays
int(game_print_to_background)(uint16_t x, uint16_t y, xpm_image_t img){
  for(uint16_t i = 0; i < img.height; i++){
        for(uint16_t j = 0; j < img.width; j++){
            if(img.bytes[((i*img.width)+j)*bytesPerPixel]!=0)
                memcpy(&background.map[((y+i)*background.img.width + (x + j))*bytesPerPixel],&img.bytes[((i*img.width)+j)*bytesPerPixel],bytesPerPixel);
        }
    }
    return 0;
}

int(game_display_level)(){
  game_display_game_object(&background);
  if(bossSpawned && boss_state == falling){
      hulk_shadow.animations[0].frame_interval_counter = boss.animations[falling].frame_interval_counter;
      game_display_game_object(&hulk_shadow);
    }
  
  game_display_hud();

  for(uint8_t i = 0 ; i < MAX_ENEMIES; i++){
    if(enemies.enemies_created[i]){
      enemies.enemies[i].animation_no = game_get_direction(enemies.enemies[i].pos_x,enemies.enemies[i].pos_y,character.pos_x,character.pos_y);
        game_display_game_object(&enemies.enemies[i]);
      }
    }
    for(uint8_t i = 0 ; i < MAX_BULLETS; i++){
    if(bullets.bullets_created[i])
      game_display_game_object(&bullets.bullets[i]);
    }
    game_display_effects();
    return 0;
}

int(game_display_game_object)(game_object *go){
  if(!(*go).has_animation){
    video_frame_to_buffer((*go).map,(*go).img,(*go).pos_x,(*go).pos_y);
  }else{
  uint8_t sprite_to_show = 0;
  //check if its to change the sprite no
  if((*go).animations[(*go).animation_no].frame_interval_counter == (*go).animations[(*go).animation_no].animation_speed){
    (*go).animations[(*go).animation_no].frame_interval_counter = 0;
    //if it was the last frame, then play the first
    if((*go).animations[(*go).animation_no].previous_frame == (*go).animations[(*go).animation_no].no_images)
      sprite_to_show = 0;
    //else show the next one
    else
      sprite_to_show = (*go).animations[(*go).animation_no].previous_frame + 1;
  //if not, than increment the frame interval      
  }else{
    (*go).animations[(*go).animation_no].frame_interval_counter++;
    sprite_to_show = (*go).animations[(*go).animation_no].previous_frame;
  }
  //display the right sprite
  video_frame_to_buffer((*go).animations[(*go).animation_no].maps[sprite_to_show],(*go).animations[(*go).animation_no].imgs[sprite_to_show],(*go).pos_x,(*go).pos_y);
  (*go).animations[(*go).animation_no].previous_frame = sprite_to_show;  
  }
  return 0;
}

int(game_display_hud)(){
  uint16_t x_pf = 5;
  uint16_t y_pf = 5;
  video_frame_to_buffer(pointframe.bytes,pointframe,x_pf,y_pf);
  uint64_t tempScore = score;
  uint8_t tempLevel = level;
  uint8_t digit = 0;
  uint16_t x_numbers = x_pf + pointframe.width - 8 - numbers[0].width;
  uint16_t y_numbers = y_pf + 8;
   while(tempScore !=0){
    digit = tempScore%10;
    tempScore/=10;
    video_frame_to_buffer(numbers[digit].bytes,numbers[digit],x_numbers,y_numbers);
    x_numbers -= numbers[digit].width;
  }
  x_numbers = vmi.XResolution - 5 - numbers[digit].width;
  while(tempLevel !=0){
    digit = tempLevel%10;
    tempLevel/=10;
    video_frame_to_buffer(numbers[digit].bytes,numbers[digit],x_numbers,y_numbers);
    x_numbers -= numbers[digit].width;
  }
  video_frame_to_buffer(numbers[10].bytes,numbers[10],x_numbers,y_numbers);
  x_numbers -= numbers[digit].width;
  video_frame_to_buffer(numbers[11].bytes,numbers[11],x_numbers,y_numbers);
  video_frame_to_buffer(pointframe.bytes,pointframe,x_pf + pointframe.width + x_pf, y_pf);
  
  if(health>0){
  uint8_t red = (1-(float)health/100)*255;
  uint8_t green = ((float)health/100)*255;
  uint32_t color = red << 16 | green << 8;
  vg_draw_rectangle(x_pf + pointframe.width + x_pf + 8, y_pf + 8, (uint16_t)(((float)health/100) * pointframe.width - 16 ) <=pointframe.width - 16 ? (uint16_t)(((float)health/100) * pointframe.width - 16 ) : 0, pointframe.height -16,color);
  }
  
  if(bossSpawned && boss.has_collider){
    uint8_t red = (1-(float)bossHP/bossMaxHP)*255;
    uint8_t green = ((float)bossHP/bossMaxHP)*255;
    uint32_t color = red << 16 | green << 8;
    vg_draw_rectangle(boss.pos_x + boss.img.width/2 - 50, boss.pos_y, 100 , 10,0x010101);
    vg_draw_rectangle(boss.pos_x + boss.img.width/2 - 50, boss.pos_y, bossHP > 0 ? (uint16_t)(((float)bossHP/bossMaxHP) * 100 ) : 0, 10,color);
  }

  return 0;
}

int(game_display_effects)(){
  for(uint8_t i = 0; i<MAX_EFFECTS; i++){
    if(effects.effects_created[i]){
      game_display_game_object(&effects.effects[i]);
      if(effects.effects[i].animations[0].previous_frame == effects.effects[i].animations[0].no_images){
          effects.effects_created[i]=false;
          effectCounter--;
      }
    }
  }
  if(effects.bossPuffToShow){
      game_display_game_object(&effects.bossPuff);
      if(effects.bossPuff.animations[0].previous_frame == effects.bossPuff.animations[0].no_images){
        effects.bossPuffToShow = false;
      }
  }
  return 0;
}

//resets

int(game_reset_level)(){
  rtc_set_periodic_rate(0xF);
  rtc_enable_periodic_interrupts();
  background.map = xpm_load(background_map,  XPM_8_8_8, &background.img);
  //game_objects
  character.pos_x = vmi.XResolution/2;
  character.pos_y = vmi.YResolution/2;
  character.speed_x = 0;
  character.speed_y = 0;

  memset(&enemies, 0, sizeof(enemies));
  memset(&bullets, 0, sizeof(bullets));
  memset(&effects, 0, sizeof(effects));
  //hud
  
  effects.bossPuffToShow = false;
  //boss puff
  effects.bossPuff.animations[0].maps[1] = xpm_load(boss_puff_1_map, XPM_8_8_8, &effects.bossPuff.animations[0].imgs[1]);
  effects.bossPuff.animations[0].maps[2] = xpm_load(boss_puff_2_map, XPM_8_8_8, &effects.bossPuff.animations[0].imgs[2]);
  effects.bossPuff.animations[0].maps[3] = xpm_load(boss_puff_3_map, XPM_8_8_8, &effects.bossPuff.animations[0].imgs[3]);
  effects.bossPuff.animations[0].maps[4] = xpm_load(boss_puff_4_map, XPM_8_8_8, &effects.bossPuff.animations[0].imgs[4]);
  effects.bossPuff.animations[0].no_images = 4;
  effects.bossPuff.animations[0].animation_speed = 2;
  effects.bossPuff.animations[0].previous_frame = 0;
  effects.bossPuff.animations[0].frame_interval_counter = effects.bossPuff.animations[0].animation_speed;
  
  effects.bossPuff.has_animation = true;
  effects.bossPuff.animation_no = 0;

  //game info
  enemyCounter=0; 
  bulletCounter=0;
  effectCounter=0;
  //game play info
  bossMaxHP = 100;
  bossSpawned =false;
  difficulty = 1;
  enemy_reaction_time = 30;
  score = 0;
  ticks_in_game = 0;
  timerCounter = 0;
  health = 100;
  damage = 5;
  level = 1;
  rtcCounter = 0;
  charSpeed = CHAR_SPEED;

  return 0;
}

int(game_reset_config)(){
  video_free_buffer();
//unsubscribe interrupts 
  if(timer_unsubscribe_int()){
    printf("timer_unsubs failed\n");
    return 1;
  }
  if(keyboard_unsubscribe_int()){
    printf("keyboard_unsubs failed\n");
    return 1;
  }
  if(mouse_unsubscribe_int()){
    printf("mouse_unssubs failed\n");
    return 1;
  }
  if(rtc_disable_periodic_interrupts()){
    printf("rtc_dis pi failed\n");
    return 1;
  }
  if(rtc_unsubscribe_int()){
    printf("rtc_unssubs failed\n");
    return 1;
  }
  //back to text mode
  if(vg_exit()){
    printf("vg_exit failed\n");
    return 1;
  }
  return 0;
}

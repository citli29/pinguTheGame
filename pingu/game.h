#include <lcom/lcf.h>

/** @defgroup game game
 * @{
 *
 * Functions for creating the game
 */

//constants
#define FRAME_RATE 30
#define CHAR_SPEED 8
#define ENEMY_SPEED 5
#define BULLET_SPEED 15
#define GRAPHIC_MODE 0x118
#define MAX_NO_ANIMATION 6
#define NO_SPRITE_ANGLES 4
#define MAX_ENEMIES 100
#define MAX_BULLETS 50
#define MAX_EFFECTS 50

//sizes
#define CHAR_COL_X 0
#define CHAR_COL_Y 0
#define CHAR_COL_W 38
#define CHAR_COL_H 38

#define ENEMY_COL_X 0
#define ENEMY_COL_Y 0
#define ENEMY_COL_W 32
#define ENEMY_COL_H 32

#define BULLET_COL_X 0
#define BULLET_COL_Y 0
#define BULLET_COL_W 32
#define BULLET_COL_H 32

//animations
#define ANIM_SPEED 5
#define BLINK_ANIM_SPEED 1
enum game_state{menu=0,in_game=1, game_over=2, how_to =3 ,exiting = -1};
enum frame_direction{down = 2, up=0,left=1,right = 3};
enum boss_state{falling=0,idle=1,jumping=2};

/**
 * @brief animation object
 * maps -> arrays with the sprites
 * imgs -> xpm_image_t with the information about the sprites
 * no_images -> number of images on the animation
 * animation_speed -> the ammount of ticks to change sprite´
 * previous_frame -> number of the last immage shown
 * frame_interval_counter -> number of ticks since the last change of no_images
 */
typedef struct{
    uint8_t * maps[MAX_NO_ANIMATION];
    xpm_image_t imgs[MAX_NO_ANIMATION];
    uint8_t no_images;
    uint8_t animation_speed;
    uint8_t previous_frame;
    uint8_t frame_interval_counter;
} animation_object;

/**
 * @brief game object
 * pos_x -> x position of the object
 * pos_y -> y position of the object
 * speed_x -> speed of the object in the x axys
 * speed_y -> speed of the object in the y axys
 * img -> the xpm_image_t with the information about the sprite
 * map -> array with the sprite
 * has_collider -> bool that says if the object has colliders or not
 * collider_x -> according to the inicial point of the sprite, the x position where the collider starts
 * collider_y -> according to the inicial point of the sprite, the y position where the collider starts
 * collider_height -> the collider's height
 * collider_width -> the collider's width
 * has_animation -> bool that says if the object has animations 
 * animation_no -> the number of the animation beeiing displayed
 * animations -> array with the animation_object s
 */
typedef struct{
    int16_t pos_x;
    int16_t pos_y;
    int16_t speed_x;
    int16_t speed_y;
    xpm_image_t img;
    uint8_t *map;
    bool has_collider;
    uint16_t collider_x;
    uint16_t collider_y;
    uint16_t collider_width;
    uint16_t collider_height;
    bool has_animation;
    uint8_t animation_no;
    animation_object animations[NO_SPRITE_ANGLES];
} game_object;

/**
 * @brief enemy struct
 * a struct with the information about the enemies
 * enemies -> array with the object of the enemies
 * enemies_created -> array with information about if the enemies are alive or not
 * enemies_delay -> array with information about the current delay of the enemies moves
 * reaction_time -> reaction time of the enemies
 * to_spawn -> bool that says if an enemy will spawn
 */
typedef struct {
  game_object enemies[MAX_ENEMIES];
  bool enemies_created[MAX_ENEMIES];
  uint8_t enemies_delay[MAX_ENEMIES];
  uint8_t reaction_time;
  bool to_spawn;
}enemyStruct;
/**
 * @brief bullet struct
 * a struct with the information about the bullets
 * bullets -> array with the object of the bullets
 * bullets_created -> array with information about if the bullets are active or not
 */
typedef struct {
  game_object bullets[MAX_BULLETS];
  bool bullets_created[MAX_BULLETS];
}bulletStruct;
/**
 * @brief effect struct
 * a struct with the information about the effects
 * effects -> array with the object of the effects
 * effects_created -> array with information about if the effects are active or not
 * bossPuff -> game object with the animation of the boss puff 
 * bossPuffToShow -> bool that says if the boss puff animation is showing
 */
typedef struct {
  game_object effects[MAX_EFFECTS];
  bool effects_created[MAX_EFFECTS];
  game_object bossPuff;
  bool bossPuffToShow;
}effectStruct;

//initial configs
/**
 * @brief set the initial config
 * subscribe to interrupts
 * set grapics mode
 * loads to memmory sprite information 
 * @return 0 if success
 */
int(game_set_initial_config)();
/**
 * @brief loads to memmory sprite information 
 * @return 0 if success
 */
int(game_set_sprite_info)();
//main controller
/**
 * @brief according to the game state, controls what happens on the screen
 * and the game logic
 * @return 0 if success
 */
int(game_controller)();
//handlers
/**
 * @brief according to the keys pressed assigns it to the movement of the character
 * @return 0 if success
 */
int(game_keyboard_handler)();
/**
 * @brief according to the bytes received updates the mouse game object
 * @return 0 if success
 */
int(game_mouse_handler)();
/**
 * @brief increments the tick count and according to the frame rate calls the game controller method
 * @return 0 if success
 */
int(game_timer_handler)();
/**
 * @brief spawns enemies and increment rtc counter
 * @return 0 if success
 */
int(game_rtc_handler)();
/**
 * @brief according to the game state handles what happens when left mouse button is pressed
 * @return 0 if success
 */
int(game_click_handler)();
//in game logic
/**
 * @brief according to the game state spawns a boss
 * sets the game to over
 * generate enemies
 * levels up
 * @return 0 if success
 */
int(game_level_controller)();
/**
 * @brief according to the keys pressed move the character object
 * @return 0 if success
 */
int(game_move_character)();
/**
 * @brief according to the current speed_x and speed y updates 
 * character and enemies positions 
 * @return 0 if success
 */
int(game_move_game_objects)();
/**
 * @brief according to the speed_x and speed y updates enemies position
 * @return 0 if success
 */
int(game_move_enemy)(uint8_t pos);
/**
 * @brief updates enemies speed according to the character position
 * @param pos position of the enemy on the array
 * @return 0 if success
 */
int(game_update_enemies_speed)();
/**
 * @brief creates a bullet
 * @return 0 if success
 */
int(game_shoot)();
/**
 * @brief kills an enmy
 * @param pos position of the enemy on the array
 * @return 0 if success
 */
int(game_kill_enemy)(uint8_t pos);
/**
 * @brief updates the bullets
 * @return 0 if success
 */
int(game_update_bullets)();
/**
 * @brief creates an enemy
 * @return 0 if success
 */
int(game_generate_enemy)();
/**
 * @brief creates a blink effect on the position x and y
 * @param x x position
 * @param y y position
 * @return 0 if success
 */
int(game_create_blink)(int16_t x, int16_t y);
/**
 * @brief spawns a boss
 * @return 0 if success
 */
int(game_spawn_boss)();
/**
 * @brief updates boss according to his state
 * @return 0 if success
 */
int(game_update_boss)();
//math
/**
 * @brief checks if a game object is in bounds according to the background collider
 * @param go
 * @return true if its in bound
 */
bool(game_check_if_in_bounds)(game_object go);
/**
 * @brief checks if two rectangles collided
 * @param x1 position x of rectangle 1
 * @param y1 position y of rectangle 1
 * @param width1 width of the rectangle 1
 * @param height1 height of the rectangle 1
 * @param x2 position x of rectangle 2
 * @param y2 position y of rectangle 2
 * @param width2 width of the rectangle 2
 * @param height2 height of the rectangle 2
 * @return true if collided
 */
bool(game_has_colided)(int16_t x1, int16_t y1, uint16_t width1, uint16_t height1,
      int16_t x2, int16_t y2, uint16_t width2, uint16_t height2);
/**
 * @brief checks if two objects colided if the collider is a rectangle
 * @param g1 game object 1
 * @param g2 game object 2
 * @return true if colided
 */
bool(game_has_colided_obj)(game_object g1, game_object g2);
/**
 * @brief checks if two circles colided
 * @param radius1 radius of circle 1
 * @param x1 position x of circle 1
 * @param y1 position y of circle 1
 * @param radius2 radius of circle 2
 * @param x2 position x of circle 2
 * @param y2 position y of circle 2
 */
bool(game_has_colided_circle)(uint16_t radius1, uint16_t x1, uint16_t y1, uint16_t radius2, uint16_t x2, uint16_t y2);
/**
 * @brief calculate a trajectory from 1 to 2 at a certain speed
 * @param x1 position x of 1
 * @param y1 position y of 1
 * @param x2 position x of 2
 * @param y2 position y of 2
 * @param x1 position x of 1
 * @param speed speed to move
 * @param vx x speed to be calculated
 * @param vy y speed to be calculated
 */
int(game_calculate_trajectory_at_speed)(int16_t x1, int16_t y1, int16_t x2,int16_t y2,int16_t speed,int16_t *vx, int16_t *vy);
/**
 * @brief returns the direction that faces point 1 to 2 
 * -45 to 45 degrees right
 * 135 to 45 degrees up
 * 135 to 225 degrees left
 * 225 to 315 degress down
 * @param x1 position x of 1
 * @param y1 position y of 1
 * @param x2 position x of 2
 * @param y2 position y of 2
 * @return enum according to the position
 */
enum frame_direction (game_get_direction)(int16_t x1, int16_t y1, int16_t x2,int16_t y2);
//menu
/**
 * @brief displays the menu
 * @return 0 if success
 */
int(game_display_menu)();
//game_over
/**
 * @brief sets the game state to game over
 * @return 0 if success
 */
int(game_set_game_over)();
/**
 * @brief displays the game over layout
 * @return 0 if success
 */
int(game_display_game_over)();
//displays
/**
 * @brief prints a sprite to the background map
 * @param x position x of the sprite
 * @param y position y of the sprite
 * @param img xpm_image_t of the sprite 
 * @return 0 if success
 */
int(game_print_to_background)(uint16_t x, uint16_t y, xpm_image_t img);
/**
 * @brief displays all the level information 
 * calls function to display background, boss shadows, hud, enemies, bullets and effects
 * @return 0 if success
 */
int(game_display_level)();
/**
 * @brief displays the effects
 * @return 0 if success
 */
int(game_display_effects)();
/**
 * @brief displays a game object
 * @param go game object to display
 * @return 0 if success
 */
int(game_display_game_object)(game_object * go);
/**
 * @brief displays the hud
 * @return 0 if success
 */
int(game_display_hud)();
//resets
/**
 * @brief resets the level configurations
 * @return 0 if success
 */
int(game_reset_level)();
/**
 * @brief resets the minix configurations to default
 * @return 0 if success
 */
int(game_reset_config)();

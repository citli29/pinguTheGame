#include <lcom/lcf.h>
/** @defgroup video video
 * @{
 *
 * Functions for using the video card
 */
/**
 * @brief set graphic to a mode
 * @param mode the mode to set
 * @return 0 if success
 */
int (video_set_mode)(uint16_t mode);
/**
 * @brief allow memory mapping
 * @return 0 if success
 */
int (video_allow_memory_mapping)();
/**
 * @brief updates vbe_mode_info_t according to the mode
 * @param mode the mode
 * @return 0 if success
 */
int(video_get_mode_info)(uint16_t mode);
/**
 * @brief copies the information on the buffer to video memory
 * @return 0 if success
 */
int(video_buffer_to_vg)();
/**
 * @brief copies a sprite to a specific position to frame buffer
 * @param map the array with pixel information
 * @param img xpm_image_t with information about the sprite
 * @param x x position
 * @param y y position
 * @return 0 if success
 */
int(video_frame_to_buffer)(uint8_t *map,xpm_image_t img,uint16_t x,uint16_t y);
/**
 * @brief frees the memmory of the buffer
 * @return 0 if success
 */
int(video_free_buffer)();

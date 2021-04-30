#include <lcom/lcf.h>
#define RTC_IRQ8 8
#define RTC_REGISTERA 10
#define RTC_REGISTERB 11
#define RTC_REGISTERC 12
#define RTC_REGISTERD 13
#define RTC_ADDR_REG 0x70
#define RTC_DATA_REG 0x71
/** @defgroup rtc rtc
 * @{
 *
 * Functions for using the rtc
 */
/**
 * @brief subscribe to rtc interruptions
 * @param bit_no that will be filled with the hook id 
 * @return 0 if success
 */
int (rtc_subscribe_int)(uint8_t *bit_no) ;
/**
 * @brief unsubscribe to rtc interruptions
 * @return 0 if success
 */
int (rtc_unsubscribe_int)() ;
/**
 * @brief set a periodic rate on rtc
 * @param rate_selector the RS0-RS3 bits
 * @return 0 if success 
 */
int (rtc_set_periodic_rate)(uint8_t rate_selector);
/**
 * @brief enable periodic interrupts
 * @return 0 if success
 */
int (rtc_enable_periodic_interrupts)();
/**
 * @brief disable periodic interrupts
 * @return 0 if success
 */
int (rtc_disable_periodic_interrupts)();
/**
 * @brief read register c flags
 * @return 0 if success
 */
int (rtc_ih)();

#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Sleep for the specified amount of microseconds
 * @param[in] us Number of microseconds to sleep
 */
void inv_ixm42xxx_sleep_us(unsigned int us);

/**
 * @brief Get the current time in microseconds
 * @return Current time in microseconds
 */
uint64_t inv_ixm42xxx_get_time_us(void);

/**
 * @brief Disable interrupts (platform-specific)
 */
void inv_helper_disable_irq(void);

/**
 * @brief Enable interrupts (platform-specific)
 */
void inv_helper_enable_irq(void);

#ifdef __cplusplus
}
#endif

#endif
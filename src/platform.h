#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#include <stdint.h>
#include "Ixm42xxxDriver_HL.h"
#include "InvError.h"
#include "Ixm42xxxDefs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct imu_spi_cfg
{
int type;
int fd;
char *device;
uint8_t mode;
uint8_t bits;
int speed;
uint16_t delay;
} imu_spi_cfg_t;
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

int platform_spi_read(struct inv_ixm42xxx_serif *serif, uint8_t reg, uint8_t *buf, uint32_t len); 
int platform_spi_write(struct inv_ixm42xxx_serif *serif, uint8_t reg, const uint8_t *buf, uint32_t len); 
int spi_open(imu_spi_cfg_t *pstSpiCfg);
void spi_write_read_data(int fd, void *tx_data, void *rx_data, int len);
#ifdef __cplusplus
}
#endif

#endif

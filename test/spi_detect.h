#ifndef _SPI_DETECT_H_
#define _SPI_DETECT_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief SPI检测工具的主要函数
 * @param[in] device_path SPI设备路径，默认为"/dev/spidev0.0"
 * @return 成功返回0，失败返回非0值
 */
int spi_detect_sensor(const char *device_path);

#ifdef __cplusplus
}
#endif
#endif
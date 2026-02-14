#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "spi_detect.h"
#include "Ixm42xxxTransport.h"
#include "Ixm42xxxDefs.h"
#include "Ixm42xxxDriver_HL.h"

#ifdef __linux__
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#endif

int spi_detect_sensor(const char *device_path) 
{
    printf("IIM42652 SPI Detection Tool\n");
    printf("============================\n");

#ifdef __linux__
    int fd;
    uint8_t tx_buffer[2];
    uint8_t rx_buffer[2];
    struct spi_ioc_transfer tr = {0};

    // Open SPI device
    fd = open(device_path, O_RDWR);
    if (fd < 0) {
        printf("Error: Failed to open SPI device %s: %s\n", device_path, strerror(errno));
        return -1;
    }
    printf("Successfully opened SPI connection to %s\n", device_path);

    // Prepare WHOAMI register read command (MPUREG_WHO_AM_I = 0x75, with MSB set for read)
    tx_buffer[0] = 0x80 | 0x75;  // Register address with read bit set (0x80)
    tx_buffer[1] = 0x00;         // Dummy byte for clocking in response

    // Configure SPI transfer
    tr.tx_buf = (unsigned long)tx_buffer;
    tr.rx_buf = (unsigned long)rx_buffer;
    tr.len = 2;
    tr.speed_hz = 1000000;  // 1MHz SPI clock
    tr.bits_per_word = 8;
    tr.delay_usecs = 0;
    tr.cs_change = 0;

    // Perform SPI transfer using the correct macro
    if (ioctl(fd, SPI_IOC_MESSAGE(1), &tr) < 0) {
        printf("Error: SPI transfer failed: %s\n", strerror(errno));
        close(fd);
        return -1;
    }

    // Extract WHOAMI value from received data
    uint8_t whoami_value = rx_buffer[1];  // Second byte contains the register value
    printf("WHOAMI register value: 0x%02X\n", whoami_value);

    // Check if WHOAMI value matches IIM42652
    if (whoami_value == IIM42652_WHOAMI) {  // Use the correct WHOAMI value for IIM42652
        printf("SUCCESS: IIM42652 chip detected via SPI! (Correct WHOAMI value)\n");
        printf("Chip is present and responding correctly on SPI bus.\n");
        close(fd);
        printf("\nSPI detection completed successfully!\n");
        printf("IIM42652 chip is present and communicating via SPI.\n");
        return 0;
    } else {
        printf("ERROR: Unexpected WHOAMI value! Expected: 0x%02X, Got: 0x%02X\n", 
               IIM42652_WHOAMI, whoami_value);
        close(fd);
        return -1;
    }
#else
    printf("This SPI detection tool is designed for embedded Linux systems.\n");
    printf("On desktop systems, hardware access is not available.\n");
    printf("Expected WHOAMI value: 0x%02X\n", IIM42652_WHOAMI);
    printf("Skipping actual SPI communication on non-Linux platform.\n");
    return 0;  // Return success on non-Linux platforms for framework testing
#endif
}
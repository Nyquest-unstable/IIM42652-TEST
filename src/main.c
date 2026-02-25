#include <stdio.h>
#include <linux/spi/spidev.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include "Ixm42xxxDriver_HL.h"
#include "InvError.h"
#include "Ixm42xxxDefs.h"
#include "Message.h"
#include "ErrorHelper.h"
#include "platform.h"

/* Structure to handle the device driver */
static struct inv_ixm42xxx sensor_driver;

/* Callback to handle fifo data */
static void handle_fifo_data(inv_ixm42xxx_sensor_event_t * event);

imu_spi_cfg_t imu_spi_cfg_X6 = {
    .type = NULL,
    .fd = -1,
    .device = "/dev/spidev0.0",
    .mode = SPI_CPHA | SPI_CPOL,
    .bits = 8,
    .speed = 500000,
    .delay = 0,
};

struct inv_ixm42xxx_serif serif = {
    .read_reg = platform_spi_read,      // Would need to implement SPI/I2C read
    .write_reg = platform_spi_write,     // Would need to implement SPI/I2C write
    .max_read = 256,         // Max bytes for read transaction
    .max_write = 256,        // Max bytes for write transaction
    .context = (void*)"/dev/spidev0.0",
    .serif_type = IXM42XXX_UI_SPI4,
};

static int imu_type_check(int fd)
{
    uint8_t tx_icm42688[2] = {0xF5, 0x00};
    uint8_t rx_icm42688[2] = {0};
    printf("fd is %d\n", fd);
    spi_write_read_data(fd, tx_icm42688, rx_icm42688, sizeof(rx_icm42688));
    if (0x47 == rx_icm42688[0] || 0x6F == rx_icm42688[1])
    {
        printf("i am iim42688\n");
        printf("read data is %02X, %02X\n", rx_icm42688[0], rx_icm42688[1]);
    }
    else {
        printf("read data is %02X, %02X\n", rx_icm42688[0], rx_icm42688[1]);
    }

    platform_spi_read(&serif, 0xF5, rx_icm42688, sizeof(rx_icm42688));
    if (0x47 == rx_icm42688[0] || 0x6F == rx_icm42688[1])
    {
        printf("i am iim42688\n");
        printf("read data is %02X, %02X\n", rx_icm42688[0], rx_icm42688[1]);
    }
    else {
        printf("read data is %02X, %02X\n", rx_icm42688[0], rx_icm42688[1]);
    }
    return 0;

}
int main(int argc, char **argv) 
{
    printf("IIM42652 Motion Sensor Test Application\n");
    printf("========================================\n");

    /* Example implementation would go here */
    
    // Initialize the transport interface structure
    // This is just a placeholder - in a real implementation you would
    // need to provide actual function pointers for the transport
    printf("opening spi\n");
    spi_open(&imu_spi_cfg_X6);
    
    printf("checking imu type\n");
    imu_type_check(imu_spi_cfg_X6.fd);

    printf("Attempting to initialize IIM42652 sensor...\n");
    // Initialize the sensor
    int rc = inv_ixm42xxx_init(&sensor_driver, &serif, handle_fifo_data);
    if(rc != INV_ERROR_SUCCESS) {
        printf("Failed to initialize IIM42652 sensor! Error code: %d\n", rc);
        return -1;
    }

    printf("Successfully initialized IIM42652 sensor\n");

    // Read WHOAMI register to verify the connection
    uint8_t who_am_i = 0;
    rc = inv_ixm42xxx_get_who_am_i(&sensor_driver, &who_am_i);
    if(rc != INV_ERROR_SUCCESS) {
        printf("Failed to read WHOAMI register! Error code: %d\n", rc);
        return -1;
    }

    printf("WHOAMI register value: 0x%02X\n", who_am_i);

    // Verify the WHOAMI value for IIM42652 (the correct constant is ICM_WHOAMI)
    if(who_am_i == ICM_WHOAMI) {
        printf("Successfully verified IIM42652 sensor (correct WHOAMI)\n");
    } else {
        printf("WHOAMI mismatch! Expected: 0x%02X, Got: 0x%02X\n", ICM_WHOAMI, who_am_i);
        return -1;
    }

    // Configure the sensor for basic accelerometer and gyroscope readings
    printf("Configuring sensor for basic measurements...\n");

    // Set full scale ranges
    rc |= inv_ixm42xxx_set_accel_fsr(&sensor_driver, IXM42XXX_ACCEL_CONFIG0_FS_SEL_16g);
    rc |= inv_ixm42xxx_set_gyro_fsr(&sensor_driver, IXM42XXX_GYRO_CONFIG0_FS_SEL_2000dps);

    // Set output data rates
    rc |= inv_ixm42xxx_set_accel_frequency(&sensor_driver, IXM42XXX_ACCEL_CONFIG0_ODR_50_HZ);
    rc |= inv_ixm42xxx_set_gyro_frequency(&sensor_driver, IXM42XXX_GYRO_CONFIG0_ODR_50_HZ);

    // Enable low noise mode
    rc |= inv_ixm42xxx_enable_accel_low_noise_mode(&sensor_driver);
    rc |= inv_ixm42xxx_enable_gyro_low_noise_mode(&sensor_driver);

    if(rc != INV_ERROR_SUCCESS) {
        printf("Failed to configure sensor! Error code: %d\n", rc);
        return -1;
    }

    printf("Sensor configured successfully!\n");
    printf("Starting data acquisition loop (will run for 10 seconds)...\n");

    // Main data acquisition loop
    for(int i = 0; i < 10; i++) {
        // In a real implementation, we would wait for interrupt or poll the FIFO
        // Here we just simulate the process

        printf("Reading sensor data... (iteration %d)\n", i+1);

        // In a real implementation, this would trigger reading data from the sensor
        // rc = inv_ixm42xxx_get_data_from_fifo(&sensor_driver);

        // Sleep briefly (in real implementation, replace with appropriate delay)
        #ifdef _WIN32
            Sleep(1000);  // On Windows (1 second)
        #else
            usleep(1000000);  // On Linux/Unix (1 second)
        #endif

        // For demonstration purposes, we'll just print a sample output
        printf("Sample data: Accel[X,Y,Z]: 0, 0, 0 | Gyro[X,Y,Z]: 0, 0, 0 | Temp: 0\n");
    }
    
    printf("\nTest completed successfully!\n");
    return 0;
}

/* Callback function to handle FIFO data */
static void handle_fifo_data(inv_ixm42xxx_sensor_event_t * event)
{
    // This function will be called when sensor data is available
    // In a real implementation, this would process the sensor data
    
    if(event->sensor_mask & (1 << INV_IXM42XXX_SENSOR_ACCEL)) {
        printf("Accel data: [%d, %d, %d]\n", 
               event->accel[0], event->accel[1], event->accel[2]);
    }
    
    if(event->sensor_mask & (1 << INV_IXM42XXX_SENSOR_GYRO)) {
        printf("Gyro data: [%d, %d, %d]\n", 
               event->gyro[0], event->gyro[1], event->gyro[2]);
    }
    
    if(event->sensor_mask & (1 << INV_IXM42XXX_SENSOR_TEMPERATURE)) {
        printf("Temperature: %d\n", event->temperature);
    }
}

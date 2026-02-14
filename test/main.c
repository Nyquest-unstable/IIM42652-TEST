#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "spi_detect.h"

int main(int argc, char *argv[]) 
{
    const char *device_path = "/dev/spidev0.0";  // Default SPI device path
    
    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0 && i + 1 < argc) {
            device_path = argv[++i];  // Next argument is the device path
        } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            printf("Usage: %s [-d device_path]\n", argv[0]);
            printf("Options:\n");
            printf("  -d device_path   Specify SPI device path (default: /dev/spidev0.0)\n");
            printf("  -h, --help       Show this help message\n");
            return 0;
        }
    }
    
    printf("Starting IIM42652 SPI Detection Tool...\n");
    
    int result = spi_detect_sensor(device_path);
    
    if (result == 0) {
        printf("\nDetection completed: SUCCESS\n");
        return 0;
    } else {
        printf("\nDetection completed: FAILED\n");
        return -1;
    }
}
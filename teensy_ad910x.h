/******************************************************************************
    @file:  teensy_ad910x.h
 
    @brief: AD910x driver for Teensy 4.1 - Header file
            Ported from official ADI mbed driver
-------------------------------------------------------------------------------
    Based on Analog Devices Inc. official driver
    Ported for Teensy 4.1 by user request
******************************************************************************/

#ifndef __teensy_ad910x_h__
#define __teensy_ad910x_h__

#include <Arduino.h>
#include <SPI.h>

class AD910x_TEENSY {
    public:
        // Pin definitions for Teensy 4.1
        int csb_pin;      // Chip select pin
        int resetb_pin;   // Reset pin  
        int triggerb_pin; // Trigger pin
 
        /*** Constructor ***/
        AD910x_TEENSY(int CSB = 10, int RESETB = 9, int TRIGGERB = 8);
     
        /*** SPI register addresses (same as official) ***/
        uint16_t reg_add[66] = {0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007, 0x0008, 0x0009, 0x000a, 0x000b, 0x000c, 0x000d, 0x000e, 0x001f, 0x0020, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027, 0x0028, 0x0029, 0x002a, 0x002b, 0x002c, 0x002d, 0x002e, 0x002f, 0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037, 0x003e, 0x003f, 0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0047, 0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057, 0x0058, 0x0059, 0x005a, 0x005b, 0x005c, 0x005d, 0x005e, 0x005f, 0x001e, 0x001d};
    
        // Function to set up SPI
        void spi_init(uint8_t reg_len, uint8_t mode, uint32_t hz);
    
        // SPI write function
        void spi_write(uint16_t addr, int16_t data);
    
        // SPI read function
        int16_t spi_read(uint16_t addr);

        // Function to reset SPI register values
        void AD910x_reg_reset();
        
        // Function to display register data
        void print_data(uint16_t addr, uint16_t data);
    
        // Function to write to SRAM
        void AD910x_update_sram(int16_t data[]);
    
        // Function to display n SRAM data
        void AD910x_print_sram(uint16_t n);
    
        // Function to write to device SPI registers and display updated register values
        void AD910x_update_regs(uint16_t data[]);
          
        // Function to start pattern generation
        void AD910x_start_pattern();
        
        // Function to stop pattern generation
        void AD910x_stop_pattern();
        
    private:
        SPISettings spi_settings;
};

#endif

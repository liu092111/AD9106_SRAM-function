/******************************************************************************
    @file:  teensy_ad910x.cpp
 
    @brief: AD910x driver for Teensy 4.1 - Implementation file
            Ported from official ADI mbed driver
-------------------------------------------------------------------------------
    Based on Analog Devices Inc. official driver
    Ported for Teensy 4.1 by user request
******************************************************************************/

#include "teensy_ad910x.h"

AD910x_TEENSY::AD910x_TEENSY(int CSB, int RESETB, int TRIGGERB) {
    csb_pin = CSB;
    resetb_pin = RESETB;
    triggerb_pin = TRIGGERB;
}

//  * @brief Reset AD910x SPI registers to default values
//  * @param none
//  * @return none
void AD910x_TEENSY::AD910x_reg_reset() {
    digitalWrite(resetb_pin, LOW);
    delayMicroseconds(10);
    digitalWrite(resetb_pin, HIGH);
    delay(1);
}

//  * @brief Print register address and data in hexadecimal format
//  * @param addr - SPI/SRAM register address
//  * @param data - 16-bit data
//  * @return none
void AD910x_TEENSY::print_data(uint16_t addr, uint16_t data) {
    Serial.print("0x");
    Serial.print(addr, HEX);
    Serial.print(", 0x");
    Serial.println(data, HEX);
}

//  * @brief Start pattern generation by setting AD910x trigger pin to 0
//  * @param none
//  * @return none
void AD910x_TEENSY::AD910x_start_pattern() {
    digitalWrite(triggerb_pin, LOW);
}

//  * @brief Stop pattern generation by setting AD910x trigger pin to 1
//  * @param none
//  * @return none
void AD910x_TEENSY::AD910x_stop_pattern() {
    digitalWrite(triggerb_pin, HIGH);
}

//  * @brief Write data to SRAM
//  * @param data[] - array of data to be written to SRAM
//  * @return none
void AD910x_TEENSY::AD910x_update_sram(int16_t data[]) { 
    spi_write(0x001E, 0x0004);
    
    int16_t data_shifted = 0;
    uint16_t sram_add = 0x6000;
    
    Serial.println("Writing SRAM data...");
    for (int i = 0; i < 4096; i++) {
        data_shifted = data[i] << 2;
        spi_write(sram_add + i, data_shifted);
        
        // Show progress every 500 points
        if (i % 500 == 0) {
            Serial.print("Progress: ");
            Serial.print((i * 100) / 4096);
            Serial.println("%");
        }
    }
    
    spi_write(0x001E, 0x0000);
    Serial.println("SRAM data write complete!");
}

//  * @brief Read from SRAM and print data
//  * @param n - number of SRAM addresses to be read from
//  * @return none
void AD910x_TEENSY::AD910x_print_sram(uint16_t n) {
    spi_write(0x001E, 0x000C);
    
    int16_t data_shifted = 0;
    uint16_t sram_add = 0x6000;
    
    Serial.println("Reading SRAM data:");
    for (int i = 0; i < n; i++) {
        data_shifted = spi_read(sram_add + i) >> 2;
        print_data(sram_add + i, data_shifted);
    }
    
    spi_write(0x001E, 0x0000);
}

//  * @brief Write to SPI registers, and read and print new register values
//  * @param data[] - array of data to written to SPI registers
//  * @return none
void AD910x_TEENSY::AD910x_update_regs(uint16_t data[]) {
    uint16_t data_display = 0;
    
    Serial.println("Updating registers:");
    for (int i = 0; i < 66; i++) {
        spi_write(reg_add[i], data[i]);
        data_display = spi_read(reg_add[i]);
        print_data(reg_add[i], data_display);
    }
    Serial.println("Register update complete!");
}

// ********************************************************* //
// SPI FUNCTIONS 
// ********************************************************* //

//  * @brief Set AD910x SPI word length, mode, frequency
//  * @param reg_len - SPI word length
//  * @param mode - SPI clock polarity, clock and data phase
//  * @param hz - SPI bus frequency in hz
//  * @return none
void AD910x_TEENSY::spi_init(uint8_t reg_len, uint8_t mode, uint32_t hz) {
    // Initialize pins
    pinMode(csb_pin, OUTPUT);
    pinMode(resetb_pin, OUTPUT);
    pinMode(triggerb_pin, OUTPUT);
    
    digitalWrite(csb_pin, HIGH);
    digitalWrite(resetb_pin, HIGH);
    digitalWrite(triggerb_pin, HIGH);
    
    // Initialize SPI
    SPI.begin();
    
    // Create SPI settings based on parameters
    uint8_t spi_mode = SPI_MODE0; // Default
    if (mode == 1) spi_mode = SPI_MODE1;
    else if (mode == 2) spi_mode = SPI_MODE2;
    else if (mode == 3) spi_mode = SPI_MODE3;
    
    spi_settings = SPISettings(hz, MSBFIRST, spi_mode);
    
    Serial.print("SPI initialized - Frequency: ");
    Serial.print(hz);
    Serial.print(" Hz, Mode: ");
    Serial.println(mode);
}

//  * @brief Write 16-bit data to AD910x SPI/SRAM register
//  * @param addr - SPI/SRAM address
//  * @param data - data to be written to register address
//  * @return none
void AD910x_TEENSY::spi_write(uint16_t addr, int16_t data) {
    SPI.beginTransaction(spi_settings);
    digitalWrite(csb_pin, LOW);
    
    SPI.transfer16(addr);
    SPI.transfer16(data);
    
    digitalWrite(csb_pin, HIGH);
    SPI.endTransaction();
    delayMicroseconds(1);
}

//  * @brief Read 16-bit data from AD910x SPI/SRAM register
//  * @param addr - SPI/SRAM address
//  * @return reg_data - data returned by AD910x
int16_t AD910x_TEENSY::spi_read(uint16_t addr) {
    SPI.beginTransaction(spi_settings);
    digitalWrite(csb_pin, LOW);
    
    uint16_t read_addr = 0x8000 + addr;
    SPI.transfer16(read_addr);
    int16_t reg_data = SPI.transfer16(0);
    
    digitalWrite(csb_pin, HIGH);
    SPI.endTransaction();
    delayMicroseconds(1);
    
    return reg_data;
}

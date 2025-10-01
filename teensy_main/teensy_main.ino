/*******************************************************************************
    @file:   teensy_main.ino
    
    @brief:  Main program for Teensy AD910x - Arduino sketch
            Ported from official ADI mbed main.cpp
--------------------------------------------------------------------------------
    Based on Analog Devices Inc. official main program
    Ported for Teensy 4.1 by user request
*******************************************************************************/

#include "teensy_ad910x.h"
#include "config.h"  // 使用官方的config.h

// Create device instance
AD910x_TEENSY device_single;

// Function prototypes
void setup_device();
void print_title();
void print_menu();
void prog_example1();
void prog_example2();
void generate_test_data();

void setup() {
    Serial.begin(115200);
    while (!Serial) delay(100);
    
    print_title();
    setup_device();
    generate_test_data();
    
    Serial.println("\nAD9106 Teensy Demo Ready!");
    Serial.println("Send '1' for Gaussian pulse example");
    Serial.println("Send '2' for Ramp wave example");
    Serial.println("Send 's' to stop pattern");
}

void loop() {
    if (Serial.available()) {
        char command = Serial.read();
        
        switch (command) {
            case '1':
                prog_example1();
                break;
            case '2':
                prog_example2();
                break;
            case 's':
                device_single.AD910x_stop_pattern();
                Serial.println("Pattern stopped.");
                break;
            default:
                print_menu();
                break;
        }
    }
    
    delay(100);
}

void setup_device() {
    device_single.spi_init(16, 0, 1000000);  // 16-bit, mode 0, 1MHz
    device_single.AD910x_reg_reset();
    Serial.println("Device initialized successfully!");
}

void print_title() {
    Serial.println("\n***********************************************************************");
    Serial.println("* EVAL-AD9106 Teensy 4.1 Demonstration Program                       *");
    Serial.println("*                                                                     *");
    Serial.println("* This program demonstrates SRAM waveform generation with AD9106     *");
    Serial.println("* using Teensy 4.1 instead of SDP-K1 controller board.              *");
    Serial.println("* Ported from official ADI mbed driver.                              *");
    Serial.println("***********************************************************************");
}

void print_menu() {
    Serial.println("\nAvailable Commands:");
    Serial.println("  1 - 4 Gaussian Pulses with Different Start Delays");
    Serial.println("  2 - 4 Pulses Generated from SRAM Ramp Vector");
    Serial.println("  s - Stop pattern generation");
    Serial.println("Select an option:");
}

void prog_example1() {
    Serial.println("\n4 Gaussian Pulses with Different Start Delays and Digital Gain Settings");
    device_single.AD910x_update_sram(example1_RAM_gaussian);
    device_single.AD910x_update_regs(AD9106_example1_regval);
    device_single.AD910x_start_pattern();
    Serial.println("Gaussian pulse pattern started!");
}

void prog_example2() {
    Serial.println("\n4 Pulses Generated from an SRAM Ramp Vector");
    device_single.AD910x_update_sram(example2_4096_ramp);
    device_single.AD910x_update_regs(AD9106_example2_regval);
    device_single.AD910x_start_pattern();
    Serial.println("Ramp wave pattern started!");
}

void generate_test_data() {
    Serial.println("Generating test waveform data...");
    
    // Generate Gaussian-like pulse (simplified)
    for (int i = 0; i < 4096; i++) {
        if (i < 1000) {
            example1_RAM_gaussian[i] = 0;
        } else if (i < 2000) {
            // Rising edge
            example1_RAM_gaussian[i] = (i - 1000) * 4;
        } else if (i < 3000) {
            // Falling edge
            example1_RAM_gaussian[i] = 4095 - (i - 2000) * 4;
        } else {
            example1_RAM_gaussian[i] = 0;
        }
        
        // Clamp to 12-bit range
        if (example1_RAM_gaussian[i] > 4095) example1_RAM_gaussian[i] = 4095;
        if (example1_RAM_gaussian[i] < 0) example1_RAM_gaussian[i] = 0;
    }
    
    // Generate ramp wave
    for (int i = 0; i < 4096; i++) {
        example2_4096_ramp[i] = i - 2048;  // -2048 to +2047
    }
    
    Serial.println("Test data generation complete!");
}

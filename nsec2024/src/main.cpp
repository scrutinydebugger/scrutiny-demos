//    main.cpp
//        NorthSec 2024 demo. 
//        Scrutiny demonstration used during nsec2024 talk
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-demos)
//
//   Copyright (c) 2024 Scrutiny Debugger

#include <stdio.h>
#include <Arduino.h>
#include "scrutiny.hpp"
#include "scrutiny_integration.hpp"
#include "bno055.hpp"
#include "math.h"

#include "twi.h"


constexpr uint8_t BNO055_I2C_ADDR{ 0x28 };
constexpr uint8_t PIN_BNO055_RESET{ 7 };
constexpr uint32_t MILLION{ 1000000 };
constexpr uint32_t TASK_100HZ_TIME_US{ MILLION/100 };
constexpr uint32_t TASK_20HZ_TIME_US{ MILLION/20 };
constexpr uint32_t TASK_1HZ_TIME_US{ MILLION/1 };

BNO055::Driver bno055;

void twi_slave_tx_callback(void){
}

void twi_slave_rx_callback(uint8_t* data, int len){
    (void)data;
    (void)len;
}

void twi_master_rx_callback(uint8_t* data, int len){
    digitalWrite(A2, 1);
    bno055.twi_rx_callback(data, len);
    digitalWrite(A2, 0);
}

void twi_master_tx_callback(void){
    digitalWrite(A3, 1);
    bno055.twi_tx_callback();
    digitalWrite(A3, 0);
}

void func(float timestep)
{
    static volatile float sinewave = 0;
    static volatile float sinewave_freq = 0;
    static volatile float sinewave_phase = 0;

    sinewave_phase += 2.0f * M_PI * sinewave_freq * timestep;
    if (sinewave_phase >= 2.0f * M_PI)
    {
        sinewave_phase -= floor(sinewave_phase / 2.0f / M_PI) * 2.0f * M_PI;
    }
    sinewave = sin(sinewave_phase);
    static_cast<void>(sinewave);
}


void setup() {
    Serial.begin(120200);
    
    twi_init();
    twi_attachSlaveTxEvent(twi_slave_tx_callback); // default callback must exist
    twi_attachSlaveRxEvent(twi_slave_rx_callback); // default callback must exist
    twi_attachMasterRxEvent(twi_master_rx_callback); // default callback must exist
    twi_attachMasterTxEvent(twi_master_tx_callback); // default callback must exist
    twi_setFrequency(400000);

    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(PIN_BNO055_RESET, OUTPUT);     
    pinMode(A0, OUTPUT);     
    pinMode(A1, OUTPUT);     
    pinMode(A2, OUTPUT);     
    pinMode(A3, OUTPUT);     
    pinMode(A4, OUTPUT);     
    digitalWrite(PIN_BNO055_RESET, HIGH);  // Remove the reset signal
    nsec2024_demo_config_scrutiny();
    
    bno055.init(BNO055_I2C_ADDR);
    bno055.wait_ready(1000);
    bno055.read_info(); 
}

void task_100hz(){
    static volatile uint32_t var_100hz=0;
    digitalWrite(A1, 1);
    var_100hz++;
    bno055.initiate_interrupt_read(BNO055::Driver::InterruptReadMode::SINGLE);
    func(0.01);
    task_100hz_loop_handler.process();
    digitalWrite(A1, 0);
}

void task_1hz(){
    static volatile uint32_t var_1hz=0;
    static int led_state = LOW;
    var_1hz++;

    digitalWrite(LED_BUILTIN, led_state);
    led_state = (led_state == LOW) ? HIGH : LOW;
}

void task_20hz(){
    static volatile uint32_t task_20hz_counter=0;
    static volatile bool task_20hz_counter_enable=false;
    if (task_20hz_counter_enable){
        task_20hz_counter++;
    }
    task_20hz_loop_handler.process();
}


void loop() {
    static uint32_t last_timestamp_us = micros();
    static uint32_t last_timestamp_task_1hz_us = micros();
    static uint32_t last_timestamp_task_20hz_us = micros();
    static uint32_t last_timestamp_task_100hz_us = micros();

    uint32_t const timestamp_us = micros();

    if (timestamp_us - last_timestamp_task_100hz_us >= TASK_100HZ_TIME_US){
        task_100hz();   // Could be in a different time domain (thread)
        last_timestamp_task_100hz_us = timestamp_us;
    }

    if (timestamp_us - last_timestamp_task_20hz_us >= TASK_20HZ_TIME_US){
        task_20hz();     // Could be in a different time domain (thread)
        last_timestamp_task_20hz_us = timestamp_us;
    }

    if (timestamp_us - last_timestamp_task_1hz_us >= TASK_1HZ_TIME_US){
        task_1hz();     // Could be in a different time domain (thread)
        last_timestamp_task_1hz_us = timestamp_us;
    }

    bno055.process();   // Starts a new reading if the previous is complete

    uint32_t const timediff_100ns { (timestamp_us - last_timestamp_us) * 10};
    task_idle_loop_handler.process(timediff_100ns); // Variable Frequency loop. Need to provide the timestep
    nsec2024_demo_update_scrutiny_main(timediff_100ns);
    last_timestamp_us = timestamp_us;
}
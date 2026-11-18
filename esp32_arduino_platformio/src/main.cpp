//    main.cpp
//        ESP32 Arduino PlatformIO Demo for Scrutiny Debugger
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-demos)

#include <Arduino.h>
#include "scrutiny_integration.hpp"

// Global demo variables to inspect via Scrutiny VarMap
uint32_t g_cycle_counter = 0;
float g_sine_wave = 0.0f;
bool g_heartbeat = false;
uint16_t g_analog_read = 0;

void setup()
{
    Serial.begin(115200);

    // Configure Scrutiny
    esp32_demo_config_scrutiny();
}

void loop()
{
    static unsigned long last_time = micros();
    unsigned long now = micros();
    unsigned long elapsed_us = now - last_time;
    last_time = now;

    // Simulate demo telemetry
    g_cycle_counter++;
    g_sine_wave = sinf((float)millis() / 1000.0f);
    g_heartbeat = (millis() / 500) % 2 == 0;
    g_analog_read = (uint16_t)((sinf((float)millis() / 2000.0f) + 1.0f) * 2047.5f);

    // Update Scrutiny (time step in 100ns units: 1us = 10 units)
    esp32_demo_update_scrutiny(elapsed_us * 10);
}

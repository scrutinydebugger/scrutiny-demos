#include "bno055.hpp"
#include <stdint.h>
#include <Arduino.h>

extern "C"{
    #include "twi.h"
}

namespace BNO055{

    Driver::Driver() : 
        m_i2c_addr{0},
        m_last_error_code{0},
        m_error{Driver::Error::NO_INIT},
        m_double_buffer_flag{false},
        m_interrupt_read_state{InterruptReadState::IDLE},
        m_acc{ 0,0,0 },
        m_gyro{ 0,0,0 },
        m_mag{ 0,0,0 }
    {
    }

    void Driver::twi_rx_callback(uint8_t *data, int len){
        uint8_t const write_index = db_get_write_index();
        if (!m_i2c_data_available){
            memcpy((uint8_t*)m_i2c_rx_buffer[write_index], data, len);
            m_i2c_data_available = true;
            m_i2c_data_len = len;
            db_swap_buffers();
        }
    }

    void Driver::twi_tx_callback(){

        if (m_interrupt_read_state == InterruptReadState::READ_ACCEL
            || m_interrupt_read_state == InterruptReadState::READ_GYRO
            || m_interrupt_read_state == InterruptReadState::READ_MAG
            ){
            // size = 6. SendStop = 1. Block = 0.
            twi_readFrom(m_i2c_addr, nullptr, 6, 1, 0);    // Non blocking read does not require a buffer
        }

    }

    void Driver::process(){
        if (m_i2c_data_available)
        {
            volatile uint8_t * readbuf = m_i2c_rx_buffer[db_get_read_index()];
            if (m_interrupt_read_state == InterruptReadState::READ_ACCEL){
                if (m_i2c_data_len != 6){
                    m_interrupt_read_state = InterruptReadState::ERROR;
                } else {
                    m_acc.x = make_i16(readbuf[0], readbuf[1]);
                    m_acc.y = make_i16(readbuf[2], readbuf[3]);
                    m_acc.z = make_i16(readbuf[4], readbuf[5]);
                    initiate_read_gyro();
                }
            } else if (m_interrupt_read_state == InterruptReadState::READ_GYRO){
                if (m_i2c_data_len != 6){
                    m_interrupt_read_state = InterruptReadState::ERROR;
                } else {
                    m_gyro.x = make_i16(readbuf[0], readbuf[1]);
                    m_gyro.y = make_i16(readbuf[2], readbuf[3]);
                    m_gyro.z = make_i16(readbuf[4], readbuf[5]);
                    //initiate_read_mag();
                    if (m_interrupt_read_mode == InterruptReadMode::CONTINUOUS){
                        initiate_read_accel();
                    } else {
                        m_interrupt_read_state = InterruptReadState::IDLE;
                    }
                }
            } 
            /*
            else if (m_interrupt_read_state == InterruptReadState::READ_MAG){
                if (m_i2c_data_len != 6){
                    m_interrupt_read_state = InterruptReadState::ERROR;
                } else {
                    m_mag.x = make_i16(readbuf[0], readbuf[1]);
                    m_mag.y = make_i16(readbuf[2], readbuf[3]);
                    m_mag.z = make_i16(readbuf[4], readbuf[5]);
                    
                    if (m_interrupt_read_mode == InterruptReadMode::CONTINUOUS){
                        initiate_read_accel();
                    } else {
                        m_interrupt_read_state = InterruptReadState::IDLE;
                    }
                }
            }
            */
            m_i2c_data_available = false;
        }
    }


    void Driver::init(uint8_t i2c_addr)
    {
        m_i2c_addr = i2c_addr;
        m_error = Driver::Error::NOT_READY;
        m_last_error_code=0;
        m_interrupt_read_state = InterruptReadState::IDLE;

        m_chip_info = ChipInfo{0,0,0,0,0};
        m_acc.x = m_acc.y = m_acc.z = 0;
        m_gyro.x = m_gyro.y = m_gyro.z = 0;
        m_mag.x = m_mag.y = m_mag.z = 0;
        m_i2c_data_available = false;
        m_i2c_data_len = 0;
    }

    bool Driver::wait_ready(uint32_t const timeout_ms) {
        uint32_t tstart=millis();
        uint8_t chip_id = 0;

        if (m_interrupt_read_state != InterruptReadState::IDLE){
            set_error(Error::INTERRUPT_READ_ENABLED);
        }

        if (! (m_error == Error::NO_ERROR || m_error==Error::NOT_READY) ){
            goto wait_ready_err;
        }

        while (true) {
            // Read may fail here. It is not a hard error.
            if (read_register(RegisterAddress_Page0::CHIP_ID, &chip_id) && chip_id == CHIP_ID){
                break;
            }
            delay(10);
            if (millis() - tstart >= timeout_ms){
                goto wait_ready_err;
            }
        }
        
        // Wait for end of of system init and end of POST
        while (true) {
            if (!read_register(RegisterAddress_Page0::SYS_STATUS, (uint8_t*)&m_sys_status_at_boot)){
                goto wait_ready_err;
            }
            if (m_sys_status_at_boot == Registers::SYS_STATUS::SystemIdle
            || m_sys_status_at_boot == Registers::SYS_STATUS::SystemError){
                break;
            }
            delay(10);
            if (millis() - tstart >= timeout_ms){
                goto wait_ready_err;
            }
        }

        if (m_sys_status_at_boot == Registers::SYS_STATUS::SystemError){
            read_register(BNO055::RegisterAddress_Page0::SYS_ERR, (uint8_t*)&m_sys_error_at_boot);
            goto wait_ready_err;
        }

        
        // Set Accelerometer / Magnetometer / Gyro mode
        if (!write_register(RegisterAddress_Page0::OPR_MODE, OperatingMode::CONFIGMODE)){
            goto wait_ready_err;
        }
        delay(20);  // 17ms per datasheet

        // Unit selection
        Registers::UNIT_SEL unit_reg;
        unit_reg.val=0;
        unit_reg.bits.ACC_Unit = AccelerationUnits::MilligG;
        unit_reg.bits.EUL_Unit = EulerUnits::Degrees;
        unit_reg.bits.GYR_Unit = AngularRateUnits::DegreesPerSec;
        unit_reg.bits.ORI_Android_Windows = OrientationMode::Android;
        unit_reg.bits.TEMP_Unit = TemperatureUnit::Celsius;
        if (!write_register(RegisterAddress_Page0::UNIT_SEL, unit_reg.val)){
            goto wait_ready_err;
        }

        // Power mode
        Registers::PWR_MODE power_mode_reg;
        power_mode_reg.val=0;
        power_mode_reg.bits.Power_Mode = PowerMode::NormalMode;
        if (!write_register(RegisterAddress_Page0::PWR_MODE, power_mode_reg.val)){
            goto wait_ready_err;
        }

        // Switch to page 1
        if (!write_register(PAGE_ID_ADDR, 1)){
            goto wait_ready_err;
        }

        // Power mode
        Registers::ACC_Config acc_config;
        acc_config.val=0;
        acc_config.bits.ACC_BW = AccelerometerBandwidth::BW_15_63Hz;
        acc_config.bits.ACC_PWR_Mode = AccelerometerPowerMode::Normal;
        acc_config.bits.ACC_Range = AccelerometerGRange::Range_4G;
        if (!write_register(RegisterAddress_Page1::ACC_Config, acc_config.val)){
            goto wait_ready_err;
        }

        Registers::GYR_Config_0 gyr_config0;
        gyr_config0.val=0;
        gyr_config0.bits.GYR_Bandwidth = GyroscopeBandwidth::BW_12Hz;
        gyr_config0.bits.GYR_Range = GyroscopeRange::Range_500dps;
        if (!write_register(RegisterAddress_Page1::GYR_Config_0, gyr_config0.val)){
            goto wait_ready_err;
        }

        Registers::GYR_Config_1 gyr_config1;
        gyr_config1.val=0;
        gyr_config1.bits.GYR_Power_Mode = GyroscopeOperationMode::Normal;   // PowerMode = OperationMode. Following datasheet naming
        if (!write_register(RegisterAddress_Page1::GYR_Config_1, gyr_config1.val)){
            goto wait_ready_err;
        }

        Registers::MAG_Config mag_config;
        mag_config.val=0;
        mag_config.bits.MAG_Power_Nmode = MagnetometerPowerMode::Normal;
        mag_config.bits.MAG_OPR_Mode = MagnetometerOperationMode::Regular;
        mag_config.bits.MAG_Data_output_rate = MagnetometerDataOutputRate::Rate_15Hz;
        if (!write_register(RegisterAddress_Page1::MAG_Config, mag_config.val)){
            goto wait_ready_err;
        }

        if (!write_register(PAGE_ID_ADDR, 0)){
            goto wait_ready_err;
        }

        // Set Accelerometer / Magnetometer / Gyro mode
        if (!write_register(RegisterAddress_Page0::OPR_MODE, OperatingMode::AMG)){
            goto wait_ready_err;
        }
        delay(10); // 7ms per datasheet
        
        // Wait for sensors to start
        while (true) {
            if (!read_register(RegisterAddress_Page0::SYS_STATUS, (uint8_t*)&m_sys_status_at_boot)){
                goto wait_ready_err;
            }
            if (m_sys_status_at_boot != Registers::SYS_STATUS::SystemIdle)
            {
                break;
            }
            delay(10);
            if (millis() - tstart >= timeout_ms){
                goto wait_ready_err;
            }
        }

        if (m_sys_status_at_boot == Registers::SYS_STATUS::SystemError){
            if (!read_register(BNO055::RegisterAddress_Page0::SYS_ERR, (uint8_t*)&m_sys_error_at_boot)){
                goto wait_ready_err;
            };
        }

        clear_error();
        digitalWrite(A4, 1);
        return true;

    wait_ready_err:
        twi_releaseBus();
        return false;
    }


    bool Driver::write_register(uint8_t const reg, uint8_t const val, bool block, uint32_t timeout_us)  {
        uint8_t data[2] { reg, val};
        twi_setTimeoutInMicros(timeout_us, 0);
        m_last_error_code = twi_writeTo(m_i2c_addr, data, sizeof(data), (int) block, 1);
        return (m_last_error_code == 0);
    }
    
    bool Driver::read_register(uint8_t const reg, uint8_t *val, uint32_t timeout_us) {
        return read_registers(reg, val, 1, timeout_us);
    }

    bool Driver::read_registers(uint8_t const reg, uint8_t* buffer, int size, uint32_t timeout_us)  {
        uint8_t qty_read;
        twi_setTimeoutInMicros(timeout_us, 0);
        m_last_error_code = twi_writeTo(m_i2c_addr, const_cast<uint8_t*>(&reg), 1, 1, 1);
        if (m_last_error_code != 0){
            goto read_registers_err;
        }
        qty_read = twi_readFrom(m_i2c_addr, buffer, size, 1, 1);
        if (qty_read != size){
            goto read_registers_err;
        }
        return true;

    read_registers_err:
        twi_releaseBus();
        return false;
    }

    void Driver::read_info(){
        if (m_interrupt_read_state != InterruptReadState::IDLE){
            set_error(Error::INTERRUPT_READ_ENABLED);
        }
        if (is_error()){
            return;
        }
        uint8_t buffer[6];
        write_register(PAGE_ID_ADDR, 0);
        if (read_registers(RegisterAddress_Page0::ACC_ID, buffer, 6, 5000)){
            m_chip_info.acc_chip_id = buffer[0];
            m_chip_info.mag_chip_id = buffer[1];
            m_chip_info.gyro_chip_id = buffer[2];
            m_chip_info.sw_revision = buffer[3];
            m_chip_info.sw_revision |= (static_cast<uint16_t>(buffer[4]) << 8);
            m_chip_info.bootloader_version = buffer[5];
        } else{
            set_error(Error::FAILED_READ_INFO);
        }

    }

    XYZ<int16_t> Driver::get_accel() {
        return m_acc;   // Return a copy
    }

    XYZ<int16_t> Driver::get_gyro() {
        return m_gyro;  // Return a copy
    }

    XYZ<int16_t> Driver::get_mag() { 
        return m_mag;   // Return a copy
    }

    void Driver::initiate_interrupt_read(InterruptReadMode const mode){
        if (m_error == Error::NO_ERROR && m_interrupt_read_state == InterruptReadState::IDLE){
            m_interrupt_read_mode = mode;
            m_double_buffer_flag = false;
            initiate_read_accel();
        }
    }

    void Driver::stop_interrupt_read(){
        if (m_interrupt_read_state != InterruptReadState::IDLE){
            twi_releaseBus();
            m_interrupt_read_state=InterruptReadState::IDLE;
        }
    }

    void Driver::initiate_read_accel(){
        m_interrupt_read_state = InterruptReadState::READ_ACCEL;
        uint8_t data[1] = {BNO055::RegisterAddress_Page0::ACC_DATA_X_LSB};
        twi_writeTo(m_i2c_addr, data, 1, 0, 0);
    }

    void Driver::initiate_read_gyro(){
        m_interrupt_read_state = InterruptReadState::READ_GYRO;
        uint8_t data[1] = {BNO055::RegisterAddress_Page0::GYR_DATA_X_LSB};
        twi_writeTo(m_i2c_addr, data, 1, 0, 0);
    }

    void Driver::initiate_read_mag(){
        m_interrupt_read_state = InterruptReadState::READ_MAG;
        uint8_t data[1] = {BNO055::RegisterAddress_Page0::MAG_DATA_X_LSB};
        twi_writeTo(m_i2c_addr, data, 1, 0, 0);
    }
}

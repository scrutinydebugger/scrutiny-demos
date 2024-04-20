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
        m_continuous_read_state{ContinuousReadState::IDLE},
        m_continuous_read_buffer{0U},
        m_continuous_read_size{0U},
        m_acc{ XYZ<uint16_t>{0,0,0}, XYZ<uint16_t>{0,0,0}},
        m_gyro{ XYZ<uint16_t>{0,0,0}, XYZ<uint16_t>{0,0,0}},
        m_mag{ XYZ<uint16_t>{0,0,0}, XYZ<uint16_t>{0,0,0}}
    {
    }

    void Driver::twi_rx_callback(uint8_t *data, int len){
        uint8_t const write_index = db_get_write_index();
        if (m_continuous_read_state == ContinuousReadState::READ_ACCEL){
            if (len != 6){
                m_continuous_read_state = ContinuousReadState::ERROR;
            } else {
                m_acc[write_index].x = make_u16(data[0], data[1]);
                m_acc[write_index].y = make_u16(data[2], data[3]);
                m_acc[write_index].z = make_u16(data[4], data[5]);
                initiate_read_gyro();
            }
        } else if (m_continuous_read_state == ContinuousReadState::READ_GYRO){
            if (len != 6){
                m_continuous_read_state = ContinuousReadState::ERROR;
            } else {
                m_gyro[write_index].x = make_u16(data[0], data[1]);
                m_gyro[write_index].y = make_u16(data[2], data[3]);
                m_gyro[write_index].z = make_u16(data[4], data[5]);
                initiate_read_mag();
            }
        } else if (m_continuous_read_state == ContinuousReadState::READ_MAG){
            if (len != 6){
                m_continuous_read_state = ContinuousReadState::ERROR;
            } else {
                m_mag[write_index].x = make_u16(data[0], data[1]);
                m_mag[write_index].y = make_u16(data[2], data[3]);
                m_mag[write_index].z = make_u16(data[4], data[5]);
                initiate_read_accel();
            }
        }

        if (m_continuous_read_state != ContinuousReadState::ERROR){
            db_swap_buffers();
        }
    }

    void Driver::twi_tx_callback(){

        if (m_continuous_read_state == ContinuousReadState::READ_ACCEL
            || m_continuous_read_state == ContinuousReadState::READ_GYRO
            || m_continuous_read_state == ContinuousReadState::READ_MAG
            ){
            // size = 6. SendStop = 1. Block = 0.
            twi_readFrom(m_i2c_addr, const_cast<uint8_t*>(m_continuous_read_buffer), 6, 1, 0);    
        }

    }


    void Driver::init(uint8_t i2c_addr)
    {
        m_i2c_addr = i2c_addr;
        m_error = Driver::Error::NOT_READY;
        m_last_error_code=0;
        m_continuous_read_state = ContinuousReadState::IDLE;
        m_continuous_read_size=0;

        m_chip_info = ChipInfo{0,0,0,0,0};
        for (int i=0; i<2; i++){
            m_acc[i].x = m_acc[i].y = m_acc[i].z = 0;
            m_gyro[i].x = m_gyro[i].y = m_gyro[i].z = 0;
            m_mag[i].x = m_mag[i].y = m_mag[i].z = 0;
        }
    }

    bool Driver::wait_ready(uint32_t const timeout_ms) {
        uint32_t tstart=millis();
        uint8_t chip_id = 0;

        if (m_continuous_read_state != ContinuousReadState::IDLE){
            set_error(Error::CONTINUOUS_READ_ENABLED);
        }

        if (m_error != Error::NO_ERROR){
            goto wait_ready_err;
        }

        while (true) {
            if (read_register(RegisterAddress_Page0::CHIP_ID, &chip_id) && chip_id == CHIP_ID){
                break;
            }
            delay(10);
            if (millis() - tstart >= timeout_ms){
                goto wait_ready_err;
            }
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
        write_register(RegisterAddress_Page0::UNIT_SEL, unit_reg.val);

        // Power mode
        Registers::PWR_MODE power_mode_reg;
        power_mode_reg.val=0;
        power_mode_reg.bits.Power_Mode = PowerMode::NormalMode;
        write_register(RegisterAddress_Page0::PWR_MODE, power_mode_reg.val, true);

        // Set Accelerometer / Magnetometer / Gyro mode
        if (!write_register(RegisterAddress_Page0::OPR_MODE, OperatingMode::AMG)){
            goto wait_ready_err;
        }
        delay(10); // 7ms per datasheet
        
        clear_error();
        return true;

    wait_ready_err:
        twi_releaseBus();
        return false;
    }


    bool Driver::write_register(uint8_t const reg, uint8_t const val, bool block, uint32_t timeout_us)  {
        uint8_t data[2] { reg, val};
        twi_setTimeoutInMicros(timeout_us, true);
        m_last_error_code = twi_writeTo(m_i2c_addr, data, sizeof(data), (int) block, 1);
        return (m_last_error_code == 0);
    }
    
    bool Driver::read_register(uint8_t const reg, uint8_t *val, bool block, bool timeout_us) {
        return read_registers(reg, val, 1, block, timeout_us);
    }

    bool Driver::read_registers(uint8_t const reg, uint8_t* buffer, int size, bool block, uint32_t timeout_us)  {
        uint8_t qty_read;
        twi_setTimeoutInMicros(timeout_us, 1);
        m_last_error_code = twi_writeTo(m_i2c_addr, const_cast<uint8_t*>(&reg), 1, (int) block, 0);
        if (m_last_error_code != 0){
            goto read_registers_err;
        }
        qty_read = twi_readFrom(m_i2c_addr, buffer, size, 1, (int)block);
        if (block && qty_read != size){
            goto read_registers_err;
        }

read_registers_err:
        twi_releaseBus();
        return false;
    }

    void Driver::read_info(){
        if (m_continuous_read_state != ContinuousReadState::IDLE){
            set_error(Error::CONTINUOUS_READ_ENABLED);
        }
        if (is_error()){
            return;
        }
        uint8_t buffer[6];
        write_register(PAGE_ID_ADDR, 0);
        if (read_registers(RegisterAddress_Page0::ACC_ID, buffer, 6, true, 5000)){
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

    XYZ<uint16_t> Driver::get_accel() volatile {
        // Return a copy. Index is read once, so no race conditions should happen.
        return m_acc[db_get_read_index()];
    }

    XYZ<uint16_t> Driver::get_gyro() volatile {
        // Return a copy. Index is read once, so no race conditions should happen.
        return m_gyro[db_get_read_index()];
    }

    XYZ<uint16_t> Driver::get_mag() volatile {
        // Return a copy. Index is read once, so no race conditions should happen.
        return m_mag[db_get_read_index()];
    }

    void Driver::initiate_continuous_read(){
        m_double_buffer_flag = false;
        initiate_read_accel();
    }

    void Driver::stop_continuous_read(){
        twi_releaseBus();
        m_continuous_read_state=ContinuousReadState::IDLE;
    }

    void Driver::initiate_read_accel(){
        m_continuous_read_state = ContinuousReadState::READ_ACCEL;
        uint8_t data[1] = {BNO055::RegisterAddress_Page0::ACC_DATA_X_LSB};
        twi_writeTo(m_i2c_addr, data, 1, 0, 0);
    }

    void Driver::initiate_read_gyro(){
        m_continuous_read_state = ContinuousReadState::READ_GYRO;
        uint8_t data[1] = {BNO055::RegisterAddress_Page0::GYR_DATA_X_LSB};
        twi_writeTo(m_i2c_addr, data, 1, 0, 0);
    }

    void Driver::initiate_read_mag(){
        m_continuous_read_state = ContinuousReadState::READ_MAG;
        uint8_t data[1] = {BNO055::RegisterAddress_Page0::MAG_DATA_X_LSB};
        twi_writeTo(m_i2c_addr, data, 1, 0, 0);
    }
}

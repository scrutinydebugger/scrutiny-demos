#ifndef BNO055_HPP
#define BNO055_HPP

#include <stdint.h>

#include "twi.h"


namespace BNO055{
    constexpr uint8_t CHIP_ID{ 0xA0 };
    constexpr uint8_t PAGE_ID_ADDR{ 0x07 };
    namespace RegisterAddress_Page0 {
        enum Vals{
            AXIS_MAP_SIGN = 0x42,
            AXIS_MAP_CONFIG = 0x41,
            TEMP_SOURCE = 0x40,
            SYS_TRIGGER = 0x3F,
            PWR_MODE = 0x3E,
            OPR_MODE = 0x3D,
            UNIT_SEL = 0x3B,
            SYS_ERR = 0x3A,
            
            SYS_STATUS = 0x39,
            SYS_CLK_STATUS = 0x38,
            INT_STA = 0x37,
            ST_RESULT = 0x36,
            CALIB_STAT = 0x35,
            TEMP = 0x34,
            GRV_Data_Z_MSB = 0x33,
            GRV_Data_Z_LSB = 0x32,
            GRV_Data_Y_MSB = 0x31,
            GRV_Data_Y_LSB = 0x30,
            GRV_Data_X_MSB = 0x2F,
            GRV_Data_X_LSB = 0x2E,
            LIA_Data_Z_MSB = 0x2D,
            LIA_Data_Z_LSB = 0x2C,
            LIA_Data_Y_MSB = 0x2B,
            LIA_Data_Y_LSB = 0x2A,
            LIA_Data_X_MSB = 0x29,
            LIA_Data_X_LSB = 0x28,
            QUA_Data_z_MSB = 0x27,
            QUA_Data_z_LSB = 0x26,
            QUA_Data_y_MSB = 0x25,
            QUA_Data_y_LSB = 0x24,
            QUA_Data_x_MSB = 0x23,
            QUA_Data_x_LSB = 0x22,
            QUA_Data_w_MSB = 0x21,
            QUA_Data_w_LSB = 0x20,
            EUL_Pitch_MSB = 0x1F,
            EUL_Pitch_LSB = 0x1E,
            EUL_Roll_MSB = 0x1D,
            EUL_Roll_LSB = 0x1C,
            EUL_Heading_MSB = 0x1B,
            EUL_Heading_LSB = 0x1A,
            GYR_DATA_Z_MSB = 0x19,
            GYR_DATA_Z_LSB = 0x18,
            GYR_DATA_Y_MSB = 0x17,
            GYR_DATA_Y_LSB = 0x16,
            GYR_DATA_X_MSB = 0x15,
            GYR_DATA_X_LSB = 0x14,
            MAG_DATA_Z_MSB = 0x13,
            MAG_DATA_Z_LSB = 0x12,
            MAG_DATA_Y_MSB = 0x11,
            MAG_DATA_Y_LSB = 0x10,
            MAG_DATA_X_MSB = 0xF,
            MAG_DATA_X_LSB = 0xE,
            ACC_DATA_Z_MSB = 0xD,
            ACC_DATA_Z_LSB = 0xC,
            ACC_DATA_Y_MSB = 0xB,
            ACC_DATA_Y_LSB = 0xA,
            ACC_DATA_X_MSB = 0x9,
            ACC_DATA_X_LSB = 0x8,
            Page_ID = 0x7,
            BL_Rev_ID = 0x6,
            SW_REV_ID_MSB = 0x5,
            SW_REV_ID_LSB = 0x4,
            GYR_ID = 0x3,
            MAG_ID = 0x2,
            ACC_ID = 0x1,
            CHIP_ID  = 0x0,
        };
    }

    namespace RegisterAddress_Page1 {
        enum Vals{
            GYR_AM_SET = 0x1F,
            GYR_AM_THRES = 0x1E,
            GYR_DUR_Z = 0x1D,
            GYR_HR_Z_SET = 0x1C,
            GYR_DUR_Y = 0x1B,
            GYR_HR_Y_SET = 0x1A,
            GYR_DUR_X = 0x19,
            GYR_HR_X_SET = 0x18,
            GYR_INT_SETING = 0x17,
            ACC_NM_SET = 0x16,
            ACC_NM_THRE = 0x15,
            ACC_HG_THRES = 0x14,
            ACC_HG_DURATION = 0x13,
            ACC_INT_Set = 0x12,
            ACC_AM_THRES = 0x11,
            INT_EN = 0x10,
            INT_MSK = 0xF,
            GYRO_AME = 0xE,
            GYR_Sleep_Config = 0xD,
            ACC_Sleep_Config = 0xC,
            GYR_Config_1 = 0xB,
            GYR_Config_0 = 0xA,
            MAG_Config = 0x9,
            ACC_Config = 0x8,
            Page_ID = 0x7
        };
    }


    namespace CalibrationStatus{
        enum Vals{
            NotCalibrated=0,
            Invalid1=1,
            Invalid2=2,
            FullyCalibrated=3
        };
    }

    namespace SelfTestResult{
        enum Vals{
            Failed=0,
            Passed=1
        };
    }

    namespace InterruptStatus{
        enum Vals{
            NotTriggered=0,
            Triggered=1
        };
    }

    namespace InterruptEnable{
        enum Vals{
            Disabled=0,
            Enabled=1
        };
    }

    namespace ClockCalibStatus{
        enum Vals{
            FreeToConfigure=0,
            CalibrationInProgress=1
        };
    }

    namespace OrientationMode{
        enum Vals{
            Windows = 0,
            Android = 1
        };
    }
    
    namespace TemperatureUnit{
        enum Vals{
            Celsius = 0,
            Fahrenheit = 1
        };
    }
    
    namespace EulerUnits{
        enum Vals{
            Degrees = 0,
            Radians = 1
        };
    }
    
    namespace AngularRateUnits{
        enum Vals{
            DegreesPerSec = 0,
            RadiansPerSec = 1
        };
    }
        
    namespace AccelerationUnits{
        enum Vals{
            MeterPerSec2 = 0,
            MilligG = 1
        };
    }
    
    namespace MotionInterruptMode{
        enum Vals{
            NoMotion = 0,
            SlowMotion = 1
        };
    }

    namespace SystemStatusCode{
        enum Vals{
            SystemIdle=0,
            SystemError=1,
            InitializingPeripherals=2,
            SystemInitialization=3,
            ExecutingSelfTest=4,
            SensorFusionAlgorithmRunning=5,
            SystemRunningWithoutSensorFusion=6
        };
    }

    namespace SystemErrorCode{
        enum Vals{
            NoError=0,
            PeripheralInitError=1,
            SystemInitError=2,
            SelfTestFailed=3,
            RegisterMapValueOOR=4,
            RegisterMapAddrOOR=5,
            RegisterMapWriteError=6,
            LowPowerNotAvailableForSelectedOM=7,
            AccelerometerPowerModeNotAvailable=8,
            FusionAlgorithmError=9,
            SensorConfigurationError=0xA,
        };
    }

    namespace OperatingMode{
        enum Vals{
            CONFIGMODE = 0,
            ACCONLY = 1,
            MAGONLY = 2,
            GYROONLY = 3,
            ACCMAG = 4,
            ACCGYRO = 5,
            MAGGYRO = 6,
            AMG = 7,
            IMU = 8,
            COMPASS = 9,
            M4G = 0xA,
            NDOF_FMC_OFF = 0xB,
            NDOF = 0xC
        };
    }

    namespace AxisRepresentation{
        enum Vals{
            XAxis = 0,
            YAxis = 1,
            ZAxis = 2,
            Invalid = 3
        };
    }

    namespace AxisSign{
        enum Vals{
            Positive = 0,
            Negative = 1
        };  
    }

    namespace TempSource{
        enum Vals{
            Accelerometer = 0,
            Gyroscope = 1,
            Invalid2 = 2,
            Invalid3 = 3
        };
    }

    namespace AccelerometerPowerMode {
        enum Vals{
            Normal = 0,
            Suspend = 1,
            LowPower1 = 2,
            Standby = 3,
            LowPower2 = 4,
            DeepSuspend = 5,
            Invalid6= 6,
            Invalid7 = 7
        };
    }

    namespace AccelerometerBandwidth {
        enum Vals{
            BW_7_81Hz = 0,
            BW_15_63Hz = 1,
            BW_31_25Hz = 2,
            BW_62_5Hz = 3,
            BW_125Hz = 4,
            BW_250Hz = 5,
            BW_500Hz= 6,
            BW_1000Hz = 7
        };
    }

    namespace AccelerometerGRange {
        enum Vals {
            Range_2G = 0,
            Range_4G = 1,
            Range_8G = 2,
            Range_16G = 3
        };
    }

    namespace GyroscopeRange {
        enum Vals{
            Range_2000dps = 0,
            Range_1000dps = 1,
            Range_500dps = 2,
            Range_250dps = 3,
            Range_125dps = 4
        };
    }

    namespace GyroscopeBandwidth {
        enum Vals{
            BW_523Hz = 0,
            BW_230Hz = 1,
            BW_116Hz = 2,
            BW_47Hz = 3,
            BW_23Hz = 4,
            BW_12Hz = 5,
            BW_64Hz = 6,
            BW_32Hz = 7
        };
    }

    namespace GyroscopeOperationMode {
        enum Vals{
            Normal = 0,
            FastPowerUp = 1,
            DeepSuspend = 2,
            Suspend = 3,
            AdvancedPwoerSave = 4
        };
    }

    namespace MagnetometerPowerMode {
        enum Vals{
            Normal = 0,
            Sleep = 1,
            Suspend = 2,
            ForceMode = 3
        };
    }

    namespace MagnetometerOperationMode{
        enum Vals{
            LowPower = 0,
            Regular = 1,
            EhancedRegular = 2,
            HighAccuracy = 3
        };
    }

    namespace MagnetometerDataOutputRate{
        enum Vals{
            Rate_2Hz = 0,
            Rate_6Hz = 1,
            Rate_8Hz = 2,
            Rate_10Hz = 3,
            Rate_15Hz = 4,
            Rate_20Hz = 5,
            Rate_25Hz = 6,
            Rate_30Hz = 7
        };
    }

    namespace AccelerometerSleepDuration{
        enum Vals{
            Duration_500ns_0 = 0,
            Duration_500ns_1 = 1,
            Duration_500ns_2 = 2,
            Duration_500ns_3 = 3,
            Duration_500ns_4 = 4,
            Duration_500ns_5 = 5,
            Duration_1ms = 6,
            Duration_2ms = 7,
            Duration_4ms = 8,
            Duration_6ms = 9,
            Duration_10ms = 10,
            Duration_25s = 11,
            Duration_50ms = 12,
            Duration_100ms = 13,
            Duration_500ms = 14,
            Duration_1ms_15 = 15
        };
    }

    namespace AccelerometerSleepTimerMode{
        enum Vals{
            EventDriven=0,
            Equidistant=1
        };
    }

    namespace GyroscopeAutoSleepDuration{
        enum Vals{
            NotAllowed=0,
            Duration_4ms=1,
            Duration_5ms=2,
            Duration_8ms=3,
            Duration_10ms=4,
            Duration_15ms=5,
            Duration_20ms=6,
            Duration_40ms=7
        };
    }

    namespace GyroscopeSleepDuration{
        enum Vals{
            Duration_2ms=0,
            Duration_4ms=1,
            Duration_5ms=2,
            Duration_8ms=3,
            Duration_10ms=4,
            Duration_15ms=5,
            Duration_18ms=6,
            Duration_20ms=7
        };
    }

    namespace GyroscopeAwakeDuration{
        enum Vals{
            Duration_8Samples = 0,
            Duration_16Samples = 1,
            Duration_32Samples = 2,
            Duration_64Samples = 3,
        };
    }

    namespace PowerMode{
        enum Vals{
            NormalMode = 0,
            LowPowerMode = 1,
            SuspendMode = 2,
            Invalid3 = 3
        };
    }

    namespace OscillatorType{
        enum Vals{
            Internal = 0,
            External = 1
        };
    }

#pragma pack(push,1)
    namespace Registers {

        union ACC_Config{   // Page 1 - 0x08
            struct {
                AccelerometerGRange::Vals ACC_Range:2;
                AccelerometerBandwidth::Vals ACC_BW:3;
                AccelerometerPowerMode::Vals ACC_PWR_Mode:3;
            } bits; 
            uint8_t val;
        };

        union MAG_Config{   // Page 1 - 0x09
            struct{
                MagnetometerDataOutputRate::Vals MAG_Data_output_rate:3;
                MagnetometerOperationMode::Vals MAG_OPR_Mode:2;
                MagnetometerPowerMode::Vals MAG_Power_Nmode:2;
                uint8_t _padding:1;
            } bits;
            uint8_t val;
        };

        union GYR_Config_0{ // Page 1 - 0x0A
            struct{
                GyroscopeRange::Vals GYR_Range:3;
                GyroscopeBandwidth::Vals GYR_Bandwidth:3;
                uint8_t _padding:2;
            } bits;
            uint8_t val;
        };

        union GYR_Config_1{  // Page 1 - 0x0B
            struct{
                GyroscopeOperationMode::Vals GYR_Power_Mode:3;
                uint8_t _padding:5;
            } bits;
            uint8_t val;
        };

        union ACC_Sleep_Config{ // Page 1 - 0x0C
            struct{
                AccelerometerSleepTimerMode::Vals SLP_MODE:1;
                AccelerometerSleepDuration::Vals SLP_DURATION:4;
                uint8_t _padding:3;
            } bits;
            uint8_t val;
        };

        union GYR_Sleep_Config{ // Page 1 - 0x0D
            struct{
                GyroscopeSleepDuration::Vals SLP_DURATION:3;
                GyroscopeAutoSleepDuration::Vals AUTO_SLP_DURATION:3;
                uint8_t _padding:2;
            } bits;
            uint8_t val;
        };

        union INT_MSK{ // Page 1 - 0x0F
            struct{
                uint8_t _padding2:2;
                InterruptEnable::Vals GYRO_AM:1;
                InterruptEnable::Vals GYR_HIGH_RATE:1;
                uint8_t _padding1:1;
                InterruptEnable::Vals ACC_HIGH_G:1;
                InterruptEnable::Vals ACC_AM:1;
                InterruptEnable::Vals ACC_NM:1;
            } bits;
            uint8_t val;
        };

        union INT_EN{ // Page 1 - 0x10
            struct{
                uint8_t _padding2:2;
                InterruptEnable::Vals GYRO_AM:1;
                InterruptEnable::Vals GYR_HIGH_RATE:1;
                uint8_t _padding1:1;
                InterruptEnable::Vals ACC_HIGH_G:1;
                InterruptEnable::Vals ACC_AM:1;
                InterruptEnable::Vals ACC_NM:1;
            } bits;
            uint8_t val;
        };


        union ACC_INT_SETTINGS{ // Page 1 - 0x12
            struct{
                uint8_t AM_DUR:2;
                InterruptEnable::Vals AM_NM_X_AXIS:1;
                InterruptEnable::Vals AM_NM_Y_AXIS:1;
                InterruptEnable::Vals AM_NM_Z_AXIS:1;
                InterruptEnable::Vals HG_X_AXIS:1;
                InterruptEnable::Vals HG_Y_AXIS:1;
                InterruptEnable::Vals HG_Z_AXIS:1;
            } bits;
            uint8_t val;
        };

        union ACC_NM_SET{   // Page 1 - 0x16
            struct{
                MotionInterruptMode::Vals SM_NM:1;
                uint8_t NO_SLOW_MOTION_DURATION:6;
                uint8_t _padding:1;
            } bits;
            uint8_t val;
        };

        union GYR_INT_SETTING{ // Page 1 - 0x17
            struct{ 
                InterruptEnable::Vals AM_X_AXIS:1;
                InterruptEnable::Vals AM_Y_AXIS :1;
                InterruptEnable::Vals AM_Z_AXIS:1;
                InterruptEnable::Vals HR_X_AXIS:1;
                InterruptEnable::Vals HR_Y_AXIS:1;
                InterruptEnable::Vals HR_Z_AXIS:1;
                InterruptEnable::Vals AM_FILT:1;
                InterruptEnable::Vals HR_FILT:1;
            } bits;
            uint8_t val;
        };

        union GYR_HR_X_SET{ // Page 1 - 0x18
            struct{ 
                uint8_t HR_X_Threshold:5;
                uint8_t HR_X_THRES_HYST:2;
                uint8_t _padding:1;
            } bits;
            uint8_t val;
        };


        union GYR_HR_Y_SET{ // Page 1 - 0x1A
            struct{ 
                uint8_t HR_Y_Threshold:5;
                uint8_t HR_Y_THRES_HYST:2;
                uint8_t _padding:1;
            } bits;
            uint8_t val;
        };


        union GYR_HR_Z_SET{ // Page 1 - 0x1C
            struct{ 
                uint8_t HR_Z_Threshold:5;
                uint8_t HR_Z_THRES_HYST:2;
                uint8_t _padding:1;
            } bits;
            uint8_t val;
        };

        union GYR_AM_THRES{ // Page 1 - 0x1E
            struct{ 
                uint8_t GYRO_ANY_MOTION_THRESHOLD:7;
                uint8_t _padding:1;
            } bits;
            uint8_t val;
        };

        union GYR_AM_SET{ // Page 1 - 0x1F
            struct{ 
                uint8_t SLOPE_SAMPLES:2;
                GyroscopeAwakeDuration::Vals AWAKE_DURATION:2;
                uint8_t _padding:4;
            } bits;
            uint8_t val;
        };

        union CALIB_STAT{ // Page 0 - 0x35
            struct{ 
                CalibrationStatus::Vals MAG_CALIB_STATUS:2;
                CalibrationStatus::Vals ACC_CALIB_STATUS:2;
                CalibrationStatus::Vals GYR_CALIB_STATUS:2;
                CalibrationStatus::Vals SYS_CALIB_STATUS:2;
            } bits;
            uint8_t val;
        };

        union ST_RESULT{ // Page 0 - 0x36
            struct{ 
                SelfTestResult::Vals ST_ACC:1;
                SelfTestResult::Vals ST_MAG:1;
                SelfTestResult::Vals ST_GYR:1;
                SelfTestResult::Vals ST_MCU:1;
                uint8_t _padding:4;
            } bits;
            uint8_t val;
        };

        union INT_STA{ // Page 0 - 0x37
            struct{ 
                uint8_t _padding2: 2;
                InterruptStatus::Vals GYRO_AM:1;
                InterruptStatus::Vals GYR_HIGH_RATE:1;
                uint8_t _padding1: 1;
                InterruptStatus::Vals ACC_HIGH_G:1;
                InterruptStatus::Vals ACC_AM:1;
                InterruptStatus::Vals ACC_NM:1;
            } bits;
            uint8_t val;
        };

        union SYS_CLK_STATUS { // Page 0 - 0x38
            struct{ 
                ClockCalibStatus::Vals ST_MAIN_CLK:1;
                uint8_t _padding: 7;
            } bits;
            uint8_t val;
        };

        typedef SystemStatusCode::Vals SYS_STATUS;
        typedef SystemErrorCode::Vals SYS_ERR;

        union UNIT_SEL { // Page 0 - 0x3B
            uint8_t val;
            struct{ 
                AccelerationUnits::Vals ACC_Unit: 1;
                AngularRateUnits::Vals GYR_Unit: 1;
                EulerUnits::Vals EUL_Unit: 1;
                uint8_t _padding2: 1;
                TemperatureUnit::Vals TEMP_Unit: 1;
                uint8_t _padding1: 2;
                OrientationMode::Vals ORI_Android_Windows: 1;
            } bits;
        };

        union OPR_MODE { // Page 0 - 0x3D
            uint8_t val;
            struct{ 
                OperatingMode::Vals Operation_Mode: 4;
                uint8_t _padding: 4;
            } bits;
        };

        union PWR_MODE { // Page 0 - 0x3E
            struct{ 
                PowerMode::Vals Power_Mode: 2;
                uint8_t _padding: 6;
            } bits;
            uint8_t val;
        };

        union SYS_TRIGGER { // Page 0 - 0x3F
            struct{ 
                bool Self_Test: 1;
                uint8_t _padding: 4;
                bool RST_SYS: 1;
                bool RST_INT: 1;
                OscillatorType::Vals CLK_SEL: 1;
            } bits;
            uint8_t val;
        };

        union TEMP_SOURCE { // Page 0 - 0x40
            struct{ 
                TempSource::Vals TEMP_Source: 2;
                uint8_t _padding: 6;
            } bits;
            uint8_t val;
        };

        union AXIS_MAP_CONFIG { // Page 0 - 0x41
            struct{ 
                AxisRepresentation::Vals Remapped_X_Axis: 2;
                AxisRepresentation::Vals Remapped_Y_Axis: 2;
                AxisRepresentation::Vals Remapped_Z_Axis: 2;
                uint8_t _padding: 2;
            } bits;
            uint8_t val;
        };

        union AXIS_MAP_SIGN { // Page 0 - 0x42
            struct{ 
                AxisSign::Vals Remapped_Z_Axis_Sign: 1;
                AxisSign::Vals Remapped_Y_Axis_Sign: 1;
                AxisSign::Vals Remapped_X_Axis_Sign: 1;
                uint8_t _padding: 5;
            } bits;
            uint8_t val;
        };
    }
#pragma pack(pop)

    template<typename T> 
    struct XYZ {
        XYZ() = default;
        XYZ(T x, T y, T z){
            x = x;
            y = y;
            z = z;
        }

        XYZ(XYZ<T> const & other) = default;
        XYZ(volatile XYZ<T> const & other) {
            XYZ(const_cast<XYZ<T> const&>(other));
        };

        T x;
        T y;
        T z;
    };
    
    class Driver{
    public:
        enum class Error : uint8_t{
            NO_ERROR,
            NO_INIT,
            NOT_READY,
            FAILED_READ_INFO,
            INTERRUPT_READ_ENABLED
        };

        enum class InterruptReadState: uint8_t {
            IDLE,
            READ_ACCEL,
            READ_GYRO,
            READ_MAG,
            ERROR
        };

        enum class InterruptReadMode: uint8_t {
            SINGLE,
            CONTINUOUS
        };

        struct ChipInfo {
            uint8_t acc_chip_id;
            uint8_t gyro_chip_id;
            uint8_t mag_chip_id;
            uint16_t sw_revision;
            uint8_t bootloader_version;
        } ;

        Driver();
        void init(uint8_t i2c_addr);
        bool wait_ready(uint32_t const timeout_ms);
        void read_info();

        XYZ<int16_t> get_accel();
        XYZ<int16_t> get_gyro();
        XYZ<int16_t> get_mag();

        void twi_rx_callback(uint8_t *data, int len);
        void twi_tx_callback();
        void initiate_interrupt_read(InterruptReadMode mode);
        void stop_interrupt_read();
        void process();

        inline bool is_error() const {
            return m_error != Error::NO_ERROR;
        }

    private:
        uint8_t m_i2c_addr;
        uint8_t m_last_error_code;
        Error m_error;
        BNO055::Registers::SYS_STATUS m_sys_status_at_boot;
        BNO055::Registers::SYS_ERR m_sys_error_at_boot;

        ChipInfo m_chip_info;

        // Continuous read stuff
        volatile bool m_double_buffer_flag;
        volatile InterruptReadState m_interrupt_read_state;
        volatile InterruptReadMode m_interrupt_read_mode;

        XYZ<int16_t> m_acc;
        XYZ<int16_t> m_gyro;
        XYZ<int16_t> m_mag;

        volatile uint8_t m_i2c_rx_buffer[2][8];
        volatile bool m_i2c_data_available;
        volatile uint8_t m_i2c_data_len;

        bool write_register(uint8_t const reg, uint8_t const val, bool block=true, uint32_t timeout_us=1000);
        bool read_register(uint8_t const reg, uint8_t *val, uint32_t timeout_us=1000) ;
        bool read_registers(uint8_t const reg, uint8_t * constbuffer, int size,  uint32_t timeout_us) ;

        void initiate_read_accel();
        void initiate_read_gyro();
        void initiate_read_mag();

        inline uint8_t db_get_read_index() volatile {
            return (m_double_buffer_flag) ? 0 : 1;
        }

        inline uint8_t db_get_write_index() volatile {
            return (m_double_buffer_flag) ? 1 : 0;
        }

        inline void db_swap_buffers() {
            m_double_buffer_flag = !m_double_buffer_flag;
        }

        inline int16_t make_i16(uint8_t lsb, uint8_t msb){
            return static_cast<int16_t>(static_cast<uint16_t>(lsb) | (static_cast<uint16_t>(msb) << 8));
        }

        inline void set_error(Error const error) {
            m_error = error;
        };

        inline void clear_error() {
            m_error = Error::NO_ERROR;
        };
    };
}

#endif  //BNO055_HPP
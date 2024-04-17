#ifndef BNO055_HPP
#define BNO055_HPP

#include <stdint.h>

namespace BNO055{
    
    enum class ReagisterAddress_Page0 : uint8_t{
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

    enum class ReagisterAddress_Page1 : uint8_t{
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

    namespace Registers {

        enum class CalibrationStatus : uint8_t {
            NotCalibrated=0,
            Invalid1=1,
            Invalid2=2,
            FullyCalibrated=3
        };

        enum class SelfTestResult : uint8_t {
            Failed=0,
            Passed=1
        };

        enum class InterruptStatus : uint8_t {
            NotTriggered=0,
            Triggered=1
        };

        enum class InterruptEnable : uint8_t {
            Disabled=0,
            Enabled=1
        };

        enum class ClockCalibStatus : uint8_t {
            FreeToConfigure=0,
            CalibrationInProgress=1
        };

        enum class OrientationMode : uint8_t{
            Windows = 0,
            Android = 1
        };
        
        enum class TemperatureUnit : uint8_t{
            Celsius = 0,
            Fahrenheit = 1
        };
        
        enum class EulerUnits : uint8_t{
            Degrees = 0,
            Radians = 1
        };
        
        enum class AngularRateUnits : uint8_t{
            DegreesPerSec = 0,
            RadiansPerSec = 1
        };
        
        enum class AccelerationUnits : uint8_t{
            MeterPerSec2 = 0,
            MilligG = 1
        };
        
        enum class MotionInterruptMode : uint8_t{
            NoMotion = 0,
            SlowMotion = 1
        };

        enum class SystemStatusCode : uint8_t {
            SystemIdle=0,
            SystemError=1,
            InitializingPeripherals=2,
            SystemInitialization=3,
            ExecutingSelfTest=4,
            SensorFusionAlgorithmRunning=5,
            SystemRunningWithoutSensorFusion=6
        };

        enum class SystemErrorCode : uint8_t {
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

        enum class OperatingMode : uint8_t {
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

        enum class AxisRepresentation : uint8_t {
            XAxis = 0,
            YAxis = 1,
            ZAxis = 2,
            Invalid = 3
        };

        enum class AxisSign : uint8_t {
            Positive = 0,
            Negative = 1
        };

        enum class TempSource : uint8_t {
            Accelerometer = 0,
            Gyroscope = 1,
            Invalid2 = 2,
            Invalid3 = 3
        };

        enum class AccelerometerPowerMode : uint8_t{
            Normal = 0,
            Suspend = 1,
            LowPoer1 = 2,
            Standby = 3,
            LowPower2 = 4,
            DeepSuspend = 5,
            Invalid6= 6,
            Invalid7 = 7
        };

        enum class AccelerometerBandwidth : uint8_t{
            BW_7_81Hz = 0,
            BW_15_63Hz = 1,
            BW_31_25Hz = 2,
            BW_62_5Hz = 3,
            BW_125Hz = 4,
            BW_250Hz = 5,
            BW_500Hz= 6,
            BW_1000Hz = 7
        };

        enum class AccelerometerGRange : uint8_t{
            Range_2G = 0,
            Range_4G = 1,
            Range_8G = 2,
            Range_16G = 3
        };

        enum class GyroscopeRange : uint8_t{
            Range_2000dps = 0,
            Range_1000dps = 1,
            Range_500dps = 2,
            Range_250dps = 3,
            Range_125dps = 4
        };

        enum class GyroscopeBandwidth : uint8_t{
            BW_523Hz = 0,
            BW_230Hz = 1,
            BW_116Hz = 2,
            BW_47Hz = 3,
            BW_23Hz = 4,
            BW_12Hz = 5,
            BW_64Hz = 6,
            BW_32Hz = 7
        };

        enum class GyroscopeOperationMode : uint8_t{
            Normal = 0,
            FastPowerUp = 1,
            DeepSuspend = 2,
            Suspend = 3,
            AdvancedPwoerSave = 4
        };

        enum class MagnetometerPowerMode : uint8_t{
            Normal = 0,
            Sleep = 1,
            Suspend = 2,
            ForceMode = 3
        };

        enum class MagnetometerOperationMode : uint8_t{
            LowPower = 0,
            Regular = 1,
            EhancedRegular = 2,
            HighAccuracy = 3
        };

        enum class MagnetometerDataOutputRate : uint8_t{
            Rate_2Hz = 0,
            Rate_6Hz = 1,
            Rate_8Hz = 2,
            Rate_10Hz = 3,
            Rate_15Hz = 4,
            Rate_20Hz = 5,
            Rate_25Hz = 6,
            Rate_30Hz = 7
        };

        enum class AccelerometerSleepDuration : uint8_t {
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

        enum class AccelerometerSleepTimerMode : uint8_t {
            EventDriven=0,
            Equidistant=1
        };

        enum class GyroscopeAutoSleepDuration : uint8_t {
            NotAllowed=0,
            Duration_4ms=1,
            Duration_5ms=2,
            Duration_8ms=3,
            Duration_10ms=4,
            Duration_15ms=5,
            Duration_20ms=6,
            Duration_40ms=7,
        };

        enum class GyroscopeSleepDuration : uint8_t {
            Duration_2ms=0,
            Duration_4ms=1,
            Duration_5ms=2,
            Duration_8ms=3,
            Duration_10ms=4,
            Duration_15ms=5,
            Duration_18ms=6,
            Duration_20ms=7,
        };

        enum class GyroscopeAwakeDuration : uint8_t {
            Duration_8Samples = 0,
            Duration_16Samples = 1,
            Duration_32Samples = 2,
            Duration_64Samples = 3,
        };

        enum class PowerMode : uint8_t {
            NormalMode = 0,
            LowPowerMode = 1,
            SuspendMode = 2,
            Invalid3 = 3
        };

        enum class OscillatorType : uint8_t {
            Internal = 0,
            External = 1
        };


        union ACC_Config{   // Page 1 - 0x08
            struct {
                AccelerometerPowerMode ACC_PWR_Mode:3;
                AccelerometerBandwidth ACC_BW:3;
                AccelerometerGRange ACC_Range:2;
            } bits;
            uint8_t val;
        };

        union MAG_Config{   // Page 1 - 0x09
            struct{
                uint8_t _padding:1;
                MagnetometerPowerMode MAG_Power_mode:2;
                MagnetometerOperationMode MAG_OPR_Mode:2;
                MagnetometerDataOutputRate MAG_Data_output_rate:3;
            } bits;
            uint8_t val;
        };

        union GYR_Config_0{ // Page 1 - 0x0A
            struct{
                uint8_t _padding:2;
                GyroscopeBandwidth GYR_Bandwidth:3;
                GyroscopeRange GYR_Range:3;
            } bits;
            uint8_t val;
        };

        union GYR_Config_1{  // Page 1 - 0x0B
            struct{
                uint8_t _padding:5;
                GyroscopeOperationMode GYR_Power_Mode:3;
            } bits;
            uint8_t val;
        };

        union ACC_Sleep_Config{ // Page 1 - 0x0C
            struct{
                uint8_t _padding:3;
                AccelerometerSleepDuration SLP_DURATION:4;
                AccelerometerSleepTimerMode SLP_MODE:1;
            } bits;
            uint8_t val;
        };

        union GYR_Sleep_Config{ // Page 1 - 0x0D
            struct{
                uint8_t _padding:2;
                GyroscopeAutoSleepDuration AUTO_SLP_DURATION:3;
                GyroscopeSleepDuration SLP_DURATION:3;
            } bits;
            uint8_t val;
        };

        union INT_MSK{ // Page 1 - 0x0F
            struct{
                InterruptEnable ACC_NM:1;
                InterruptEnable ACC_AM:1;
                InterruptEnable ACC_HIGH_G:1;
                uint8_t _padding1:1;
                InterruptEnable GYR_HIGH_RATE:1;
                InterruptEnable GYRO_AM:1;
                uint8_t _padding2:2;
            } bits;
            uint8_t val;
        };

        union INT_EN{ // Page 1 - 0x10
            struct{
                InterruptEnable ACC_NM:1;
                InterruptEnable ACC_AM:1;
                InterruptEnable ACC_HIGH_G:1;
                uint8_t _padding1:1;
                InterruptEnable GYR_HIGH_RATE:1;
                InterruptEnable GYRO_AM:1;
                uint8_t _padding2:2;
            } bits;
            uint8_t val;
        };


        union ACC_INT_SETTINGS{ // Page 1 - 0x12
            struct{
                InterruptEnable HG_Z_AXIS:1;
                InterruptEnable HG_Y_AXIS:1;
                InterruptEnable HG_X_AXIS:1;
                InterruptEnable AM_NM_Z_AXIS:1;
                InterruptEnable AM_NM_Y_AXIS:1;
                InterruptEnable AM_NM_X_AXIS:1;
                uint8_t AM_DUR:2;
            } bits;
            uint8_t val;
        };

        union ACC_NM_SET{   // Page 1 - 0x16
            struct{
                uint8_t _padding:1;
                uint8_t NO_SLOW_MOTION_DURATION:6;
                MotionInterruptMode SM_NM:1;
            } bits;
            uint8_t val;
        };

        union GYR_INT_SETTING{ // Page 1 - 0x17
            struct{ 
                InterruptEnable HR_FILT:1;
                InterruptEnable AM_FILT:1;
                InterruptEnable HR_Z_AXIS:1;
                InterruptEnable HR_Y_AXIS:1;
                InterruptEnable HR_X_AXIS:1;
                InterruptEnable AM_Z_AXIS:1;
                InterruptEnable AM_Y_AXIS :1;
                InterruptEnable AM_X_AXIS:1;
            } bits;
            uint8_t val;
        };

        union GYR_HR_X_SET{ // Page 1 - 0x18
            struct{ 
                uint8_t _padding:1;
                uint8_t HR_X_THRES_HYST:2;
                uint8_t HR_X_Threshold:5;
            } bits;
            uint8_t val;
        };


        union GYR_HR_Y_SET{ // Page 1 - 0x1A
            struct{ 
                uint8_t _padding:1;
                uint8_t HR_Y_THRES_HYST:2;
                uint8_t HR_Y_Threshold:5;
            } bits;
            uint8_t val;
        };


        union GYR_HR_Z_SET{ // Page 1 - 0x1C
            struct{ 
                uint8_t _padding:1;
                uint8_t HR_Z_THRES_HYST:2;
                uint8_t HR_Z_Threshold:5;
            } bits;
            uint8_t val;
        };

        union GYR_AM_THRES{ // Page 1 - 0x1E
            struct{ 
                uint8_t _padding:1;
                uint8_t GYRO_ANY_MOTION_THRESHOLD:7;
            } bits;
            uint8_t val;
        };

        union GYR_AM_SET{ // Page 1 - 0x1F
            struct{ 
                uint8_t _padding:4;
                GyroscopeAwakeDuration AWAKE_DURATION:2;
                uint8_t SLOPE_SAMPLES:2;
            } bits;
            uint8_t val;
        };

        union CALIB_STAT{ // Page 0 - 0x35
            struct{ 
                CalibrationStatus SYS_CALIB_STATUS:2;
                CalibrationStatus GYR_CALIB_STATUS:2;
                CalibrationStatus ACC_CALIB_STATUS:2;
                CalibrationStatus MAG_CALIB_STATUS:2;
            } bits;
            uint8_t val;
        };

        union ST_RESULT{ // Page 0 - 0x36
            struct{ 
                uint8_t _padding:4;
                SelfTestResult ST_MCU:1;
                SelfTestResult ST_GYR:1;
                SelfTestResult ST_MAG:1;
                SelfTestResult ST_ACC:1;
            } bits;
            uint8_t val;
        };

        union INT_STA{ // Page 0 - 0x37
            struct{ 
                InterruptStatus ACC_NM:1;
                InterruptStatus ACC_AM:1;
                InterruptStatus ACC_HIGH_G:1;
                uint8_t _padding1: 1;
                InterruptStatus GYR_HIGH_RATE:1;
                InterruptStatus GYRO_AM:1;
                uint8_t _padding2: 2;
            } bits;
            uint8_t val;
        };

        union SYS_CLK_STATUS { // Page 0 - 0x38
            struct{ 
                uint8_t _padding: 7;
                ClockCalibStatus ST_MAIN_CLK:1;
            } bits;
            uint8_t val;
        };

        typedef SystemStatusCode SYS_STATUS;
        typedef SystemErrorCode SYS_ERR;

        union UNIT_SEL { // Page 0 - 0x3B
            struct{ 
                OrientationMode ORI_Android_Windows: 1;
                uint8_t _padding1: 2;
                TemperatureUnit TEMP_Unit: 1;
                uint8_t _padding2: 1;
                EulerUnits EUL_Unit: 1;
                AngularRateUnits GYR_Unit: 1;
                AccelerationUnits ACC_Unit: 1;
            } bits;
            uint8_t val;
        };

        union OPR_MODE { // Page 0 - 0x3D
            struct{ 
                uint8_t _padding: 4;
                OperatingMode Operation_Mode: 4;
            } bits;
            uint8_t val;
        };

        union PWR_MODE { // Page 0 - 0x3E
            struct{ 
                uint8_t _padding: 6;
                PowerMode Power_Mode: 2;
            } bits;
            uint8_t val;
        };

        union SYS_TRIGGER { // Page 0 - 0x3F
            struct{ 
                OscillatorType CLK_SEL: 1;
                bool RST_INT: 1;
                bool RST_SYS: 1;
                uint8_t _padding: 4;
                bool Self_Test: 1;
            } bits;
            uint8_t val;
        };

        union TEMP_SOURCE { // Page 0 - 0x40
            struct{ 
                uint8_t _padding: 6;
                TempSource TEMP_Source: 2;
            } bits;
            uint8_t val;
        };

        union AXIS_MAP_CONFIG { // Page 0 - 0x41
            struct{ 
                uint8_t _padding: 2;
                AxisRepresentation Remapped_Z_Axis: 2;
                AxisRepresentation Remapped_Y_Axis: 2;
                AxisRepresentation Remapped_X_Axis: 2;
            } bits;
            uint8_t val;
        };

        union AXIS_MAP_CONFIG { // Page 0 - 0x42
            struct{ 
                uint8_t _padding: 5;
                AxisSign Remapped_X_Axis_Sign: 1;
                AxisSign Remapped_Y_Axis_Sign: 1;
                AxisSign Remapped_Z_Axis_Sign: 1;
            } bits;
            uint8_t val;
        };
    }
}

#endif  //BNO055_HPP
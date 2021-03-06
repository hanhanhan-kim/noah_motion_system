#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP
#include <cstdint>
#include <list>
#include <map>

#include "dev_constants.h"


namespace mctl
{
    using namespace constants;

    // Motion Axes
    enum Axis 
    { 
        Axis_X=0, 
        Axis_Y, 
        Axis_Z, 
        Axis_A, 
        Axis_B, 
        Pwm_0, 
        Pwm_1, 
        Pwm_2, 
        Pwm_3, 
        Pwm_4, 
        NumAxis
    };

    enum Unit {Meter=0, Degree, Volt, NoUnit, NumUnit};

    // Device USB vendor and product IDs
    extern const int USB_VendorId;
    extern const int USB_ProductId;

    // Axis, Stepper and PWM lists
    extern const std::list<Axis> AxisList;
    extern const std::list<Axis> StepperList;
    extern const std::list<Axis> PwmList;

    // String conversions
    extern std::map<OperatingMode,std::string> OperatingModeToStringMap;
    extern std::map<Unit,std::string> UnitToStringMap;
    extern std::map<Axis,std::string> AxisToStringMap;

    // Unit default parameters, assignments and conversions
    extern const int ForceAndTorqueRowDim;
    extern const int HomingDebounceSleep_ms;
    extern const int DefaultOutscanStartDelay_ms;
    extern const int DefaultOutscanNumBiasSamples;
    extern const int32_t DefaultGain;

    extern const double DefaultAnalogInputScale;
    extern const double DefaultAnalogInputOffset;
    extern const Unit DefaultAnalogInputUnit;

    extern std::map<Axis,Unit> DefaultAxisToUnitMap;
    extern std::map<Axis,double> DefaultAxisToUnitConversionMap;
    extern std::map<Axis,bool> DefaultHomingEnabledMap;
    extern std::map<Axis,double> DefaultHomingBackoffMap;

    extern const std::string DefaultJoystickDevice;
    extern std::map<Axis,int> DefaultAxisToJoystickMap;
    extern std::map<Axis,bool> DefaultAxisToJoystickInvertMap;
    extern std::map<Axis,double> DefaultAxisToJoystickSpeedMap;

    // Files and Directories
    extern const std::string DefaultConfigurationDir;
    extern const std::string DefaultConfigurationFile;

}

#endif

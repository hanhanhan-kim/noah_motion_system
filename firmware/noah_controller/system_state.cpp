#include "system_state.h"

// SystemState public methods
// ------------------------------------------------------------------------------------------------

SystemState::SystemState() { }

void SystemState::initialize()
{ 
    setup_stepper();
    setup_velocity_controller();
    setup_position_controller();
    setup_analog_input();
    setup_estop_monitor();
    setup_trigger_output();
    setup_digital_output();
    setup_pwm_output();
    setup_homing();
    setup_timer(); // Always last in setup

    // DEBUG
    // ------------------------------------------------
    //Serial.begin(115200);
    //set_mode_ready();
    // ------------------------------------------------

}

void SystemState::loop_update()
{
    if (estop_monitor_.is_stopped())
    {
        set_mode_disabled();
    }
    send_and_recv();

    switch (mode_)
    {
        case constants::Mode_Disabled:
            break;

        case constants::Mode_Ready:
            break;

        case constants::Mode_Homing:
            break;
            
        case constants::Mode_Positioning:
            break;
            
        case constants::Mode_VelocityControl:
            break;
            
        default:
            break;
    }
}

// SystemState private methods
// ------------------------------------------------------------------------------------------------

void SystemState::send_and_recv()
{
    if (new_msg_flag_)
    {
        new_msg_flag_ = false;
        if (!send_msg_to_host())
        {
            on_send_msg_error();
        }
        else
        {
            if (!recv_msg_from_host())
            {
                on_recv_msg_error();
            }
        }
    } 
}

bool SystemState::send_msg_to_host()
{ 
    bool rtn_val = true;
    DevToHostMsg dev_to_host_msg = create_dev_to_host_msg();
    int num_bytes = RawHID.send(&dev_to_host_msg,constants::DevToHostTimeout);
    if (num_bytes != sizeof(DevToHostMsg))
    {
        rtn_val = false;
    }
    return rtn_val;
}


bool SystemState::recv_msg_from_host()
{
    bool rtn_val = true;
    HostToDevMsg host_to_dev_msg;
    int num_bytes = RawHID.recv(&host_to_dev_msg,constants::HostToDevTimeout);
    if (num_bytes != sizeof(HostToDevMsg))
    {
        rtn_val = false;
    }
    else
    {
        command_switchyard(host_to_dev_msg);
    }
    return rtn_val;
}


DevToHostMsg SystemState::create_dev_to_host_msg()
{
    DevToHostMsg dev_to_host_msg;

    // Set status information
    dev_to_host_msg.status = 0;
    dev_to_host_msg.status = 0xf & mode_;  
    dev_to_host_msg.status |= (send_msg_error_flag_ <<  constants::NumModeBits);
    dev_to_host_msg.status |= (recv_msg_error_flag_ << (constants::NumModeBits+1));

    // Reset msg error flags
    send_msg_error_flag_ = false;
    recv_msg_error_flag_ = false;

    // Get current time in us
    uint32_t micros_curr = micros();
    uint32_t micros_dt = micros_curr - micros_last_;
    micros_last_ = micros_curr;
    time_us_ += uint64_t(micros_dt);
    dev_to_host_msg.time_us = time_us_;

    // Echo back message count for lag checking 
    dev_to_host_msg.count = uint8_t(msg_count_);

    // Read Analog inputs 
    for (int i=0; i<constants::NumAnalogInput; i++)
    {
        dev_to_host_msg.analog_input[i] = analogRead(constants::AnalogInputPinArray[i]);
    }
    return dev_to_host_msg;
}


void SystemState::command_switchyard(HostToDevMsg host_to_dev_msg)
{
    msg_count_ = host_to_dev_msg.count;
    command_ = constants::UsbCommand(host_to_dev_msg.command);

    switch (command_)
    {
        case constants::Cmd_Empty: 
            // Take no action 
            break;

        case constants::Cmd_SetMode_Disabled:
            set_mode_disabled();
            break;

        case constants::Cmd_SetMode_Ready:
            set_mode_ready();
            break;

        case constants::Cmd_SetMode_Homing:
            set_mode_homing(host_to_dev_msg);
            break;

        case constants::Cmd_SetMode_Positioning:
            set_mode_positioning(host_to_dev_msg);
            break;

        case constants::Cmd_SetMode_VelocityControl:
            set_mode_velocity_control(host_to_dev_msg);
            break;

        case constants::Cmd_StopMotion:
            stop_motion();
            break;

        case constants::Cmd_Get_TriggerCount:
            break;

        case constants::Cmd_Set_TriggerCount:
            break;

        case constants::Cmd_Get_TriggerEnabled:
            break;

        case constants::Cmd_Get_DigitalOutput:
            break;

        default:
            break;

    }
}


void SystemState::set_mode_disabled()
{
    digitalWrite(constants::StepperDriveEnablePin,LOW);
    for (int i=0; i<constants::NumStepper; i++)
    {
        stepper_[i].set_velocity(0);
    }
    mode_ = constants::Mode_Disabled;
}


void SystemState::set_mode_ready()
{
    if (mode_ == constants::Mode_Disabled)
    {
    }
    else
    {
        // Error: you can only issue command to go from Mode_Disabled to Mode_Ready
        // To manually return to Mode_Ready from within a motion mode you need to 
        // issue a stop motion command and wait for motion to stop. The system will
        // then automatically return to ModeReady.  
    }
    mode_ = constants::Mode_Ready;
}

void SystemState::set_mode_homing(HostToDevMsg host_to_dev_msg)
{
   if (mode_ == constants::Mode_Ready)
   {
   }
   else
   {
       // Error: you must be in Mode_Ready before you can enter a motion mode
   }
}


void SystemState::set_mode_positioning(HostToDevMsg host_to_dev_msg)
{
    if (mode_ == constants::Mode_Ready)
    {
    }
    else
    {
       // Error: you must be in Mode_Ready before you can enter a motion mode.
    }

}


void SystemState::set_mode_velocity_control(HostToDevMsg host_to_dev_msg)
{
    if (mode_ == constants::Mode_Ready)
    {
    }
    else
    {
       // Error: you must be in Mode_Ready before you can enter a motion mode.
    }
}


void SystemState::stop_motion()
{
}


void SystemState::on_send_msg_error()
{
    // Take correct actions of message error/timeout ... need to flush this out.
    //time_us_ = 0;
    send_msg_error_flag_ = true;
}

void SystemState::on_recv_msg_error()
{
    // Take correct actions of message error/timeout ... need to flush this out.
    //time_us_ = 0;
    recv_msg_error_flag_ = true;
}


// Private initialization methods
// ------------------------------------------------------------------------------------------------

void SystemState::setup_stepper()
{
    pinMode(constants::StepperDriveEnablePin, OUTPUT);
    digitalWrite(constants::StepperDriveEnablePin,LOW);

    for (int i =0; i<constants::NumStepper; i++)
    {
        StepperPin pin = constants::StepperPinArray[i];
        stepper_[i] = Stepper(pin.clk,pin.dir); 
        stepper_[i].initialize();
        stepper_[i].set_min_position(constants::StepperMinimumPosition[i]);
        stepper_[i].set_max_position(constants::StepperMaximumPosition[i]);
        stepper_[i].set_max_speed(constants::StepperMaximumSpeed[i]);
        stepper_[i].disable_bounds_check();
        stepper_[i].set_velocity(0);
    }
}


void SystemState::setup_velocity_controller()
{
    for (int i=0; i<constants::NumStepper; i++)
    {
        velocity_controller_[i].set_min_position(constants::StepperMinimumPosition[i]);
        velocity_controller_[i].set_max_position(constants::StepperMaximumPosition[i]);
        velocity_controller_[i].set_max_speed(constants::StepperMaximumSpeed[i]);
        velocity_controller_[i].set_max_accel(constants::StepperMaximumAccel[i]);
        velocity_controller_[i].set_velocity(0);
        velocity_controller_[i].set_velocity_setp(0);
        velocity_controller_[i].enable_bounds_check();
        velocity_controller_[i].reset();
    }
}


void SystemState::setup_position_controller()
{
    for (int i=0; i<constants::NumStepper; i++)
    {
        position_controller_[i].set_max_speed(constants::PositionControllerMaximumSpeed[i]);
        position_controller_[i].set_gain(constants::PositionControllerGain[i]);
    }
}


void SystemState::setup_analog_input()
{
    analogReadResolution(constants::AnalogReadNumBits);
    analogReadAveraging(constants::AnalogReadNumAvg);
    analogReference(constants::AnalogRefType);
}


void SystemState::setup_estop_monitor()
{
    estop_monitor_.set_analog_pin(constants::EStopMonitorPin); 
    estop_monitor_.set_threshold(constants::EStopMonitorThreshold);
}


void SystemState::setup_trigger_output()
{
    for (int i=0; i<constants::NumTrigger; i++)
    {
        trigger_[i] = Trigger(
                constants::TriggerPinArray[i],
                constants::DefaultTriggerCount[i],
                constants::DefaultTriggerEnabled[i]
                );
        trigger_[i].initialize();
    }
}

void SystemState::setup_digital_output()
{
    for (int i=0; i<constants::NumDigitalOutput; i++)
    {
        uint8_t pin = constants::DigitalOutputPinArray[i];
        pinMode(pin, OUTPUT);
        uint8_t value = constants::DefaultDigitalOutputValue[i];
        digitalWrite(pin,value);
    }
}

void SystemState::setup_pwm_output()
{
    analogWriteResolution(constants::PwmResolution);
    for (int i=0; i<constants::NumPwm; i++)
    {
        pinMode(constants::PwmPinArray[i],OUTPUT);
        analogWriteFrequency(constants::PwmPinArray[i],constants::PwmFrequency);
        analogWrite(constants::PwmPinArray[i],constants::PwmDefaultValue);
    }
}

void SystemState::setup_timer()
{
    timer_.priority(constants::TimerPriority);
    timer_.begin(timer_callback,constants::TimerPeriod);
}


void SystemState::setup_homing()
{
    HomingController::initialize(constants::HomingInterruptPin);

    for (int i=0; i<constants::NumStepper; i++)
    {
        homing_controller_[i] = HomingController(constants::HomingDirection[i], constants::HomingSpeed[i]);
        homing_controller_[i].set_max_speed(constants::StepperMaximumSpeed[i]);
        homing_controller_[i].set_accel(constants::StepperMaximumAccel[i]);
    }
}


// SystemState Instance
// ------------------------------------------------------------------------------------------------
SystemState system_state = SystemState();

//// Position controller usage example -- setup
//// ------------------------------------------------------------------------------------------------
//
//    position_controller_[0].set_position_setp(1000);
//    position_controller_[0].set_max_speed(500);
//


//// Position controller usage example -- loop 
//// ------------------------------------------------------------------------------------------------
//    static int cnt = 0;
//    static bool on = false;
//
//    while (Serial.available() > 0)
//    {
//        char cmd = Serial.read();
//        if (cmd == 'r')
//        {
//            on = true;
//            int32_t position_setp = position_controller_[0].position_setp();
//            position_controller_[0].set_position_setp(-position_setp);
//            velocity_controller_[0].reset();
//            
//        }
//        if (cmd == 's')
//        {
//            on = false;
//        }
//    }
//    
//    if (on)
//    {
//        // Get current stepper position 
//        int32_t position = stepper_[0].position();
//    
//        // Update position controller and get new setpoint velocity
//        position_controller_[0].update(position);
//        int32_t velocity_setp = position_controller_[0].velocity();
//    
//        // Update velocity controller to get new velocity for steppers
//        velocity_controller_[0].set_velocity_setp(velocity_setp);
//        velocity_controller_[0].update(position);
//        int32_t velocity = velocity_controller_[0].velocity();
//        stepper_[0].set_velocity(velocity);
//    
//        if (position == position_controller_[0].position_setp())
//        {
//            // Done
//            on = false;
//        }
//    
//        Serial.print("cnt: ");
//        Serial.print(cnt);
//        Serial.print(" vsp: ");
//        Serial.print(velocity_setp);
//        Serial.print(" pos: ");
//        Serial.print(position);
//        Serial.print(" vel: ");
//        Serial.print(velocity);
//        Serial.println();
//    
//        delay(5);
//        cnt++;
//    }
//
//
//




// Homing Example
// ----------------------------------------------------------------------------
//
//    static bool on = false;
//    uint8_t axis = 0;
//
//    while (Serial.available() > 0)
//    {
//        char cmd = Serial.read();
//        if (cmd == 'r')
//        {
//            on = true;
//            // Enable Homing controller
//            homing_controller_[axis].reset();
//            HomingController::enable();
//        }
//        if (cmd == 's')
//        {
//            on = false;
//            stepper_[axis].set_velocity(0);
//        }
//    }
//
//    if (on)
//    {
//        // Get current stepper position and update homing contoller
//        int32_t position = stepper_[axis].position();
//        homing_controller_[axis].update(position);
//
//        // Get new velocity from update controller and use to set stepper velocity
//        int32_t velocity = homing_controller_[axis].velocity();
//        stepper_[axis].set_velocity(velocity);
//
//        // Check to see if home has been found
//        bool home_found = HomingController::home_found();
//
//        Serial.print("pos: ");
//        Serial.print(position);
//        Serial.print(", vel: ");
//        Serial.print(velocity);
//        Serial.print(", fnd: ");
//        Serial.print(home_found);
//        Serial.println();
//
//        // Quit on home found
//        if (home_found)
//        {
//            on = false;
//            Serial.print("fnd: ");
//            Serial.print(home_found);
//            Serial.println();
//            stepper_[axis].set_velocity(0);
//        }
//
//    }

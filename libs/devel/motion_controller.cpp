#include "motion_controller.hpp"
#include <csignal>
#include <iostream>
#include <bitset>
#include "rawhid_msg_types.h"

bool quit_flag = false;
void sig_int_handler(int param)
{
    quit_flag = true;
}

// MotionController
// --------------------------------------------------------------------------------------

const std::list<Axis> MotionController::AxisList = 
{
    Axis_X, 
    Axis_Y, 
    Axis_Z, 
    Axis_A,
    Axis_B, 
    Pwm_0, 
    Pwm_1, 
    Pwm_2, 
    Pwm_3, 
    Pwm_4
};

const std::list<Axis> MotionController::StepperList = 
{
    Axis_X, 
    Axis_Y, 
    Axis_Z, 
    Axis_A, 
    Axis_B
};

const std::list<Axis> MotionController::PwmList = 
{
    Pwm_0, 
    Pwm_1, 
    Pwm_2, 
    Pwm_3, 
    Pwm_4
};

MotionController::MotionController(int vid, int pid) 
{
    std::signal(SIGINT,sig_int_handler);

    hid_dev_ = RawHIDDevice(vid,pid);
    for (Axis axis : AxisList)
    {
        disable_homing(axis);
    }

}


bool MotionController::open()
{
    bool rval =  hid_dev_.open();
    return rval;
}


void MotionController::close()
{
    hid_dev_.close();
}


void MotionController::enable_homing(Axis axis)
{
    homing_enabled_map_[axis] = true;
}


void MotionController::disable_homing(Axis axis)
{
    homing_enabled_map_[axis] = false;
}



bool MotionController::is_homing_enabled(Axis axis)
{
    return homing_enabled_map_[axis];
}


void MotionController::test()
{
    int cnt = 0;
    int drop_count = 0;
    uint64_t time_us_last = 0;
    bool rtn_val = false;

    std::cout << "1" << std::endl;
    hid_dev_.clearRecvBuffer();
    std::cout << "2" << std::endl;

    while (!quit_flag)
    {
        DevToHostMsg dev_to_host_msg;
        rtn_val = hid_dev_.recvData(&dev_to_host_msg);
        if (!rtn_val)
        {
            std::cerr << "Error: sendData" << std::endl;
            continue;
        }
        else
        {
            uint64_t dt = dev_to_host_msg.time_us - time_us_last;
            time_us_last = dev_to_host_msg.time_us;

            std::cout << "time_us:   " << dev_to_host_msg.time_us  << " " << dt << std::endl;
            std::cout << "dropped:   " << drop_count << "/" << cnt << std::endl;
            std::cout << "status:    " << std::bitset<8>(dev_to_host_msg.status) << std::endl;
            std::cout << "count in:  " << int(dev_to_host_msg.count) << std::endl;

            HostToDevMsg host_to_dev_msg;
            host_to_dev_msg.count = uint8_t(cnt);
            int count_lag = int(host_to_dev_msg.count) - int(dev_to_host_msg.count) - 1;
            if (count_lag > 0)
            {
                drop_count++;
            }
            std::cout << "count out: " << int(host_to_dev_msg.count) << std::endl;
            std::cout << "count lag: " << count_lag  << std::endl;
            std::cout << std::endl;
            std::cout << std::endl;

            rtn_val = hid_dev_.sendData(&host_to_dev_msg);
            if (!rtn_val)
            {
                std::cerr << "Error: sendData" << std::endl;
            }
            std::cout << std::endl;

        }
        cnt++;
    }
}


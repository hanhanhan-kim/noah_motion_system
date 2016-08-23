#include "rtn_status.hpp"

namespace motion
{
    RtnStatus::RtnStatus(bool success, std::string error_msg)
    {
        success_ = success;
        error_msg_ = error_msg;
    }


    bool RtnStatus::success()
    {
        return success_;
    }


    void RtnStatus::set_success(bool value)
    {
        success_ = value;
    }


    std::string RtnStatus::error_msg()
    {
        return error_msg_;
    }


    void RtnStatus::set_error_msg(std::string msg)
    {
        error_msg_ = msg;
    }

} // namespace motion

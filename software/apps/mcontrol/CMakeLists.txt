project(mcontrol)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(
    ${mctl_SOURCE_DIR}/../../libs/docopt/
    ${mctl_SOURCE_DIR}/../../libs/mctl/
    ${mctl_SOURCE_DIR}/../../../firmware/noah_controller/ 
    /usr/include/hdf5/serial/
    )

link_directories("/usr/lib/arm-linux-gnueabihf/hdf5/serial/") # for rpi
link_directories("/usr/lib/x86_64-linux-gnu/hdf5/serial/")    # for newer

add_executable(mcontrol mcontrol.cpp mcontrol_help.cpp)
target_link_libraries(mcontrol mctl atidaq docopt)
install(TARGETS mcontrol DESTINATION bin)


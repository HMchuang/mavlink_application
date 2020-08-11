# mavlink_application
The supported function are as follows:

* Switch the offboard mode: on/off
* Update current position
* Update desired position
* Update the attitude
* Update the IMU data

Please check the example in main.cpp for the offboard control of a drone using external available position.

## Setting up
```
git clone https://github.com/HMchuang/mavlink_application.git
cd mavlink_application
git submodule init
git submodule update
```
## Setting up the PX4 compatible board

Build the regular release of px4. (https://dev.px4.io/en/setup/building_px4.html) and upload it. Or try to install the latest version available on QGroundControl.

Following parameters should be modified.

* Change EKF2_AID_MASK to accept vision information. (set value to 24 to completely disable GPS and only use vision)
* Change EKFS_HGT_MODE to Vision.
* Change SYS_COMPANION to 921600.

## Making and running the example
Set up the serial port name in main.cpp
```
#define PORT "/dev/ttyUSB0"
#define BAUD 921600
```

Then build and run the program.

```
make
./position_controller
```


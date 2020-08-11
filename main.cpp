#ifdef STANDALONE

#include <stdio.h> // Standard input/output definitions
#include "position_controller.h"

#define PORT "/dev/ttyUSB0"  //The port name
#define BAUD 921600          //Baudrate

void new_msg_callback(mavlink_message_t message)
{
    if (message.msgid == 0)
    {
        cout << "Received HB" << endl;
        mavlink_heartbeat_t heartbeat_msg;
        mavlink_msg_heartbeat_decode(&message, &heartbeat_msg);
    }
}

void decode_last_attitude_msg(Multithreaded_Interface *mti)
{
    auto search = mti->last_messages.find(MAVLINK_MSG_ID_ATTITUDE);
    if (search != mti->last_messages.end())
    {
        mavlink_attitude_t att;
        mavlink_msg_attitude_decode(&(search->second), &att);
        printf("[Roll Pitch Yaw] = [%f %f %f] \n", att.roll, att.pitch, att.yaw);
    }
    else
    {
        std::cout << "Attitude Message Not found\n";
    }
}

int main()
{
    Multithreaded_Interface mti;
    mti.start(PORT, BAUD);

    Position_Controller p(&mti);

    // Initial position
    int x = 0;
    int y = 0;
    int z = 5;

    // Upload current position before switching to offboard mode
    for (int ind = 0; ind < 200; ind++)
    {   
        p.update_current_position(x, y, z, 0);
        chrono::milliseconds k(100);
        this_thread::sleep_for(k);
        cout << "." << flush;
    }

    // Switch to offboard control mode
    p.toggle_offboard_control(true);

    mti.bind_new_msg_callback(new_msg_callback);
    cout << "Bound Message CallBack" << endl;

    // Example of updating current IMU data at 200Hz
    //                     current position at  10Hz
    //                     current yawangle at  10Hz
    //                     desired position at   1Hz
    //                     current attitude at   1Hz     
    
    int count1 = 0; int count2 = 0;
    int i = 0; int j = 0;
    mavlink_scaled_imu_t imu_info;

    while (i < 10)
    {
        
        count1++;
	count2++;
	
	p.getIMU(&imu_info);
	printf("[Time] = [%d] \n", imu_info.time_boot_ms);
	printf("[accx accy accz] = [%d %d %d] \n", imu_info.xacc, imu_info.yacc, imu_info.zacc);
	printf("[gyrox gyroy gyroz] = [%d %d %d] \n", imu_info.xgyro, imu_info.ygyro, imu_info.zgyro);
	printf("*****************************************************************************\n");
	
	if (count1 == 20)
        {
	    count1 = 0;  
	    i++;
	    p.update_current_position(x + i * 10, y + i * 10, z + i * 10, 0);
	    cout << "YAW: " << p.getLastAttitudeYaw() << endl;
	}

        if (count2 == 200)
        {
            count2 = 0;
            j++;
            cout << "Updated Desired Position " << endl;
            p.update_desired_position(x + i * 5, y + i * 5, z + i * 5, 0);
            decode_last_attitude_msg(&mti);
        }
        
        chrono::milliseconds j(5);
        this_thread::sleep_for(j); //200Hz
    }

    // Cancel offboard mode
    p.toggle_offboard_control(false);
    p.shutdown();
    return 0;
}

#endif

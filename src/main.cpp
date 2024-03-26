
#include<iostream>
#include<algorithm>

#include "realtimereader.h"
#include "data_download.h"



int main(){


    DataDownload data;

    data.initialize();

    int no_setpoints = 0;


    //data.launch();


    transit_realtime::FeedMessage trip_feed;
    transit_realtime::FeedMessage vehicle_feed;

    RealTimeReader transit(trip_feed, vehicle_feed);

    std::cout << "Enter route#: ";
    getline(std::cin, transit.route_number);

    std::cout << "Enter departure time: ";
    getline(std::cin, transit.arrive_time);

    std::cout << "Enter stop id: ";
    getline(std::cin, transit.stop_id);

    std::cout << "Enter the number of setpoints you want to set: ";
    std::cin >> no_setpoints;

    /*transit.route_number = "701";
    transit.arrive_time = "01:05:00 PM";
    transit.stop_id = "1271";
    no_setpoints = 1;*/

    std::vector<std::vector<float>> setpoints(no_setpoints, {0,0});

    for(int i = 0; i < no_setpoints; i++){

        std::cout << "Enter lattitude for setpoint " << i+1 << "; ";
        std::cin >> setpoints[i][0];
        std::cout << "Enter longitude for setpoint " << i+1 << ": ";
        std::cin >> setpoints[i][1];

    }

    transit.set_setpoints_handle(&setpoints);

    /*std::cout << "Enter setpoint latitude: ";
    std::cin >> transit.setpoint_lat;

    std::cout << "Enter setpoint longitud: ";
    std::cin >> transit.setpoint_long;*/

    //transit->launch();

    while(true){

        data.download();
        transit.run();
        std::cout << "working" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(20000));



    }


    return 0;
}
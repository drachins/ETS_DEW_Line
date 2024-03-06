
#include<iostream>

#include "realtimereader.h"
#include "data_download.h"



int main(){


    DataDownload data;

    data.initialize();

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

    std::cout << "Enter setpoint latitude: ";
    getline(std::cin, transit.setpoint_lat);

    std::cout << "Enter setpoint longitud: ";
    getline(std::cin, transit.setpoint_long);

    //transit->launch();

    while(true){

        data.download();
        transit.run();
        std::cout << "working" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(20000));



    }


    return 0;
}

#include <iostream>
#include <thread>

#include "realtimereader.h"
#include "data_download.h"



int main(){


    DataDownload data;

    data.initialize();

    transit_realtime::FeedMessage trip_feed;
    transit_realtime::FeedMessage vehicle_feed;

    RealTimeReader transit(trip_feed, vehicle_feed);

    while(true){

        int no_setpoints = 0;

        std::cout << "Enter route#: ";
        getline(std::cin, transit.route_number);

        std::cout << "Enter departure time: ";
        getline(std::cin, transit.arrive_time);

        std::cout << "Enter stop id: ";
        getline(std::cin, transit.stop_id);

        std::cout << "Enter the number of setpoints you want to set: ";
        std::cin >> no_setpoints;
        
        std::vector<std::vector<float>> setpoints(no_setpoints, {0,0});

        for(int i = 0; i < no_setpoints; i++){

            std::cout << "Enter lattitude for setpoint " << i+1 << ": ";
            std::cin >> setpoints[i][0];
            std::cout << "Enter longitude for setpoint " << i+1 << ": ";
            std::cin >> setpoints[i][1];

        }

        transit.set_setpoints_handle(&setpoints);

        transit.trip_ongoing = true;

        while(transit.trip_ongoing){

            data.download();
            transit.run();
            std::cout << "working" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(20000));

        }
    }




    return 0;
}
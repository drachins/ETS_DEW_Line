
#include<iostream>
#include<memory>

#include "realtimereader.h"
#include "data_download.h"






int main(){


    DataDownload data;

    data.initialize();

    transit_realtime::FeedMessage trip_feed;
    transit_realtime::FeedMessage vehicle_feed;

    std::shared_ptr<RealTimeReader> transit(new DataDownload(trip_feed, vehicle_feed));

    /*std::cout << "Enter route#: ";
    getline(std::cin, transit.route_number);

    std::cout << "Enter departure time: ";
    getline(std::cin, transit.arrive_time);

    std::cout << "Enter stop id: ";
    getline(std::cin, transit.stop_id);*/

    transit->route_number = "005";
    transit->arrive_time = "11:16:00 PM";
    transit->stop_id = "1622";

    transit->launch();

    while(data.operating){

        if(data.transmission_complete){

            data.download();
            std::this_thread::sleep_for(std::chrono::milliseconds(30000));
        }
        
        else{
            
            transit->SetDataBool(true);

        }

     }


    return 0;
}
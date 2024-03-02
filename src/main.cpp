
#include<iostream>
#include<memory>

#include "realtimereader.h"
#include "data_download.h"






int main(){


    DataDownload data;

    data.initialize();

    //data.launch();

    while(true){

        std::ofstream* output1 = NULL;
        std::ofstream* output2 = NULL;
        output1 = new std::ofstream("TripUpdates.pb", std::ios::binary | std::ios::trunc);
        output2 = new std::ofstream("VehiclePositions.pb", std::ios::binary | std::ios::trunc);
        data.SetWriteDataObj1(output1);
        data.SetWriteDataObj2(output2);

        data.download();

        delete output1;
        delete output2;

        std::cout << "working" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(20000));



    }

    //transit_realtime::FeedMessage trip_feed;
    //transit_realtime::FeedMessage vehicle_feed;

    //std::shared_ptr<RealTimeReader> transit(new RealTimeReader(trip_feed, vehicle_feed));

    /*std::cout << "Enter route#: ";
    getline(std::cin, transit.route_number);

    std::cout << "Enter departure time: ";
    getline(std::cin, transit.arrive_time);

    std::cout << "Enter stop id: ";
    getline(std::cin, transit.stop_id);

    transit->route_number = "005";
    transit->arrive_time = "11:16:00 PM";
    transit->stop_id = "1622";

    transit->launch();

    while(data.operating){

        if(data.transmission_complete){

            data.download();
        }
        
        else{
            
            transit->SetDataBool(true);

        }

        std::this_thread::sleep_for(std::chrono::milliseconds(30000));

     }*/


    return 0;
}
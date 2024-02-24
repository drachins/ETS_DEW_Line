
#include<iostream>
#include<fstream>

#include "realtimereader.h"
#include "trip.h"
#include "gtfs-realtime.pb.h"


int main(){

    const char* FILE1 = "/Users/davidrachinsky/the_workspace/realtime_transit/build/TripUpdates.pb";
    const char* FILE2 = "/Users/davidrachinsky/the_workspace/realtime_transit/build/VehiclePositions.pb";

    std::fstream input1(FILE1, std::ios::in | std::ios::binary);
    std::fstream input2(FILE2, std::ios::in | std::ios::binary);


    transit_realtime::FeedMessage trip_feed;
    transit_realtime::FeedMessage vehicle_feed;

    if(!trip_feed.ParseFromIstream(&input1)){
        std::cerr << "Cant parse trip message!" << std::endl;
        return -1;
    }
    if(!vehicle_feed.ParseFromIstream(&input2)){
        std::cerr << "Cant parse vehicle message!" << std::endl;
        return -1;
    }

    

    RealTimeReader transit(trip_feed, vehicle_feed);
    Trip* trip;


    std::cout << "Enter route#: ";
    getline(std::cin, transit.route_number);

    std::cout << "Enter departure time: ";
    getline(std::cin, transit.arrive_time);

    std::cout << "Enter stop id: ";
    getline(std::cin, transit.stop_id);

    trip = transit.ExtractInfo();

    std::cout << "Route #: " << trip->get_route_no() << " Bus Stop ID: " << transit.stop_id << " Departure Time: " << transit.arrive_time << std::endl;
    std::cout << "Bus #: " << trip->get_bus_no() << " Location: " << trip->get_latitude() << ", " << trip->get_longitude() << std::endl;

    return 0;
}
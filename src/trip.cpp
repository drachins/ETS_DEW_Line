
#include "trip.h"
#include <iostream>
Trip::Trip(){}

void Trip::set_bus_stops(transit_realtime::TripUpdate trip_update){

    

    for(int i = 0; i < trip_update.stop_time_update_size(); i++){

        const transit_realtime::TripUpdate_StopTimeUpdate& stop_time = trip_update.stop_time_update(i);
        const transit_realtime::TripUpdate_StopTimeEvent& departure = stop_time.departure();

        Bus_Stop bus_stop;
        bus_stop.stop_id = stop_time.stop_id();
        bus_stop.stop_time = departure.time();
        bus_stops.push_back(bus_stop);

    }

}

void Trip::set_bus_no(std::string _bus_no){

    bus_no = _bus_no;
    std::cout << "echo0" << std::endl;

}

Trip::~Trip(){}



#include "trip.h"
#include <iostream>
Trip::Trip(){}

void Trip::set_bus_stops(transit_realtime::TripUpdate trip_update){

    uint16_t mst = 25200;

    bus_stops.erase(bus_stops.begin(), bus_stops.end());
    
    for(int i = 0; i < trip_update.stop_time_update_size(); i++){

        const transit_realtime::TripUpdate_StopTimeUpdate& stop_time = trip_update.stop_time_update(i);
        const transit_realtime::TripUpdate_StopTimeEvent& departure = stop_time.departure();

        const int64_t time = departure.time();
        const int32_t delay = departure.delay();
        date::sys_seconds tp{std::chrono::seconds{time - delay - mst}};
        std::string time_str = date::format("%I:%M:%S %p", tp);

        Bus_Stop bus_stop;
        bus_stop.stop_id = stop_time.stop_id();
        bus_stop.stop_time = time_str;
        bus_stops.push_back(bus_stop);

    }

}

void Trip::set_bus_no(std::string _bus_no){

    bus_no = _bus_no;

}

Trip::~Trip(){}


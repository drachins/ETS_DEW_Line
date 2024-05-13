#ifndef REALTIMEREADER_H
#define REALTIMEREADER_H

#include <string>
#include <vector>
#include <fstream>
#include <iterator>
#include <iostream>
#include <stdio.h>
#include <iomanip>


#include "gtfs-realtime.pb.h"
#include "trip.h"
#include "realtimereader.h"

struct System_Bus_Stops{

    std::string stop_number;
    float stop_lattitude;
    float stop_longitude;

};



class RealTimeReader{

 public:

    RealTimeReader();
    ~RealTimeReader();

    void run();
    void set_setpoints_handle(std::vector<std::vector<float>>* _setpoints){setpoints = _setpoints;};


    bool trip_ongoing{false};


    std::string route_number;
    std::string arrive_time;
    std::string stop_id;
    float setpoint_lat;
    float setpoint_long;

    transit_realtime::FeedMessage trip_feed;
    transit_realtime::FeedMessage vehicle_feed;


    Trip* bus_trip;


 private:

    bool CheckForInfo(std::vector<Bus_Stop>* _bus_stops);
    void TrackBus();
    bool CheckIfPastSetpoint();
    void ExtractBusStopInfo();

    void ExtractTripInfo();
    void ExtractVehicleInfo();
    void FindLastStop(Trip* bus_trip);

    bool past_setpoint{false};
    bool first_operation{true};
    std::vector<std::vector<float>>* setpoints;
    std::vector<std::vector<std::string>>* bus_stops;
    std::vector<System_Bus_Stops> sys_bus_stops;
    System_Bus_Stops last_stop_location;

   
    std::vector<const transit_realtime::FeedEntity> trip_ent;
    std::vector<const transit_realtime::FeedEntity> vehicle_ent;
    const char* filepath_trip = "/Users/davidrachinsky/the_workspace/realtime_transit/build/TripUpdate.pb";
    const char* filepath_vehicle = "/Users/davidrachinsky/the_workspace/realtime_transit/build/VehiclePositions.pb";    

};

#endif
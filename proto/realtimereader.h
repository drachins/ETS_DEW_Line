#ifndef REALTIMEREADER_H
#define REALTIMEREADER_H

#include <string>
#include <vector>
#include <fstream>
#include <iterator>
#include <iostream>
#include <stdio.h>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <tuple>


#include "gtfs-realtime.pb.h"
#include "trip.h"
#include "realtimereader.h"


struct Setpoints{

    float setpoint_long;
    float setpoint_latt;
    int setpoint_index;

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

    void ExtractShapeInfo();
    std::vector<uint8_t>FindCommas(std::string _line);
    bool FindNearestPoint(std::tuple<float, float, uint16_t> bus_loc, std::tuple<float, float, uint16_t> shape_point, float delta);
    uint16_t GetBearing(float _delta_latt, float _delta_long);

    void ExtractTripInfo();
    void ExtractVehicleInfo();

    bool past_setpoint{false};
    bool first_operation{true};
    std::vector<std::vector<float>>* setpoints;
    std::vector<std::tuple<float, float, uint16_t>> route_shape;
    int16_t bus_trip_index;
    std::tuple<float, float, uint16_t> current_bus_pos;

   
    std::vector<const transit_realtime::FeedEntity> trip_ent;
    std::vector<const transit_realtime::FeedEntity> vehicle_ent;
    const char* filepath_trip = "/Users/davidrachinsky/the_workspace/realtime_transit/build/TripUpdate.pb";
    const char* filepath_vehicle = "/Users/davidrachinsky/the_workspace/realtime_transit/build/VehiclePositions.pb";    

};

#endif
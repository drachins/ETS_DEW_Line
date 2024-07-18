#ifndef REALTIMEREADER_H
#define REALTIMEREADER_H

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <cmath>
#include <utility>
#include <algorithm>


#include "gtfs-realtime.pb.h"
#include "trip.h"


class RealTimeReader{

 public:

    RealTimeReader();
    ~RealTimeReader();

    void run();
    void set_setpoints(std::vector<std::vector<float>> _setpoints){u_setpoints = _setpoints;};

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
    std::vector<std::tuple<float, float, int>> ExtractShapeInfo(std::string trip_no);
    void SetSetpoints();
    std::vector<int>FindCommas(std::string _line);
    void FindNearestPoint(int& _index, std::vector<std::tuple<float, float, int>>* _route_shape);
    int GetBearing(float _delta_latt, float _delta_long);
    void ConcShapeVectors();

    void ExtractTripInfo();
    void ExtractVehicleInfo();

    bool past_setpoint{false};
    bool first_operation{true};
    std::vector<std::vector<float>> u_setpoints;
    std::vector<int> setpoint_indices;
    std::vector<std::tuple<float, float, int>> route_shape;
    std::vector<std::tuple<float, float, int>> first_route_shape;
    int index{0};
    std::tuple<float, float, int> current_bus_pos;

    std::vector<const transit_realtime::FeedEntity> trip_ent;
    std::vector<const transit_realtime::FeedEntity> vehicle_ent;
    const char* filepath_trip = "/Users/davidrachinsky/the_workspace/realtime_transit/build/TripUpdate.pb";
    const char* filepath_vehicle = "/Users/davidrachinsky/the_workspace/realtime_transit/build/VehiclePositions.pb";    
};

#endif
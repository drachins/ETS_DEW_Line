#ifndef REALTIMEREADER_H
#define REALTIMEREADER_H

#include<string>
#include<vector>
#include<fstream>

#include "gtfs-realtime.pb.h"
#include "trip.h"
#include "realtimereader.h"


class RealTimeReader{

 public:

    RealTimeReader(const transit_realtime::FeedMessage _trip_feed, const transit_realtime::FeedMessage _vehicle_feed);
    ~RealTimeReader();


    //void launch();
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

    void ExtractTripInfo();
    void ExtractVehicleInfo();

    bool past_setpoint{false};
    bool first_operation{true};
    std::vector<std::vector<float>>* setpoints;
    std::string last_stop;
   
    std::vector<const transit_realtime::FeedEntity> trip_ent;
    std::vector<const transit_realtime::FeedEntity> vehicle_ent;
    const char* filepath_trip = "/Users/davidrachinsky/the_workspace/realtime_transit/build/TripUpdate.pb";
    const char* filepath_vehicle = "/Users/davidrachinsky/the_workspace/realtime_transit/build/VehiclePositions.pb";    

};

#endif
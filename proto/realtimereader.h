#ifndef REALTIMEREADER_H
#define REALTIMEREADER_H

#include<string>
#include<vector>
#include<chrono>
#include<thread>
#include<fstream>

#include "gtfs-realtime.pb.h"
#include "date.h"
#include "trip.h"
#include "realtimereader.h"


class RealTimeReader{

 public:

    RealTimeReader(const transit_realtime::FeedMessage _trip_feed, const transit_realtime::FeedMessage _vehicle_feed);
    ~RealTimeReader();


    //void launch();
    void run();

    bool first_operation{true};


    std::string route_number;
    std::string arrive_time;
    std::string stop_id;
    float setpoint_lat;
    float setpoint_long;

    transit_realtime::FeedMessage trip_feed;
    transit_realtime::FeedMessage vehicle_feed;


    Trip* bus_trip;


 private:

    bool CheckForInfo(const transit_realtime::TripUpdate* _trip, const transit_realtime::TripDescriptor* _trip_disc);
    void TrackBus();
    bool CheckIfPastSetpoint();

    void ExtractTripInfo();
    void ExtractVehicleInfo();

    float latitude_delta = 100000;
    float longitude_delta = 100000;
    bool past_setpoint{false};
   
    std::vector<const transit_realtime::FeedEntity> trip_ent;
    std::vector<const transit_realtime::FeedEntity> vehicle_ent;
    std::vector<std::thread> _bus_thread;
    const char* filepath_trip = "/Users/davidrachinsky/the_workspace/realtime_transit/build/TripUpdate.pb";
    const char* filepath_vehicle = "/Users/davidrachinsky/the_workspace/realtime_transit/build/VehiclePositions.pb";    

};

#endif
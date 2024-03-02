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
    Trip* ExtractInfo();

    bool GetDataBool(){return data_ready;};
    void SetDataBool(bool _data_ready){data_ready = _data_ready;};

    

    //void launch();
    void run();

    bool operating{true};


    std::string route_number;
    std::string arrive_time;
    std::string stop_id;

    transit_realtime::FeedMessage trip_feed;
    transit_realtime::FeedMessage vehicle_feed;

    Trip* bus_trip;


 private:

    bool CheckForInfo(const transit_realtime::TripUpdate* _trip, const transit_realtime::TripDescriptor* _trip_disc);
   

    std::mutex _reader_lock;
    bool data_ready{false};
    std::vector<const transit_realtime::FeedEntity> trip_ent;
    std::vector<const transit_realtime::FeedEntity> vehicle_ent;
    std::vector<std::thread> _bus_thread;
    const char* filepath_trip = "/Users/davidrachinsky/the_workspace/realtime_transit/build/TripUpdate.pb";
    const char* filepath_vehicle = "/Users/davidrachinsky/the_workspace/realtime_transit/build/VehiclePositions.pb";    

};

#endif
#ifndef REALTIMEREADER_H
#define REALTIMEREADER_H

#include<string>
#include<vector>
#include<chrono>

#include "gtfs-realtime.pb.h"
#include "date.h"
#include "trip.h"


class RealTimeReader{

 public:

    RealTimeReader(const transit_realtime::FeedMessage _trip_feed, const transit_realtime::FeedMessage _vehicle_feed);
    ~RealTimeReader();
    Trip* ExtractInfo();

    std::string route_number;
    std::string arrive_time;
    std::string stop_id;

    const transit_realtime::FeedMessage trip_feed;
    const transit_realtime::FeedMessage vehicle_feed;

    Trip bus_trip;


 private:

    //void launch();
    //void run();
    bool CheckForInfo(const transit_realtime::TripUpdate* _trip, const transit_realtime::TripDescriptor* _trip_disc);

    std::vector<const transit_realtime::FeedEntity> trip_ent;
    std::vector<const transit_realtime::FeedEntity> vehicle_ent;

    //std::thread bus_thread;

};

#endif
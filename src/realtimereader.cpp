
#include "/Users/davidrachinsky/the_workspace/realtime_transit/proto/realtimereader.h"


RealTimeReader::RealTimeReader(const transit_realtime::FeedMessage _trip_feed, const transit_realtime::FeedMessage _vehicle_feed)
    :trip_feed(_trip_feed),
     vehicle_feed(_vehicle_feed){}


bool RealTimeReader::CheckForInfo(const transit_realtime::TripUpdate* _trip, const transit_realtime::TripDescriptor* _trip_disc){

    uint32_t smt = 25200;

    for(int i = 0; i < _trip->stop_time_update_size(); i++){

        const transit_realtime::TripUpdate_StopTimeUpdate& stop_time = _trip->stop_time_update(i);
        const transit_realtime::TripUpdate_StopTimeEvent& departure = stop_time.departure();
        const int64_t time = departure.time();
        const int32_t delay = departure.delay();

        date::sys_seconds tp{std::chrono::seconds{time - delay - smt}};
        std::string time_str = date::format("%I:%M:%S %p", tp);
        if(stop_time.stop_id() == stop_id){
            std::cout << _trip_disc->route_id() << " : " << time_str << " : " << stop_time.stop_id() << std::endl;
            std::cout << departure.delay() << std::endl;
        }
        if(time_str == arrive_time && stop_time.stop_id() == stop_id){
            return true;
        }

    }
    return false;



}

Trip* RealTimeReader::ExtractInfo(){


    for(int i = 0; i < trip_feed.entity_size(); i++){

        trip_ent.push_back(trip_feed.entity(i));

    }

    for(int t = 0; t < vehicle_feed.entity_size(); t++){

        vehicle_ent.push_back(vehicle_feed.entity(t));

    }

    for(auto&& itr : trip_ent){

        const transit_realtime::TripUpdate& trip = itr.trip_update();
        const transit_realtime::TripDescriptor& trip_disc = trip.trip();

        if(trip_disc.route_id() == route_number && CheckForInfo(&trip, &trip_disc)){

            const transit_realtime::VehicleDescriptor& vehicle =  trip.vehicle();

            bus_trip.set_trip_no(itr.id()); bus_trip.set_bus_no(vehicle.label()); bus_trip.set_route_no(trip_disc.route_id());
            bus_trip.set_bus_stops(trip);

        }

    }

    for(auto&& itr : vehicle_ent){

        
        if(itr.id() == bus_trip.get_bus_no()){
            
            const transit_realtime::VehiclePosition& vehicle = itr.vehicle();
            const transit_realtime::Position& position = vehicle.position();

            bus_trip.set_longitude(position.longitude());
            bus_trip.set_latitude(position.latitude());


        }

    }
    
    return &bus_trip;

}

RealTimeReader::~RealTimeReader(){}




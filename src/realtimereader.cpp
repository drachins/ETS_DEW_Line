
#include "/Users/davidrachinsky/the_workspace/realtime_transit/proto/realtimereader.h"


RealTimeReader::RealTimeReader(const transit_realtime::FeedMessage _trip_feed, const transit_realtime::FeedMessage _vehicle_feed)
    :trip_feed(_trip_feed),
     vehicle_feed(_vehicle_feed){

        bus_trip = new(Trip);

    }



void RealTimeReader::run(){


    std::ifstream input1("TripUpdates.pb", std::ios::binary);
    std::ifstream input2("VehiclePositions.pb", std::ios::binary);


    if(!trip_feed.ParseFromIstream(&input1)){
        std::cerr << "Can't parse trip message!" << std::endl;
    }

    if(!vehicle_feed.ParseFromIstream(&input2)){
        std::cerr << "Can't parse vehicle message!" << std::endl;
    }


    if(first_operation){
        ExtractTripInfo();
        ExtractVehicleInfo();
    }
    else{
        TrackBus();
    }


    std::cout << "Route #: " << bus_trip->get_route_no() << " Bus Stop ID: " << stop_id << " Departure Time: " << arrive_time << std::endl;
    std::cout << "Bus #: " << bus_trip->get_bus_no() << " Location: " << bus_trip->get_latitude() << ", " << bus_trip->get_longitude() << ", Bearing: " << bus_trip->get_bearing() << std::endl;

    if(past_setpoint){
        std::cout << "Bus #: " << bus_trip->get_bus_no() << " Has passed set point at [" << setpoint_lat << " , " << setpoint_long << "]" << std::endl;
        past_setpoint = false;
    }

}


bool RealTimeReader::CheckForInfo(const transit_realtime::TripUpdate* _trip){

    uint32_t mst = 21600;

    for(int i = 0; i < _trip->stop_time_update_size(); i++){

        const transit_realtime::TripUpdate_StopTimeUpdate& stop_time = _trip->stop_time_update(i);
        const transit_realtime::TripUpdate_StopTimeEvent& departure = stop_time.departure();
        const transit_realtime::VehicleDescriptor& vehicle =  _trip->vehicle();
        const int64_t time = departure.time();
        const int32_t delay = departure.delay();
        date::sys_seconds tp{std::chrono::seconds{time - delay - smt}};
        std::string time_str = date::format("%I:%M:%S %p", tp);
        
        if(stop_time.stop_id() == "1271"){
            std::cout << vehicle.label() << " " << time_str << " " << stop_time.stop_id() << " " << delay << std::endl;
        }
        if(time_str == arrive_time && stop_time.stop_id() == stop_id){
            return true;
        }

    }
    return false;



}


void RealTimeReader::ExtractTripInfo(){

    for(int i = 0; i < trip_feed.entity_size(); i++){

        trip_ent.push_back(trip_feed.entity(i));

    }    


    for(auto&& itr : trip_ent){

        const transit_realtime::TripUpdate& trip = itr.trip_update();
        const transit_realtime::TripDescriptor& trip_disc = trip.trip();
        const transit_realtime::VehicleDescriptor& vehicle =  trip.vehicle();
        
        if((trip_disc.route_id() == route_number) && CheckForInfo(&trip)){

            std::cout << trip_disc.route_id() << std::endl;
            bus_trip->set_trip_no(itr.id()); bus_trip->set_bus_no(vehicle.label()); bus_trip->set_route_no(trip_disc.route_id());
            bus_trip->set_bus_stops(trip);

        }
    }

    first_operation = false;

}

void RealTimeReader::ExtractVehicleInfo(){

    for(int t = 0; t < vehicle_feed.entity_size(); t++){

        vehicle_ent.push_back(vehicle_feed.entity(t));

    }

    for(auto&& itr : vehicle_ent){

        
        if(itr.id() == bus_trip->get_bus_no()){
            
            const transit_realtime::VehiclePosition& vehicle = itr.vehicle();
            const transit_realtime::Position& position = vehicle.position();

            bus_trip->set_longitude(position.longitude());
            bus_trip->set_latitude(position.latitude());
            bus_trip->set_bearing(position.bearing());


        }

    }



}

void RealTimeReader::TrackBus(){

    ExtractVehicleInfo();

    for(std::vector<float> str : *setpoints){

        if(abs(bus_trip->get_latitude() - str[0])  <= 0.001 && abs(bus_trip->get_longitude() - str[1]) <= 0.001){
            past_setpoint = CheckIfPastSetpoint();
        }
    }

    /*latitude_delta = abs(bus_trip->get_latitude() - setpoint_lat);
    longitude_delta = abs(bus_trip->get_longitude() - setpoint_long);

    if(latitude_delta <= 0.01 && longitude_delta <= 0.01){
        
        past_setpoint = CheckIfPastSetpoint();

    }*/
}

bool RealTimeReader::CheckIfPastSetpoint(){

    if(bus_trip->get_bearing()  == 0){
        if(bus_trip->get_latitude() >= setpoint_lat){
            return true;
        }
    }

    else if(bus_trip->get_bearing() > 0 && bus_trip->get_bearing() < 90){
        if(bus_trip->get_latitude() >= setpoint_lat && bus_trip->get_longitude() >= setpoint_long){
            return true;
        }
    }

    else if(bus_trip->get_bearing() == 90){
        if(bus_trip->get_longitude() >= setpoint_long){
            return true;
        }
    }

    else if(bus_trip->get_bearing() > 90 && bus_trip->get_bearing() < 180){
        if(bus_trip->get_latitude() >= setpoint_lat && bus_trip->get_longitude() >= setpoint_long){
            return true;
        } 
    }

    else if(bus_trip->get_bearing() == 180){
        if(bus_trip->get_latitude() >= setpoint_lat){
            return true;
        }
    }

    else if(bus_trip->get_bearing() > 180 && bus_trip->get_bearing() < 270){
        if(bus_trip->get_latitude() <= setpoint_lat && bus_trip->get_longitude() <= setpoint_long){
            return true;
        }

    }

    else if(bus_trip->get_bearing() == 270){
        if(bus_trip->get_longitude() <= setpoint_long){
            return true;
        }
    }

    else if(bus_trip->get_bearing() > 270){
        if(bus_trip->get_latitude() >= setpoint_lat && bus_trip->get_longitude() <= setpoint_long){
            return true;
        }
    }

    return false;

}

RealTimeReader::~RealTimeReader(){

    delete bus_trip;
    //_bus_thread[0].join();

}




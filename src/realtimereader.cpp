
#include "realtimereader.h"


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

    if(bus_trip->get_current_stop() == bus_trip->get_bus_stops()->back().stop_id){
        trip_ongoing = false;
        std::cout << "Bus has reached final stop. " << std::endl;
    }

}


bool RealTimeReader::CheckForInfo(std::vector<Bus_Stop>* _bus_stops){


    for(auto&& itr : *_bus_stops){

        if(itr.stop_id == stop_id && itr.stop_time == arrive_time){
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

        bus_trip->set_bus_stops(trip);
        
        if((trip_disc.route_id() == route_number) && CheckForInfo(bus_trip->get_bus_stops())){

            bus_trip->set_trip_no(itr.id()); bus_trip->set_bus_no(vehicle.label()); bus_trip->set_route_no(trip_disc.route_id());
            bus_trip->set_bus_stops(trip);

            for(Bus_Stop itr : *bus_trip->get_bus_stops()){

                std::cout << itr.stop_id << " " << itr.stop_time << std::endl;

            }

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
            bus_trip->set_current_stop(vehicle.stop_id());


        }

    }

    



}

void RealTimeReader::TrackBus(){

    ExtractVehicleInfo();

    for(int i = 0; i < setpoints->size(); i++){

        setpoint_lat = setpoints->at(i).at(0);
        setpoint_long = setpoints->at(i).at(1);
        std::cout << setpoint_lat << ", " << setpoint_long << std::endl;
        if(abs(bus_trip->get_latitude() - setpoint_lat)  <= 0.003 && abs(bus_trip->get_longitude() - setpoint_long) <= 0.003){
            past_setpoint = CheckIfPastSetpoint();
        }
        if(past_setpoint){
            setpoints->erase(setpoints->begin() + i);
            break;
        }
    }

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
        if(bus_trip->get_latitude() <= setpoint_lat){
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

}




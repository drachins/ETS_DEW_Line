
#include "realtimereader.h"


RealTimeReader::RealTimeReader(){

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

    ExtractBusStopInfo();

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

    if(abs(bus_trip->get_latitude() - last_stop_location.stop_lattitude) < 0.0001 && abs(bus_trip->get_longitude() - last_stop_location.stop_longitude) < 0.0001){
        trip_ongoing = false;
    }



}

void RealTimeReader::ExtractBusStopInfo(){

std::ifstream input("stops.txt");

    std::string line, stop_no, stop_latt, stop_long;
    std::string service_tag_0 = "0,,0,0"; 
    std::string service_tag_1 = "0,,0,1";

    if(input.is_open()){

        while(std::getline(input, line)){

            std::string service_tag = line.substr(line.size() - std::distance(line.end() - 7, line.end()), 6);

            if(service_tag == service_tag_0 || service_tag == service_tag_1){
                stop_no = line.substr(0, 5);
                if(stop_no.at(4) == ','){
                    stop_no.erase(4);
                }
                stop_latt = line.substr(line.size() - std::distance(line.end() - 35, line.end()), 9);
                stop_long = line.substr(line.size() - std::distance(line.end() - 25, line.end()), 11);

                System_Bus_Stops bus_stop;
                bus_stop.stop_number = stop_no;
                try{
                    bus_stop.stop_lattitude = std::stof(stop_latt);
                    bus_stop.stop_longitude = std::stof(stop_long);
                }
                catch(...){
                    continue;
                }


                sys_bus_stops.push_back(bus_stop);
            }

        }
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

        bus_trip->set_bus_stops(trip_feed.entity(i).trip_update());

        if(trip_feed.entity(i).trip_update().trip().route_id() == route_number && CheckForInfo(bus_trip->get_bus_stops())){

            bus_trip->set_trip_no(trip_feed.entity(i).id()); bus_trip->set_bus_no(trip_feed.entity(i).trip_update().vehicle().label());
            bus_trip->set_route_no(trip_feed.entity(i).trip_update().trip().route_id()); bus_trip->set_bus_stops(trip_feed.entity(i).trip_update());

            for(auto& v : *bus_trip->get_bus_stops()){
                std::cout << v.stop_id << " " << v.stop_time << std::endl;
            }

            FindLastStop(bus_trip);  

        }

    }    

    first_operation = false;

}

void RealTimeReader::FindLastStop(Trip* bus_trip){

    std::string last_stop;

    if(bus_trip->get_bus_stops()->back().stop_time == "06:00:00 PM"){
        last_stop = bus_trip->get_bus_stops()->at(bus_trip->get_bus_stops()->size() - 2).stop_id;
    }
    else{
        last_stop = bus_trip->get_bus_stops()->back().stop_id;
    }

    std::cout << last_stop << std::endl;

    for(auto& v : sys_bus_stops){

        if(v.stop_number == last_stop){
            last_stop_location.stop_number = last_stop;
            last_stop_location.stop_lattitude = v.stop_lattitude;
            last_stop_location.stop_longitude = v.stop_longitude;
        }

    }

    std::cout << last_stop_location.stop_number << " " << last_stop_location.stop_lattitude << " " << last_stop_location.stop_longitude << std::endl;

}

void RealTimeReader::ExtractVehicleInfo(){

    for(int t = 0; t < vehicle_feed.entity_size(); t++){

        if(vehicle_feed.entity(t).id() == bus_trip->get_bus_no()){

            bus_trip->set_longitude(vehicle_feed.entity(t).vehicle().position().longitude());
            bus_trip->set_latitude(vehicle_feed.entity(t).vehicle().position().latitude());
            bus_trip->set_bearing(vehicle_feed.entity(t).vehicle().position().bearing());
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




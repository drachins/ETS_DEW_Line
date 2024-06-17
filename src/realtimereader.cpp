
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


    if(first_operation){
        ExtractTripInfo();
        ExtractVehicleInfo();
        ExtractShapeInfo();
    }
    else{
        TrackBus();
    }


    std::cout << "Route #: " << bus_trip->get_route_no() << " Bus Stop ID: " << stop_id << " Departure Time: " << arrive_time << std::endl;
    std::cout << "Bus #: " << bus_trip->get_bus_no(); 
    printf(" Location: [%f5, %f5], Bearing: %f\n", bus_trip->get_latitude(), bus_trip->get_longitude(), bus_trip->get_bearing());

    if(past_setpoint){
        
        std::cout << "################################################################################################################" << std::endl;
        std::cout << std::fixed << "##### Bus #: " << bus_trip->get_bus_no() << " Has passed set point at [" << std::setprecision(6) << setpoint_lat << " , " << std::setprecision(6) << setpoint_long << "] #####" << std::endl;
        std::cout << "################################################################################################################" << std::endl;
        past_setpoint = false;

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
            bus_trip->set_direction(trip_feed.entity(i).trip_update().trip().direction_id());


            for(auto& v : *bus_trip->get_bus_stops()){
                std::cout << v.stop_id << " " << v.stop_time << std::endl;
            }
            

        }

    }    

    first_operation = false;

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

    current_bus_pos = std::make_tuple(bus_trip->get_latitude(), bus_trip->get_longitude(), static_cast<uint16_t>(bus_trip->get_bearing()));
    FindNearestPoint();

    std::cout << std::endl;
    std::cout << "Index: " << index << std::endl;
    std::cout << std::get<0>(route_shape.at(index)) << ", " << std::get<1>(route_shape.at(index)) << ", " <<std::get<2>(route_shape.at(index)) << std::endl;

}

void RealTimeReader::ExtractShapeInfo(){

    std::fstream trip_input("trips.txt");
    std::fstream shape_input("shapes.txt");

    std::string trip_line;
    std::string shape_line;

    std::string shape_id;
    std::vector<uint8_t> n_commas;


    if(trip_input.is_open()){

        while(std::getline(trip_input, trip_line)){

            n_commas = FindCommas(trip_line);
            if(trip_line.substr(n_commas[1]+1, 8) == bus_trip->get_trip_no()){
                shape_id = trip_line.substr(n_commas[5]+1, n_commas[6] - n_commas[5] - 1);
            }
        }

    }


    std::cout << shape_id << std::endl;

    float latt_past = 0.0f, long_past = 0.0f;

    if(shape_input.is_open()){

        while(std::getline(shape_input, shape_line)){

            auto n_shape_id = shape_line.find(shape_id);

            if(n_shape_id != std::string::npos){

                auto n_latt = shape_line.find_first_of(",");
                auto n_long = shape_line.find_last_of(",");
  
                float latt_curr = stof(shape_line.substr(n_latt + 1, 9));
                float long_curr = stof(shape_line.substr(n_long - 11, 11));

                float delta_latt = latt_curr - latt_past;
                float delta_long = long_curr - long_past;

                uint16_t bearing = GetBearing(delta_latt, delta_long);

                if(bearing == UINT16_MAX){
                    bearing = std::get<2>(route_shape.back());
                }                

                std::tuple<float, float, uint16_t> shape_point{latt_past, long_past, bearing};
                route_shape.push_back(shape_point);

                latt_past = latt_curr;
                long_past = long_curr;


            }

        }

    }

    route_shape.erase(route_shape.begin() + 0);
    
    current_bus_pos = std::make_tuple(bus_trip->get_latitude(), bus_trip->get_longitude(), static_cast<uint16_t>(bus_trip->get_bearing()));
    FindNearestPoint();

    route_shape.erase(route_shape.begin(), route_shape.begin() + index);

    std::cout << index << std::endl;
    std::cout << std::endl;

    index = 0;

    for(auto& itr : route_shape){

        printf("%f6, %f6, %i\n", std::get<0>(itr), std::get<1>(itr), std::get<2>(itr));

    }

}

std::vector<uint8_t> RealTimeReader::FindCommas(std::string _line){

    std::vector<uint8_t> n_commas;

    for(int i = 0; i < _line.size(); i++){

        if(_line[i] == ','){
            n_commas.push_back(i);
        }

    }

    return n_commas;

}

void RealTimeReader::FindNearestPoint(){

    bool index_found{false};
    float delta = 0.0001;

    while(!index_found){

        for(int i = index; i < route_shape.size(); i++){
            if(sqrt(pow(std::get<0>(route_shape.at(i)) - std::get<0>(current_bus_pos), 2) + pow(std::get<1>(route_shape.at(i)) - std::get<1>(current_bus_pos), 2)) < delta && std::get<2>(current_bus_pos) == std::get<2>(route_shape.at(i))){
                index = i;
                index_found = true;
                std::cout << "TRUE" << std::endl;
                break;
            }
        }

        delta += 0.0001;

        if(delta > 0.01){
            std::cout << "Coudin't find nearest shape point" << std::endl;
            break;
        }

    }

}

uint16_t RealTimeReader::GetBearing(float _delta_latt, float _delta_long){

    uint16_t bearing;
    float hypot  = sqrt(pow(_delta_latt, 2) + pow(_delta_long, 2));

    if(_delta_latt == 0 && _delta_long == 0){
        return -1;
    }
    else{
        bearing  = static_cast<uint16_t>(acos(abs(_delta_long)/hypot) * 180/(2 * acos(0.0)));
    }
    

    if(bearing >= 0 && bearing <= 15){
        bearing = 0;
    }
    else if(bearing > 15 && bearing < 75){
        bearing = 45;
    }
    else if(bearing >= 75 && bearing <= 90){
        bearing = 90;
    }

    if(_delta_latt > 0 && _delta_long >= 0){
        return(bearing = 90 - bearing);
    }
    else if(_delta_latt <= 0 && _delta_long > 0){
        return(bearing += 90);
    }
    else if(_delta_latt  < 0 && _delta_long <= 0){
        return(bearing = 270 - bearing);
    }
    else if(_delta_latt >= 0 && _delta_long < 0){
        bearing += 270;
        if(bearing == 360){
            return(bearing = 0);
        }
        else{
            return bearing;
        }
    }

    return bearing;


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




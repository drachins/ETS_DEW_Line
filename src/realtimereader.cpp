
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
        SetSetpoints();
    }
    else{
        TrackBus();
    }


    std::cout << "Route #: " << bus_trip->get_route_no() << " Bus Stop ID: " << stop_id << " Departure Time: " << arrive_time << std::endl;
    std::cout << "Bus #: " << bus_trip->get_bus_no(); 
    printf(" Location: [%f5, %f5], Bearing: %f\n", bus_trip->get_latitude(), bus_trip->get_longitude(), bus_trip->get_bearing());




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

            for(auto&& v : *bus_trip->get_bus_stops()){
                std::cout << v.stop_id << " " << v.stop_time << std::endl;
            }
            

        }

    }    

    std::cout << "route shape of requested bus trip" << std::endl;
    route_shape = ExtractShapeInfo(bus_trip->get_trip_no());



}

void RealTimeReader::ExtractVehicleInfo(){



    std::string trip_numb; 
    for(int t = 0; t < vehicle_feed.entity_size(); t++){

        if(vehicle_feed.entity(t).id() == bus_trip->get_bus_no()){

            bus_trip->set_longitude(vehicle_feed.entity(t).vehicle().position().longitude());
            bus_trip->set_latitude(vehicle_feed.entity(t).vehicle().position().latitude());
            bus_trip->set_bearing(vehicle_feed.entity(t).vehicle().position().bearing());
            std::cout << "trip id: " << vehicle_feed.entity(t).vehicle().trip().trip_id() << std::endl;
            std::cout << bus_trip->get_bus_no() << std::endl;

            if(first_operation){
                if(vehicle_feed.entity(t).vehicle().trip().trip_id() != bus_trip->get_trip_no()){
                    std::cout << "route shape of current trip" << std::endl;
                    first_route_shape = ExtractShapeInfo(vehicle_feed.entity(t).vehicle().trip().trip_id());
                    std::vector<std::tuple<float, float, int>> route_shape_conc;
                    route_shape.reserve(first_route_shape.size() + route_shape.size());
                    route_shape_conc.insert(route_shape_conc.begin(), first_route_shape.begin(), first_route_shape.end());
                    route_shape_conc.insert(route_shape_conc.end(), route_shape.begin(), route_shape.end());
                    route_shape = route_shape_conc;

                    std::cout << "full route shape" << std::endl;
                    for(auto&& itr : route_shape){

                        printf("%f6, %f6, %i\n", std::get<0>(itr), std::get<1>(itr), std::get<2>(itr));

                    }
                    std::cout << std::endl;

                    first_operation = false;
                }
                else
                    first_operation = false;
            }
        }

    }


}

void RealTimeReader::TrackBus(){

    ExtractVehicleInfo();

    current_bus_pos = std::make_tuple(bus_trip->get_latitude(), bus_trip->get_longitude(), static_cast<int>(bus_trip->get_bearing()));

    FindNearestPoint(index, &route_shape);

    std::cout << std::endl;
    std::cout << "Index: " << index << std::endl;
    std::cout << std::get<0>(route_shape.at(index)) << ", " << std::get<1>(route_shape.at(index)) << ", " <<std::get<2>(route_shape.at(index)) << std::endl;

    if(auto ind = std::find_if(setpoint_indices.begin(), setpoint_indices.end(), [=](int ttr){return index >= ttr;}); ind != setpoint_indices.end()){
        int t_ind = std::distance(setpoint_indices.begin(), ind);
        std::cout << "################################################################################################################" << std::endl;
        std::cout << std::fixed << "##### Bus #: " << bus_trip->get_bus_no() << " Has passed setpoint " << "at [" << std::setprecision(6) << u_setpoints[t_ind].at(0) << " , " << std::setprecision(6) << u_setpoints[t_ind].at(1) << "] #####" << std::endl;
        std::cout << "################################################################################################################" << std::endl;
        setpoint_indices.erase(setpoint_indices.begin() + t_ind);
        u_setpoints.erase(u_setpoints.begin() + t_ind);
    }


}

std::vector<std::tuple<float, float, int>> RealTimeReader::ExtractShapeInfo(std::string trip_no){

    std::fstream trip_input("trips.txt");
    std::fstream shape_input("shapes.txt");

    std::string trip_line;
    std::string shape_line;

    std::string shape_id =  "FFFF";
    std::vector<int> n_commas;
    std::vector<std::tuple<float, float, int>> _route_shape;
    int _index{0};

    std::cout << bus_trip->get_trip_no() << std::endl;
    if(trip_input.is_open()){

        while(std::getline(trip_input, trip_line)){

            n_commas = FindCommas(trip_line);
            if(trip_line.substr(n_commas[1]+1, 8) == trip_no){
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

                int bearing = GetBearing(delta_latt, delta_long);

                if(bearing == -1){
                    bearing = std::get<2>(_route_shape.back());
                }                

                std::tuple<float, float, int> shape_point{latt_past, long_past, bearing};

                _route_shape.push_back(shape_point);

                latt_past = latt_curr;
                long_past = long_curr;


            }

        }

    }


    if(!_route_shape.empty()){

        _route_shape.erase(_route_shape.begin() + 0);

        current_bus_pos = std::make_tuple(bus_trip->get_latitude(), bus_trip->get_longitude(), static_cast<int>(bus_trip->get_bearing()));

        FindNearestPoint(_index, &_route_shape);

        _route_shape.erase(_route_shape.begin(), _route_shape.begin() + _index);

        std::cout << _index << std::endl;
        std::cout << std::endl;

        std::cout << std::endl;
        for(auto&& itr : _route_shape){

            printf("%f6, %f6, %i\n", std::get<0>(itr), std::get<1>(itr), std::get<2>(itr));

        }
        std::cout << std::endl;
        
;    }
    
    return _route_shape;

}

std::vector<int> RealTimeReader::FindCommas(std::string _line){

    std::vector<int> n_commas;

    for(int i = 0; i < _line.size(); i++){

        if(_line[i] == ','){
            n_commas.push_back(i);
        }

    }

    return n_commas;

}

void RealTimeReader::FindNearestPoint(int& _index, std::vector<std::tuple<float, float, int>>* _route_shape){

    float delta = 0.0001;
    bool index_found{false};

    while(!index_found){

        for(int i = _index; i < _route_shape->size(); i++){
            if(sqrt(pow(std::get<0>(_route_shape->at(i)) - std::get<0>(current_bus_pos), 2) + pow(std::get<1>(_route_shape->at(i)) - std::get<1>(current_bus_pos), 2)) < delta && std::get<2>(current_bus_pos) == std::get<2>(_route_shape->at(i))){
                _index = i;
                std::cout << "TRUE" << std::endl;
                index_found  = true;
                break;

            }
        }

        delta += 0.0001;

        if(delta > 0.001){
            std::cout << "Coudin't find nearest shape point" << std::endl;
            break;
        }

    }

}

int RealTimeReader::GetBearing(float _delta_latt, float _delta_long){

    int bearing;
    float hypot  = sqrt(pow(_delta_latt, 2) + pow(_delta_long, 2));

    if(_delta_latt == 0 && _delta_long == 0){
        return -1;
    }
    else{
        bearing  = static_cast<int>(acos(abs(_delta_long)/hypot) * 180/(2 * acos(0.0)));
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



void RealTimeReader::SetSetpoints(){


    for(auto itr : u_setpoints){

        std::vector<std::pair<float, int>> setpoint_distances;

        for(int t = 0; t < route_shape.size(); t++){

            float distance = sqrt(pow(itr.at(0) - std::get<0>(route_shape[t]), 2) + pow(itr.at(1) - std::get<1>(route_shape[t]), 2));
            int t_index = t;

            std::pair<float, int> setpoint_distance = std::make_pair(distance, t_index);
            setpoint_distances.push_back(setpoint_distance);
        }

        std::sort(setpoint_distances.begin(), setpoint_distances.end());

        setpoint_indices.push_back(std::get<1>(setpoint_distances.front()));

    }

    for(auto ntr : setpoint_indices){

        printf("%i: [%6f, %6f]\n", ntr, std::get<0>(route_shape[ntr]), std::get<1>(route_shape[ntr]));

    }

}


RealTimeReader::~RealTimeReader(){

    delete bus_trip;
}




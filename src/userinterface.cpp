
#include "userinterface.h"

void UserInterface::UserInput(){

        int no_setpoints = 0;

        std::cout << "Enter route#: ";
        getline(std::cin, transit->route_number);

        std::cout << "Enter departure time: ";
        getline(std::cin, transit->arrive_time);

        std::cout << "Enter stop id: ";
        getline(std::cin, transit->stop_id);

        std::cout << "Enter the number of setpoints you want to set: ";
        std::cin >> no_setpoints;
        
        std::vector<std::vector<float>> setpoints(no_setpoints, {0,0});

        for(int i = 0; i < no_setpoints; i++){

            std::cout << "Enter lattitude for setpoint " << i+1 << ": ";
            std::cin >> setpoints[i][0];
            std::cout << "Enter longitude for setpoint " << i+1 << ": ";
            std::cin >> setpoints[i][1];

        }


        realtime->set_setpoints(setpoints);  
  
}

void UserInterFace::Display(){

        std::cout << "Index: " << realtime->index << std::endl;
        std::cout << std::get<0>(route_shape.at(index)) << ", " << std::get<1>(route_shape.at(index)) << ", " <<std::get<2>(route_shape.at(index)) << ", " << std::get<3>(route_shape.at(index)) << std::endl;
        
        std::cout << "Route #: " << bus_trip->get_route_no() << " Bus Stop ID: " << stop_id << " Departure Time: " << arrive_time << std::endl;
        std::cout << "Bus #: " << bus_trip->get_bus_no(); 
        printf(" Location: [%f5, %f5], Bearing: %f\n", bus_trip->get_latitude(), bus_trip->get_longitude(), bus_trip->get_bearing());
}







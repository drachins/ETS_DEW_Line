#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <memory>
#include <functional>
#include <curl/curl.h>
#include <algorithm>

#include "/Users/davidrachinsky/the_workspace/realtime_transit/proto/gtfs-realtime.pb.h"
#include "/Users/davidrachinsky/the_workspace/realtime_transit/proto/date.h"
#include "trip.h"


bool check_for_stop_id_time(const transit_realtime::TripUpdate trip, std::string *_stop_id, std::string *_departure_time){

    uint32_t smt = 25200;

    for(int i = 0; i < trip.stop_time_update_size(); i++){

        const transit_realtime::TripUpdate_StopTimeUpdate& stop_time = trip.stop_time_update(i);
        const transit_realtime::TripDescriptor& trip_disc = trip.trip();
        const transit_realtime::TripUpdate_StopTimeEvent& departure = stop_time.departure();
        const int64_t time = departure.time();
        const int32_t delay = departure.delay();

        date::sys_seconds tp{std::chrono::seconds{time - delay - smt}};
        std::string time_str = date::format("%I:%M:%S %p", tp);
        if(stop_time.stop_id() == *_stop_id){
            std::cout << trip_disc.route_id() << " : " << time_str << " : " << stop_time.stop_id() << std::endl;
            std::cout << departure.delay() << std::endl;
        }
        if(time_str == *_departure_time && stop_time.stop_id() == *_stop_id){
            return true;
        }

    }
    return false;



}

void extract_info(transit_realtime::FeedMessage* trip_message, transit_realtime::FeedMessage* vehicle_message, 
                    std::string* _route_number, std::string* _departure_time, std::string* _stop_id){

    Trip _trip;

    for(int i = 0; i < trip_message->entity_size(); i++){

        const transit_realtime::FeedEntity& entity = trip_message->entity(i);
        const transit_realtime::TripUpdate& trip = entity.trip_update();
        const transit_realtime::TripDescriptor& trip_disc = trip.trip();

        if(trip_disc.route_id() == *_route_number && check_for_stop_id_time(trip, _stop_id, _departure_time)){

            const transit_realtime::VehicleDescriptor& vehicle =  trip.vehicle();

            _trip.set_trip_no(entity.id()); _trip.set_bus_no(vehicle.label()); _trip.set_route_no(trip_disc.route_id());
            _trip.set_bus_stops(trip);

        }



    }

    for(int t = 0; t < vehicle_message->entity_size(); t++){

        const transit_realtime::FeedEntity& entity = vehicle_message->entity(t);
        
        if(entity.id() == _trip.get_bus_no()){
            
            const transit_realtime::VehiclePosition& vehicle = entity.vehicle();
            const transit_realtime::Position& position = vehicle.position();

            _trip.set_longitude(position.longitude());
            _trip.set_latitude(position.latitude());


        }

    }
    

    std::cout << "Route #: " << _trip.get_route_no() << " Bus Stop ID: " << *_stop_id << " Departure Time: " << *_departure_time << std::endl;
    std::cout << "Bus #: " << _trip.get_bus_no() << " Location: " << _trip.get_latitude() << ", " << _trip.get_longitude() << std::endl;
}

/*size_t write_data_1(void* buffer, size_t size, size_t nmemb, void* userp){

    std::cout << "echo" << std::endl;
    size_t real_size = size * nmemb;
    std::fstream input((char*) buffer, std::ios::in | std::ios::binary);
    auto feed = static_cast<transit_realtime::FeedMessage*>(userp);
    feed->ParseFromIstream(&input);

    return real_size;


}

size_t write_data_2(void* buffer, size_t size, size_t nmemb, void* userp){

    std::cout << "echo" << std::endl;
    size_t real_size = size * nmemb;
    std::fstream input((char*) buffer, std::ios::in | std::ios::binary);
    auto feed = static_cast<transit_realtime::FeedMessage*>(userp);
    feed->ParseFromIstream(&input);
    

    return real_size;

}*/




int main(int argc, char* argv[]){

    curl_global_init(CURL_GLOBAL_ALL);

    CURL* curl_1;
    CURL* curl_2;


    if(argc < 2){
        std::cerr << "Usage: " << "GTFS_FEED" << std::endl;
        return -1;
    }

    transit_realtime::FeedMessage trip_feed;
    transit_realtime::FeedMessage vehicle_feed;

    /*curl_1 = curl_easy_init();
    curl_2 = curl_easy_init();

    curl_easy_setopt(curl_1, CURLOPT_URL, "http://gtfs.edmonton.ca/TMGTFSRealTimeWebService/TripUpdate/TripUpdates.pb");
    curl_easy_setopt(curl_2, CURLOPT_URL, "http://gtfs.edmonton.ca/TMGTFSRealTimeWebService/Vehicle/VehiclePositions.pb");

    curl_easy_setopt(curl_1, CURLOPT_WRITEFUNCTION, write_data_1);
    curl_easy_setopt(curl_1, CURLOPT_WRITEDATA, &trip_feed);
    curl_easy_setopt(curl_1, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(curl_1, CURLOPT_FOLLOWLOCATION, 1L);

    curl_easy_setopt(curl_2, CURLOPT_WRITEFUNCTION, write_data_2);
    curl_easy_setopt(curl_2, CURLOPT_WRITEDATA, &vehicle_feed);
    curl_easy_setopt(curl_2, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(curl_2, CURLOPT_FOLLOWLOCATION, 1L);

    CURLcode res1 = curl_easy_perform(curl_1);

    if(res1 != CURLE_OK){

        std::cerr << "curl_easy_perform() failed! " << curl_easy_strerror(res1) << std::endl;
        return -1;

    }
    else{
        std::cout << "res1 is ok" << std::endl;
    }

    CURLcode res2 = curl_easy_perform(curl_2);

    if(res2 != CURLE_OK){

        std::cerr << "curl_easy_perform() failed! " << curl_easy_strerror(res2) << std::endl;
        return -1;

    }
    else{
        std::cout << "res2 is ok" << std::endl;
    }

    std::cout << trip_feed.entity_size() << std::endl;
    std::cout << vehicle_feed.entity_size() << std::endl;*/

    std::fstream input1(argv[1], std::ios::in | std::ios::binary);
    std::fstream input2(argv[2], std::ios::in | std::ios::binary);

    if(!trip_feed.ParseFromIstream(&input1)){
        std::cerr << "Cant parse trip message!" << std::endl;
        return -1;
    }
    if(!vehicle_feed.ParseFromIstream(&input2)){
        std::cerr << "Cant parse vehicle message!" << std::endl;
        return -1;
    }
    
    std::string route_numbr;
    std::string arrive_time;
    std::string stop_id;


    std::cout << "Enter route#: ";
    getline(std::cin, route_numbr);

    std::cout << "Enter departure time: ";
    getline(std::cin, arrive_time);

    std::cout << "Enter stop id: ";
    getline(std::cin, stop_id);


    extract_info(&trip_feed, &vehicle_feed, &route_numbr, &arrive_time, &stop_id);

    //curl_easy_cleanup(curl_1);
    //curl_easy_cleanup(curl_2);

    return 0;


}
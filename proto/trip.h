#ifndef TRIP_H
#define TRIP_H


#include <vector>
#include <memory>
#include <string>
#include "/Users/davidrachinsky/the_workspace/realtime_transit/proto/gtfs-realtime.pb.h"

struct Bus_Stop{

   std::string stop_id;
   std::string stop_time;

};


class Trip{

 public:
    
   Trip();
   ~Trip();

   void set_trip_no(std::string _trip_no){trip_no = _trip_no;}
   void set_bus_no(std::string _bus_no);
   void set_route_no(std::string _route_no){route_no = _route_no;}
   void set_bus_stops(transit_realtime::TripUpdate trip_update);
   void set_latitude(float _lattitude){lattitude = _lattitude;} 
   void set_longitude(float _longitude){longitude = _longitude;}
   void set_bearing(float _bearing){bearing = _bearing;}

   float get_latitude(){return lattitude;}
   float get_longitude(){return longitude;}
   float get_bearing(){return bearing;}
   std::string get_bus_no(){return bus_no;}
   std::string get_route_no(){return route_no;}


 private:
   std::string trip_no;
   std::string route_no;
   std::string bus_no;
   std::vector<Bus_Stop> bus_stops;
   float longitude = 0;
   float lattitude = 0;
   float bearing = 0;


};

#endif
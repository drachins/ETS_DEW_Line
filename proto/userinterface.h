#ifindef USERINTERFACE_H
#define USERINTERFACE_H

#include<iostream>

#include "realtimereader.h"
#include "trip.h"

class UserInterface{
  public:
    UserInterface();
    void UserInput();
    void setRealTimeHandle(RealTimeReader* _realtime){realtime = _realtime;}
    void setTripHandle(Trip* _trip){trip = _trip;}
    ~UserInterface();
  private:

    RealTimeReader* realtime;
    Trip* trip;
};


#endif

#ifindef USERINTERFACE_H
#define USERINTERFACE_H

#include<iostream>


#include "realtimereader.h"

class UserInterface{
  public:
    UserInterface();
    void UserInput();
    void SetRealTimeHandle(RealTimeReader* _realtime){realtime = _realtime;}
    ~UserInterface();
  private:

    RealTimeReader* realtime;
};


#endif

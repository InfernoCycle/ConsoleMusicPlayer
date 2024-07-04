#define INFERNO_H
#ifdef INFERNO_H

#include <iostream>
#include "bass.h"

class Listener{
  public:
    void running(int i/*HCHANNEL &channel*/){
      bool play_trap = false;
      bool pause_trap = false;

      std::cout << "\n" << i << "\n";
      /*
      while(true){
        DWORD active = BASS_ChannelIsActive(channel);
        if(active == BASS_ACTIVE_STOPPED){
          break;
        }
        if(active == BASS_ACTIVE_PLAYING){
          if(!play_trap){
            std::cout << "is Playing \n";
            play_trap = true;
            pause_trap = false;
          }
        }
        if(active == BASS_ACTIVE_PLAYING){
          if(!pause_trap){
            std::cout << "is Paused\n";
            pause_trap = true;
            play_trap = false;
          }
        }
      }*/
    }
};

#endif
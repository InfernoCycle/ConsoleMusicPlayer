#define addon
#pragma once
#ifdef addon
#include <codecvt>
#include <locale>
#include <iostream>
#include "addon.h"
#include "bass.h"
#include "options.h"
#include <fstream>
#include <unistd.h>

namespace pincer{
  class control{
    private:
      bool loaded = false;
      int output_device = -1;
      float volume = 0.2f;
      int sample_rate = 44100;
      int offset = 0;
      int start_pos = 0;
      std::string dest_file = "";
      float original_volume;

    public:
      void helper(){
        std::cout << 
        "play = start current playing song\n" <<
        "stop = stop current playing song. unloads the file that was playing.\n" <<
        "pause = pauses the currently playing song. using 'play' starts the song at the current song position.\n" <<
        "restart = restarts the song from the beginning.\n" <<
        "forward = moves the song up 5 seconds.\n" <<
        "backwards = moves the song back 5 seconds.\n" <<
        "volume <0 to 100> = changes the volume of the computer from 0 to 100.\n" <<
        "unload = unsets the current song so you can load a new one. does the same thing as 'stop'\n" <<
        "load <filepath> = sets a new file to play. If one is already playing, then it will be unloaded.\n" <<
        "pos <default 'secs' | 'mins'> = shows current position out of the full length that the song is at.\n" <<
        "copy <dest_file> = makes a copy of the currently loaded file.\n" <<
        "loaded = shows the current file loaded.\n" <<
        "length = shows the length of the song.\n"; 
      }

      void load(char* filename, HCHANNEL *channel, float *origin_vol){
        std::cout << filename << "\n";
        if(this->loaded){
          return;
        }else{
          this->startSong(output_device, volume, sample_rate, offset, start_pos, filename, origin_vol, channel, false);
        }
      }

      void unload(float *origin_vol){
        if(this->loaded){
          this->stopSong(*origin_vol);
        }
      }

      bool getLoaded(){
        return this->loaded;
      }

      void length(HCHANNEL channel, std::string type="secs"){
        if(this->loaded){
          if(type == "mins"){
            const float playback_length_bytes = BASS_ChannelGetLength(channel, BASS_POS_BYTE);
            const float mins = BASS_ChannelBytes2Seconds(channel, playback_length_bytes) / 60;
            std::cout << "Length: " << mins << " Minutes\n";
          }else{
            const float playback_length_bytes = BASS_ChannelGetLength(channel, BASS_POS_BYTE);
            std::cout << "Length: " << BASS_ChannelBytes2Seconds(channel, playback_length_bytes) << " Seconds\n";
          }
        }
      }

      void copy_file_to(std::string src, std::string dest){
        if(src != dest){
          std::ifstream infile(src);

          std::ofstream outfile(dest);

          std::string data;

          while(std::getline(infile, data)){
            outfile << data;
            std::cout << data << "\n";
            outfile.flush();
          }

          outfile.close();
          infile.close();
        }
        
      }

      bool showOutputs(int output=-1){
        BASS_DEVICEINFO info;
        int count = 0;
        std::cout << "\n-------------------------------------------------------------------------------------------------------------\n";
        while(BASS_GetDeviceInfo(count, &info)){
          std::cout << "Device Number?: " << count+1 << "\n";
          std::cout << "Device Name: " << info.name << "\n-------------------------------------------------------------------------------------------------------------\n";
          if(count == output-1){
            return output;
          }
          count++;
        }
        return -1;
      }
      void showInputs(){

      }

      bool startSong(int output_device=-1, float volume=0.2f, int sample_rate=44000, int offset=0, int start_pos=0, char *filename=NULL, float *original_vol=NULL, HCHANNEL *channel=NULL, bool play_on_start=true){
        HSAMPLE hm;
        //std::cout<< "filename: " << filename << "\n";
        //HSTREAM stream = BASS_StreamCreateFile(FALSE, "SUI UZI - Imperfect.mp3.mp3", 0, 0, BASS_SAMPLE_MONO);
        BASS_Init(output_device, sample_rate, 0, 0, NULL);
        *original_vol = BASS_GetVolume();
        hm = BASS_SampleLoad(FALSE, filename, offset, 0, BASS_SAMPLE_LOOP, BASS_SAMPLE_MONO);
        std::cout << "Error Code: " << BASS_ErrorGetCode() << "\n";
        if(BASS_ErrorGetCode() != 0){
          this->loaded = false;
          return false;
        }
        this->loaded = true;
        BASS_SetVolume(volume);
        //HCHANNEL channel = BASS_SampleGetChannel(hm, FALSE);
        *channel = BASS_SampleGetChannel(hm, FALSE);
        BASS_ChannelSetPosition(*channel, BASS_ChannelSeconds2Bytes(*channel, start_pos), BASS_POS_BYTE);
        
        if(play_on_start){
          BASS_ChannelPlay(*channel, FALSE);
        }

        std::cout << "Error Code: " << BASS_ErrorGetCode() << "\n";

        if(BASS_ErrorGetCode() != 0){
          return false;
        }
        return true;
      }

      void restart(HCHANNEL channel){
        BASS_ChannelSetPosition(channel, BASS_ChannelSeconds2Bytes(channel, 0), BASS_POS_BYTE);
      }

      void forward(HCHANNEL channel){
        const float cur_pos = BASS_ChannelGetPosition(channel, BASS_POS_BYTE);
        const float playback_length_bytes = BASS_ChannelGetLength(channel, BASS_POS_BYTE);
        //float playback_length_seconds = BASS_ChannelBytes2Seconds(channel, playback_length_bytes);

        const float bytes_pos_ratio = cur_pos/playback_length_bytes;
        float cur_pos_bytes = bytes_pos_ratio*playback_length_bytes;

        float cur_pos_seconds = BASS_ChannelBytes2Seconds(channel, cur_pos_bytes);

        /*float bytes_a_second = playback_length_bytes/playback_length_seconds;

        std::cout << "cur_pos(bytes): " << cur_pos << "\n";
        std::cout << "playback length (bytes): " << playback_length_bytes << "\n";
        std::cout << "playback length (seconds): " << playback_length_seconds << "\n";
        std::cout << "overall bytes per seconds: " << bytes_a_second << "\n";
        std::cout << "cur bytes per seconds: " << bytes_pos_ratio << "\n";
        std::cout << "cur_pos(seconds): " << cur_pos_seconds << "\n";*/
        
        BASS_ChannelSetPosition(channel, BASS_ChannelSeconds2Bytes(channel, cur_pos_seconds+5), BASS_POS_BYTE);
      }

      void back(HCHANNEL channel){
        const float cur_pos = BASS_ChannelGetPosition(channel, BASS_POS_BYTE);
        const float playback_length_bytes = BASS_ChannelGetLength(channel, BASS_POS_BYTE);
        const float bytes_pos_ratio = cur_pos/playback_length_bytes;
        float cur_pos_bytes = bytes_pos_ratio*playback_length_bytes;

        float cur_pos_seconds = BASS_ChannelBytes2Seconds(channel, cur_pos_bytes);
        BASS_ChannelSetPosition(channel, BASS_ChannelSeconds2Bytes(channel, cur_pos-5), BASS_POS_BYTE);
      }

      void playSong(HCHANNEL channel){
        BASS_ChannelPlay(channel, FALSE);
      }

      void pauseSong(HCHANNEL channel){
        BASS_ChannelPause(channel);
      }

      void change_volume(float volume){
        if(volume > 100){
          volume = 100;
        }

        BASS_SetVolume(volume/100);
      }

      void stopSong(float original_volume){
        this->loaded = false;
        BASS_Stop();
        sleep(1);
        BASS_SetVolume(original_volume); //set original volume back
        BASS_Free();
      }

      void help(){
        std::cout<<"Arguments:\n-o <number> = output device\n-v <number> = volume from 0 to 1\n-s <number> = sample rate\n-off <number> = offset\n--outputs = show all available output devices\n--start <seconds> = position to start at in a file.\n-cp <destination_file>\n--help = show all available arguments";
      }
  };
};



#endif
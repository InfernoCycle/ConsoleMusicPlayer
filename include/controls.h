#define INFERNO_H
#ifdef INFERNO_H

#include <codecvt>
#include <locale>
#include <iostream>
#include "addon.h"
#include "bass.h"
//#include "options.h"
#include <fstream>
#include <unistd.h>
#include <thread>
#include <mutex>
#include "listener.h"
#include <sstream>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

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

      void read_write_playlist(std::wstring playlist_name, std::wstring file){
        std::ifstream ifile;
        std::ofstream ofile;
        
        std::wcout << file << L"\n";

        static std::wstring_convert< std::codecvt_utf8<wchar_t>, wchar_t > converter;
        std::string playlist = converter.to_bytes(playlist_name);
        std::string added_file = converter.to_bytes(file);

        ifile.open("./playlist.json");
        std::string data = "";

        json j;
        ifile >> j;

        //std::cout << j[playlist] << "\n";
        
        if(playlist.size() > 0){
          if(j[playlist] == j["null"]){
            std::cout << "nothing happened cause it doesn't exist\n";
            if(added_file.size() > 0){
              std::ifstream test1(added_file);
              if(test1.is_open()){
                j[playlist] = {added_file};
                std::cout << "Added the Playlist '" << playlist << "', and the file '" << added_file << "'\n";
                ofile.open("./playlist.json");
                ofile << j;
              }else{
                std::cout << "Cannot add a file that does not exist.\n";
              }
              test1.close();
            }else{
              std::cout << "No File was entered for this playlist\n";
            }
            
          }else{
            std::cout << "hold up, wait a minute, something ain't right \n";
            
            if(added_file.size() > 0){
              //check if file already in list

              for(auto it = j[playlist].begin(); it < j[playlist].end(); it++){
                std::cout << *it << "\n";
                if(*it == added_file){
                  std::cout << "That file is already in this playlist\n";
                  return;
                }
              }

              //add file to playlist
              j[playlist].insert(j[playlist].end(), added_file);
              std::cout << "Added the file '" << added_file << "' to the playlist '" << playlist << "'\n";

              std::ifstream test1(added_file);
              if(test1.is_open()){
                j[playlist] = {added_file};
                ofile.open("./playlist.json");
                ofile << j;
              }else{
                std::cout << "Cannot add a file that does not exist.\n";
              }
              test1.close();
            }else{
              std::cout << "No File was entered for this playlist\n";
            }
          }
        }else{
          std::cout << "A playlist was not chosen.\n";
        }
        
        //j[playlist_name] = file;

        std::cout << std::setw(2) << j << "\n";

        ifile.close();
        ofile.close();
      }

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

      void load(wchar_t* filename, HCHANNEL *channel, float *origin_vol){
        std::wcout << L"Filename Before StartSong: " << filename << L"\n";
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

      void length(HCHANNEL channel, std::wstring type=L"secs"){
        if(this->loaded){
          if(type == L"mins"){
            const float playback_length_bytes = BASS_ChannelGetLength(channel, BASS_POS_BYTE);
            const float mins = BASS_ChannelBytes2Seconds(channel, playback_length_bytes) / 60;
            std::cout << "Length: " << mins << " Minutes\n";
          }else{
            const float playback_length_bytes = BASS_ChannelGetLength(channel, BASS_POS_BYTE);
            std::cout << "Length: " << BASS_ChannelBytes2Seconds(channel, playback_length_bytes) << " Seconds\n";
          }
        }
      }

      void copy_file_to(std::wstring src, std::wstring dest){
        if(src != dest){
          std::wifstream infile(src.c_str());

          std::wofstream outfile(dest.c_str());

          std::wstring data;

          while(std::getline(infile, data)){
            outfile << data;
            std::wcout << data << "\n";
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

      bool startSong(int output_device=-1, float volume=0.2f, int sample_rate=44000, int offset=0, int start_pos=0, wchar_t *filename=NULL, float *original_vol=NULL, HCHANNEL *channel=NULL, bool play_on_start=true){
        HSAMPLE hm;
        std::wcout<< L"filename: " << filename << L"\n";
        /*std::wstring shorten = filename;
        std::string s(shorten.begin(), shorten.end());
        std::cout << s << "\n";*/
        std::wifstream st(filename);
        std::cout << "is Open: " << st.is_open() << "\n";
        st.close();
        //HSTREAM stream = BASS_StreamCreateFile(FALSE, "SUI UZI - Imperfect.mp3.mp3", 0, 0, BASS_SAMPLE_MONO);
        BASS_Init(output_device, sample_rate, BASS_SAMPLE_MONO, 0, NULL);
        *original_vol = BASS_GetVolume();
        hm = BASS_SampleLoad(FALSE, filename, offset, 0, BASS_SAMPLE_LOOP, BASS_SAMPLE_MONO);
        std::cout << "\nError Code: " << BASS_ErrorGetCode() << "\n";
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
        Listener listen;

        //std::thread t1(&Listener::running, listen, std::move(123));
        //t1.detach();

        //this->t1.join(); call this if you want blocking

        return true;
      }

      void foo(int num){
        std::cout << num << "\n";
      }

      void restart(HCHANNEL channel){
        /*if(this->loaded && BASS_ACTIVE_STOPPED == BASS_ChannelIsActive(channel)){
          BASS_ChannelPlay(channel, true);
        }*/
        BASS_ChannelPlay(channel, true);
        //BASS_ChannelSetPosition(channel, BASS_ChannelSeconds2Bytes(channel, 0), BASS_POS_BYTE);
      }

      void forward(HCHANNEL channel, int changeAmount = 5){
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

      void back(HCHANNEL channel, int changeAmount = 5){
        const float cur_pos = BASS_ChannelGetPosition(channel, BASS_POS_BYTE);
        const float playback_length_bytes = BASS_ChannelGetLength(channel, BASS_POS_BYTE);
        const float bytes_pos_ratio = cur_pos/playback_length_bytes;
        float cur_pos_bytes = bytes_pos_ratio*playback_length_bytes;

        float cur_pos_seconds = BASS_ChannelBytes2Seconds(channel, cur_pos_bytes);
        BASS_ChannelSetPosition(channel, BASS_ChannelSeconds2Bytes(channel, cur_pos_seconds-5), BASS_POS_BYTE);
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

      float change_position(HCHANNEL channel, int pos){
        float cur_pos_bytes = BASS_ChannelGetPosition(channel, BASS_POS_BYTE);
        float cur_pos = BASS_ChannelBytes2Seconds(channel, cur_pos_bytes);
        return cur_pos;
      }

      void position(HCHANNEL channel){
        float cur_pos_bytes = BASS_ChannelGetPosition(channel, BASS_POS_BYTE);
        float cur_pos_seconds = BASS_ChannelBytes2Seconds(channel, cur_pos_bytes);

        int minutes = (static_cast<int>(cur_pos_seconds)/60);
        int seconds = static_cast<int>(cur_pos_seconds) % 60;
        
        float length_bytes = BASS_ChannelGetLength(channel, BASS_POS_BYTE);
        float length_seconds = BASS_ChannelBytes2Seconds(channel, length_bytes);

        int minutes_full = (static_cast<int>(length_seconds)/60);
        int seconds_full = static_cast<int>(length_seconds) % 60;

        if(std::to_string(seconds).length() > 1){
          if(std::to_string(seconds_full).length() > 1){
            std::cout << "Time: " << minutes << ":" << seconds << " / " << minutes_full << ":" << seconds_full << "\n";
          }else{
            std::cout << "Time: " << minutes << ":" << seconds << " / " << minutes_full << ":0" << seconds_full << "\n";
          }
        }else{
          if(std::to_string(seconds_full).length() > 1){
            std::cout << "Time: " << minutes << ":0" << seconds << " / " << minutes_full << ":" << seconds_full << "\n";
          }else{
            std::cout << "Time: " << minutes << ":0" << seconds << " / " << minutes_full << ":0" << seconds_full << "\n";
          }
        }
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

      void create_write_json(std::wstring playlist_name, std::wstring file){
        std::ifstream ifile;
        std::ofstream ofile;

        ifile.open("./playlist.json");
        if(!ifile.is_open()){
          ofile.open("./playlist.json");
          ofile << "{}";
        }

        ifile.close();
        ofile.close();

        this->read_write_playlist(playlist_name, file);
      }
  };
};



#endif
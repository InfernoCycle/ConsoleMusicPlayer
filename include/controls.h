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

      std::string to_utf8(std::wstring& wide_string)
      {
          static std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;
          return utf8_conv.to_bytes(wide_string);
      }

      void read_write_playlist(std::wstring playlist_name, std::wstring file){
        std::ifstream ifile;
        std::ofstream ofile;
        
        //std::wcout << "Filename: " << file << L"\n";
        char *buffer = new char[1024];
        DWORD siz = 1024;

        std::wcout << L"\n";

        int bufferSize = WideCharToMultiByte(CP_UTF8, 0, playlist_name.c_str(), -1, NULL, 0, NULL, NULL);
        WideCharToMultiByte(CP_UTF8, 0, playlist_name.c_str(), -1, buffer, bufferSize, NULL, NULL);

        std::string filen = this->to_utf8(file);
        //std::wcout << "Buffer: " << buffer << "\n";
        
        //delete [] buffer;
        //static std::wstring_convert< std::codecvt_utf8<wchar_t>, wchar_t > converter;
        //std::string playlist = converter.to_bytes(playlist_name);
        //std::string added_file = converter.to_bytes(file);

        ifile.open("./playlist.json");
        std::string data = "";

        json j;
        ifile >> j;

        //std::cout << j[playlist] << "\n";
        
        if(playlist_name.size() > 0){
          if(j[buffer] == j["null"]){
            std::wcout << L"Playlist doesn't exist, will create it if a file was entered\n\n";
            if(file.size() > 0){
              std::wifstream test1(file.c_str());
              if(test1.is_open()){
                j[buffer] = {filen};
                std::wcout << L"Added the Playlist '" << playlist_name << L"', and the file '" << file << L"'\n";
                ofile.open("./playlist.json");
                ofile << j;
              }else{
                std::wcout << L"Cannot add a file that does not exist.\n";
              }
              test1.close();
            }else{
              std::wcout << L"No File was entered for this playlist. Cannot create an empty playlist.\n";
            }
            
          }
          else{
            //std::wcout << L"hold up, wait a minute, something ain't right \n";
            
            if(file.size() > 0){
              //check if file already in list
              for(auto it = j[buffer].begin(); it < j[buffer].end(); it++){
                //std::cout << *it << "\n";
                if(*it == filen){
                  std::wcout << L"That file is already in this playlist\n";
                  return;
                }
              }

              //add file to playlist
              //D:\Downloads\Masayoshi Oishi - Super Adorable Gal ｜ OPテーマ「Hokkaido Gals Are Super Adorable!」.mp3.mp3
              std::wifstream test1(file.c_str());
              if(test1.is_open()){
                //j[buffer] = {filen};
                ofile.open("./playlist.json");
                j[buffer].insert(j[buffer].end(), filen);
                ofile << j;

                std::wcout << L"Added the file '" << file << L"' to the playlist '" << playlist_name << L"'\n";
              }else{
                std::wcout << L"Cannot add a file that does not exist.\n";
              }
              test1.close();
            }else{
              std::wcout << L"No File was entered for this playlist\n";
            }
          }
        }
        else{
          std::wcout << L"A playlist was not chosen.\n";
        }
        
        //j[playlist_name] = file;

        //std::wcout << std::setw(2) << j << L"\n";

        ifile.close();
        ofile.close();
      }

    public:
      void helper(){
        std::wcout << 
        L"play = start current playing song\n" <<
        L"stop = stop current playing song. unloads the file that was playing.\n" <<
        L"pause = pauses the currently playing song. using 'play' starts the song at the current song position.\n" <<
        L"restart = restarts the song from the beginning.\n" <<
        L"forward = moves the song up 5 seconds.\n" <<
        L"backwards = moves the song back 5 seconds.\n" <<
        L"volume <0 to 100> = changes the volume of the computer from 0 to 100.\n" <<
        L"unload = unsets the current song so you can load a new one. does the same thing as 'stop'\n" <<
        L"load <filepath> = sets a new file to play. If one is already playing, then it will be unloaded.\n" <<
        L"pos <default 'secs' | 'mins'> = shows current position out of the full length that the song is at.\n" <<
        L"copy <dest_file> = makes a copy of the currently loaded file.\n" <<
        L"loaded = shows the current file loaded.\n" <<
        L"length = shows the length of the song.\n"; 
      }

      void load(std::wstring filename, HCHANNEL *channel, float *origin_vol){
        std::wcout << L"Filename Before StartSong: " << filename << L"\n";
        if(this->loaded){
          return;
        }else{
          this->startSong<std::wstring>(output_device, volume, sample_rate, offset, start_pos, filename, origin_vol, channel, false);
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
            std::wcout << L"Length: " << mins << L" Minutes\n";
          }else{
            const float playback_length_bytes = BASS_ChannelGetLength(channel, BASS_POS_BYTE);
            std::wcout << L"Length: " << BASS_ChannelBytes2Seconds(channel, playback_length_bytes) << L" Seconds\n";
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
            std::wcout << data << L"\n";
            outfile.flush();
          }

          outfile.close();
          infile.close();
        }
        
      }

      bool showOutputs(int output=-1){
        BASS_DEVICEINFO info;
        int count = 0;
        std::wcout << L"\n-------------------------------------------------------------------------------------------------------------\n";
        while(BASS_GetDeviceInfo(count, &info)){
          std::wcout << L"Device Number?: " << count+1 << L"\n";
          std::wcout << L"Device Name: " << info.name << L"\n-------------------------------------------------------------------------------------------------------------\n";
          if(count == output-1){
            return output;
          }
          count++;
        }
        return -1;
      }

      void showInputs(){

      }

      template<typename T>
      bool startSong(int output_device=-1, float volume=0.2f, int sample_rate=44000, int offset=0, int start_pos=0, T filename=NULL, float *original_vol=NULL, HCHANNEL *channel=NULL, bool play_on_start=true){
        HSAMPLE hm;
        std::wcout<< "filename: " << filename << "\n";
        /*std::wstring shorten = filename;
        std::string s(shorten.begin(), shorten.end());
        std::cout << s << "\n";*/
        std::wifstream st(filename.c_str());
        std::wcout << L"is Open: " << st.is_open() << L"\n";
        st.close();
        //HSTREAM stream = BASS_StreamCreateFile(FALSE, "SUI UZI - Imperfect.mp3.mp3", 0, 0, BASS_SAMPLE_MONO);
        BASS_Init(output_device, sample_rate, BASS_SAMPLE_MONO, 0, NULL);
        *original_vol = BASS_GetVolume();
        hm = BASS_SampleLoad(FALSE, filename.c_str(), offset, 0, BASS_SAMPLE_LOOP, BASS_SAMPLE_MONO);
        std::wcout << L"\nError Code: " << BASS_ErrorGetCode() << L"\n";
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

        std::wcout << L"Error Code: " << BASS_ErrorGetCode() << L"\n";

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
        std::wcout << num << L"\n";
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
            std::wcout << L"Time: " << minutes << L":" << seconds << L" / " << minutes_full << L":" << seconds_full << L"\n";
          }else{
            std::wcout << L"Time: " << minutes << L":" << seconds << L" / " << minutes_full << L":0" << seconds_full << L"\n";
          }
        }else{
          if(std::to_string(seconds_full).length() > 1){
            std::wcout << L"Time: " << minutes << L":0" << seconds << L" / " << minutes_full << L":" << seconds_full << L"\n";
          }else{
            std::wcout << L"Time: " << minutes << L":0" << seconds << L" / " << minutes_full << L":0" << seconds_full << L"\n";
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
        std::wcout<<L"Arguments:\n-o <number> = output device\n-v <number> = volume from 0 to 1\n-s <number> = sample rate\n-off <number> = offset\n--outputs = show all available output devices\n--start <seconds> = position to start at in a file.\n-cp <destination_file>\n--help = show all available arguments";
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

      //list all playlists currently made
      void list_playlists(){
        return;
      }

      //list all files in a playlist
      void list_playlists_files(){
        return;
      }
  };
};



#endif
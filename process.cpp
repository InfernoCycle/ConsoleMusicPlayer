#include <iostream>
#include <vector>
#include <cmath>
#include "include/addon.h"
#include "include/bass.h"
#include <string>
#include <regex>
#include <fstream>
#include <windows.h>
#include <unistd.h>
#include <codecvt>

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

bool startSong(int output_device=-1, float volume=0.2f, int sample_rate=44000, int offset=0, int start_pos=0, char *filename=NULL, float *original_vol=NULL, HCHANNEL *channel=NULL){
  HSAMPLE hm;
  //HSTREAM stream = BASS_StreamCreateFile(FALSE, "SUI UZI - Imperfect.mp3.mp3", 0, 0, BASS_SAMPLE_MONO);
  BASS_Init(output_device, sample_rate, 0, 0, NULL);
  *original_vol = BASS_GetVolume();
  BASS_SetVolume(volume);
  hm = BASS_SampleLoad(FALSE, filename, offset, 0, BASS_SAMPLE_LOOP, BASS_SAMPLE_MONO);
  std::cout << "Error Code: " << BASS_ErrorGetCode() << "\n";
  //HCHANNEL channel = BASS_SampleGetChannel(hm, FALSE);
  *channel = BASS_SampleGetChannel(hm, FALSE);
  BASS_ChannelSetPosition(*channel, BASS_ChannelSeconds2Bytes(*channel, start_pos), BASS_POS_BYTE);
  BASS_ChannelPlay(*channel, FALSE);

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

void stopSong(float original_volume){
  BASS_Stop();
  sleep(1);
  BASS_SetVolume(original_volume); //set original volume back
  BASS_Free();
}

void help(){
  std::cout<<"Arguments:\n-o <number> = output device\n-v <number> = volume from 0 to 1\n-s <number> = sample rate\n-off <number> = offset\n--outputs = show all available output devices\n--start <seconds> = position to start at in a file.\n-cp <destination_file>\n--help = show all available arguments";
}

//original main uses args: int argc, char * argv[]
//wmain uses args: int argc, wchar_t *argv[ ], wchar_t *envp[ ]
int wmain(int argc, wchar_t *argv[ ], wchar_t *envp[ ]){
  std::wifstream infile(argv[1]);
  std::cout << "File is Open?: " << infile.is_open() << "\n";
  std::wcout << argv[1];
  infile.close();
  // Read and check command line parameters.
  //std::cout << argc << ", " << strcmp(argv[1], "help") << "\n";
  
  /*UNCOMMENT HEREstd::setlocale(LC_ALL, "en_US.utf8");
  
  if(strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "help") == 0 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "-help") == 0){
    help();
    return 0;
  }

  if(strcmp(argv[1], "--outputs") == 0){
    showOutputs();
    return 0;
  }

  int output_device = -1;
  float volume = 0.2f;
  int sample_rate = 44100;
  int offset = 0;
  int start_pos = 0;
  std::string dest_file = "";
  float original_volume;
  HCHANNEL channel;

  std::regex word_regex(".+\\.(mp3|wav|mpe4)$", std::regex_constants::ECMAScript | std::regex_constants::icase);
  
  bool has_file = false;
  bool copy_enabled = false;

  char * filename;

  bool has_run = false;
  
  for(int i = 1; i < argc; i++){
    if(strcmp(argv[i], "-cp") == 0){
      try{
        dest_file = argv[i+1];
        copy_enabled = true;
        continue;
      }catch(std::invalid_argument){
        std::cout << "Error: Invalid path";
        break;
      }
      continue;
    }

    if(std::regex_search(argv[i], word_regex)){
      if(has_run){
        continue;
      }
      has_run = true;
      filename = argv[i];
      std::cout << filename << "\n";
      /*std::cout << "Filename: " << filename << "\n";
      auto begin = std::sregex_iterator(filename.begin(), filename.end(), word_regex);
      auto end = std::sregex_iterator();

      std::smatch match = *begin;
      std::string match_str = match.str();
      std::cout << match_str << "\n";*/
      /*UNCOMMENT HEREhas_file = true;
    }
    if(strcmp(argv[i], "-o") == 0){
      //std::cout << "out put value is out?: " << argv[i+1] << "\n";
      try{
        output_device = std::stoi(argv[i+1])-1;
      }catch(std::invalid_argument){
        std::cout << "Error: Please type an integer. Here are a list of outputs available on this device: \n";
        showOutputs();
      }
    }

    if(strcmp(argv[i], "-v") == 0){
      //std::cout << "volume value is out? " << argv[i] << "\n";
      try{
        float levels = std::stof(argv[i+1]);
        if(levels > 1){
          levels = 1.0f;
        }
        volume = levels;
      }catch(std::invalid_argument){
        std::cout << "Error: Please type a number from 0 to 1";
      }
    }

    if(strcmp(argv[i], "-s") == 0){
      try{
        sample_rate = std::stoi(argv[i+1]);
      }catch(std::invalid_argument){
        std::cout << "Please type a sample rate number";
      }
    }
    
    if(strcmp(argv[i], "-off") == 0){
      //std::cout << "offset value is out? " << argv[i] << "\n";
      try{
        offset = std::stoi(argv[i+1]);
      }catch(std::invalid_argument){
        std::cout << "Please type an offset number.";
      }
    }

    if(strcmp(argv[i], "--start") == 0){
      //std::cout << "offset value is out? " << argv[i] << "\n";
      try{
        start_pos = std::stoi(argv[i+1]);
      }catch(std::invalid_argument){
        std::cout << "Please type a start number in seconds.";
      }
    }
  }

  if(has_file){
    std::cout << "\nOutput Dev: " << output_device << ", Volume: " << volume << ", Sample Rate: " << sample_rate << ", Offset: " << offset << ", Start Position: " << start_pos << ", Filename: " << filename << ", Output File: " << dest_file << "\n";
    
    if(copy_enabled){
      copy_file_to(filename, dest_file);
    }
    while(*filename != '\0'){
      wchar_t ch = *filename;
      std::cout << "Code: " << ch << ", Character: " << *filename << "\n";
      filename++;
    }
    std::cout << "\n";

    if(startSong(output_device, volume, sample_rate, offset, start_pos, filename, &original_volume, &channel)){
      std::string command = "";
      while(true){
        std::cout << ">"; std::getline(std::cin, command);
        if(command == "play"){
          playSong(channel);
        }
        if(command == "restart"){
          restart(channel);
        }
        if(command == "pause"){
          pauseSong(channel);
        }
        if(command == "stop"){
          stopSong(original_volume);
        }if(command == "forward"){
          forward(channel);
        }if(command == "backward"){
          back(channel);
        }if(command == "exit"){
          break;
        }
        else{
          continue;
        }
      }
      //std::cout << command << "\n";
      //system("pause");
      stopSong(original_volume);
    };
  }else{
    std::cout << "\nno file was picked\n";
  }*/
  
  return 0;
}
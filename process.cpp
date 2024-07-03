#include <iostream>
#include <vector>
#include <cmath>
#include "include/addon.h"
//#include "include/bass.h"
#include <string>
#include <regex>
#include <fstream>
#include <windows.h>
#include <unistd.h>
#include "include/options.h"
#include "include/controls.h";

//original main uses args: int argc, char * argv[]
//wmain uses args: int argc, wchar_t *argv[ ], wchar_t *envp[ ]
int main(int argc, char * argv[]){
  StringMan * build;

  /*for(int i = 0; i < 2; i++){
    std::cout << (*lumps)[i] << "\n";
  }*/

  pincer::control *controls = new pincer::control();

  // Read and check command line parameters.
  //std::cout << argc << ", " << strcmp(argv[1], "help") << "\n";
  
  /*UNCOMMENT HERE/*/std::setlocale(LC_ALL, "en_US.utf8");

  /*std::ifstream infile(argv[1]);
  std::cout<< "File Exist: " << infile.is_open() << "\n";
  infile.close();*/

  if(argc > 1){
    if(strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "help") == 0 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "-help") == 0){
      controls->help();
      return 0;
    }

    if(strcmp(argv[1], "--outputs") == 0){
      controls->showOutputs();
      return 0;
    }
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
  
  if(argc > 1){
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
        /*UNCOMMENT HERE*/has_file = true;
      }
      if(strcmp(argv[i], "-o") == 0){
        //std::cout << "out put value is out?: " << argv[i+1] << "\n";
        try{
          output_device = std::stoi(argv[i+1])-1;
        }catch(std::invalid_argument){
          std::cout << "Error: Please type an integer. Here are a list of outputs available on this device: \n";
          controls->showOutputs();
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
  }

  if(!has_file){
    build->println("No file selected");
  }else{
    if(!controls->startSong(output_device, volume, sample_rate, offset, start_pos, filename, &original_volume, &channel)){
      build->println("Could not read the file. This could be due to the program not being able to read special character files.");
    };
    build->println("File Loaded/Playing: " + static_cast<std::string>(filename));
    //std::cout << "\nOutput Dev: " << output_device << ", Volume: " << volume << ", Sample Rate: " << sample_rate << ", Offset: " << offset << ", Start Position: " << start_pos << ", Filename: " << filename << ", Output File: " << dest_file << "\n";
  }

  if(copy_enabled){
    controls->copy_file_to(filename, dest_file);
  }

  std::cout << "\n";

  std::string command = "";

  std::string stripped_str = "";
  std::string* split_str = NULL;

  std::string type = "Noload";

  while(true){
    stripped_str = "";
    split_str = NULL;

    if(controls->getLoaded()){type = "Loaded";}else{type="Noload";}
    std::cout << "\n" << type << ">"; std::getline(std::cin, command);

    if(command != ""){
      stripped_str = build->strip(command, command.size());
      split_str = build->split(stripped_str, ' ', stripped_str.size(), 1);
    }else{
      continue;
    }
    
    if(split_str[0] == "play"){
      controls->playSong(channel);
    }
    if(split_str[0] == "restart"){
      controls->restart(channel);
    }
    if(split_str[0] == "pause"){
      controls->pauseSong(channel);
    }
    if(split_str[0] == "stop"){
      controls->stopSong(original_volume);
    }
    if(split_str[0] == "forward"){
      controls->forward(channel);
    }
    if(split_str[0] == "backward"){
      controls->back(channel);
    }
    if(split_str[0] == "exit"){
      break;
    }
    if(split_str[0] == "load"){ //how the frick do you split a string
      if(controls->getLoaded()){
        build->println("Can't load another file because another one is already loaded. run 'unload' to deselect the current file.");
        continue;
      }
      if(split_str[1] != ""){
        const int filename_length = split_str[1].size();
        char *newFile = new char[filename_length];

        for(int k = 0; k < filename_length; k++){
          newFile[k] = split_str[1][k];
        }

        controls->load(newFile, &channel, &original_volume);
      }else{build->println("No file was entered.");}
    }
    if(split_str[0] == "unload"){
      if(controls->getLoaded()){
        controls->unload(&original_volume);
      }else{build->println("No file is currently loaded.");}
    }
    if(split_str[0] == "volume"){
      if(split_str[1] != ""){
        try{
          controls->change_volume(std::stof(split_str[1]));
        }catch(...){
          build->println("Invalid value entered.");
        }
      }
      
      std::cout << "Volume: " << split_str[1] << "\n";
    }
    if(split_str[0] == "length"){
      if(split_str[1] != ""){
        controls->length(channel, split_str[1]);
      }else{
        controls->length(channel);
      }
    }
    if(command == "help"){
      controls->helper();
    }
    else{
      continue;
    }
  }
  //std::cout << command << "\n";
  //system("pause");
  controls->stopSong(original_volume);

  delete build;
  delete controls;
  return 0;
}
#include <iostream>
#include <vector>
#include <cmath>
//#include "include/addon.h"
//#include "include/bass.h"
#include <string>
#include <regex>
#include <fstream>
#include <windows.h>
#include <unistd.h>
#include "include/options.h"
#include "include/controls.h"
//#include "include/Create.h"

//original main uses args: int argc, char * argv[]
//wmain uses args: int argc, wchar_t *argv[ ], wchar_t *envp[ ]
int main(int argc, char * argv[]){
  StringMan * build;

  LPWSTR *args = CommandLineToArgvW(GetCommandLineW(), &argc);
  
  //build->println(std::to_string(build->wstrcmp(args[1], L"--help")));

  //return -1;
  pincer::control *controls = new pincer::control();
  //std::cout << check_create() << "\n";
  // Read and check command line parameters.
  //std::cout << argc << ", " << strcmp(argv[1], "help") << "\n";
  
  /*UNCOMMENT HERE/*///std::setlocale(LC_ALL, "en_US.utf8");

  /*std::ifstream infile(argv[1]);
  std::cout<< "File Exist: " << infile.is_open() << "\n";
  infile.close();*/
  
  if(argc > 1){
    if(build->wstrcmp(args[1], L"--help") == 1 || build->wstrcmp(args[1], L"help") == 1 || build->wstrcmp(args[1], L"-h") == 1 || build->wstrcmp(args[1], L"-help") == 1){
      controls->help();
      return 0;
    }

    if(build->wstrcmp(args[1], L"--outputs") == 1){
      controls->showOutputs();
      return 0;
    }
  }

  int output_device = -1;
  float volume = 0.2f;
  int sample_rate = 44100;
  int offset = 0;
  int start_pos = 0;
  std::wstring dest_file = L"";
  float original_volume;
  HCHANNEL channel;

  std::basic_regex<wchar_t> word_regex(L".+\\.(mp3|wav|mpe4)$", std::regex_constants::ECMAScript | std::regex_constants::icase);
  
  bool has_file = false;
  bool copy_enabled = false;

  wchar_t * filename;

  bool has_run = false;
  
  if(argc > 1){
    for(int i = 1; i < argc; i++){
      if(build->wstrcmp(args[1], L"-cp") == 1){
        try{
          dest_file = args[i+1];
          copy_enabled = true;
          continue;
        }catch(std::invalid_argument){
          std::cout << "Error: Invalid path";
          break;
        }
        continue;
      }
      
      if(std::regex_search(args[i], word_regex)){
        if(has_run){
          continue;
        }
        has_run = true;
        filename = args[i];
        std::cout << filename << "\n";
        /*std::cout << "Filename: " << filename << "\n";
        auto begin = std::sregex_iterator(filename.begin(), filename.end(), word_regex);
        auto end = std::sregex_iterator();

        std::smatch match = *begin;
        std::string match_str = match.str();
        std::cout << match_str << "\n";*/
        /*UNCOMMENT HERE*/has_file = true;
      }
      if(build->wstrcmp(args[1], L"-o") == 1){
        //std::cout << "out put value is out?: " << argv[i+1] << "\n";
        try{
          output_device = std::stoi(args[i+1])-1;
        }catch(std::invalid_argument){
          std::cout << "Error: Please type an integer. Here are a list of outputs available on this device: \n";
          controls->showOutputs();
        }
      }

      if(build->wstrcmp(args[1], L"-v") == 1){
        //std::cout << "volume value is out? " << argv[i] << "\n";
        try{
          float levels = std::stof(args[i+1]);
          if(levels > 1){
            levels = 1.0f;
          }
          volume = levels;
        }catch(std::invalid_argument){
          std::cout << "Error: Please type a number from 0 to 1";
        }
      }

      if(build->wstrcmp(args[1], L"-s") == 1){
        try{
          sample_rate = std::stoi(args[i+1]);
        }catch(std::invalid_argument){
          std::cout << "Please type a sample rate number";
        }
      }
      
      if(build->wstrcmp(args[1], L"-off") == 1){
        //std::cout << "offset value is out? " << argv[i] << "\n";
        try{
          offset = std::stoi(args[i+1]);
        }catch(std::invalid_argument){
          std::cout << "Please type an offset number.";
        }
      }

      if(build->wstrcmp(args[1], L"--start") == 1){
        //std::cout << "offset value is out? " << argv[i] << "\n";
        try{
          start_pos = std::stoi(args[i+1]);
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
    build->wprintln(L"File Loaded/Playing: " + static_cast<std::wstring>(filename));
    //std::cout << "\nOutput Dev: " << output_device << ", Volume: " << volume << ", Sample Rate: " << sample_rate << ", Offset: " << offset << ", Start Position: " << start_pos << ", Filename: " << filename << ", Output File: " << dest_file << "\n";
  }

  if(copy_enabled){
    controls->copy_file_to(filename, dest_file);
  }

  std::cout << "\n";

  std::wstring stripped_str = L"";
  //std::string* split_str = NULL;

  std::string type = "Noload";

  while(true){
    std::wstring command;
    std::wstring* split_str = NULL;
    stripped_str = L"";

    if(controls->getLoaded()){type = "Loaded";}else{type="Noload";}

    std::cout << "\n" << type << ">"; 
    //std::wcin.ignore();
    //filenames don't support umulauts: äöüß
    try{
      std::getline(std::wcin, command);
    }catch(...){
      break;
    }
    /*std::wcout << command << "\n";
    stripped_str = build->w_strip(command, command.size());
    split_str = build->w_split(stripped_str, ' ', stripped_str.size(), 1);
    std::wcout << stripped_str << L"\n";
    continue;*/

    if(command != L""){
      stripped_str = build->w_strip(command, command.size());
      split_str = build->w_split(stripped_str, ' ', stripped_str.size(), 1);
    }else{
      delete [] split_str;
      continue;
    }
    
    if(split_str[0] == L"play"){
      controls->playSong(channel);
    }
    if(split_str[0] == L"restart"){
      controls->restart(channel);
    }
    if(split_str[0] == L"pause"){
      controls->pauseSong(channel);
    }
    if(split_str[0] == L"stop"){
      controls->stopSong(original_volume);
    }
    if(split_str[0] == L"forward"){
      controls->forward(channel);
    }
    if(split_str[0] == L"backward"){
      controls->back(channel);
    }
    if(split_str[0] == L"pos"){
      if(!controls->getLoaded()){
        build->println("No file is loaded to show the position.");
        delete [] split_str;
        continue;
      }
      if(split_str[1] != L""){
        controls->position(channel);
        delete [] split_str;
        continue;
      }else{
        controls->position(channel);
        delete [] split_str;
        continue;
      }
    }
    if(split_str[0] == L"exit"){
      break;
    }
    if(split_str[0] == L"load"){ //how the frick do you split a string
      if(controls->getLoaded()){
        build->println("Can't load another file because another one is already loaded. run 'unload' to deselect the current file.");
        delete [] split_str;
        continue;
      }
      if(split_str[1] != L""){
        const int filename_length = split_str[1].size();
        //std::wcout << split_str[1] + L" Canon Event" << "\n";
        
        wchar_t *newFile = new wchar_t[filename_length];

        for(int k = 0; k < filename_length; k++){
          newFile[k] = split_str[1][k];
        }

        controls->load(newFile, &channel, &original_volume);
      }else{build->println("No file was entered.");}
    }
    if(split_str[0] == L"unload"){
      if(controls->getLoaded()){
        controls->unload(&original_volume);
      }else{build->println("No file is currently loaded.");}
    }
    if(split_str[0] == L"volume"){
      if(split_str[1] != L""){
        try{
          controls->change_volume(std::stof(split_str[1]));
        }catch(...){
          build->println("Invalid value entered.");
        }
      }
      
      std::wcout << L"Volume: " << split_str[1] << "\n";
    }
    if(split_str[0] == L"length"){
      if(split_str[1] != L""){
        controls->length(channel, split_str[1]);
      }else{
        controls->length(channel);
      }
    }
    if(command == L"help"){
      controls->helper();
    }
    else{
      delete [] split_str;
      continue;
    }
    delete [] split_str;
  }
  //std::cout << command << "\n";
  //system("pause");
  controls->stopSong(original_volume);

  delete build;
  delete controls;
  return 0;
}
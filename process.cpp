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
#include <locale.h>
#include <filesystem>
#include <io.h>
#include <fcntl.h>

#define UNICODE
#define BUFFER 2024

//std::vector<std::wstring> locals;

//#include "include/Create.h"
/*BOOL CALLBACK MyFuncLocaleEx(LPWSTR pStr, DWORD dwFlags, LPARAM lparam)
{
    locals.push_back(pStr);
    return TRUE;
}*/

//original main uses args: int argc, char * argv[]
//wmain uses args: int argc, wchar_t *argv[ ], wchar_t *envp[ ]
int main(int argc, char * argv[]){
  _setmode(_fileno(stdout), _O_U8TEXT);
  _setmode(_fileno(stdin), _O_U8TEXT);
  
  //SetConsoleOutputCP(CP_UTF8);
  //std::setlocale(LC_ALL, "C.UTF-8");
  //std::setlocale(LC_ALL, "ja_JP.utf8");
  //std::setlocale(LC_ALL, "en_US.utf8");

  StringMan * build;

  LPWSTR *args = CommandLineToArgvW(GetCommandLineW(), &argc);

  //build->println(std::to_string(build->wstrcmp(args[1], L"--help")));

  //return -1;
  pincer::control *controls = new pincer::control();
  //std::cout << check_create() << "\n";
  // Read and check command line parameters.
  //std::cout << argc << ", " << strcmp(argv[1], "help") << "\n";
  
  /*UNCOMMENT HERE/*///std::setlocale(LC_ALL, "en_US.utf8");

  std::wstring testing;
  std::wcout << L"Enter file name: ";
  //std::cin.imbue(std::locale("en-US.utf8"));
  std::getline(std::wcin, testing);

  Convertors conversion = Convertors();

  //conversion.wstr_to_u16(testing);
  //std::u32string convert = conversion.str_to_u32(testing);
  //std::string new_val = conversion.u32_to_str(convert);

  /*EnumSystemLocalesEx(MyFuncLocaleEx, LOCALE_ALL, NULL, NULL);

  for (std::vector<std::wstring>::const_iterator str = locals.begin(); str != locals.end(); ++str){
    std::wcout << *str << std::endl;
  }*/


  try{
    std::cout << std::endl;
    std::string unic = u8"D:/Downloads/Masayoshi Oishi - Super Adorable Gal ｜ OPテーマ「Hokkaido Gals Are Super Adorable!」.mp3.mp3";
    std::u16string newer = conversion.str_to_u16(unic);
    std::string finally = conversion.u16_to_str(newer);
    std::cout << "Compiled Version: " << finally << "\n";
    std::wcout << L"\nInput Version: " << testing << L"\n";
    std::cout << "\nInput Version Size: " << testing.size() << "\n";
  }catch(std::range_error& e){
    std::cout << e.what();
  }
  
  if(argc > 1){
    int size = std::string(argv[1]).size();
    std::cout << "Command Version: " << size << "\n";
  }

  //std::cout << new_val << "\n";

  /*std::ifstream infile(testing.c_str());
  std::cout<< "File Exist: " << infile.is_open() << "\n";
  infile.close();*/

  /*std::cout << "File Exist2: " << std::filesystem::exists(testing.c_str()) << "\n";*/
  return -1;

  if(argc > 1){
    if(build->strcmp(argv[1], "--help") == 1 || build->wstrcmp(args[1], L"help") == 1 || build->wstrcmp(args[1], L"-h") == 1 || build->wstrcmp(args[1], L"-help") == 1){
      controls->help();
      return 0;
    }

    if(build->strcmp(argv[1], "--outputs") == 1){
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

  std::basic_regex<char> word_regex(".+\\.(mp3|wav|mpe4)$", std::regex_constants::ECMAScript | std::regex_constants::icase);
  
  bool has_file = false;
  bool copy_enabled = false;

  char * filename;

  bool has_run = false;
  
  if(argc > 1){
    for(int i = 1; i < argc; i++){
      if(build->strcmp(argv[i], "-cp") == 1){
        try{
          dest_file = argv[i+1];
          copy_enabled = true;
          continue;
        }catch(...){
          std::cout << "Error: Invalid path, could not copy file.";
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
      if(build->strcmp(argv[i], "-o") == 1){
        //std::cout << "out put value is out?: " << argv[i+1] << "\n";
        try{
          output_device = std::stoi(argv[i+1])-1;
        }catch(...){
          std::cout << "Invalid value, using the default output device of -1.\n";
          //controls->showOutputs();
        }
      }

      if(build->strcmp(argv[i], "-v") == 1){
        //std::cout << "volume value is out? " << argv[i] << "\n";
        try{
          float levels = std::stof(argv[i+1]);
          if(levels > 1){
            levels = 1.0f;
          }
          volume = levels;
        }catch(...){
          std::cout << "Invalid value, using the default of 0.2.\n";
        }
      }

      if(build->strcmp(argv[i], "-s") == 1){
        try{
          sample_rate = std::stoi(argv[i+1]);
        }catch(...){
          std::cout << "Invalid Sample Rate, using the default value of 44100.\n";
        }
      }
      
      if(build->strcmp(argv[i], "-off") == 1){
        //std::cout << "offset value is out? " << argv[i] << "\n";
        try{
          offset = std::stoi(argv[i+1]);
        }catch(...){
          std::cout << "Offset was invalid using the default value of 0.\n";
        }
      }

      if(build->strcmp(argv[i], "--start") == 1){
        //std::cout << "offset value is out? " << argv[i] << "\n";
        try{
          start_pos = std::stoi(argv[i+1]);
        }catch(...){
          start_pos = 0;
          std::cout << "Invalid Value. File will begin from the beginning.\n";
        }
      }
    }
  }

  if(!has_file){
    build->println("No file selected");
  }else{
    //std::wcout << filename << L"\n";
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

  std::string stripped_str = "";
  //std::string* split_str = NULL;

  std::string type = "Noload";
  
  _locale_t things = _get_current_locale();
  std::cout << things->mbcinfo << '\n';

  while(true){
    std::string command;
    std::string* split_str = NULL;
    stripped_str = "";

    if(controls->getLoaded()){type = "Loaded";}else{type="Noload";}

    std::cout << "\n" << type << ">"; 
    //std::wcin.ignore();
    //filenames don't support umulauts: äöüß
    try{
      std::cin.clear(); 
      std::getline(std::cin, command);
    }catch(...){
      break;
    }

    if(command != ""){
      std::string *regular_sub = build->split(command, ' ', command.size(), 1);
      //std::string regular = build->wstrtostr(regular_sub);
      //std::wstring *splitter = build->w_split(regular_sub, ' ', regular_sub.size(), 1);
      //std::wcout << "Regular: " << splitter[1] << L"\n";
      std::cout << "regulated: " << command;
      //stripped_str = build->strip(command, command.size());
      //split_str = build->split(command, ' ', command.size(), 1);
      
      //std::wifstream fail(stripped_str);
      //std::cout << "Is Open: " << fail.is_open() << "\n";
      //fail.close();
      //delete [] splitter;
      delete [] split_str;
      delete build;
      delete controls;
      break;
    }else{
      delete [] split_str;
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
    if(split_str[0] == "pos"){
      if(!controls->getLoaded()){
        build->println("No file is loaded to show the position.");
        delete [] split_str;
        continue;
      }
      if(split_str[1] != ""){
        controls->position(channel);
        delete [] split_str;
        continue;
      }else{
        controls->position(channel);
        delete [] split_str;
        continue;
      }
    }
    if(split_str[0] == "exit"){
      break;
    }
    if(split_str[0] == "load"){ //how the frick do you split a string
      if(controls->getLoaded()){
        build->println("Can't load another file because another one is already loaded. run 'unload' to deselect the current file.");
        delete [] split_str;
        continue;
      }
      if(split_str[1] != ""){
        const int filename_length = split_str[1].size();
        std::wstring one = build->strtowstr(split_str[1]);
        std::string two = build->wstrtostr(one);

        /*std::cout << "File Size: " << filename_length << "\n";
        std::cout << "Filename using stripped: " << stripped_str << "\n";
        std::cout << "Filename Before Load: " << split_str[1] << "\n";
        std::cout << "Filename with UTF-8 conversion: " << command << "\n";
        std::cout << split_str[1] + " Canon Event" << "\n";*/
        std::cout << "To UTF-8: " << two << "\n";
        std::ifstream good(two);
        std::cout << "Is Open: " << good.is_open() << "\n";
        good.close();
        break;
        char *newFile = new char[filename_length];

        for(int k = 0; k < filename_length; k++){ //problem here
          newFile[k] = split_str[1][k];
          //std::wcout << k << ". " << split_str[1][k] << L"\n";
        }

        controls->load(newFile, &channel, &original_volume);
        delete [] newFile;
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
    if(split_str[0] == "add"){
      try{
        std::string * addition = build->split(stripped_str, ' ', stripped_str.size(), 2);
        controls->create_write_json(addition[1], addition[2]);
        return -1;
      }catch(...){
        std::cout<<"The format for add is 'add <playlist_name> <file_name>'\n";
      }
    }
    if(split_str[0] == "playlists"){
      std::cout << "Coming Soon";
    }
    if(command == "help"){
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
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
#include <thread>

#define UNICODE
#define BUFFER 2024
#define FILEPATH "C:\\Users\\"+user_name()+"\\AppData\\Local\\Inferno\\MP3_Console\\playlist.json"

struct playlist_funcs{
  bool playlist_loaded;
  bool next_requested;
  bool prev_requested;
  bool exit_playlist;
  std::string playlist_name;
};

struct user_settings{
  int output_device;
  float volume;
  int sample_rate;
  int offset;
  int start_pos;
};

class Listener{
  public:
    void running(HCHANNEL *channel, std::vector<std::wstring> playlist, pincer::control *controls, playlist_funcs *obj, int song_start=0){
      bool play_trap = false;
      bool pause_trap = false;
      bool load_trap = false;
      bool skip_trap = false;

      /*std::wcout << L"\n" << (controls)->getLoaded() << L"\n";*/

      /*       
      std::wstring delims = L"\\/";
      for(auto it = playlists.begin(); it < playlists.end(); it++){
        std::wifstream iis((*it).c_str());
        std::wcout << iis.is_open() << L"\n";
        iis.close();
        //std::wcout << build->w_deldup(*it, (*it).size(), true, delims) << L"\n";
      }
      */
      //auto it = playlist.begin();

      int index = song_start;
      
      if(index < playlist.size()){
        controls->load(playlist[index], channel);
      }else{
        controls->load(playlist[0], channel);
      }

      DWORD active = BASS_ChannelIsActive(*channel);
      controls->playSong(*channel);

      obj->exit_playlist = false;
      obj->next_requested = false;
      obj->prev_requested = false;

      while(true){
        active = BASS_ChannelIsActive(*channel); //required to be in here, for updates
        if(obj->playlist_loaded){
          if(!load_trap){
            load_trap = true;
            std::wcout << "A Playlist is already running";
            continue;
          }
        }
        if(obj->exit_playlist){
          controls->stopSong();
          break;
        }

        if(obj->prev_requested){
          obj->prev_requested = false;

          index-=1;
          if(index > -1){
            controls->unload();
            controls->load(playlist[index], channel);
            controls->playSong(*channel);
          }else{
            continue;
            //do something here i guess
          }
        }

        //active = BASS_ChannelIsActive(channel);
        if(active == BASS_ACTIVE_STOPPED || obj->next_requested){
          index+=1;
          obj->next_requested = false;

          if(active == BASS_ACTIVE_STOPPED){
            if(index < playlist.size()){
              controls->unload();
              controls->load(playlist[index], channel);
              controls->playSong(*channel);
            }/*else{
              controls->stopSong();
              index = 0;
              controls->load(playlist[index], channel);
              //do something here i guess
            }*/
          }
          else{
            if(index < playlist.size()){
              controls->unload();
              controls->load(playlist[index], channel);
              controls->playSong(*channel);
            }else{
              continue;
              //do something here i guess
            }
          }
          
          //std::wcout << L"stopped the play" << L"\n";
          //break;
        }
        /*if(active == BASS_ACTIVE_PLAYING){
          if(!play_trap){
            obj->playlist_loaded = true;

            std::wcout << L"is Playing \n";
            play_trap = true;
            pause_trap = false;
          }
        }
        if(active == BASS_ACTIVE_PAUSED){
          if(!pause_trap){
            std::wcout << L"is Paused\n";
            pause_trap = true;
            play_trap = false;
          }
        }*/
      }
    }
};

bool playPlaylist(std::wstring playlist_name, HCHANNEL *channel, StringMan *build, pincer::control *controls, playlist_funcs *obj){
  std::vector<std::wstring> playlists = controls->get_playlists_files(build->w_strip(playlist_name, playlist_name.size()), build);
  Listener *listen = new Listener();
  try{
    std::wcout << playlist_name << L"\n";
    int cliff = 123;
    if(playlists.size() == 0){
      std::wcout << L"Not a valid Playlist\n";
      return false;
    }
    std::thread t1([listen, playlists, channel, controls, obj](){listen->running(channel, playlists, controls, obj);});
    t1.detach();
    return true;
  }catch(...){
    return false;
  }
  return false;
}

std::string user_name()
{
  const short maxUserLength = 256;
  char buffer[maxUserLength+1]{};
  DWORD len = maxUserLength+1;
  if( ::GetUserNameA( buffer, &len ) ) return buffer;
  else return {}; // or: return "" ;
}

bool create_directory(){
  try{
    std::string test = "C:\\Users\\"+user_name()+"\\AppData\\Local\\Inferno\\MP3_Console";

    std::filesystem::create_directories(test);
    return true;
  }catch(...){
    return false;
  }
}
//std::vector<std::wstring> locals;

//#include "include/Create.h"

//original main uses args: int argc, char * argv[]
//wmain uses args: int argc, wchar_t *argv[ ], wchar_t *envp[ ]
int main(int argc, char * argv[]){
  _setmode(_fileno(stdout), _O_WTEXT);
  _setmode(_fileno(stdin), _O_WTEXT);
  UINT oldcp = GetConsoleOutputCP();

  playlist_funcs obj;
  playlist_funcs * pobj = &obj;

  pobj->exit_playlist = false;
  pobj->next_requested = false;
  pobj->playlist_loaded = false;
  pobj->playlist_name = "";
  pobj->prev_requested = false;

  create_directory();
  //SetConsoleOutputCP(CP_UTF8);
  //std::setlocale(LC_ALL, "C.UTF-8");
  //std::setlocale(LC_ALL, "ja_JP.utf8");
  //std::setlocale(LC_ALL, "en_US.utf8");

  StringMan * build;

  LPWSTR *args = CommandLineToArgvW(GetCommandLineW(), &argc);

  //build->println(std::to_string(build->wstrcmp(args[1], L"--help")));

  //return -1;
  pincer::control *controls = new pincer::control(FILEPATH);
  //std::cout << check_create() << "\n";
  // Read and check command line parameters.
  //std::cout << argc << ", " << strcmp(argv[1], "help") << "\n";
  
  /*UNCOMMENT HERE/*///std::setlocale(LC_ALL, "en_US.utf8");

  /*wchar_t *testing = new wchar_t[BUFFER];
  HANDLE in = GetStdHandle(STD_INPUT_HANDLE);
  std::wcout << L"Enter file name: ";
  DWORD char_read;

  std::wstring ins;
  std::getline(std::wcin, ins);
  std::wcout << ins << L"\n";
  
  Convertors conversion = Convertors();

  //WideCharToMultiByte(CP_UTF8, 0, testing, -1, mem2, bufferSize, NULL, NULL);

  //std::cout << new_val << "\n";

  std::wifstream infile(ins.c_str());
  std::wcout<< L"File Exist: " << infile.is_open() << L"\n";
  infile.close();*/

  /*std::cout << "File Exist2: " << std::filesystem::exists(testing.c_str()) << "\n";*/

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

  user_settings *presets = new user_settings;
  presets->offset = 0;
  presets->output_device = -1;
  presets->volume = 0.2f;
  presets->sample_rate = 44100;
  presets->start_pos = 0;

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

  std::wstring filename;

  bool has_run = false;
  
  //area for command usage only
  if(argc > 1){
    for(int i = 1; i < argc; i++){
      if(build->wstrcmp(args[i], L"-cp") == 1){
        try{
          dest_file = args[i+1];
          copy_enabled = true;
          continue;
        }catch(...){
          std::wcout << L"Error: Invalid path, could not copy file.";
          break;
        }
        continue;
      }
      
      if(std::regex_search(args[i], word_regex)){
        if(has_run){
          continue;
        }
        has_run = true;
        
        filename = std::wstring(args[i]);
        std::wcout << filename << L"\n";
        
        /*std::cout << "Filename: " << filename << "\n";
        auto begin = std::sregex_iterator(filename.begin(), filename.end(), word_regex);
        auto end = std::sregex_iterator();

        std::smatch match = *begin;
        std::string match_str = match.str();
        std::cout << match_str << "\n";*/
        /*UNCOMMENT HERE*/has_file = true;
      }
      if(build->wstrcmp(args[i], L"-o") == 1){
        //std::cout << "out put value is out?: " << argv[i+1] << "\n";
        try{
          presets->output_device = std::stoi(args[i+1])-1;
          output_device = std::stoi(args[i+1])-1;
        }catch(...){
          std::wcout << L"Invalid value, using the default output device of -1.\n";
          //controls->showOutputs();
        }
      }

      if(build->wstrcmp(args[i], L"-v") == 1){
        //std::cout << "volume value is out? " << argv[i] << "\n";
        try{
          float levels = std::stof(args[i+1]);
          if(levels > 1){
            levels = 1.0f;
          }
          presets->volume = levels;
          volume = levels;
        }catch(...){
          std::wcout << L"Invalid value, using the default of 0.2.\n";
        }
      }

      if(build->wstrcmp(args[i], L"-s") == 1){
        try{
          presets->sample_rate = std::stoi(args[i+1]);
          sample_rate = std::stoi(args[i+1]);
        }catch(...){
          std::wcout << L"Invalid Sample Rate, using the default value of 44100.\n";
        }
      }
      
      if(build->wstrcmp(args[i], L"-off") == 1){
        //std::cout << "offset value is out? " << argv[i] << "\n";
        try{
          presets->offset = std::stoi(args[i+1]);
          offset = std::stoi(args[i+1]);
        }catch(...){
          std::wcout << L"Offset was invalid using the default value of 0.\n";
        }
      }

      if(build->wstrcmp(args[i], L"--start") == 1){
        //std::cout << "offset value is out? " << argv[i] << "\n";
        try{
          presets->start_pos = std::stoi(args[i+1]);
          start_pos = std::stoi(args[i+1]);
        }catch(...){
          presets->start_pos = 0;
          start_pos = 0;
          std::wcout << L"Invalid Value. File will begin from the beginning.\n";
        }
      }
    }
  }

  if(!has_file){
    build->wprintln(L"No file selected");
  }else{
    //std::wcout << filename << L"\n";
    if(!controls->startSong<std::wstring>(output_device, volume, sample_rate, offset, start_pos, filename, &original_volume, &channel)){
      build->wprintln(L"Could not read the file. This could be due to the program not being able to read special character files.");
    };
    build->wprintln(L"File Loaded/Playing: " + static_cast<std::wstring>(filename));
    //std::cout << "\nOutput Dev: " << output_device << ", Volume: " << volume << ", Sample Rate: " << sample_rate << ", Offset: " << offset << ", Start Position: " << start_pos << ", Filename: " << filename << ", Output File: " << dest_file << "\n";
  }

  if(copy_enabled){
    controls->copy_file_to(filename, dest_file);
  }

  std::wcout << L"\n";

  std::wstring stripped_str = L"";
  //std::string* split_str = NULL;

  std::wstring type = L"Noload";
  
  //_locale_t things = _get_current_locale();
  //std::wcout << things->mbcinfo << L'\n';

  while(true){
    std::wstring command;
    std::wstring* split_str = NULL;
    stripped_str = L"";

    if(controls->getLoaded()){type = L"Loaded";}else{type=L"Noload";}

    //std::wcin.ignore();
    //filenames don't support umulauts: äöüß
    try{
      std::wcin.clear();
      std::wcout << L"\n" << type << L">"; 
      std::getline(std::wcin, command);
      std::wcin.ignore(0, L'\n');
      split_str = build->w_split(command, L' ', command.size(), 1);
      stripped_str = build->w_strip(command, command.size());
    }catch(...){
      std::wcout << L"Problem with the command\n";
      break;
    }

    if(command == L""){
      continue;
    }
    
    if(split_str[0] == L"play"){
      try{
        if(split_str[1] != L""){
          if(controls->getLoaded()){
            std::wcout << L"A File is already loaded. Please unload it to use a playlist.\n";
            //playPlaylist(split_str[1], &channel, build, controls);
          }else{
            playPlaylist(split_str[1], &channel, build, controls, pobj);
          }
        }else{
          controls->playSong(channel);
        }
      }catch(...){
        std::wcout << "Problem occurred\n";
        controls->playSong(channel);
      }
    }
    if(split_str[0] == L"restart"){
      controls->restart(channel);
    }
    if(split_str[0] == L"pause"){
      controls->pauseSong(channel);
    }
    if(split_str[0] == L"stop"){
      controls->stopSong();
      pobj->exit_playlist = true;
    }
    if(split_str[0] == L"forward"){
      controls->forward(channel, 50);
    }
    if(split_str[0] == L"backward"){
      controls->back(channel);
    }
    if(split_str[0] == L"next"){
      pobj->next_requested = true;
    }
    if(split_str[0] == L"prev"){
      pobj->prev_requested = true;
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
      delete [] split_str;
      break;
    }
    if(split_str[0] == L"load"){ //how the frick do you split a string
      if(controls->getLoaded()){
        build->wprintln(L"Can't load another file because another one is already loaded. run 'unload' to deselect the current file.");
        delete [] split_str;
        continue;
      }
      if(split_str[1] != L""){
        const int filename_length = split_str[1].size();

        /*std::cout << "File Size: " << filename_length << "\n";
        std::cout << "Filename using stripped: " << stripped_str << "\n";
        std::cout << "Filename Before Load: " << split_str[1] << "\n";
        std::cout << "Filename with UTF-8 conversion: " << command << "\n";
        std::cout << split_str[1] + " Canon Event" << "\n";*/
        //std::wcout << L"To UTF-8: " << two << L"\n";
        
        std::wifstream good(split_str[1].c_str());
        std::wcout << L"Is Open: " << good.is_open() << L"\n";
        good.close();
        
        wchar_t *newFile = new wchar_t[filename_length];

        for(int k = 0; k < filename_length; k++){ //problem here
          if(split_str[1][k] == L'\n' || split_str[1][k] == L'\0'){
            break;
          }
          std::wcout << split_str[1][k];
          newFile[k] = split_str[1][k];
          //std::wcout << k << ". " << split_str[1][k] << L"\n";
        }
        std::wcout << "\n" << newFile << L"\n";
        
        controls->load(split_str[1], &channel);
        delete [] newFile;
      }else{build->wprintln(L"No file was entered.");}
    }
    if(split_str[0] == L"unload"){
      if(controls->getLoaded()){
        controls->unload();
      }else{build->wprintln(L"No file is currently loaded.");}
    }
    if(split_str[0] == L"volume"){
      if(split_str[1] != L""){
        try{
          controls->change_volume(std::stof(split_str[1]), channel);
        }catch(...){
          build->wprintln(L"Invalid value entered.");
        }
      }
      
      std::wcout << L"Volume: " << split_str[1] << L"\n";
    }
    if(split_str[0] == L"length"){
      if(split_str[1] != L""){
        controls->length(channel, split_str[1]);
      }else{
        controls->length(channel);
      }
    }
    if(split_str[0] == L"add"){
      try{
        std::wstring * addition = build->w_split(stripped_str, ' ', stripped_str.size(), 2);
        controls->create_write_json(addition[1], addition[2]);
        //return -1;
      }catch(...){
        std::wcout<<L"The format for add is 'add <playlist_name> <file_name>'\n";
      }
    }
    if(split_str[0] == L"playlists"){
      try{
        if(split_str[1] == L""){
          controls->list_playlists();
        }else{
          std::wstring *choices = build->w_split(split_str[1], ' ', split_str[1].size(), 3);
          if(choices[0] == L"remove"){
            try{
              controls->remove_playlist_file(choices[1], choices[2], build);
            }catch(...){
              continue;
            }
          }
          //std::wcout << split_str[1] << L"\n";
          controls->list_playlists_files(split_str[1], build);
        }
      }catch(std::regex_error e){
        //controls->list_playlists();
        std::wcout << e.what();
		    continue;
      }
      
      //std::wcout << L"Coming Soon\n";
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
  controls->stopSong();

  //delete build;
  //delete controls;
  //delete pobj;

  SetConsoleOutputCP(oldcp);
  return 0;
}


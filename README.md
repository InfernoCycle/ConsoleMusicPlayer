# What is this?
Music Player using windows console instead of a GUI. Still a work in progress.

# Required Tools
C++ 11 or Up Compiler

1. [MSYS2](https://www.msys2.org/)
2. [MINGW](https://www.mingw-w64.org/downloads/)
3. [Visual Studio](https://visualstudio.microsoft.com/vs/features/cplusplus/)

# Build
All the libraries needed are already inside this project. The only thing you need to do is run the "run.bat" file in the command prompt from the same directory as the "process.cpp" file.

# After Build
A file called "process2.exe" should be built in the same directory. You can then run the file in a command prompt or by running an mp3 file as the argument using theis application.

# Using Program
Using the program is pretty straightforward as all it does is play music for now.

## Available Commands (That Work)

Commands Include:
1. `play - starts currently loaded song`
2. `stop - stops the currently loaded song if it wasn't already. Also unloads the song file.`
3. `pause - pauses currently loaded song.`
4. `restart - replays the currently loaded song from the beginning.`
5. `forward - fast forwards current file by 5 seconds.`
6. `backward - rewinds current file by 5 seconds.`
7. `volume <val> - changes the volume of the song. Volume ranges from 0 to 100.`
8. `load - loads a media file into the stream.`
9. `unload - does the same thing that the 'stop' command does.`
10. `pos - shows the current position a song is at.`
11. `length <secs | mins> - shows the length of a song in minutes or seconds (default).`
12. `playlists <playlist_name:optional> - lists all playlists available and files in that playlist.`
13. `add <playlist_name> <filename> - adds a file to the specified playlist.`
14. `help - shows all available commands and what they do.`
15. `exit - exits the program`
> [!NOTE]
> Behavior of the app may vary by the OS used. Also, output has been known to display duplicates in Windows 8.1

>[!WARNING]
> Please Beware of any sudden volume changes when changing songs.

## Program in Use
```
Noload>load C:/songs/song.mp3
Loaded>play

Loaded>pause

Loaded>pos
Time: 0:22/3:45

Loaded>volume 55
Volume: 55

Loaded>length
Length: 225.00 Seconds

Loaded>stop

Noload>
```

## Supported Files
Only MP3 files have been tested. But as found on the front page of the [Un4seen BASS library documentation](https://www.un4seen.com/). 

Supported files include:
```Support for WAV/AIFF/MP3/MP2/MP1/OGG and custom generated samples```

# Files I Made
I did make custom header files with custom functions that rebuilt already made functions in C++ due to some of them being difficult to use.

The Header Files that were made by me are "addon.h", "options.h", and "controls.h" in the include folder.

# Operating Systems Supported
This build has been tested on Windows 8.1, and 10. I have no idea if it can be used on Unix Platforms.

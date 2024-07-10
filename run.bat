@echo off

echo Building the application...

g++ process.cpp -I include -L lib -lbass -o process2.exe
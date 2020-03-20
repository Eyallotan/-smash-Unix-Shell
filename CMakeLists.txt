cmake_minimum_required(VERSION 3.10)
project(OS_HW1)

set(CMAKE_CXX_STANDARD 11)

add_executable(temp smash.cpp signals.h signals.cpp Commands.cpp Commands.h
GetCurrDirCommand.cpp SmallShell.h JobsList.cpp CommandsHistory.cpp
        CommandHistoryEntry.cpp HistoryCommand.cpp ChangeDirCommand.cpp
        JobEntry.cpp ExternalCommand.cpp ShowPidCommand.cpp KillCommand.cpp
        ForegroundCommand.cpp BackgroundCommand.cpp CopyCommand.cpp JobsCommand.cpp QuitCommand.cpp RedirectionCommand.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -o main")
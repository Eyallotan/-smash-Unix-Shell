//
// Created by eyall on 11/21/2019.
//

#include <cstdio>
#include <unistd.h>
#include "Commands.h"
#include <iostream>
#include <string.h>

ShowPidCommand::ShowPidCommand(const char *cmd_line) : BuiltInCommand
(cmd_line) {}

void ShowPidCommand::execute() {
    pid_t pid=getpid(); //always successful
    std::cout<<"smash pid is "<<pid<<std::endl;
}


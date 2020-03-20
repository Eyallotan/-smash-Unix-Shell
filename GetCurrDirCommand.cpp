//
// Created by tomer on 11/19/2019.
//

#include <cstdio>
#include <unistd.h>
#include "Commands.h"


GetCurrDirCommand::GetCurrDirCommand(const char *cmd_line) :BuiltInCommand
(cmd_line) {}


void GetCurrDirCommand::execute() {
    char pwd[LINUX_MAX_PATH];
    void* res=getcwd(pwd,LINUX_MAX_PATH);
    if(!res){
        perror("smash error: getcwd failed");
        return;
    }
    printf("%s\n",pwd);
}
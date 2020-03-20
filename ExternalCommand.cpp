//
// Created by eyall on 11/19/2019.
//

#include <cstdio>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include "Commands.h"
#include "SmallShell.h"
#include <sys/wait.h>

#define BASH_LOCATION "/bin/bash"
#define BASH_FLAGS "-c"

ExternalCommand::ExternalCommand(const char *cmd_line, JobsList *job_list) :
Command (cmd_line) , job_list(job_list){
    is_external_command = true;
}

ExternalCommand::ExternalCommand(const char* cmd_line, JobsList* job_list, const char*
redirection_line) :
        Command (cmd_line,redirection_line) , job_list(job_list){
    is_external_command = true;
}

void ExternalCommand::execute() {
    pid_t pid=fork();
    if (pid==-1){
        perror("smash error: fork failed");
        return;
    }
    if (pid>0) { //parent-smash
        handleExecutedCommand(pid);
        return;
    }
    //child-bash
    setpgrp();
    _removeBackgroundSign((char*)cmd_line);
    const char *argv[]={"bash", BASH_FLAGS, cmd_line, nullptr};
    if(execv(BASH_LOCATION,(char**)argv)<0) { //run external command in bash
        perror("smash error: execv failed");
        exit(1);
    }
}

void ExternalCommand::handleExecutedCommand(pid_t child_pid) {
    if (_isBackgroundComamnd(cmd_line)) { //background command
        job_list->addJob(this, child_pid, false);
    } else { //external in fg
        SmallShell& shell=SmallShell::getInstance();
        shell.setCurrentPidInFg(child_pid);
        shell.setCurrentCommandInFg(this);
        if (waitpid(child_pid, NULL, WUNTRACED) <0) {
            perror("smash error: waitpid failed");
        }
    }
}
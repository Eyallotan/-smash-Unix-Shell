//
// Created by eyall on 11/29/2019.
//

#include <cstdio>
#include <unistd.h>
#include "Commands.h"
#include <iostream>
#include <string.h>
#include "SmallShell.h"

QuitCommand::QuitCommand(const char *cmd_line, JobsList *jobs) :
BuiltInCommand(cmd_line), job_list(jobs){}

void QuitCommand::execute() {
    if (num_of_arguments>1) {
        if (strcmp("kill", args[1]) == 0) {
            job_list->killAllJobs();
        }
    }
    exit(0); //frees all SmallShell resources
}

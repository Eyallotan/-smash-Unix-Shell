//
// Created by eyall on 11/21/2019.
//

#include <cstdio>
#include <unistd.h>
#include "Commands.h"
#include <iostream>
#include <string.h>

#define KILL_ARGUMENTS 3
#define MAX_SIG_NUM 31
#define MIN_SIG_NUM 1

KillCommand::KillCommand(const char *cmd_line, JobsList *jobs) :
BuiltInCommand(cmd_line), job_list(jobs){}

void KillCommand::execute() {
    job_list->removeFinishedJobs();
    if (num_of_arguments!=KILL_ARGUMENTS){
        std::cout<<"smash error: kill: invalid arguments"<<std::endl;
        return;
    }
    int sig_num=parseArgument(args[1]);
    int job_id=parseArgument(args[2]);
    if (sig_num==NOT_AN_INTEGER || job_id==NOT_AN_INTEGER){
        std::cout<<"smash error: kill: invalid arguments"<<std::endl;
        return;
    }
    sig_num *=-1;
    if (!job_list->jobExsistInList(job_id)){
        std::cout<<"smash error: kill: job-id "<<job_id<<" does not "
                                                       "exist"<<std::endl;
        return;
    }
    pid_t pid=job_list->getJobById(job_id)->getPID();
    int res=kill(pid,sig_num);
    if (res==-1){
        perror("smash error: kill failed");
        return;
    }
    else{
        std::cout<<"signal number "<<sig_num<<" was sent to pid "
                                              ""<<pid<<std::endl;
    }

}

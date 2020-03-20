//
// Created by eyall on 11/21/2019.
//

#include <cstdio>
#include <unistd.h>
#include "Commands.h"
#include <iostream>
#include <string.h>
#include <sys/wait.h>
#include "SmallShell.h"
#define MAX_FG_ARGUMENTS 2



ForegroundCommand::ForegroundCommand(const char *cmd_line, JobsList *jobs) :
        BuiltInCommand(cmd_line), jobs_list(jobs){}


void ForegroundCommand::execute() {
    jobs_list->removeFinishedJobs(); //clean job list before execution
    JobsList::JobEntry* job_to_fg;
    int job_id_to_fg;
    if (num_of_arguments==1){
        job_to_fg=jobs_list->getLastJob(&job_id_to_fg);
        if (job_id_to_fg==-1){
            std::cout<<"smash error: fg: jobs list is empty"<<std::endl;
            return;
        }
        FgExecuteAux(job_id_to_fg,job_to_fg);
        return;
    }
    job_id_to_fg=parseArgument(args[1]);
    if (num_of_arguments>MAX_FG_ARGUMENTS || job_id_to_fg==NOT_AN_INTEGER){
        std::cout<<"smash error: fg: invalid arguments"<<std::endl;
        return;
    }

     //num of arguments == MAX_FG_ARGUMENTS
     if (!jobs_list->jobExsistInList(job_id_to_fg)){
         std::cout<<"smash error: fg: job-id "<<job_id_to_fg<<" does not "
                                                           "exist"<<std::endl;
         return;
     }
    job_to_fg=jobs_list->getJobById(job_id_to_fg);
    FgExecuteAux(job_id_to_fg,job_to_fg);
}

void ForegroundCommand::FgExecuteAux(int job_id_to_fg, JobsList::JobEntry*
job_to_fg){
    pid_t pid_of_fg=job_to_fg->getPID();
    std::cout<<job_to_fg->getCommandLine()<<" : "<<pid_of_fg<<std::endl;
    if (job_to_fg->getJobType()==JobsList::JobType::STOPPED) {
        if (kill(pid_of_fg, SIGCONT) < 0) {
            perror("smash error: kill failed");
            return;
        }
    }
    SmallShell& shell=SmallShell::getInstance();
    shell.setCurrentPidInFg(pid_of_fg);
    shell.setCurrentCommandInFg(job_to_fg->getCommand());
    int status;
    int res=waitpid(pid_of_fg,&status,WUNTRACED);
    if(res>0) {
        if (!WIFSTOPPED(status)) {
            jobs_list->removeJobById(job_id_to_fg); //got ctrl+c
        }
    }
    if (res==-1){
        perror("smash error: waitpid failed");
    }
}


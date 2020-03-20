//
// Created by eyall on 11/21/2019.
//

#include <cstdio>
#include <unistd.h>
#include "Commands.h"
#include <iostream>
#include <string.h>

#define INVALID_JOB_ID -1
#define BG_MAX_ARGS 2

BackgroundCommand::BackgroundCommand(const char *cmd_line, JobsList *jobs) :
                        BuiltInCommand(cmd_line), jobs_list(jobs) {}


void BackgroundCommand::execute() {
    jobs_list->removeFinishedJobs();
    JobsList::JobEntry *job = NULL;
    if(num_of_arguments == 1) {
        int jobId;
        job = jobs_list->getLastStoppedJob(&jobId);
        if(INVALID_JOB_ID == jobId) {
            std::cout << "smash error: bg: there is no stopped jobs to resume"<< std::endl;
            return;
        }
        job->continueJob();
        return;
    }
    int job_id=parseArgument(args[1]);
    if (num_of_arguments > BG_MAX_ARGS || job_id==NOT_AN_INTEGER) {
        std::cout << "smash error: bg: invalid arguments" << std::endl;
        return;
    }

    job = jobs_list->getJobById(job_id);
    if(!job) {
        std::cout << "smash error: bg: job-id "<< job_id <<
        " does not exist"<< std::endl;
        return;
    }
    if(job->getJobType() == JobsList::JobType::RUNNING) {
        std::cout << "smash error: bg: job-id "<< job_id <<
            " is already running in the background" << std::endl;
        return;
    }

    job->continueJob();
}
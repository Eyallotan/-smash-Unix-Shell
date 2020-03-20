//
// Created by eyall on 11/23/2019.
//

#include <cstdio>
#include <unistd.h>
#include "Commands.h"

JobsCommand::JobsCommand(const char *cmd_line, JobsList *jobs) :
BuiltInCommand(cmd_line), jobs_list(jobs)
{}

void JobsCommand::execute() {
    jobs_list->removeFinishedJobs();
    jobs_list->printJobsList();
}

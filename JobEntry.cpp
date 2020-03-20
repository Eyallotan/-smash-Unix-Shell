//
// Created by tomer on 11/19/2019.
//

#include "Commands.h"

JobsList::JobEntry::JobEntry(pid_t pid, int job_id, JobType
job_type, Command *cmd): pid(pid), job_id(job_id), job_type(job_type), cmd
(cmd) {
    entry_time=time(NULL);
    if (entry_time==-1){
        perror("smash error: time failed");
    }
}

JobsList::JobEntry::~JobEntry() {
    delete cmd;
}
const char* JobsList::JobEntry::getCommandLine() {
    return cmd->getCmdLine();
}

int JobsList::JobEntry::getJobId() {
    return job_id;
}

int JobsList::JobEntry::getPID() {
    return pid;
}

time_t JobsList::JobEntry::getEntryTime() {
    return entry_time;
}

JobsList::JobType JobsList::JobEntry::getJobType() {
    return job_type;
}

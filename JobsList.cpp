//
// Created by tomer on 11/19/2019.
//

#include "Commands.h"
#include <iostream>
#include <sys/wait.h>


using namespace std;

JobsList::JobsList() :job_list(nullptr), max_curr_id(0){
    job_list = new list<JobEntry*>();
}

JobsList::~JobsList() {
    list<JobEntry*>::iterator it=job_list->begin();
    while (it!=job_list->end()){
        delete *it;
        job_list->erase(it++);
    }
    delete job_list;
}

void JobsList::addJob(Command *cmd, pid_t pid, bool isStopped) {
    JobType status=RUNNING;
    if (isStopped){
        status=STOPPED;
    }
    JobEntry *job_entry = new JobEntry(pid,max_curr_id+1,status,cmd);
    job_list->push_back(job_entry);
    max_curr_id++;
}

void JobsList::printJobsList(){
   for(list<JobEntry*>::iterator it=job_list->begin();it!=job_list->end();
   ++it){
       time_t time_elapsed=time(nullptr)-(*it)->getEntryTime();
       if ((*it)->getJobType()==RUNNING) {
           cout << "[" << (*it)->getJobId() << "] " << (*it)->getCommandLine()
                << " : " << (*it)->getPID() << " " << time_elapsed << " secs"
                << endl;
       }
       else {
           cout << "[" << (*it)->getJobId() << "] " << (*it)->getCommandLine()
                << " : " << (*it)->getPID() << " " << time_elapsed <<
                " secs (stopped)"<< endl;
       }
   }


}
void JobsList::removeFinishedJobs()

{
    list<JobEntry*>::iterator it=job_list->begin();
    while (it!=job_list->end()){
        pid_t curr_pid = (*it)->getPID();
        pid_t res=waitpid(curr_pid,NULL,WNOHANG);
        if (res==-1){
            perror("smash error: waitpid failed");
            return;
        }
        if (res == curr_pid) {
            if ((*it)->getJobId()==max_curr_id) {
                delete *it;
                job_list->erase(it++);
                if (job_list->empty()) {
                    max_curr_id = 0;
                    return;
                }
                max_curr_id=job_list->back()->getJobId(); //get the highest
                // active job id in list
            }
            else {
                delete *it;
                job_list->erase(it++);
            }
        }
        else {
            ++it;
        }
    }
}

bool JobsList::jobExsistInList(int job_id){
    for(list<JobEntry*>::iterator it=job_list->begin();it!=job_list->end();
        ++it){
        if ((*it)->getJobId()==job_id){
            return true;
        }
    }
    return false;
}

JobsList::JobEntry* JobsList::getJobById(int job_id){
    for(list<JobEntry*>::iterator it=job_list->begin();it!=job_list->end();
        ++it){
        if ((*it)->getJobId()==job_id){
            return *it;
        }
    }
    return nullptr; //job not found
}

void JobsList::removeJobById(int job_id) {
    for(list<JobEntry*>::iterator it=job_list->begin();it!=job_list->end();
        ++it) {
        if ((*it)->getJobId() == job_id) {
            delete *it;
            job_list->erase(it);
            if(job_id==max_curr_id) {
                if (job_list->empty()) {
                    max_curr_id = 0;
                    return;
                }
                max_curr_id=job_list->back()->getJobId();
            }
            return;
        }
    }

}

JobsList::JobEntry* JobsList::getLastStoppedJob(int *jobId) {
    JobEntry *result = NULL;
    *jobId = -1; // list is empty
    for(JobEntry* job : *job_list) {
        if(job->getJobType() == STOPPED) {
            *jobId= job->getJobId();
            result = job;
        }
    }
    return result;
}

Command* JobsList::JobEntry::getCommand() {
    return cmd;
}

JobsList::JobEntry * JobsList::getJobByPid(pid_t pid){
    for(list<JobEntry*>::iterator it=job_list->begin();it!=job_list->end();
        ++it){
        if ((*it)->getPID()==pid){
            return *it;
        }
    }
    return nullptr; //job not found
}

JobsList::JobEntry * JobsList::getLastJob(int* lastJobId){
    if (job_list->empty()){
        *lastJobId=-1;
        return nullptr;
    }
    *lastJobId=job_list->back()->getJobId();
    return job_list->back();
}

void JobsList::JobEntry::continueJob() {
    std::cout << cmd->getCmdLine() << " : " << pid << std::endl;
    job_type = RUNNING;
    if (kill(pid, SIGCONT) < 0) {
        perror("smash error: kill failed");
    }
}

void JobsList::killAllJobs() {
    removeFinishedJobs(); //clear job list first
    std::cout<<"smash: sending SIGKILL signal to "<<job_list->size()<<" "
                                                                      "jobs:"<<std::endl;
    for(list<JobEntry*>::iterator it=job_list->begin();it!=job_list->end();
        ++it){
        if(kill((*it)->getPID(),SIGKILL)==0) {
            std::cout << (*it)->getPID() << ": " << (*it)->getCommandLine()
                      << std::endl;
        }
        else {
            perror("smash error: kill failed");
            return;
        }
    }
    removeFinishedJobs(); //clear job list after killing all processes
}

void JobsList::changeJobStatusToStopped(pid_t pid){
    for(list<JobEntry*>::iterator it=job_list->begin();it!=job_list->end();
        ++it){
        if ((*it)->getPID()==pid){
            (*it)->setJobType(JobsList::JobType::STOPPED);
        }
    }
}

void JobsList::JobEntry::setJobType(JobsList::JobType new_job_type) {
    job_type=new_job_type;
}

void JobsList::resetJobTimer(pid_t pid) {
    for(list<JobEntry*>::iterator it=job_list->begin();it!=job_list->end();
        ++it){
        if ((*it)->getPID()==pid){
            (*it)->resetTimer();
        }
    }
}

void JobsList::JobEntry::resetTimer() {
    entry_time=time(nullptr);
}
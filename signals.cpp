#include <iostream>
#include <signal.h>
#include "signals.h"
#include "Commands.h"
#include "SmallShell.h"
#include <sys/wait.h>

using namespace std;

void ctrlZHandler(int sig_num) {
	std::cout<<"smash: got ctrl-Z"<<std::endl;
	SmallShell& shell=SmallShell::getInstance();
	pid_t pid_in_fg=shell.getCurrentPidInFg();
	if (!pid_in_fg){
	    return;
	}
	int res=waitpid(pid_in_fg,NULL,WNOHANG);
	if (res==-1){
        perror("smash error: waitpid failed");
	}
	if (!res){
		if(kill(pid_in_fg,SIGSTOP)==0){
		    std::cout<<"smash: process "<<pid_in_fg<<" was stopped"<<std::endl;
            shell.addStoppedJobtoList(shell.getCurrentCommandInFg(), pid_in_fg);
		}
		else{
            perror("smash error: kill failed");
		}
	}
}

void ctrlCHandler(int sig_num) {
    std::cout<<"smash: got ctrl-C"<<std::endl;
    SmallShell& shell=SmallShell::getInstance();
    pid_t pid_in_fg=shell.getCurrentPidInFg();
    if (!pid_in_fg){
        return;
    }
    int res=waitpid(pid_in_fg,NULL,WNOHANG);
    if (res==-1){
        perror("smash error: waitpid failed");
    }
    if (!res){
        if(kill(pid_in_fg,SIGKILL)==0){
            std::cout<<"smash: process "<<pid_in_fg<<" was killed"<<std::endl;
        }
        else{
            perror("smash error: kill failed");
        }
    }
}

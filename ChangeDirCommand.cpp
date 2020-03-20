//
// Created by eyall on 11/19/2019.
//

#include <cstdio>
#include <unistd.h>
#include "Commands.h"
#include <iostream>
#include <string.h>
#include "SmallShell.h"

#define MAX_CHDIR_ARGUMENTS 2

ChangeDirCommand::ChangeDirCommand(const char *cmd_line, char **plastPwd)
        : BuiltInCommand(cmd_line), plastPwd(plastPwd){}

void ChangeDirCommand::execute() {
    if (num_of_arguments>MAX_CHDIR_ARGUMENTS){
        //to many arguments
        std::cout<<"smash error: cd: too many arguments"<<std::endl;
    }
    else if (num_of_arguments==1) {
        std::cout<<"smash error: cd: missing argument"<<std::endl;
    }
    else if (strcmp(args[1],"-")==0){ //change back to last directory
        if (strcmp("",(const char*)*plastPwd)==0){
            std::cout<<"smash error: cd: OLDPWD not set"<<std::endl;
            //OLDPWD not set
        }
        else{
            char* temp=new char[LINUX_MAX_PATH];
            strcpy(temp,*plastPwd); //path to go to
            void* res=getcwd(*plastPwd,LINUX_MAX_PATH); //update new "old path"
            if(!res){
                perror("smash error: getcwd failed");
                delete [] temp;
                return;
            }
            int res1=chdir(temp); //go to last path
            if (res1==-1){
                perror("smash error: chdir failed");
                delete [] temp;
                return;
            }
            delete [] temp;
        }
    }
    else{ //change directory to path
        void* res=getcwd(*plastPwd,LINUX_MAX_PATH);
        if(!res){
            perror("smash error: getcwd failed");
            return;
        }
        int res1=chdir(args[1]);
        if (res1==-1){
            perror("smash error: chdir failed");
        }

    }
}
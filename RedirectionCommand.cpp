//
// Created by eyall on 12/2/2019.
//

#include <cstdio>
#include <unistd.h>
#include "Commands.h"
#include <iostream>
#include <string.h>
#include <fcntl.h>
#include "SmallShell.h"

using namespace std;

RedirectionCommand::RedirectionCommand(const char *cmd_line, bool is_append):
Command(cmd_line) , redirection_path(nullptr) , main_cmd(nullptr),
redirect_with_append(is_append){

    ///parsing and getting all paths///

    /* parse command to redirect from cmd_line */
    char* cmd_line_copy=new char[strlen(cmd_line)+1];
    strcpy(cmd_line_copy,cmd_line);
    std::string command_to_redirect=parseCommandToRedirect(cmd_line_copy);
    char* command= new char[command_to_redirect.length()+1];
    strcpy(command,command_to_redirect.c_str());

    /* parse path to redirect from cmd_line */
    redirection_path=parsePathToRedirect(cmd_line);

    /// finished parsing ///

    /* create main command */
    createMainCommand(cmd_line,command_to_redirect,command);
    delete [] cmd_line_copy;
}
/* we can assume arguments to redirection will be set properly*/
void RedirectionCommand::createMainCommand(const char* cmd_line, string
command_to_redirect, char* command){
    /* retrieve shell fields */
    SmallShell& shell=SmallShell::getInstance();
    char* last_pwd=shell.getLastPwd();
    CommandsHistory* history=shell.getCommandsHistory();;
    JobsList* jobs_list=shell.getJobsList();

    if (command_to_redirect.find("pwd")==0) {
        main_cmd = new GetCurrDirCommand(command);
    }
    else if (command_to_redirect.find("cd")==0){
        main_cmd =  new ChangeDirCommand(command, &last_pwd);
    }
    else if (command_to_redirect.find("history")==0) {
        main_cmd =  new HistoryCommand(command, history);
    }
    else if (command_to_redirect.find("jobs")==0) {
        main_cmd =  new JobsCommand(command, jobs_list);
    }
    else if (command_to_redirect.find("showpid")==0) {
        main_cmd =  new ShowPidCommand(command);
    }
    else if (command_to_redirect.find("kill")==0){
        main_cmd =  new KillCommand(command,jobs_list);
    }
    else if (command_to_redirect.find("fg")==0) {
        main_cmd =  new ForegroundCommand(command, jobs_list);
    }
    else if (command_to_redirect.find("bg")==0) {
        main_cmd =  new BackgroundCommand(command, jobs_list);
    }
    else if (command_to_redirect.find("quit")==0){
        main_cmd =  new QuitCommand(command, jobs_list);
    }
    else if (command_to_redirect.find("cp")==0){
        main_cmd =  new CopyCommand(command);
    }
    else {
        char* cmd_line_copy=new char[strlen(cmd_line)+1];
        strcpy(cmd_line_copy,cmd_line); //copy cmd_line to pass to external command
        main_cmd =  new ExternalCommand(command,jobs_list,cmd_line_copy);
    }
}

std::string RedirectionCommand::parseCommandToRedirect(const char* cmd_line){
    bool add_background_sign=false;

    if (_isBackgroundComamnd(cmd_line)) {
        _removeBackgroundSign((char*)cmd_line);
        add_background_sign=true;
    }
    std::string cmd=(string)cmd_line;
    std::string command_to_redirect;
    if (redirect_with_append){
        command_to_redirect = cmd.substr(0,cmd.find(">>")); //before '>>' sign
    }
    else {
        command_to_redirect = cmd.substr(0, cmd.find(">")); //before '>' sign
    }
    if (add_background_sign) {
        command_to_redirect.append("&");
    }

    return command_to_redirect;
}

char* RedirectionCommand::parsePathToRedirect(const char *cmd_line) {
    std::string cmd=(string)cmd_line;
    std::string path;
    if (redirect_with_append) {
        path = cmd.substr(cmd.find(">>") + 2);
    }
    else {
        path = cmd.substr(cmd.find(">") + 1);
    }
    path = _ltrim(path);
    char *redir_path = new char[path.length()+1];
    strcpy(redir_path,path.c_str());
    if (_isBackgroundComamnd(redir_path)){
        _removeBackgroundSign(redir_path);
    }

    return redir_path;
}


void RedirectionCommand::execute() {
    int stdout_copy=dup(STDOUT_FILENO); //copy stdout fd
    if (stdout_copy==-1){
        perror("smash error: dup failed");
    }
    if (close(STDOUT_FILENO)<0){
        perror("smash error: close failed");
    } // close stdout
    int new_fd;
    if (redirect_with_append) {
       new_fd = open(redirection_path, O_WRONLY | O_CREAT | O_APPEND , 0666);
       if (new_fd==-1){
           perror("smash error: open failed");
       }
    }
    else {
        new_fd = open(redirection_path, O_WRONLY | O_CREAT | O_TRUNC , 0666);
        if (new_fd==-1){
            perror("smash error: open failed");
        }
    }
    //redirection path in assigned to fd=1
    main_cmd->execute();
    if (close(new_fd)<0){
        perror("smash error: close failed");
    } //close file
    if (dup2(stdout_copy,STDOUT_FILENO)<0){
        perror("smash error: dup2 failed");
    } //copy back stdout to fd no. 1
    if (close(stdout_copy)<0){
        perror("smash error: close failed");
    }

}

RedirectionCommand::~RedirectionCommand() {
    delete redirection_path;
    SmallShell& shell=SmallShell::getInstance();
    if (!main_cmd->isExternalCommand() || shell.IsFgStoppedExternalCmd
    (main_cmd->getMainCmdLine())) {
        delete main_cmd;
    }//else - main command will be deleted by job list


}
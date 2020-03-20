#include <unistd.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <sys/wait.h>
#include <iomanip>
#include "Commands.h"
#include "SmallShell.h"

using namespace std;

#if 0
#define FUNC_ENTRY()  \
  cout << __PRETTY_FUNCTION__ << " --> " << endl;

#define FUNC_EXIT()  \
  cout << __PRETTY_FUNCTION__ << " <-- " << endl;
#else
#define FUNC_ENTRY()
#define FUNC_EXIT()
#endif

/* Parsing functions */

const std::string WHITESPACE = " \n\r\t\f\v";
string _ltrim(const std::string& s)
{
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}

string _rtrim(const std::string& s)
{
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

string _trim(const std::string& s)
{
    return _rtrim(_ltrim(s));
}

int _parseCommandLine(const char* cmd_line, char** args) {
    FUNC_ENTRY()
    int i = 0;
    std::istringstream iss(_trim(string(cmd_line)).c_str());
    for(std::string s; iss >> s; ) {
        args[i] = (char*)malloc(s.length()+1);
        memset(args[i], 0, s.length()+1);
        strcpy(args[i], s.c_str());
        args[++i] = NULL;
    }
    return i;

    FUNC_EXIT()
}


//detects if there is a '&' sign in the command
bool _isBackgroundComamnd(const char* cmd_line) {
  const string whitespace = " \t\n";
  const string str(cmd_line);
  return str[str.find_last_not_of(whitespace)] == '&';
}

//removes '&' sign
void _removeBackgroundSign(char* cmd_line) {
  const string whitespace = " \t\n";
  const string str(cmd_line);
  // find last character other than spaces
  size_t idx = str.find_last_not_of(whitespace);
  // if all characters are spaces then return
  if (idx == string::npos) {
    return;
  }
  // if the command line does not end with & then return
  if (cmd_line[idx] != '&') {
    return;
  }
  // replace the & (background sign) with space and then remove all tailing spaces.
  cmd_line[idx] = ' ';
  // truncate the command line string up to the last non-space character
  cmd_line[str.find_last_not_of(whitespace, idx-1) + 1] = 0;
}

//////////////// end of parsing functions ////////////////

/* SmallShell functions: */

SmallShell::SmallShell() :last_pwd(nullptr),history(nullptr), jobs_list
(nullptr), curr_pid_in_fg(0), external_command_in_fg(nullptr),
running_command(nullptr){
    history = new CommandsHistory();
    last_pwd = new char[LINUX_MAX_PATH];
    jobs_list = new JobsList();
}

SmallShell::~SmallShell() {
    delete history;
    delete [] last_pwd;
    delete jobs_list;
    delete external_command_in_fg;
    delete running_command;
}

/**
* Creates and returns a pointer to Command class which matches the given command line (cmd_line)
*/
Command * SmallShell::CreateCommand(const char* cmd_line) {
    string cmd_s = _ltrim(cmd_line);
    char* trim_cmd=new char[cmd_s.length()+1];
    strcpy(trim_cmd,cmd_s.c_str()); //helper function+delete trim_cmd
    Command* command=createCommandAux(cmd_s,cmd_line,trim_cmd);
    delete [] trim_cmd;
    return command;
}

Command* SmallShell::createCommandAux(string cmd_s, const char* cmd_line, char*
trim_cmd ){
    if (cmd_s.find(">>")!=std::string::npos){
        return new RedirectionCommand(cmd_line, true);
    }
    else if (cmd_s.find(">")!=std::string::npos){
        return new RedirectionCommand(cmd_line, false);
    }
    else if (cmd_s.find("pwd ") == 0 || strcmp(trim_cmd,"pwd")==0) {
        return new GetCurrDirCommand(cmd_line);
    }
    else if (cmd_s.find("cd ") == 0){
        return new ChangeDirCommand(cmd_line, &last_pwd);
    }
    else if(cmd_s.find("history ") == 0 || strcmp(trim_cmd,"history")==0) {
        return new HistoryCommand(cmd_line, history);
    }
    else if(cmd_s.find("jobs ") == 0 || strcmp(trim_cmd,"jobs")==0) {
        return new JobsCommand(cmd_line, jobs_list);
    }
    else if(cmd_s.find("showpid ") == 0 || strcmp(trim_cmd,"showpid")==0) {
        return new ShowPidCommand(cmd_line);
    }
    else if (cmd_s.find("kill ") == 0){
        return new KillCommand(cmd_line,jobs_list);
    }
    else if (cmd_s.find("fg ")==0 || strcmp(trim_cmd,"fg")==0) {
        return new ForegroundCommand(cmd_line, jobs_list);
    }
    else if (cmd_s.find("bg ") == 0 || strcmp(trim_cmd,"bg")==0) {
        return new BackgroundCommand(cmd_line, jobs_list);
    }
    else if (cmd_s.find("quit ") == 0 || strcmp(trim_cmd,"quit")==0){
        return new QuitCommand(cmd_line, jobs_list);
    }
    else if (cmd_s.find("cp ") == 0){
        return new CopyCommand(cmd_line);
    }
    else {
        return new ExternalCommand(cmd_line,jobs_list);
    }
}

void SmallShell::executeCommand(const char *cmd_line) {
    curr_pid_in_fg=0;
    history->addRecord(cmd_line);
    char* copied_cmd_line= new char[strlen(cmd_line)+1];
    strcpy(copied_cmd_line,cmd_line);
    Command* cmd = CreateCommand(copied_cmd_line);
    running_command=cmd;
    cmd->execute();
}

bool SmallShell::IsFgStoppedExternalCmd(const char *cmd_line) {
    return !_isBackgroundComamnd(cmd_line) &&
           waitpid(curr_pid_in_fg,NULL, WNOHANG);
}

void SmallShell::runCommandCleanup() {
    if (!running_command->isExternalCommand() ||
        IsFgStoppedExternalCmd(running_command->getCmdLine())) {
        // if it is fg stopped external command we want to save the Command
        delete running_command;
        external_command_in_fg= nullptr;
        curr_pid_in_fg=0;
    }//background command will be deleted when finished
    jobs_list->removeFinishedJobs(); //job list cleanup in the end of each
    // executed command
}

void SmallShell::setCurrentPidInFg(pid_t pid)  {
    curr_pid_in_fg=pid;
}

pid_t SmallShell::getCurrentPidInFg()  {
    return curr_pid_in_fg;
}

void SmallShell::addStoppedJobtoList(Command *cmd, pid_t pid) {
    if (jobs_list->getJobByPid(pid)){ /*job already in list (got fg and then
        ctrl+z) */
        jobs_list->changeJobStatusToStopped(pid); //retains job id
        jobs_list->resetJobTimer(pid);
    }
    else {
        jobs_list->addJob(cmd, pid, true);
    }
}

void SmallShell::setCurrentCommandInFg(Command *cmd) {
    external_command_in_fg=cmd;
}

Command* SmallShell::getCurrentCommandInFg() {
    return external_command_in_fg;
}

CommandsHistory* SmallShell::getCommandsHistory(){
    return history;
}

char* SmallShell::getLastPwd(){
    return last_pwd;
}

JobsList* SmallShell::getJobsList(){
    return jobs_list;
}


//////////////// end of SmallShell functions ////////////////


/* Command functions: */

Command::Command(const char *cmd_line) : cmd_line(cmd_line), args(nullptr),
num_of_arguments(0) , redirection_cmd_line(nullptr){
    args=(char**)malloc(sizeof(char*)*strlen(cmd_line)+1);
    num_of_arguments=_parseCommandLine(cmd_line,args);
}

Command::Command(const char *cmd_line, const char* redirection_line):
cmd_line(cmd_line), args(nullptr), num_of_arguments(0),
redirected_command(true), redirection_cmd_line(redirection_line)  {
    args=(char**)malloc(sizeof(char*)*strlen(cmd_line)+1);
    num_of_arguments=_parseCommandLine(cmd_line,args);
}

Command::~Command() {
    for (int i = 0; i < num_of_arguments; i++) {
        if (args[i]) {
            free(args[i]);
        }
    }
    free(args);
    delete cmd_line;
    delete redirection_cmd_line;
}

const char* Command::getCmdLine() {
    if (redirected_command) {
        return redirection_cmd_line;
    }
    return cmd_line;
}

bool Command::isExternalCommand() {
    return is_external_command;
}

const char* Command::getMainCmdLine() { //for redirection D'tor
    return cmd_line;
}

int Command::parseArgument(char *argument) {
    unsigned int i = 0;
    if (argument[0] == '-' && strlen(argument)>1) { //negative number
        i = 1;
    }
    for (; i < strlen(argument); i++) {
        if (!isdigit(argument[i])) {
            return NOT_AN_INTEGER;
        }
    }
    return atoi(argument);
}

//////////////// end of Command functions ////////////////
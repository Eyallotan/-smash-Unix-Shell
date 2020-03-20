//
// Created by tomer on 11/19/2019.
//

#ifndef OS_1_SMALLSHELL_H
#define OS_1_SMALLSHELL_H


#include "Commands.h"

class SmallShell {
private:
    SmallShell();
    char* last_pwd;
    CommandsHistory* history;
    JobsList *jobs_list;
    pid_t curr_pid_in_fg;
    Command* external_command_in_fg;
    Command* running_command;

public:
    Command *CreateCommand(const char* cmd_line);
    SmallShell(SmallShell const&)      = delete; // disable copy ctor
    void operator=(SmallShell const&)  = delete; // disable = operator
    static SmallShell& getInstance() // make SmallShell singleton
    {
        static SmallShell instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }
    ~SmallShell();
    void executeCommand(const char* cmd_line);
    void runCommandCleanup();
    void setCurrentPidInFg(pid_t pid);
    pid_t getCurrentPidInFg();
    void addStoppedJobtoList(Command* cmd,pid_t pid);
    void setCurrentCommandInFg(Command* cmd);
    Command* getCurrentCommandInFg();
    CommandsHistory* getCommandsHistory();
    char* getLastPwd();
    JobsList* getJobsList();
    bool IsFgStoppedExternalCmd(const char *cmd_line);
    Command* createCommandAux(std::string cmd_s, const char* cmd_line, char*
    trim_cmd );

};


#endif //OS_1_SMALLSHELL_H

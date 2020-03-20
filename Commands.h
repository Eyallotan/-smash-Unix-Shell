#ifndef SMASH_COMMAND_H_
#define SMASH_COMMAND_H_

#include <vector>
#include <unistd.h>
#include <string>
#include <list>
#include <time.h>
#include <signal.h>

#define COMMAND_ARGS_MAX_LENGTH (200)
#define COMMAND_MAX_ARGS (20)
#define HISTORY_MAX_RECORDS (50)
#define LINUX_MAX_PATH (4096)
#define SMASH_MAX_PROCESSES (100)
#define NOT_AN_INTEGER -2

int _parseCommandLine(const char* cmd_line, char** args);
bool _isBackgroundComamnd(const char* cmd_line);
void _removeBackgroundSign(char* cmd_line);
std::string _ltrim(const std::string& s);
class JobsList;

class Command {
protected:
    const char* cmd_line;
    char** args;
    int num_of_arguments;
    bool is_external_command = false;
    bool redirected_command = false;
    const char* redirection_cmd_line;

public:
  Command(const char* cmd_line);
  Command(const char* cmd_line,const char* redirection_line);
  virtual ~Command();
  virtual void execute() = 0;
  const char* getCmdLine();
  const char* getMainCmdLine(); //for redirection cleanup
  bool isExternalCommand();
  int parseArgument(char* argument);
  //virtual void prepare();
  //virtual void cleanup();
};


class BuiltInCommand : public Command {
 public:
  BuiltInCommand(const char* cmd_line) : Command(cmd_line) {}
  virtual ~BuiltInCommand() {}
};


class ExternalCommand : public Command {
  private:
   JobsList* job_list;
   void handleExecutedCommand(pid_t child_pid);
 public:
  ExternalCommand(const char* cmd_line, JobsList* job_list);
  ExternalCommand(const char* cmd_line, JobsList* job_list, const char*
  redirection_line);
  virtual ~ExternalCommand(){}
  void execute() override;
};

/*class PipeCommand : public Command {
 public:
  PipeCommand(const char* cmd_line);
  virtual ~PipeCommand() {}
  void execute() override;
};
 */

class RedirectionCommand : public Command {
private:
 const char* redirection_path;
 Command* main_cmd;
 bool redirect_with_append;
 public:
  explicit RedirectionCommand(const char* cmd_line, bool is_append);
  virtual ~RedirectionCommand();
  void execute() override;
    std::string parseCommandToRedirect(const char*
    cmd_line);
    char* parsePathToRedirect(const char* cmd_line);
    void createMainCommand(const char* cmd_line, std::string
    command_to_redirect, char* command);
  //void prepare() override;
  //void cleanup() override;
};

class ChangeDirCommand : public BuiltInCommand {
private:
  char** plastPwd;
public:
  ChangeDirCommand(const char* cmd_line,char** plastPwd);
  virtual ~ChangeDirCommand() {}
  void execute() override;
};


class GetCurrDirCommand : public BuiltInCommand {
 public:
  GetCurrDirCommand(const char* cmd_line);
  virtual ~GetCurrDirCommand() {}
  void execute() override;
};


class ShowPidCommand : public BuiltInCommand {
 public:
  ShowPidCommand(const char* cmd_line);
  virtual ~ShowPidCommand() {}
  void execute() override;
};

class QuitCommand : public BuiltInCommand {
  private:
    JobsList* job_list;
  public:
  QuitCommand(const char* cmd_line, JobsList* jobs);
  virtual ~QuitCommand() {}
  void execute() override;
};

class CommandsHistory {
 protected:
  class CommandHistoryEntry {
  private:
      int sequence_number;
      const char* command_line;
  public:
      CommandHistoryEntry(int sequence_number,  const char* command_line);
      ~CommandHistoryEntry();
      int getSequenceNumber();
      const char* getCmdLine();
      void increaseSequenceNumber();
  };
 public:
  explicit CommandsHistory();
  ~CommandsHistory();
  void addRecord(const char* cmd_line);
  void printHistory();
private:
    std::vector<CommandHistoryEntry*> history_log;
    int command_sequence;
    // last command index in the vector
    int command_index;
    bool is_exceeded_max_records;
};

class HistoryCommand : public BuiltInCommand {
private:
    CommandsHistory *history;
 public:
  HistoryCommand(const char* cmd_line, CommandsHistory* history);
  virtual ~HistoryCommand() {}
  void execute() override;
};

class JobsCommand : public BuiltInCommand {
 private:
 JobsList* jobs_list;
 public:
  JobsCommand(const char* cmd_line, JobsList* jobs);
  virtual ~JobsCommand() {}
  void execute() override;
};

class KillCommand : public BuiltInCommand {
 private:
    JobsList* job_list;
 public:
  KillCommand(const char* cmd_line, JobsList* jobs);
  virtual ~KillCommand() {}
  void execute() override;
};

class CopyCommand : public BuiltInCommand {
private:
    char *old_path;
    char *new_path;
 public:
  CopyCommand(const char* cmd_line);
  virtual ~CopyCommand() {}
  void execute() override;
};

class JobsList {
public:
    enum JobType {
        RUNNING, STOPPED
    };
    class JobEntry {
    private:
        pid_t pid;
        int job_id;
        JobType job_type;
        Command *cmd;
        time_t entry_time;
    public:
        JobEntry(pid_t pid, int job_id, JobType job_type, Command *cmd);
        ~JobEntry();
        const char* getCommandLine();
        Command* getCommand();
        int getJobId();
        int getPID();
        time_t getEntryTime();
        JobType getJobType();
        void continueJob();
        void setJobType(JobType job_type);
        void resetTimer();
    };
public:
    JobsList();
    ~JobsList();
    void addJob(Command* cmd, pid_t pid, bool isStopped = false);
    void printJobsList();
    void killAllJobs();
    void removeFinishedJobs();
    JobEntry * getJobById(int jobId);
    JobEntry * getJobByPid(pid_t pid);
    void removeJobById(int jobId);
    JobEntry * getLastJob(int* lastJobId);
    JobEntry *getLastStoppedJob(int *jobId);
    bool jobExsistInList(int job_id);
    void changeJobStatusToStopped(pid_t pid);
    void resetJobTimer(pid_t pid);
private:
    std::list<JobEntry*> *job_list;
    int max_curr_id;
};

class BackgroundCommand : public BuiltInCommand {
private:
    JobsList* jobs_list;
public:
    BackgroundCommand(const char* cmd_line, JobsList* jobs);
    virtual ~BackgroundCommand() {}
    void execute() override;
};

class ForegroundCommand : public BuiltInCommand {
private:
    JobsList* jobs_list;
public:
    ForegroundCommand(const char* cmd_line, JobsList* jobs);
    virtual ~ForegroundCommand() {}
    void execute() override;
    void FgExecuteAux(int job_id_to_fg, JobsList::JobEntry* job_to_fg);
};

#endif //SMASH_COMMAND_H_

SUBMITTERS := 302288527_307916502
COMPILER := g++
COMPILER_FLAGS := --std=c++11 -Wall
SRCS := smash.cpp Commands.cpp JobEntry.cpp ExternalCommand.cpp ShowPidCommand.cpp KillCommand.cpp ForegroundCommand.cpp BackgroundCommand.cpp CopyCommand.cpp JobsCommand.cpp QuitCommand.cpp signals.cpp GetCurrDirCommand.cpp JobsList.cpp CommandsHistory.cpp CommandHistoryEntry.cpp HistoryCommand.cpp ChangeDirCommand.cpp RedirectionCommand.cpp
OBJS=$(subst .cpp,.o,$(SRCS))
HDRS := SmallShell.h Commands.h signals.h  
SMASH_BIN := smash

$(SMASH_BIN): $(OBJS)
	$(COMPILER) $(COMPILER_FLAGS) $^ -o $@

$(OBJS): %.o: %.cpp
	$(COMPILER) $(COMPILER_FLAGS) -c $^

zip: $(SRCS) $(HDRS)
	zip $(SUBMITTERS).zip $^ submitters.txt Makefile

clean:
	rm -rf $(SMASH_BIN) $(OBJS) 
	rm -rf $(SUBMITTERS).zip

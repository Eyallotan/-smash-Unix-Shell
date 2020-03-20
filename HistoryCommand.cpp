//
// Created by tomer on 11/19/2019.
//

#include "Commands.h"

HistoryCommand::HistoryCommand(const char *cmd_line, CommandsHistory *history)
                        : BuiltInCommand(cmd_line) {
    this->history = history;
}
void HistoryCommand::execute() {
    history->printHistory();
}
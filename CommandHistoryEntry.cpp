//
// Created by tomer on 11/19/2019.
//
#include <string.h>
#include "Commands.h"

using namespace std;

CommandsHistory::CommandHistoryEntry::CommandHistoryEntry(int sequence_number,
        const char *command_line) {
    this->sequence_number = sequence_number;
    char* copied_cmd_line= new char[strlen(command_line)+1];
    strcpy(copied_cmd_line,command_line);
    this->command_line = copied_cmd_line;
}

CommandsHistory::CommandHistoryEntry::~CommandHistoryEntry() {
    delete command_line;
}

const char* CommandsHistory::CommandHistoryEntry::getCmdLine() {
    return command_line;
}

int CommandsHistory::CommandHistoryEntry::getSequenceNumber() {
    return sequence_number;
}

void CommandsHistory::CommandHistoryEntry::increaseSequenceNumber() {
    sequence_number++;
}
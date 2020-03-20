//
// Created by tomer on 11/19/2019.
//

#include <iostream>
#include <iomanip>
#include <string.h>
#include "Commands.h"

CommandsHistory::CommandsHistory() : history_log(HISTORY_MAX_RECORDS, NULL),
command_sequence(0), command_index(0), is_exceeded_max_records(false) {}

void CommandsHistory::printHistory() {
    int low = 0;
    if(is_exceeded_max_records) {
        low = (command_index + 1) % HISTORY_MAX_RECORDS;
    }
    for (int i = 0; i < HISTORY_MAX_RECORDS; ++i) {
        int current = (low+i) % HISTORY_MAX_RECORDS;
        CommandHistoryEntry *entry = history_log[current];
        if(entry) {
            std::cout << std::right << std::setw(5) << entry->getSequenceNumber()
                      << "  " << entry->getCmdLine() << std::endl;
        }
    }
}

void CommandsHistory::addRecord(const char *cmd_line) {
    command_sequence++;
    // group same records
    if(command_sequence > 1) {// not first
        CommandHistoryEntry *previous_record = history_log[command_index];
        if(!strcmp(previous_record->getCmdLine(), cmd_line)) {
            previous_record->increaseSequenceNumber();
            return;
        }
        command_index = (command_index + 1) % HISTORY_MAX_RECORDS;
    }
    CommandHistoryEntry *record = new CommandHistoryEntry(command_sequence,
            cmd_line);
    CommandHistoryEntry * previous_entry =history_log[command_index];
    if(previous_entry) {
        delete previous_entry;
    }
    history_log[command_index] = record;
    if(!is_exceeded_max_records &&
            (command_sequence > 1 && command_index == 0)) {
        is_exceeded_max_records = true;
    }
}

CommandsHistory::~CommandsHistory() {
    for(CommandHistoryEntry* entry : history_log) {
        delete entry;
    }
}
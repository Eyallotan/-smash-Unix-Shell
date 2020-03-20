//
// Created by eyall on 11/21/2019.
//

#include <cstdio>
#include <unistd.h>
#include "Commands.h"
#include <iostream>
#include <string.h>
#include <fcntl.h>
#define SYSCALL_ERROR -1
#define BUFFER_SIZE 1024

//You can assume that cp command will be tested with only two arguments.
CopyCommand::CopyCommand(const char *cmd_line) : BuiltInCommand(cmd_line) {
    old_path = args[1];
    new_path = args[2];
}

void CopyCommand::execute() {
    char buffer[BUFFER_SIZE];

    int old_fd = open(old_path, O_RDONLY);
    if (SYSCALL_ERROR == old_fd) {
        perror("smash error: open failed");
        return;
    }
    int new_fd = open(new_path, O_WRONLY| O_CREAT | O_TRUNC, 0666);
    if (SYSCALL_ERROR == new_fd) {
        perror("smash error: open failed");
        if(close(old_fd) <0) {
            perror("smash error: close failed");
        }
        return;
    }
    ssize_t read_count;
    ssize_t write_count;
    bool hasErrorOccured = false;
    do {
        read_count = read(old_fd, buffer, sizeof(buffer));
        if(SYSCALL_ERROR == read_count) {
            perror("smash error: read failed");
            hasErrorOccured = true;
            break;
        }
        write_count = write(new_fd, buffer, read_count);
        if(SYSCALL_ERROR == write_count) {
            perror("smash error: write failed");
            hasErrorOccured = true;
            break;
        }
    }
    while (read_count != 0);

    if (close(old_fd) < 0) {
        hasErrorOccured = true;
        perror("smash error: close failed");
    }
    if (close(new_fd) < 0) {
        hasErrorOccured = true;
        perror("smash error: close failed");
    }
    if(!hasErrorOccured) {
        std::cout<<"smash: "<<old_path<<" was copied to "<<new_path<<std::endl;
    }
}
//
// Created by xiaolu on 2022/10/26.
//

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <binder/Parcel.h>
#include <utils/String16.h>
#include <utils/Log.h>
#include <cutils/properties.h>
#include <BinderData.h>
#include <time.h>

namespace hssl {

#define PROPERTY_BINDER_WATCHED_UID "sandbox.binder.watched.uid"
#define INVALID_UID_STRING "-1"
#define MAX_CMDLINE_SIZE 256

class AppSandbox {
public:
static void getProcessCmdline(int pid, char *cmdline, int max_cmdline_size) {
    char cmdline_proc[30];
    memset(cmdline_proc, 0, 30);
    sprintf(cmdline_proc, "/proc/%d/cmdline", pid);
    int proc_fd = open(cmdline_proc, O_RDONLY | O_CLOEXEC);
    if (proc_fd > 0){
        if (read(proc_fd, cmdline, max_cmdline_size) < 0) {
            cmdline[0] = '\0';
        }
    }
    close(proc_fd);
}

static bool filterCaller(int uid, int pid) {
    if (pid == getpid()) {
        return false;
    }
    char target_uid_str[PROPERTY_VALUE_MAX];
    property_get(PROPERTY_BINDER_WATCHED_UID, target_uid_str, INVALID_UID_STRING);
    int target_uid = strtol(target_uid_str, nullptr, 0);
    return target_uid == uid;
}

static bool filterReceiver(int uid, int pid) {
    if (uid == 0) {
        return false;
    }
    char cmdline[MAX_CMDLINE_SIZE];
    AppSandbox::getProcessCmdline(pid, cmdline, MAX_CMDLINE_SIZE);

    char receiver_blacklist[][MAX_CMDLINE_SIZE] = {
        "surfaceflinger"
    };
    for (int i = 0; i < 1; ++i) {
        if (strcmp(cmdline, receiver_blacklist[i]) == 0) {
            return false;
        }
    }
    return true;
}

static int sendThroughSocket(uint32_t code, const android::Parcel &data, android::Parcel *reply, int callingUid, int callingPid, int operation){
    hssl::BinderData SandboxData = hssl::BinderData();
    SandboxData.uid = callingUid;
    SandboxData.pid = callingPid;
    
    SandboxData.operation = operation;
    SandboxData.code = code;

    // we need to record timestamp of the binder request
    struct timeval tv;
    gettimeofday(&tv, NULL);
    uint64_t timestamp = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    SandboxData.timestamp = timestamp;

    const uint8_t* DATA;

    switch(operation) {
        case hssl::BinderData::BC_TRANSACTION:
        case hssl::BinderData::BR_TRANSACTION:
            SandboxData.dataLen = data.dataSize();
            DATA = data.data();
            SandboxData.data = new uint8_t[SandboxData.dataLen];
            if (SandboxData.data == nullptr) {
                return -1;
            }
            memset(SandboxData.data, 0, SandboxData.dataLen);
            memcpy(SandboxData.data, DATA, SandboxData.dataLen);
            break;
        case hssl::BinderData::BR_REPLY:
            if (reply) {
                SandboxData.dataLen = reply->dataSize();
                DATA = reply->data();
                SandboxData.data = new uint8_t[SandboxData.dataLen];
                if (SandboxData.data == nullptr) {
                    return -1;
                }
                memset(SandboxData.data, 0, SandboxData.dataLen);
                memcpy(SandboxData.data, DATA, SandboxData.dataLen);
            }
            break;
        default:
            return -1;
    }

    DATA = nullptr;

    struct sockaddr_un server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sun_family = AF_UNIX;
    strcpy(server_address.sun_path, "#AppSandbox_Binder");
    server_address.sun_path[0] = 0;
    
    int sockfd;
    if((sockfd = socket(AF_UNIX, SOCK_STREAM|SOCK_CLOEXEC, 0)) < 0) {
        delete[] SandboxData.data;
        close(sockfd);
        return -1;
    }
    int bytes_sent = 0;
    if (connect(sockfd, (const struct sockaddr *)&server_address,
        offsetof(struct sockaddr_un, sun_path) + 1 + strlen("AppSandbox_Binder")) == 0 ){
        bytes_sent = SandboxData.writeToFd(sockfd);
    }
    delete[] SandboxData.data;
    close(sockfd);
    return bytes_sent;
}

};
}

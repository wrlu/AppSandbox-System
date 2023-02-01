//
// Created by xiaolu on 2022/7/28.
//

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <utils/Log.h>

namespace hssl {

#define MAX_BINDER_DATA_HEADER_LEN 1024
#ifdef __aarch64__
#define F_UINT64 "%lu"
#endif

#ifdef __x86_64__
#define F_UINT64 "%lu"
#endif

#ifdef __arm__
#define F_UINT64 "%llu"
#endif

#ifdef __i386__
#define F_UINT64 "%llu"
#endif

class BinderData {
public:
    int uid;
    int pid;
    int operation;
    uint32_t code;
    uint8_t *data;
    size_t dataLen;
    uint64_t timestamp;

    const static int BC_TRANSACTION = 0;
    const static int BR_TRANSACTION = 1;
    const static int BC_REPLY = 2;
    const static int BR_REPLY = 3;

    BinderData(): uid(-1), pid(-1), operation(-1), code(0), data(nullptr), dataLen(0), timestamp(0) {}

    int writeToFd(int socket_fd_local) {
        if (socket_fd_local < 0) {
            return -1;
        }
        char headerStr[MAX_BINDER_DATA_HEADER_LEN];
        char formatStr[20];
        snprintf(formatStr, 20, "%%d\n%%d\n%%u\n%s\n%%zu\n", F_UINT64);
        snprintf(headerStr, MAX_BINDER_DATA_HEADER_LEN, formatStr, uid, pid, code, timestamp, dataLen);

        int bytes_sent = send(socket_fd_local, headerStr, strlen(headerStr), 0);
        if (bytes_sent < 0) {
            return -1;
        }
        int data_bytes_sent = 0;
        if (data != nullptr && dataLen != 0) {
            data_bytes_sent = send(socket_fd_local, data, dataLen, 0);
            if (data_bytes_sent < 0) {
                return -1;
            }
        }
        return bytes_sent + data_bytes_sent;
    }
};

}

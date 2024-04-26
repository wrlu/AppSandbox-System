//
// Created by xiaolu on 2022/9/20.
//

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <utils/Log.h>

#define IN_MEMORY_DEX_FILENAME "[anonymous]"

#define FS_DATA_PATH "/data"
#define FS_STORAGE_PATH "/storage"
#define FS_SDCARD_PATH "/sdcard"
#define FS_DATA_APP_PATH "/data/app"

#define MAX_HEADER_LEN 500
#define FILE_RW_BUF_SIZE 10240

namespace appsandbox {

class DexFileData {
public:
    int uid;
    int pid;
    long timestamp;
    const char *sourceName;

    DexFileData(): uid(-1), pid(-1), timestamp(-1), sourceName(nullptr) {}
    DexFileData(int uid, int pid, time_t timestamp, const char *sourceName):
        uid(uid), pid(pid), timestamp(timestamp), sourceName(sourceName) {}

    int writeBasicInfo(int fd) {
        if (fd < 0) return -1;

        char headerStr[MAX_HEADER_LEN];
        snprintf(headerStr, MAX_HEADER_LEN, "%d\n%d\n%ld\n%s\n", uid, pid, timestamp, sourceName);

        int bytes_sent = send(fd, headerStr, strlen(headerStr), MSG_WAITALL);
        int err = 0;
        if (bytes_sent < 0) {
            err = -1;
        }        
        return err;
    }

    int writeDexFile(int fd) {
        if (fd < 0) return -1;
        if (sourceName == nullptr) return -1;

        FILE *sourceFd = fopen(sourceName, "r");
        if (sourceFd == nullptr) {
            return -1;
        }
        uint8_t *buffer = new uint8_t[FILE_RW_BUF_SIZE];

        int err = 0;
        while (feof(sourceFd) != 1) {
            int c = fread(buffer, sizeof(char), sizeof(buffer) - 1, sourceFd);
            int bytes_sent = send(fd, buffer, c, MSG_WAITALL);
            if (bytes_sent != c) {
                err = -1;
                break;
            }
        }
        delete[] buffer;
        fclose(sourceFd);
        return err;
    }
};

bool strstart(const char *str, const char *prefix) {
    return strstr(str, prefix) == str;
}

bool isHotfixDexPath(const char *sourceName) {
    if (sourceName == nullptr) return false;

    // Dex file in read-only partition should not be a hot patch.
    if (!( strstart(sourceName, FS_DATA_PATH) 
        || strstart(sourceName, FS_STORAGE_PATH)
        || strstart(sourceName, FS_SDCARD_PATH) )) {
        return false;
    }
    // Dex file in system default apk path should not be a hot patch.
    // TODO: On some Android system apk can be installed to sdcard.
    if (strstart(sourceName, FS_DATA_APP_PATH)) {
        return false;
    }
    // Otherwise should be a hot patch (like /data/user/0, /data/user_de/0 or /storage/emulated/0).
    return true;
}

int connectDexHookService() {
    struct sockaddr_un server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sun_family = AF_UNIX;
    strcpy(server_address.sun_path, "#AppSandbox_Dex");
    server_address.sun_path[0] = 0;

    int udsFd = -1;
    if((udsFd = socket(AF_UNIX, SOCK_STREAM|SOCK_CLOEXEC, 0)) >= 0) {
        if (connect(udsFd, (const struct sockaddr *)&server_address,
            offsetof(struct sockaddr_un, sun_path) + 1 + strlen("AppSandbox_Dex")) == 0 ) {
            return udsFd;
        }
        close(udsFd);
    }
    return -1;
}

int checkWatchedUid(int uid) {
    if (uid < 10000) return -1;

    return 0;
}

int writeDex(const char *sourceName){
    if (sourceName == nullptr)  return -1;

    int selfUid = getuid();

    if (checkWatchedUid(selfUid) != 0) return -1;

    if (isHotfixDexPath(sourceName)) {
        DexFileData dexFileData(selfUid, getpid(), time(nullptr), sourceName);
        int udsFd = connectDexHookService();
        int err = dexFileData.writeBasicInfo(udsFd);
        if (err != 0) {
            close(udsFd);
            return err;
        }
        err = dexFileData.writeDexFile(udsFd);
        close(udsFd);
        return err;
    }
    return -1;
}

int writeInMemoryDex(void *buffer, size_t length) {
    if (buffer == nullptr || length <= 0) return -1;
        
    int selfUid = getuid();
    if (checkWatchedUid(selfUid) != 0) return -1;

    DexFileData dexFileData(selfUid, getpid(), time(nullptr), IN_MEMORY_DEX_FILENAME);
    int udsFd = connectDexHookService();
    int err = dexFileData.writeBasicInfo(udsFd);
    if (err != 0) {
        close(udsFd);
        return err;
    }
    // Send memory dex buffer directly.
    size_t bytes_sent = send(udsFd, buffer, length, 0);
    if (bytes_sent != length) {
        err = -1;
    }
    close(udsFd);
    return err;
}

}

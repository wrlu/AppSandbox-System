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

#define MAX_DEX_FILE_HEADER_LEN 500

namespace appsandbox {

class DexFileData {
public:
    int uid;
    int pid;
    std::string packageName;
    long timestamp;
    const char *sourceName;

    DexFileData(): uid(-1), pid(-1), packageName(""), timestamp(-1), sourceName(nullptr) {}
    DexFileData(int uid, int pid, time_t timestamp, const char *sourceName):
        uid(uid), pid(pid), packageName(""), timestamp(timestamp), sourceName(sourceName) {}

    int writeBasicInfo(int fd) {
        if (fd < 0) {
            return -1;
        }
        char headerStr[MAX_DEX_FILE_HEADER_LEN];
        snprintf(headerStr, MAX_DEX_FILE_HEADER_LEN, "%d\n%d\n%s\n%ld\n%s\n", uid, pid, packageName.c_str(), timestamp, sourceName);

        int bytes_sent = send(fd, headerStr, strlen(headerStr), 0);
        if (bytes_sent < 0) {
            return -1;
        }        
        return bytes_sent;
    }

    int writeDexFile(int fd) {
        if (fd < 0) {
            return -1;
        }
        if (sourceName == nullptr) {
            return -1;
        }
        FILE *sourceFd = fopen(sourceName, "r");
        if (sourceFd == nullptr) {
            return -1;
        }
        uint8_t *buffer = new uint8_t[10240];

        while (feof(sourceFd) != 1) {
            int c = fread(buffer, 1, 1024, sourceFd);
            int bytes_sent = send(fd, buffer, c, 0);
            if (bytes_sent != c) {
                break;
            }
        }
        delete[] buffer;
        fclose(sourceFd);
        return 0;
    }
};

bool strstart(const char *str, const char *prefix) {
    return strstr(str, prefix) == str;
}

bool isHotfixDexPath(const char *sourceName) {
    if (sourceName == nullptr) {
        return false;
    }
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
    // Otherwise should be a hot patch (like /data/user/0, /data/data, /sdcard or /storage/emulated/0).
    return true;
}

int connectAppSandboxDexUDS() {
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

int writeDex(const char *sourceName){
    if (sourceName == nullptr || getuid() < 10000) {
        return -1;
    }
    if (isHotfixDexPath(sourceName)) {
        DexFileData dexFileData(getuid(), getpid(), time(nullptr), sourceName);
        int udsFd = connectAppSandboxDexUDS();
        dexFileData.writeBasicInfo(udsFd);
        dexFileData.writeDexFile(udsFd);
        close(udsFd);
    }
    return 0;
    
}

int writeInMemoryDex(void *buffer, size_t length) {
    if (buffer == nullptr || length == 0 || getuid() < 10000) {
        return -1;
    }
    DexFileData dexFileData(getuid(), getpid(), time(nullptr), IN_MEMORY_DEX_FILENAME);
    int udsFd = connectAppSandboxDexUDS();
    dexFileData.writeBasicInfo(udsFd);
    
    size_t bytes_sent = send(udsFd, buffer, length, 0);
    if (bytes_sent != length) {
        close(udsFd);
        return -1;
    }

    close(udsFd);
    return 0;
}

}

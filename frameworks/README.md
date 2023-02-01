Note: Since we modified the IPCThreadState.h file, we may have to run ```$ANDROID_BUILD_TOP/development/vndk/tools/header-checker/utils/create_reference_dumps.py  -l libbinder``` before compile

## 2022-10-26
目前代码还处于调试阶段，会有一些nasty code。
修改的代码全部位于 IPCThreadState.cpp 中，只需要替换这个文件(或者在该文件中搜索 puzzor starts/ends，并将之间的部分加入原有 IPCThreadState.cpp )即可。
主要修改逻辑：
在 executeCommand 的 BR_TRANSACTION 分支处（此时仍处于在server端的onTransact之前）解析出 Client（发送请求）的 uid，如果 uid 不在预设的黑名单（干净系统中从 data/system/packages.list 解析出所有默认应用的 uid, 一定注意 sender_uids_blacklist 变量要排好序）中，且 server 的进程不为 surfaceflinger， 则打印出当前 接受端(server) 的 pid, cmdline, uid; 发送者(Client)的 pid, cmdline, uid, parcel, transaction code。

### TODO 
1. 过滤条件可进一步完善，目前还会有一些请求的漏网之鱼没有被过滤掉，如果有必要可以进一步过滤。
2. 后续可以通过解析parcel中的数据来进一步分析行为。
3. 目前只是打印到logcat了，后面需要和原来的 sendThroughSocket 结合起来发送给前端app。
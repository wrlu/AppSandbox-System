# Android Framework with Sandbox 
## Modified project
* `art/runtime/native`: dex hook的修改
* `build/soong/scripts`: 为了能在libcore中使用`com.hssl.app.sandbox.*`的包而添加的白名单。
* `external/okhttp`: http hook的修改 @Xuyiwen
* `external/libvterm`: Lineage OS Terminal程序的依赖库。
* `frameworks/native/libs/binder`: binder hook的修改 @Shiji
* `notes`: 学习笔记，非代码
* `packages/apps/Terminal`: Lineage OS Terminal程序，使用shell uid运行。
* `packages/apps/Terminal_system`: Lineage OS Terminal程序，使用system uid运行。
* `system/core/run-as`: 修改过的run-as程序，会忽略debuggable标志来切换到目标应用的身份，方便在没有root的情况下获取应用私有目录的文件。
* `system/sepolicy`: 为了AppSandbox功能新增的SELinux规则
    * 允许应用和system_server写入AppSandbox的Unix Socket
    * 新增sandbox_prop标签用于保护Binder监控UID记录的Property
    * 允许特权应用和system_server读取AppSandbox的Property
    * 允许system_app写入AppSandbox的Property
    * 取消了只允许com.android.shell包名成为shell uid和shell标签的限制
    * 指定com.hssl.app.terminal为shell uid和shell标签

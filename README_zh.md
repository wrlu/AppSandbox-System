# AppSandbox-System
AOSP修改系统版本的App Sandbox，基于 AOSP android-12.1.0_r5 (SP2A.220505.002) 分支。

## Features & Modified projects
### Dex hook
* 修改了 `art/runtime/native`：在文件dex和内存dex加载的时候记录保存的文件。

### System tools
* Added `packages/apps/Terminal`：Lineage OS Terminal程序，使用shell权限运行。
* Added `external/libvterm`：Lineage OS Terminal程序的依赖库。
* 修改了 `system/core/run-as`：忽略debuggable标志来切换到目标应用的权限，可获取应用私有目录的文件。

### SELinux policies
* 修改了 `system/sepolicy`：新增和修改部分SELinux规则
    * 允许App和system_server写入AppSandbox的Unix Socket
    * 指定com.wrlus.app.terminal为shell uid和shell标签
    * 取消了只允许com.android.shell包名成为shell uid和shell标签的限制

### Build scripts
* 修改了 `build/soong/scripts/check_boot_jars`：新增libcore包名白名单`com.wrlus.app.sandbox.*`；
* 修改了 `build/target/product`：将Terminal和libvterm加入构建目标。

## FrontEnd
* AppSandbox-FrontEnd应用可以接收和保存系统发来的数据，详见: https://github.com/wrlu/AppSandbox-FrontEnd

## TODO List
* 内置Charles和Burp根证书到系统证书存储；
* 自定义平台签名密钥；
* 内置LSPosed；
* 内置frida-gadget。

## Credits
* 从[AppSandbox-Framework](https://github.com/wrlu/AppSandbox-Framework)项目fork出来，保留实用的功能移除了不稳定的功能。
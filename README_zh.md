# AppSandbox-System
AOSP修改系统版本的App Sandbox，基于 AOSP android-12.1.0_r5 (SP2A.220505.002) 分支。

## Features & Modified projects
### Dex hook
* 修改了 `art/runtime/native`：在文件dex和内存dex加载的时候记录保存的文件。

### System tools
* 添加了 `packages/apps/Terminal`：Lineage OS Terminal 程序，使用shell权限运行。
* 添加了 `external/libvterm`：Lineage OS Terminal 程序的依赖库。
* 修改了 `system/core/run-as`：忽略 debuggable 标志来切换到目标应用的权限，可获取应用私有目录的文件。
* 修改了 `system/ca-certificates`: 添加 charles (5f88ea7a.0) 和 burp (9a5ba575.0) 的根证书到系统。

### SELinux policies
* 修改了 `system/sepolicy`：新增和修改部分 SELinux 规则
    * 允许 App 和 system_server 写入 AppSandbox 的 Unix Socket
    * 指定 com.wrlus.app.terminal 为shell uid和 shell 标签
    * 取消了只允许 com.android.shell 包名成为 shell uid 和 shell 标签的限制
    * 新增 property 标签用于存储监视 uid，并允许 system_app 读写。

### Build scripts
* 修改了 `build/soong/scripts/check_boot_jars`：新增libcore包名白名单`com.wrlus.app.sandbox.*`；
* 修改了 `build/target/product`：将Terminal和libvterm加入构建目标。

## FrontEnd
* AppSandbox-FrontEnd 应用可以接收和保存系统发来的数据，详见: https://github.com/wrlu/AppSandbox-FrontEnd

## TODO List
* 自定义平台签名密钥；
* 内置 LSPosed；
* 内置 frida-gadget。

## Credits
* 从AppSandbox-Framework项目fork出来，保留实用的功能移除了不稳定的功能。
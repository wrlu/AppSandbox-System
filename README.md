# AppSandbox-System
The modified AOSP system edition of App Sandbox, based on AOSP android-12.1.0_r5 (SP2A.220505.002) branch.

## Features & Modified projects
### Dex hook
* Modified `art/runtime/native`: Record when dex files and in memory dex loaded。

### System tools
* Added `packages/apps/Terminal`: Lineage OS Terminal application, run as shell permission.
* Added `external/libvterm`: Dependency of Lineage OS Terminal application.
* Modified `system/core/run-as`: Switch target application's permission but ignore debuggable flags, can access private files of applications.

### SELinux policies
* Modified `system/sepolicy`：Modified and added serveral SELinux policies.
    * Allow app and system_server write unix socket to AppSandbox
    * Set shell uid and label for com.wrlus.app.terminal
    * Disable limitation of only allow com.android.shell package uses shell uid and label.

### Build scripts
* Modified `build/soong/scripts/check_boot_jars`：Add `com.wrlus.app.sandbox.*` to libcore package name allowlist.
* Modified `build/target/product`：Add Terminal and libvterm to build targets.

## FrontEnd
* AppSandbox-FrontEnd application can receive data from system and save, see also: https://github.com/wrlu/AppSandbox-FrontEnd

## TODO List
* Import Charles and Burp root certificates to system certificates storage.
* Custom platform key.
* Import LSPosed.
* Import frida-gadget.

## Credits
* Fork from [AppSandbox-Framework](https://github.com/wrlu/AppSandbox-Framework) project, keep useful features and delete unstable features.
## 沙箱构建相关 Paper

| Conference              | Title                                                        | Author                              | Institute                                                    | Intro/Abstract                                               |
| ----------------------- | ------------------------------------------------------------ | ----------------------------------- | ------------------------------------------------------------ | ------------------------------------------------------------ |
| Usenix SEC’15           | Boxify: Full-fledged App Sandboxing for Stock Android.       | Michael Backes                      | Saarland University and Max Planck Institute for Software Systems (MPI-SWS) | Usage the application virtualization and process-based privilege separation for isolation |
| ASIA CCS '15            | Hybrid User-level Sandboxing of Third-party Android Apps.    | Yajin Zhou, Kunal Patel             | NCSU, 360                                                    | 分析了其他方法的不足，提出的方法dex sandbox(API Hooking)+native sandbox(SFI-based),可能完全可以被frida代替。ClassObjects-> Sensitive Methods -> replace with a stub class + binary rewriter to implement software fault isolation like Native Client |
| NDSS'16                 | Going Native: Using a Large-Scale Analysis of Android Apps to Create a Practical Native-Code Sandboxing Policy | Vitor Afonso, Antonio Bianchi       | UCSB+ASU->Purdue (Shellphish)                                | Static filter: having a native method, having a native activity, having a call to an Exec method, having a call to a Load method, or having an ELF file inside the APK.<br />Dynamic Analysis:  including system calls, JNI calls, Binder transactions, calls to Exec methods, loading of third-party libraries, calls to native activities’ native callbacks, calls to native methods and monitor the amount of data exchanged between native and Java code<br />- libdvm->JNI call; <br />- libbinder->Binder transactions;<br />- libjavacore->Calls to Exec methods; <br />- libandroid_runtime->to monitor the use of native callbacks in native activities |
| EdgeSys'18              | Duvel: Enabling Context-driven, Multi-profile Apps on Android through Storage Sandboxing. | Sharath Chandrashekhara, Taeyeon Ki | The State University of New York                             |                                                              |
| CCS'21                  | Towards Transparent and Stealthy Android OS Sandboxing via Customizable Container-Based Virtualization | Wenna Song, Jiang Ming              | Wuhan University                                             |                                                              |
| DependSys-IoT21[JSA'21] | CamoDroid: An Android application analysis environment resilient against sandbox evasion |                                     |                                                              |                                                              |
|                         |                                                              |                                     |                                                              |                                                              |

感觉大多的隔离技术创新上和全面性上，Oracle不是很明确，而我们关注的是行为抓取。

## 分析技术和信息切入点[暂缓]

| Conference    | Title                                                        | Author                                                       | Institute                                                    |
| ------------- | ------------------------------------------------------------ | ------------------------------------------------------------ | ------------------------------------------------------------ |
| RAID‘19       | Exploring Syscall-Based Semantics Reconstruction of Android Applications | Dario Nisi, Antonio Bianchi                                  | EURECOM，ASU->Purdue (Shellphish)                            |
| Usenix SEC’22 | FOAP: Fine-Grained Open-World Android App Fingerprinting     | Jianfeng Li, Hao Zhou, Shuohan Wu, and Xiapu Luo             | The Hong Kong Polytechnic University                         |
| RAID‘19       | Towards Large-Scale Hunting for Android Negative-Day Malware | Lun-Pin Yuan, Wenjun Hu                                      | Penn State University, Palo Alto Networks Inc.               |
| Usenix SEC’18 | FlowCog: Context-aware Semantics Extraction and Analysis of Information Flow Leaks in Android Apps | Xiang Pan, Yinzhi Cao                                        | Google Inc.; Northwestern University; The Johns Hopkins University |
| UbiComp '16   | Discovering different kinds of smartphone users through their application usage behaviors | Sha Zhao, Julian Ramos                                       | Zhejiang University                                          |
| SIGKDD '17    | HinDroid: An Intelligent Android Malware Detection System Based on Structured Heterogeneous Information Network [Best Paper] | Shifu Hou, Yanfang Ye                                        | -                                                            |
| CCS'15        | GUITAR: Piecing Together Android App GUIs from Memory Images [Best Paper] | Brendan Saltaformaggio, Rohit Bhatia                         | Purdue University                                            |
| ASIA CCS '14  | Evading android runtime analysis via sandbox detection       | Timothy Vidas, Nicolas Christin                              | CMU                                                          |
| PLDI '14      | FlowDroid: precise context, flow, field, object-sensitive and lifecycle-aware taint analysis for Android apps | Steven Arzt, Siegfried Rasthofer, Christian Fritz, Eric Bodden | Technische Universität Darmstadt                             |
| CCS' 14       | Amandroid: A Precise and General Inter-component Data Flow Analysis Framework for Security Vetting of Android Apps | Fengguo Wei, Sankardas Roy                                   | -                                                            |
| TOCS          | TaintDroid: An Information-Flow Tracking System for Realtime Privacy Monitoring on Smartphones | WILLIAM ENCK                                                 | North Carolina State University                              |
| Usenix SEC’12 | DroidScope: Seamlessly Reconstructing the OS and Dalvik Semantic Views for Dynamic Android Malware Analysis | Lok Kwong Yan, Heng Yin                                      | Syracuse University                                          |



## Survey Paper

| Conference    | Title                                                        | Author                                   | Institute                      |
| ------------- | ------------------------------------------------------------ | ---------------------------------------- | ------------------------------ |
| Usenix SEC’21 | A Large-scale Temporal Measurement of Android Malicious Apps: Persistence, Migration, and Lessons Learned | Yun Shen and Pierre-Antoine Vervier      | Norton Research Group          |
| NDSS'17       | Hey, You, Get Off of My Market: Detecting Malicious Apps in Official and Alternative Android Markets | Yajin Zhou                               | NCSU                           |
| Usenix SEC’19 | HideMyApp: Hiding the Presence of Sensitive Apps on Android  | Anh Pham, Italo Dacosta                  | ABB, EPFL                      |
| RAID‘19       | Analysis of Location Data Leakage in the Internet Traffic of Android-based Mobile Devices | Nir Sivan, Ron Bitton, and Asaf Shabtai, | -                              |
| Usenix SEC’19 | 50 Ways to Leak Your Data: An Exploration of Apps' Circumvention of the Android Permissions System | Joel Reardon                             | -                              |
| ISSTA'18      | Analyzing the Analyzers: FlowDroid/IccTA, AmanDroid, and DroidSafe | Lina Qiu,                                | University of British Columbia |

## Open-Source APP Analysis Tools

- https://github.com/ashishb/android-security-awesome
- Runtime Mobile Security (RMS) or House —— Frida with Customized Scripts plus Template Engine
  - Hook LIB + Hook Filter
  - Trace Methods Args
  - Called Stack Backtrace
  - Stetho
- **Objection**
  - https://github.com/sensepost/objection

- MobSF
  - https://github.com/MobSF/Mobile-Security-Framework-MobSF
- Proxy Network: ProxyDroid
- Sandbox, but for malware.
  - CuckooDroid - Automated Android Malware Analysis with Cuckoo Sandbox. Ref: https://github.com/idanr1986/cuckoo-droid
  - Droidbox - Dynamic analysis of Android apps. Ref: https://github.com/pjlantz/droidbox
- DECAF or DroidScope
  - https://github.com/decaf-project/Droidscope
- Scanning Secrets, but need customized patterns
  - Apkleaks: https://github.com/dwisiswant0/apkleaks
- Android Malware (Analysis | Scoring) System
  - Quark Engine: https://github.com/quark-engine/quark-engine
- Androwarn
  - Useful Features
  - https://github.com/maaaaz/androwarn/

## Key CheckList For Security

- OWASP For Android Security: https://github.com/muellerberndt/android_app_security_checklist
- Have Some Code Patterns for Malicious Behaviors. Ref: https://quark-engine.github.io/ruleviewer/



## Ideas

- Timely: real-time analysis
- Patterns need be generated from samples and existing tools. -> Write Parsers.
  - Dimension: 
    - Components(e.g. receiver) + intent in manifest.
    - Essential Permissions.
      - Invariants: 突然意识到权限这个维度很好，因为不管具体的代码如何实现。要完成一定的功能，就必须申请这个权限，从某种程度上说，这个是个invariants
      - 但是和malware不一样，unfriendly app在这方面的特点性可能不明显。
    - Code snippets.
      - 例如，样本使用**DexClassLoader**加载从远程下载的jar或dex代码（但大部分是第三方广告，用白名单过滤掉）；样本在**非默认路径lib**/armeabi加载执行本地native code（常用于执行提权代码）
      - 对动态加载的jar和dex，记录其API调用及参数，例如sendTextMessage及其参数；对动态加载的native code，记录其发生的敏感系统调用，如sys_mount
      - 例如向高额扣费号码发送短信；将短信内容当成控制指令等。
    - BlackList of third-party APK.
      - e.g. xiaoniu.keepalive
      - package tree structural layouts with static features? Call Graphs?
    - API Call Sequence.
    - Native Code - syscall and args for priviledges...
  - Ref: DroidRanger
- Large-Scale manual application analysis -> New fresh Rules with robustness and accuracy
  - 这个是日常去做，集中分析可能也抓不到重点。应该先上沙箱，就会发现"在野"的很多新行为，再压缩策略，便于更多的分析。
- Heuristic Features -> Throughout
- First monitor, then aggregate and restrict finally. 
- Make it a loop
  - 建立自己的Malicious App Database。
  - After confirmation, repeat the analysis and extract features/fingerprints for re-usage.
- Incremental Scanning and feature-based filter-out Reduction
  - 分析时间是个问题吗? 如果分析一个应用需要30s，那么1h上线或更新的app要小于120个。根据cuckoo sandbox的经验，PC malware的速度大概是3-6分钟。多机器的话可以不考虑这个。
  - 不要全部app都加载分析，先通过静态确定范围，缩小之后的部分再进行动态分析或长期监视。



## Others

- Static Analysis：https://tthtlc.wordpress.com/2011/09/01/static-analysis-of-android-applications/
- santoku-linux：https://santoku-linux.com/
- More Papers: https://github.com/Reza-saeedi/Malware-Detection-Tools/blob/master/AndroidLiteratureReview.md

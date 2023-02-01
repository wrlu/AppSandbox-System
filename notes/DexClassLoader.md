## 一、代码分析

### Dex加载方法
有两种热更新的接口，一种是in-memory，另外一种是基于文件的（in-disk）:

- **DexClassLoader/PathClassLoader**
	- 根据目前的理解发现，这两个路口本来会因为optimizedDirectory是否指定产生不同的路径，一个会把dex2oat优化后的.odex文件记录到用户指定的optimizedDirectory中，另外一个会存放到默认的/data/dalvik-cache中。其他逻辑一样。但是在Android 8.0之后，PathClassLoader 和 DexClassLoader 其实已经没有区别了。因为optimizedDirectory参数已经被弃用了，默认都是传入null。而最终是采用在dex文件同级目录下添加一个 oat/ 文件作为 .odex 的存储目录。 
- **InMemoryDexClassLoader**
	- Android 8.0之后开始引入
		

### 记录插桩点
- DexClassLoader/PathClassLoader：
	- 构造函数处记录基本信息
	- DexPathList中makeDexElements进行记录【具体文件位置】
- InMemoryDexClassLoader:
	- 构造函数处记录基本信息
	- 在DexFile的openInMemoryDexFiles中记录具体内容
- loadClass名称
	- 由于第一次装载一定会走findClass这个方法，最终会进入DexPathList的findClass中【在其中记录目前loadClass的方法是来自于哪个dex/jar/apk文件中】
	- 具体所invoke的方法，是通过反射实现的，记录的话估计会爆炸。因为太多地方用到反射了
	
### 记录信息
1. 基本信息: pid, uid, packagename/process name
2. in-memory标记
3. DexFile的预读取【原始路径，记录路径】
4. loadClass信息


## 二、Hook开发

### 思路（已抛用）
- 在完成new DexClassLoader的时候，记录基本信息和原始路径集合originDexPaths以及目标路集合saveDexPaths
- 通过HashMap根据in-memory或in-disk的模式，设置origin2SaveDexPath和idx2SaveDexPath的映射关系
- 在进行loadClass的时候，读取class name，在DexListPath的findClass中cache第一次加载类名所属的dexfile，即originDexPath。并将其映射关系记录在loadClassInDexPath或loadClassInIdx中【因为只有第一次加载会去寻找，所以cache关系】
- 根据timestamp，记录每一次loadClass时，设置的class name和所属dexfile的信息到数据库中【格式：基本信息， timestamp，clas name， originDexPath，saveDexPath（通过之前的hashmap找到存储路径）】


```
    // basic information
    private long id;
    private int uid;
    private int pid;
    private String packageName;
    private long timestamp;

    private boolean inMemory;
    //in-disk: origin file to save file
    //in-memory: index of bytebuffer to save file
    private HashMap<String, String> origin2SaveDexPath;
    private HashMap<Integer, String> idx2SaveDexPath;
    //private List<String> originDexPaths;
    //private List<String> saveDexPaths;

    private String loadClass;
    private String saveDexPath;
    private String originDexPath;

    private HashMap<String, String> loadClassInDexPath;
    private HashMap<String, Integer> loadClassInIdx;

```

### 新思路
由于loadClass在不带热更新的正常apk中也会使用，所以会使得记录爆炸，因此和小路讨论完的建议是不管loadClass的classname记录了，等到要分析的时候发现有热更新接口需要逆，辅助人工进行分析就好（此时也是已解密的dexfile）。
- 记录new DexClassLoader，filter `/data/app/` 路径下的dexfile加载
- 其余按照基本信息，timestamp，originpaths进行记录
- 通过socket把originpath传递给appSandbox，由其进行存储。


### 具体实现

【Doing】

- 由于最终选择在DexFile中插桩，导致librarySearchPath无法记录
- pkg记录的问题，还未确定是否在框架内反射来解决？还是传输数据之后在AppSandbox中获得


## 三、Reference
1. [谈谈 Android 中的 PathClassLoader 和 DexClassLoader](https://juejin.cn/post/6844903929562529800)
2. [Android插件化探索](https://www.maplejaw.com/2016/05/24/Android%E6%8F%92%E4%BB%B6%E5%8C%96%E6%8E%A2%E7%B4%A2%EF%BC%88%E4%B8%80%EF%BC%89%E7%B1%BB%E5%8A%A0%E8%BD%BD%E5%99%A8DexClassLoader/)
3. [Android Apk 加固之Dex文件——InMemoryDexClassLoader](https://blog.csdn.net/q610098308/article/details/105246355)
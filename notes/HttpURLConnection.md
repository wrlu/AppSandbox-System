### Android Traffic Hooking
- HTTP打桩位置	【request】
	- aosp\external\okhttp\repackaged\okhttp-urlconnection\src\main\java\com\android\okhttp\internal\huc\HttpURLConnectionImpl.java [newHttpEngine]
		- method, protocol, headers [execute中的sendRequest()、readResponse()]
	- aosp\external\okhttp\repackaged\okhttp\src\main\java\com\android\okhttp\internal\http\HttpEngine.java
		- more headers
	
- HttpURLConnection（包括https和http）目标打桩位置
	- httpStream中的writeRequestHeaders，writeRequestBody
	- 修改桩点为https://cs.android.com/android/platform/superproject/+/master:external/okhttp/okhttp/src/main/java/com/squareup/okhttp/internal/http/HttpEngine.java;l=595
		
- HTTP打桩位置	【Response】		
	- 用户调用getResponseCode-> getInputStream -> getResponse()
	- 读取body最终追溯到RealBufferedSource.java中的read函数【聚合了多种Source类型, 根据Content-Length或Chunked要求不同，会有3种接口】
	- 但是用户主动调用read之后才有数据，需要一次读取完？需要修改大量接口
		
### Notes
1. 对整个请求包的封装：external/okhttp/repackaged/okhttp/src/main/java/com/android/okhttp/Request.java
2. 真的调用接口请求os协议栈的位置：external/okhttp/okhttp/src/main/java/com/squareup/okhttp/internal/http/Http1xStream.java
3. 有一个嵌套source：
	- 首先在用户readResponseCode->readResponse->readNetworkResponse的时候会调用openResponseBody根据headers建立不同种类的source，即为source_A【FixedLengthSource和ChunkedSource】
	- 通过Okio.buffer(source_A)将这个source_A作为RealBufferedSource的source成员
	在用户getInputStream的时候，最终进入到RealBufferedSource.inputStream()建立InputStream，里面有两个read的方法，每个read中会调用source_A中具体实现的read进行读取。
	= 因此最终建议在@Override public InputStream inputStream()中的read（里面会调用具体source的read）进行hook就可以了。【或者实在要在多种source的read中插装也可以，但是有可能还要考虑在Http2xStreeam中的source类型，如FramedSource，具体分配stream的逻辑在newStream中】
		- Does all the work necessary to build a full HTTP or HTTPS connection on a raw socket.


### Question
1. 记录的id如何设置下一条，要么干脆不要用户设置，service自增，要么就设置为ts【目前使用的是后一种，因此id的值要从int变为long】
2. 获取pkg ： https://groups.google.com/g/android-platform/c/mDOseC6X4fw?pli=1
3. Method的接口感觉使用String记录到数据库更加直观，需要使用String，目前是int
4. 请求包中的statusCode目前设置为-1，response中的method设置为-1【缺少宏，或者设置为'RESPONSE'】
5. 写入body的逻辑有点麻烦。要看看是否有body需要写【writeRecord的逻辑不要写在writeRequestHeaders，writeRequestBody中？】
6. Service的名称记录为Context.DATA_RECORD_SERVICE 【Context.DATA_RECORD_SERVICE】
7. 设置一个SetMessage的接口，response在其中的字段是分离的
8. 设置一个SetProtocol接口是否有必要
9. 是否在数据库存储的时候考虑byte [] body是空或null的情况？
10. UnknownLengthSource目前未知可能的情况【也许Transfer-Encoding: gzip包含在内】，未实现
11. request package中也有类似于source的差别，在请求阶段是称为sink。这部分还没有实现，需要的话可以在ChunkedSink， FixedLengthSink等位置追加点位
12. 之前的目标是给HttpURLConnection插桩，其实okHttp框架还有其他发送http/https traffic的方法，并没有插桩，待完善。


### Plan
- < 100k: 全局变量 【bytes[], size】—— close - writerecord
- \> 100k && chunked\[100m\] : 共享内存【mem file】-- close -- writerecord fd 【fd由service回收】
- 已改成localSocket
 


### WorkFlow

详细工作流可见img/HttpUrlConnection_Workflow.jpg

```
 * A socket connection that can be used to send HTTP/1.1 messages. This class
 * strictly enforces the following lifecycle:
 * <ol>
 *   <li>{@link #writeRequest Send request headers}.
 *  *   <li>Open a sink to write the request body. Either {@link
 *  *       #newFixedLengthSink fixed-length} or {@link #newChunkedSink chunked}.
 *  *   <li>Write to and then close that sink.
 *  *   <li>{@link #readResponse Read response headers}.
 *  *   <li>Open a source to read the response body. Either {@link
 *  *       #newFixedLengthSource fixed-length}, {@link #newChunkedSource chunked}
 *  *       or {@link #newUnknownLengthSource unknown length}.
 *   <li>Read from and close that source.
 * </ol>
 ```

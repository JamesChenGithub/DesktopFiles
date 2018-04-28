# 背景
1. 根据外部客户反馈AVSDK的问题，实验室将AVSDK重构，重构的版本XCast只提供了C/C++接口（与普通API方式差别很大），摒弃业务逻辑，只专注于音视频逻辑；
2. XCast与AVSDK接口完全不一致，要兼容现有ILive，我们需要提供一个中间层，将xcast转成类似于AVSDK的API形式，以便后期能较好地兼容ILive;
3. VIPKID作为第一个用XCast的用户，目前根据其现有业务逻辑，封装了其会用到的接口，后续会慢慢补全；

# 主流程
参考 ： <a href = "https://github.com/JamesChenGithub/ilvb_doc/blob/master/doc/XCast/xcast_flow.md#xcast%E8%BF%90%E8%A1%8C%E6%B5%81%E7%A8%8B"> XCast流程图 </a>

# XCast与AVSDK的不同之处
## 概念
1. XCast不依赖于IMSDK，内部使用的是tinyid， AVSDK主流用法是依赖IMSDK的帐号，内部作identifier与tinyid转换逻辑；
2. 鉴权方式的改变；
2. spear配置目前没有，需要业务去下载spear配置；
3. XCast里面使用的是流，AVSDK里面用的是Room;
2. XCast上行信息称为Track（video track, audio track），AVSDK里面没有类似的强聚合的概念（由endpoint , videoctrl, audioctrl间接完成）
3. 上下行叫法的改变；预览跟上行在接口上完全分开；
4. 存在一个明显的默认设备逻辑，AVSDK里面不明显；


## 接口
1. XCast使用KVC方式调用，不及AVSDK易读；
2. XCast接口少，但是需要读懂的协议多；
3. XCast主要是同步的，AVSDK是异步的；
4. XCast的错误码与AVSDK完全不一致；
5. XCast可以看成是无状态的，AVSDK里面还保留了基本的状态；
6. 较多回调的流程不再一样了；
7. 没有渲染控件；

# 接口说明

## xcast接口说明 
1. <a href="https://github.com/JamesChenGithub/XCastDemo/blob/master/xcast-demo/xcast.h">xcast头文件展示</a> 
1. <a href="https://github.com/JamesChenGithub/XCastDemo/blob/master/xcast-demo/include/xcast_const.h">xcast协议讲解</a> 

## 中间层接口说明

3. <a href="https://github.com/JamesChenGithub/XCastDemo/blob/master/xcast-demo/example/Live/XCastHelper.h">中间层展示</a>
4. 接口声明：提供同步以及异步方式返回；
2. 默认参数说明
4. <a href="https://github.com/JamesChenGithub/XCastDemo/blob/master/xcast-demo/example/Live/XCastData.h">回调说明</a>
5. 内部处理逻辑说明

# 现有实现中还没有对齐的地方
1. 自动拉画面逻辑；
2. 请求画面逻辑；

# （待补充）

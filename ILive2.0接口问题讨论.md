# ILive2.0以及XLive整体实现方式以及结构

# 结构上是否有问题

# 遇到的问题
1. 在各平台新封QAVContext作适配层是否合适 ？ 
2. startcontext时，需要tinyid，这块要调整登录接口的传参，或者该如何配合好业务自定义帐号工具？
2. ILiveSDK现有类没有明显的房间外/房间内区别，两种参景下能调用的api不一样，如何在结构上区分？？
3. QAVContext 不再暴露 videoctrl/spearenginectrl/audioctrl/room，外部之前的用户涉及到的操作切换到其他类进行调用，此处切换会有轻微改动;
4. 进房时，需要增加auth信息；
5. mac/pc 上没有设备测试模块一说了，这块逻辑调整会比较大，需要重新规划： 如进房时；
6. 缓存信息逻辑：endpoint事件缓存，videoframe缓存，以及清理缓存的时机？ 如何引导用户自定义；
7. xcast不再依赖主线程，对应ILiveSDK里面调用时是否需要作线程切换逻辑；
8. ILiveSDK在完全不依赖IMSDK情况下，是否需要再独立包装？或者如何把IM作成插件进行集成？
9. 有部分逻辑（如进房开摄像头）在底层xlive里面已经做了，ilive逻辑要重新review一下；


电话沟通结果：
1. alexi继续按现有方式提供基本能力接口，与适配层缺失的业务参数信息，郭老师届时封装时自己补齐；

2. alexi 确认热插拔回调中的 只返回了deviceName, 没有返回deviceID

3. 郭老师确认：1. onDeviceStateChanged/onAudioDeviceStateChanged/onVideoDeviceStateChanged/onDeviceCallBack 回调的区别；2.onCameraReady /onVideoStopped 回调的区别； 3. 接口异步返回方式对其现有逻辑是否有冲突；




class tencentRtcEngineObject : public IVideoFrameObserver
	, public IRtcEngineEventHandler
	, public enable_shared_from_this<tencentRtcEngineObject>
	{
	public:
		explicit tencentRtcEngineObject(SRV_LINE_TYPE eLineType);
		virtual ~tencentRtcEngineObject();
		
		static std::string getVersion();
        
        
		void EnterClassroom(const string& opt);
		void ExitClassroom();
        
        // 远端的用户的userid？uint64_t
		int MuteRemoteAudio(const std::string & config);
        
        // 只是开启停止本地的预览？远程的预览自动通过回调事件，由业务上层自己处理掉了？
		int  StartPreview();
		int  StopPreview();
        
        // config 具体是什么含义？userid
        // 只禁掉上行
		int  MuteLocalVideo(const string& config);
        
        // config 具体是什么含义？
        // 该函数功能是指关本地所有声音？是否包含处理mic的逻辑？
		int  MuteLocalAudio(const string& config);
		
//        // 本地预处理入口？
//        struct IVideoFrame final {
//            /**width of video frame */
//            int32_t width;
//            /**height of video frame */
//            int32_t height;
//            /**rotation of this frame (0, 90, 180, 270) */
//            int32_t rotation;
//            /**face count */
//            int32_t faceCount;        // 人脸识别，不用关心
//            /**data buffer */
//            std::vector<uint8_t> buffer;
//
//            IVideoFrame(int32_t width_,
//                        int32_t height_,
//                        int32_t rotation_,
//                        int32_t faceCount_,
//                        std::vector<uint8_t> buffer_)
//            : width(std::move(width_))
//            , height(std::move(height_))
//            , rotation(std::move(rotation_))
//            , faceCount(std::move(faceCount_))
//            , buffer(std::move(buffer_))
//            {}
//        };
        
//        struct IAudioFrame final {
//            /**number of samples in this frame */
//            int32_t samples;
//            /**volum */
//            int32_t volum;
//            /**data buffer */
//            std::vector<int8_t> buffer;
//
//            IAudioFrame(int32_t samples_,
//                        int32_t volum_,
//                        std::vector<int8_t> buffer_)
//            : samples(std::move(samples_))
//            , volum(std::move(volum_))
//            , buffer(std::move(buffer_))
//            {}
//        };
		virtual bool onCaptureVideoFrame(VideoFrame& videoFrame) override;
        
        // handle具体含义是什么？videoFrame是编码后上行的数据信息？
        handle / uid 都是userid
		virtual bool onCaptureVideoFrame(const char* handle,VideoFrame& videoFrame) override;
        
        // 收到远端视频信息？
		virtual bool onRenderVideoFrame(const unsigned int uid, VideoFrame& videoFrame) override;
        
        // 与上面的接口的区别是？？只是做不同的帐号体系转换？
        // VideoFrame 具体的结构提供下，需要核对下目前提供的是否全
#ifdef WIN32
		virtual bool onRenderVideoFrame(const unsigned char* uid,VideoFrame& videoFrame) override;
#else
		virtual bool onRenderVideoFrame(unsigned char* uid,VideoFrame& videoFrame) override;
#endif
		// 只是本身进房成功的回调？
        // channel : 房间号信息？
        // uid : uint64_t
        // elapsed : 消耗时间？
		virtual void onJoinChannelSuccess(const char* channel, uid_c  uid, int elapsed) override;
        
#ifndef WIN32
        // 这个只在PC有嘛？uid : uint64_t ,  elapsed 匆略
        // 按SDK 现有逻辑实现；
        // 只用来监听远端的用户的上下线情况？
        // 监课老师（不上行的用户）上下线了是否也要回调？
		virtual void onUserJoined(uid_c uid, int elapsed) override;
        
        enum USER_OFFLINE_REASON_TYPE
        {
            USER_OFFLINE_QUIT = 0,
            USER_OFFLINE_DROPPED = 1,
            USER_OFFLINE_BECOME_AUDIENCE = 2,  //下麦了
        };
		virtual void onUserOffline(uid_c uid, USER_OFFLINE_REASON_TYPE reason) override;
#endif
        
        // string deviceId, string deviceName  确认：
        deviceState
        
        // deviceId/deviceName 具体有什么区别？这边新sdk只有一个deviceName(系统分配的一个utf8唯一名称)
        // 设备名称
        // MEDIA_DEVICE_TYPE : 主要有哪些？
        // deviceState : 这边是枚举，stopped/running两种状态，对于你们是否足够？
		void onDeviceStateChanged(string deviceId, string deviceName, MEDIA_DEVICE_TYPE deviceType, string deviceState);
		
		 // 音视频设备热插拔监听？
		virtual void onAudioDeviceStateChanged(const char* deviceId, const char * deviceName, int deviceType, int deviceState) override;
		virtual void onVideoDeviceStateChanged(const char* deviceId, const char * deviceName, int deviceType, int deviceState) override;
        
        // 这个主要是什么的回调？统计信息的回调？
//        struct VipkidRtcStats
//        {
//            /* audio bit rate */
//            unsigned int rxAudioKBitRate;
//            unsigned int txAudioKBitRate;
//
//            /* video bps info */
//            VipkidMediaBaseInfo rxVideoRemoteKBitRate[MAX_DEC_NUM];         // video receive bps for remote endpoints                   <-- bpsReceive
//            unsigned int rxVideoKBitRateReceive;                        // total receive video bps for local endpoint
//            unsigned int txVideoKBitRateSend;                           // total send video bps for local endpoint                  <-- bpsSend
//
//            VipkidMediaBaseInfo decVideoRemoteKBitRate[MAX_DEC_NUM];        // video decode bps for remote endpoints                    <-- bpsDecode
//            unsigned int rxVideoKBitRateDecode;                         // total encoded video bps for local endpoint
//            unsigned int txVideoKBitRateEncode;                         // total encoded video bps for local endpoint               <-- bpsEncode
//
//            /* video fps info */
//            VipkidMediaBaseInfo rxVideoRemoteFpsRate[MAX_DEC_NUM];          // video receive (decoder) fps for remote endpoints         <-- fpsDecode
//            unsigned int rxVideoFpsRateDecode;                          // average video receive(decoder) fps for local endpoint
//            unsigned int txVideoFpsRateEncode;                          // video send(encoder) fps for local endpoint               <-- fpsEncode
//
//            VipkidMediaBaseInfo rendVideoRemoteFpsRate[MAX_DEC_NUM];        // video render fps for remote endpoints                    <-- fpsRender
//            unsigned int rxVideoFpsRateRender;                          // average video render fps
//            unsigned int txVideoFpsRateCapture;                         // video capture fps for local endpoint                     <-- fpsCapture
//
//            /* video scale info() */
//            VipkidMediaBaseInfo rxVideoRemoteHeightPixel[MAX_DEC_NUM];      // video decode height scale for remote endpoints           <-- heightDecode
//            VipkidMediaBaseInfo rxVideoRemoteWidthPixel[MAX_DEC_NUM];       // video decode width scale for remote endpoint             <-- widthDecode
//            unsigned int rxVideoHeightPixelDecode;                      // average video decode height scale for remote endpoints
//            unsigned int rxVideoWidthPixelDecode;                       // average video decode width scale for local endpoint
//            unsigned int txVideoHeightPixelEncode;                      // video encode height scale for remote endpoints           <-- heightEncode
//            unsigned int txVideoWidthPixelEncode;                       // video encode width scale for local endpoint              <-- widthEncode
//
//            VipkidMediaBaseInfo rxVideoLocalHeightPixel[MAX_DEC_NUM];       // video Render height scale for remote endpoints           <-- heightRender
//            VipkidMediaBaseInfo rxVideoLocalWidthPixel[MAX_DEC_NUM];        // video Render width scale for remote endpoint             <-- widthRender
//            unsigned int rxVideoHeightPixelRender;                      // average video Render height scale for remote endpoints
//            unsigned int rxVideoWidthPixelRender;                       // average video Render width scale for local endpoint
//
//            unsigned int txVideoHeightPixelCapture;                      // video Capture height scale for remote endpoints           <-- heightCapture
//            unsigned int txVideoWidthPixelCapture;                       // video Capture width scale for local endpoint              <-- widthCapture
//
//            //    uMediaBaseInfo rendVideoRemoteHeightPixel[MAX_DEC_NUM];   // video render scale for remote endpoints                  <-- heightRender
//            //    VKMediaBaseInfo rendVideoRemoteWidthPixel[MAX_DEC_NUM];   // video render scale for local endpoint                    <-- widthRender
//            //    unsigned int capVideoLocalHeightPixel;                    // video capture scale for remote endpoints                 <-- heightCapture
//            //    unsigned int capVideoLocalWidthPixel;                     // video capture scale for local endpoint                   <-- widthCapture
//
//            /* net state info */
//            VipkidMediaBaseInfo netPacketDownLostRate[MAX_DEC_NUM];         // loss rate received                                       <-- lossrate_receive
//            unsigned int rxNetPacketLostRate;                           // average loss rate received
//            unsigned int txNetPacketLostRate;                           // loss rate send                                           <-- lossrate_send
//
//            int local_rtt;            // kcp_rtt
//            int local_net_status_level;         // local net status level calc by kcp_rtt
//            VipkidMediaBaseInfo remote_rtt[MAX_DEC_NUM];
//
//            VipkidMediaBaseInfo rtt[MAX_DEC_NUM];                       // Round-Trip Time                                          <-- using rtt; means time taken from end-to-end
//            unsigned int  rttAvg;                                       // average Round-Trip Time
//            int rto;                                                    // Retransmission TimeOut                                   <-- using kcp rto;means time taken from end-to-server
//
//            /* others info */
//            // unsigned int duration;
//            //unsigned int users;
//            //double cpuAppUsage;
//            //double cpuTotalUsage;
//        };提供必需项
		virtual void onRtcStats(const tencentcore::rtc::VipkidRtcStats & stat) override;
        
        // PC/MAC上多设备如何回调？
		/**
		 * when the camera is ready to work, the function will be called
		 */
		virtual void onCameraReady() override;
        
        
		/**
		 * when all video stopped, the function will be called then you can repaint the video windows
		 */
        // 设置里面的预览，教室内预览；
		virtual void onVideoStopped() override;
        
        // handle 具体含义是？设备ID嘛？
		virtual void onCameraReady(const char* handle) override;
		virtual void onVideoStopped(const char* handle) override;
        
        // 本地退房回调？ stats有哪几类？
        // stats 业务统计信息，不用提供
		virtual void onLeaveChannel(const RtcStats& stats) override;
#if defined(DEVICE_CALLBACK)
         // 音视频设备热插拔监听？？？
		virtual void onDeviceCallBack(bool isvideo, unsigned int devid, unsigned int devtype, int cbid, int errcode) override;
#endif
        
		virtual void onError(int err, const char* msg) override;
        
        // API具体含义是？
        // config 对应参数的含义？
        config json对像；
		void onCameraCtrl(const string &config);
		void onMicCtrl(const string &config);
		
        // 具体的作用不是很了解，能否仔说明下？
        // 回调音频的音量信息，可忽略；
        virtual void onAudioVolumeIndication(const AudioVolumeInfo* speakers, unsigned int speakerNumber, int totalVolume) override;
		
        // 获取设备列表？ VideoDeviceInfo（摄像头？虚拟摄像头？屏幕分享？）都返回给到你们？
        // AudioDeviceInfo （麦克风？扬声器？）都返回给到你们？
        
//        typedef std::vector<AgoraRtcDeviceInfo> DevicesList;
//        struct AgoraRtcDeviceInfo{
//            AgoraRtcDeviceInfo() {
//                index = 0;
//                type = agora::rtc::UNKNOWN_AUDIO_DEVICE;
//                memset(deviceId, 0, LEN);
//                memset(deviceName, 0, LEN);
//            }
//            int index;
//            agora::rtc::MEDIA_DEVICE_TYPE type;
//            char deviceId[LEN];
//            char deviceName[LEN];
//        };
		void numerateDevices(std::vector<VipkRtcDeviceInfo>&VideoDeviceInfo,std::vector<VipkRtcDeviceInfo>&AudioDeviceInfo);
		
	};

尽量不要在主线程调用；


其他疑问：
1. 对于SDK回调的线程是否有要求？？还是不处理，由适配层自身去处理？
2. 对于同步异步的要求是怎么样的？
3.

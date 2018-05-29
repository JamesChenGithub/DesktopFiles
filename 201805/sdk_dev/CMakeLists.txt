PROJECT(xcast)

include_directories(../../../include
    ../../../xcast
    ../../../base
    ../../../core
    ../../../net
    ../../../plugins
    ../../../plugins/av_common
    ../../../third_party
    ../../../third_party/videocore/src/jni/libyuv/include)

set(xcast_dev_SOURCES MACOSX_BUNDLE
    main.m
    app_delegate.h
    app_delegate.m
    #view_controller.h
    #view_controller.mm
    main_menu.h
    main_menu.m
    LoginViewController.h
    LoginViewController.mm
    LoginViewController.xib
    MainMenu.xib
    Render/black.png
    Render/ConfUIDef.h
    Render/MAVBackView.h
    Render/MAVBackView.mm
    Render/MAVBasicVideoView.h
    Render/MAVBasicVideoView.mm
    Render/MAVBasicVideoView+EventProcessor.h
    Render/MAVBasicVideoView+EventProcessor.mm
    Render/MAVBasicVideoViewController.h
    Render/MAVBasicVideoViewController.mm
    Render/MAVCAOpenGLLayer.h
    Render/MAVCAOpenGLLayer.mm
    Render/MAVLayoutManager.h
    Render/MAVLayoutManager.mm
    Render/MAVQuadRender.h
    Render/MAVQuadRender.mm
    Render/MAVQuadRenderUtils.h
    Render/MAVQuadRenderUtils.mm
    Render/MAVRender.h
    Render/MAVRender.mm
    Render/MAVRenderUtils.h
    Render/MAVRenderUtils.mm
    Render/mavvideo_backgroud_big_old.png
    Render/mavvideo_backgroud_small.png
    Render/MAVVideoDataDispatcher.h
    Render/MAVVideoDataDispatcher.mm
    Render/Shader/mav_standard_fragment.sh
    Render/Shader/mav_standard_vertex.sh
    AccompanyViewController.h
    AccompanyViewController.mm
    AccompanyViewController.xib
)

set(xcast_dev_ARC_SOURCES
   main.m
   app_delegate.m
   main_menu.m
   LoginViewController.mm
   AccompanyViewController.mm
)

if(NOT OSX)
   message(ERROR "Should be osx platform")
endif(NOT OSX)


set_source_files_properties(${xcast_dev_ARC_SOURCES} PROPERTIES COMPILE_FLAGS "-fobjc-arc" )

set(APP_BUNDLE_IDENTIFIER "com.tencent.xcast")
set(MACOSX_BUNDLE_INFO_STRING ${APP_BUNDLE_IDENTIFIER})
set(MACOSX_BUNDLE_ICON_FILE "")
set(MACOSX_BUNDLE_GUI_IDENTIFIER ${APP_BUNDLE_IDENTIFIER})
set(MACOSX_BUNDLE_LONG_VERSION_STRING "1.0")
set(MACOSX_BUNDLE_NAME ${APP_BUNDLE_IDENTIFIER})
set(MACOSX_BUNDLE_SHORT_VERSION_STRING ${APP_BUNDLE_SHORT_VERSION_STRING})
set(MACOSX_BUNDLE_VERSION ${APP_BUNDLE_VERSION})
set(MACOSX_BUNDLE_COPYRIGHT "Copyright 2018 Tencent MediaLab")
set(MACOSX_LAUNCH_SCREEN_INTERFACE_FILE_BASE_NAME "main")
set(MACOSX_MAIN_STORYBOARD_FILE_BASE_NAME "main")

set(framework_Rqd ${PROJECT_ROOT_DIR}/third_party/RDQ/Mac/Rqd.framework)
find_osx_framework(framework_JavaScriptCore JavaScriptCore)
find_osx_framework(framework_Security Security)


if(AV_USE_TVE)
    set(av_tve_links
        av_tve
        libvideocore
        libyuv
        libBImageVision
        libhwCodec
        libH264DecLib
        libx264EncLib
        libHevcDec
        libTcHevcEnc)
else()
    set(av_tve_links)
endif(AV_USE_TVE)

if(AV_USE_TRAE)
    set(av_trae_links
        av_trae
        libtrae)
else()
    set(av_trae_links)
endif(AV_USE_TRAE)

set(osx_frameworks
   ${framework_Cocoa}
   ${framework_Foundation}
   ${framework_AVFoundation}
   ${framework_AudioToolbox}
   ${framework_AudioUnit}
   ${framework_CoreVideo}
   ${framework_CoreMedia}
   ${framework_QuartzCore}
   ${framework_SystemConfiguration}
   ${framework_JavaScriptCore}
   ${framework_JavaScriptCore}
   ${framework_Rqd}
   ${framework_Security}
   ${framework_AppKit})

set (CLANG_CXX_LIBRARY ${CMAKE_OSX_SYSROOT}/usr/lib/libz.tbd ${CMAKE_OSX_SYSROOT}/usr/lib/libstdc++.6.tbd)

set(xcast_LINKS
    xcast_static
    xc_core
    xc_plugins
    av_profile
    av_config
    sys_info
    av_session
    net_libuv
    app_http_channel
    net_http
    libtea
    ${av_trae_links}
    ${av_tve_links}
    libuv
    jansson
    av_renderer
    xc_base
    xc_net
    av_protos
    av_common
    av_udt
    udt
    libprotobuf
    libyuv
    lan_relay
    video_capture
    screen_preprocor
    ${CLANG_CXX_LIBRARY}
    ${osx_frameworks})
    
if (AV_USE_LAN_RELAY)
      list(APPEND xcast_LINKS lan_relay)
endif()
    
set(CONFIGFILES
    config/1400029761.conf
    config/1400029763.conf
    config/1400046799.conf
    config/1400036169.conf
)

set(XIBFILES
  LoginViewController.xib
  MainMenu.xib
  Render/black.png
  Render/mavvideo_backgroud_big_old.png
  Render/mavvideo_backgroud_small.png
  Render/Shader/mav_standard_fragment.sh
  Render/Shader/mav_standard_vertex.sh
  AccompanyViewController.xib
)

set_source_files_properties(${CONFIGFILES}
                           PROPERTIES MACOSX_PACKAGE_LOCATION
                           Resources)

set_source_files_properties(${XIBFILES}
                            PROPERTIES MACOSX_PACKAGE_LOCATION
                            Resources)

option(LINK_XCAST_SHARED "Use dynamic xcast library" OFF)

add_executable(xcast_dev
               ${xcast_dev_SOURCES}
               #${ICONS}
               #${IMAGES}
               ${CONFIGFILES}
               ${XIBFILES})

#Generate->Deployment Info->Deployment Target
#Build Settings->iOS Deployment Target
set_xcode_attr_property(xcast_dev MACOSX_DEPLOYMENT_TARGET "10.9")
#Build Settings->Runpath Search Paths
set_xcode_attr_property(xcast_dev LD_RUNPATH_SEARCH_PATHS "@executable_path")
set_xcode_property(xcast_dev MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/info.plist)
set_xcode_attr_property(xcast_dev GCC_GENERATE_DEBUGGING_SYMBOLS "YES")

if(LINK_XCAST_SHARED)
    set(xcast_dev_LINKKS
        xcast)
else()
    set(xcast_dev_LINKKS
        ${xcast_LINKS})
endif(LINK_XCAST_SHARED)

target_link_libraries(xcast_dev
PUBLIC
     ${xcast_dev_LINKKS}
)

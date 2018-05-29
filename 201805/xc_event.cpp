//
//  xc_event.cpp
//  test
//
//  Created by AlexiChen on 2018/4/3.
//  Copyright © 2018年 tencent. All rights reserved.
//

#include "xc_event.h"

#include <string>
#include "xc_callback.h"
#include "xcast_error.h"
#include "xc_struct.h"

namespace xc_api
{
    int32_t xc_event::on_xc_system_event(void *contextinfo, xcast_variant_t *var)
    {
        if (var && contextinfo)
        {
            xcast_data_t data(var);
            log_xc_data_json(data);
            
            int len = data.to_json(NULL, 0);
            char *json = new char[len + 1];
            data.to_json(json, len+1);
            
            if (json)
            {
                std::string js = json;
                xc_callback *callback = (xc_callback *)contextinfo;
                delete [] json;
                return callback->on_xc_system(callback, js);
            }
            delete [] json;
        }
        
        
        return XCAST_OK;
    }
    
    int32_t xc_event::on_xc_channel_event(void *contextinfo, xcast_variant_t *var)
    {
        if (var && contextinfo)
        {
            xcast_data_t data(var);
            log_xc_data_json(data);
            xc_callback *callback = (xc_callback *)contextinfo;
            
            xc_event_channel channelitem(std::move(data));
            
            switch (channelitem.type)
            {
                case xc_channel_added:
                    callback->on_xc_channel_add(callback, channelitem);
                    break;
                case xc_channel_updated:
                    callback->on_xc_channel_connected(callback, channelitem);
                    break;
                case xc_channel_removed:
                    callback->on_xc_channel_removed(callback, channelitem);
                    break;
                default:
                    break;
            }
        }
        return XCAST_OK;
    }
    
    int32_t xc_event::on_xc_stream_event(void *contextinfo, xcast_variant_t *var)
    {
        if (var && contextinfo)
        {
            xcast_data_t data(var);
            log_xc_data_json(data);
            xc_callback *callback = (xc_callback *)contextinfo;
            
            xc_device_event type = (xc_device_event)(data["type"].int32_val());
            switch (type)
            {
                case xc_stream_added:
                {
                    xc_event_stream streamitem(std::move(data));
                    callback->on_xc_stream_add(callback, streamitem);
                }
                    break;
                case xc_stream_updated:
                {
                    xc_event_stream streamitem(std::move(data));
                    callback->on_xc_stream_updated(callback, streamitem);
                }
                    break;
                case xc_stream_capture_changed:
                {
                    xc_event_stream streamitem(std::move(data));
                    callback->on_xc_stream_capture_changed(callback, streamitem);
                }
                    break;
                case xc_stream_removed:
                {
                    xc_event_stream streamitem(std::move(data));
                    callback->on_xc_stream_removed(callback, streamitem);
                }
                    break;
                case xc_stream_media:
                {
                    
                    xc_stream_type trackcls = (xc_stream_type)((int32_t)data["class"]);
                    switch (trackcls)
                    {
                        case xc_stream_msg:
                        {
                            
                        }
                            
                            break;
                        case xc_stream_video:
                        {
                            xc_video_frame frame(std::move(data));
                            callback->on_xc_stream_media_video(callback, frame);
                        }
                            break;
                        case xc_stream_audio:
                        {
                        }
                            break;
                            
                        default:
                            break;
                    }
                    
                }
                    break;
                default:
                    break;
            }
        }
        return XCAST_OK;
    }
    
    int32_t xc_event::on_xc_device_event(void *contextinfo, xcast_variant_t *var)
    {
        if (var && contextinfo)
        {
            xcast_data_t data(var);
            //log_xc_data_json(data);
            xc_callback *callback = (xc_callback *)contextinfo;
            
            xc_device_event type = (xc_device_event)(data["type"].int32_val());
            
            switch (type)
            {
                case xc_device_added:
                {
                    xc_event_device_base additem(std::move(data));
                    callback->on_xc_device_add(callback, additem);
                }
                    break;
                case xc_device_updated:
                {
                    xc_event_device updateitem(std::move(data));
                    
                    callback->on_xc_device_updated(callback, updateitem);
                }
                    break;
                case xc_device_removed:
                {
                    xc_event_device_base removeitem(std::move(data));
                    callback->on_xc_device_removed(callback, removeitem);
                }
                    break;
                case xc_device_preprocess:
                {
                    // SDK not support xc_device_preprocess yet
                    xc_event_device deviceitem(std::move(data));
                    callback->on_xc_device_preprocess(callback, deviceitem);
                }
                    break;
                    
                case xc_device_preview:
                {
                    xc_device_type cls = (xc_device_type)(data["class"].int32_val());
                    
                    switch (cls)
                    {
                        case xc_device_unknown :              /* unknown device type */
                            break;
                        case xc_device_camera:                 /* camera device */
                        {
                            xc_video_frame videoitem(std::move(data));
                            callback->on_xc_device_preview_video(callback, videoitem);
                        }
                            break;
                            //                        case xc_device_screen_capture:           /* screnn capture */
                            //                        {
                            //                            xc_video_frame &&videoitem = data_to_xc_video_frame(data);
                            //                            callback->on_xc_device_preview_video(callback, std::move(videoitem));
                            //                        }
                            //                             break;
                            //                        case xc_device_player:                   /* media player */
                            //                             break;
                            //                        case xc_device_mic:                      /* microphone device */
                            //                             break;
                            //                        case xc_device_speaker:                 /* speaker device */
                            //                             break;
                            //                        case xc_device_accompany:                /* audio acompany device */
                            //                             break;
                            //                        case xc_device_external:                 /* external device type */
                            //
                            //                            break;
                            //                        case xc_device_render : /* render device type */
                            //                            break;
                            
                        default:
                        {
                            xc_event_device deviceitem(std::move(data));
                            callback->on_xc_device_preview(callback, deviceitem);
                        }
                            break;
                    }
                    
                }
                    
                    
                    break;
                case xc_device_failure:
                {
                    xc_event_device_base failureitem(std::move(data));
                    callback->on_xc_device_failure(callback, failureitem);
                }
                    
                    break;
                default:
                    break;
            }
        }
        return XCAST_OK;
    }
    
    int32_t xc_event::on_xc_device_preprocess_event(void *contextinfo, xcast_variant_t *var)
    {
        if (var && contextinfo)
        {
            xcast_data_t data(var);
            // log_xc_data_json(data);
            xc_callback *callback = (xc_callback *)contextinfo;
            
            xc_video_frame videoitem(std::move(data));
            // just camera preprocess in current
            // TODO: add more
            videoitem.media_src = xc_media_source_camera;
            callback->on_xc_device_camera_preprocess(callback, videoitem);
        }
        return XCAST_OK;
    }
    
    int32_t xc_event::on_xc_tips_event(void *contextinfo, xcast_variant_t *var)
    {
        if (var && contextinfo)
        {
            xcast_data_t data(var);
            log_xc_data_json(data);
            
            
            int len = data.to_json(NULL, 0);
            char *json = new char[len + 1];
            data.to_json(json, len+1);
            
            if (json)
            {
                std::string js = json;
                xc_callback *callback = (xc_callback *)contextinfo;
                callback->on_xc_tips(callback, js);
                delete [] json;
            }
        }
        return XCAST_OK;
    }
    
}

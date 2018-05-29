//
//  xc_struct.cpp
//  xlive
//
//  Created by AlexiChen on 2018/4/8.
//  Copyright © 2018年 tencent. All rights reserved.
//

#include <stdlib.h>
#include <cstring>
#include "xc_struct.h"
#include "xcast_data.h"
//===========================================================================

bool xc_startup_param_t::is_vailed() const
{
    if ((tinyid == 0) || sdkappid == 0)
    {
        return false;
    }
    return true;
}


//===========================================================================

xc_event_channel_t::xc_event_channel_t(xcast_data_t &&data)
{
    
    const char *src = data["src"];
    this->src = src ? src : "";
    this->type = (xc_channel_event)((int32_t)data["type"]);
    this->state = (xc_channel_state)((int32_t)data["state"]);
    
    this->err = (int32_t)(data["err"]);
    const char *errmsg = data["err-msg"];
    this->err_msg = errmsg ? errmsg : "";
}
//===========================================================================
xc_video_frame_t::xc_video_frame_t(xcast_data_t &&data)
{
//    xc_data_log(data);
//    xc_video_frame event;
    
    const char *temp = data["src"];
    this->src = temp ? temp : "";
    
    this->type = (xc_device_event)(data["type"].int32_val());
    this->cls = (xc_device_type)(data["class"].int32_val());
    
    this->format = (xc_media_format)(data["format"].int32_val());
    this->media_src = (xc_media_source)(data["media-src"].int32_val());
    this->uin = data["uin"].uint64_val();
    
    const uint8_t *dataptr = data["data"].bytes_val();
    if (dataptr)
    {
        this->data = dataptr;
    }
    else
    {
        dataptr = (const uint8_t *)(data["data"].ptr_val());
        this->data = dataptr;
    }

    this->size = (uint32_t)(data["size"]);
    this->width = (int32_t)(data["width"]);
    this->height = (int32_t)(data["height"]);
    this->rotate = (xc_rotate)((int32_t)(data["rotate"]));
}

//===========================================================================
xc_event_device_base_t::xc_event_device_base_t(xcast_data_t &&data)
{
    const char *temp = data["src"];
    this->src = temp ? temp : "";
    
    this->type = (xc_device_event)(data["type"].int32_val());
    this->cls = (xc_device_type)(data["class"].int32_val());
    this->state = (xc_device_state)(data["state"].int32_val());
    
    this->err = (int32_t)(data["err"]);
    const char *errmsg = data["err-msg"];
    this->err_msg = errmsg ? errmsg : "";
}
//===========================================================================

xc_event_stream_t::xc_event_stream_t(xcast_data_t &&data)
{
//    xc_data_log(data);
//    xc_event_stream event;
    
    const char *temp = data["src"];
    this->src = temp ? temp : "";
    
    temp = data["src"];
    this->channel = temp ? temp : "";
    
    this->type = (xc_stream_event)((int32_t)data["type"]);
    this->cls = (xc_stream_type)((int32_t)data["class"]);
    this->direction = (xc_stream_direction)((int32_t)data["direction"]);
    this->uin = ((uint64_t)data["uin"]);
    this->index = ((uint32_t)data["index"]);
    this->state = (xc_stream_state)((uint32_t)data["state"]);
    this->active = data["active"].bool_val();
    this->quality = data["quality"].int32_val();
    this->err = (int32_t)(data["err"]);
    const char *errmsg = data["err-msg"];
    this->err_msg = errmsg ? errmsg : "";
    
    this->format = (xc_media_format)((int32_t)data["format"]);
    this->data = data["data"].bytes_val();
    this->size = (uint32_t)(data["size"]);
    this->width = (uint32_t)(data["width"]);
    this->height = (uint32_t)(data["height"]);
    
    int32_t rotate = (int32_t)(data["rotate"]);
    this->rotate = (xc_rotate)rotate;
    this->media_src = (xc_media_source)((uint32_t)(data["media-src"]));
}
//===========================================================================
xc_event_device_t::xc_event_device_t(xcast_data_t &&data)
{
//    xc_data_log(data);
//
//    xc_event_device event;
    
    const char *temp = data["src"];
    this->src = temp ? temp : "";
    
    this->type = (xc_device_event)((int32_t)data["type"]);
    this->cls = (xc_device_type)((int32_t)data["class"]);
    
    this->state = (xc_device_state)((uint32_t)data["state"]);
    
    this->err = (int32_t)(data["err"]);
    const char *errmsg = data["err-msg"];
    this->err_msg = errmsg ? errmsg : "";
    
    this->format = (xc_media_format)((int32_t)data["format"]);
    this->data = data["data"].bytes_val();
    this->size = (uint32_t)(data["size"]);
    this->width = (int32_t)(data["width"]);
    this->height = (int32_t)(data["height"]);
    this->rotate = (xc_rotate)((int32_t)(data["rotate"]));
    
    this->volume = (int32_t)(data["volume"]);
    this->dynamic_volume = (int32_t)(data["dynamic-volume"]);
    
    temp = data["player-path"];
    this->player_path = temp ? temp : "";
    
    temp = data["file-path"];
    this->file_path = temp ? temp : "";
    
    this->current_pos = (uint64_t)(data["current-pos"]);
    this->max_pos = (uint64_t)(data["max-pos"]);
    this->max_pos = (uint32_t)(data["duration"]);
    
    temp = data["accompany-source"];
    this->accompany_source = temp ? temp : "none";
    
    this->screen_left = (int32_t)(data["screen-left"]);
    this->screen_top = (int32_t)(data["screen-top"]);
    this->screen_right = (int32_t)(data["screen-right"]);
    this->screen_bottom = (int32_t)(data["screen-bottom"]);
    this->screen_fps = (int32_t)(data["screen-fps"]);
    this->screen_hwnd = (int32_t)(data["screen-hwnd"]);
}

//===========================================================================

xc_channel_auth_info_t::~xc_channel_auth_info_t()
{
    if (auth_buffer)
    {
        free(auth_buffer);
        auth_buffer = nullptr;
    }
}

bool xc_channel_auth_info_t::is_vailed() const
{
    if (auth_bits == 0)
    {
        return false;
    }
    if (auth_type == xc_auth_none)
    {
        return true;
    }
    else if (auth_type == xc_auth_manual)
    {
        return auth_buffer != nullptr && auth_buffer_size != 0;
    }
    else if (auth_type == xc_auth_auto)
    {
        if (expire_time < 0 || secret_key.length() == 0)
        {
            return false;
        }
        return true;
    }
    else
    {
        return false;
    }
}

//===========================================================================
bool xc_channel_param_t::is_valied() const
{
    if (roomid == 0 || role.length() == 0 || channelid.length() == 0)
    {
        return false;
    }
    
    bool isv = auth_info.is_vailed();
    return isv;
}

xcast_data_t xc_channel_param_t::to_xcast_data() const
{
    xcast_data_t       params, auth_info, track;
    
    params["relation_id"] = this->roomid;
    params["auto_recv"] = this->auto_recv;
    params["role"] = this->role.c_str();
    
    params["auth_type"] = this->auth_info.auth_type;
    auth_info["auth_bits"] = this->auth_info.auth_bits;
    
    if (this->auth_info.auth_type == xc_auth_none)
    {
        // do nothing
    }
    else if (this->auth_info.auth_type == xc_auth_manual)
    {
        auth_info.put_bytes("auth_buffer", this->auth_info.auth_buffer, this->auth_info.auth_buffer_size);
    }
    else if (this->auth_info.auth_type == xc_auth_auto)
    {
        auth_info["account_type"] = this->auth_info.account_type;
        auth_info["expire_time"] = this->auth_info.expire_time;
        const  char *secret_key = this->auth_info.secret_key.c_str();
        auth_info.put_bytes("secret_key", (const uint8_t *)secret_key, (uint32_t)strlen(secret_key));
    }
    
    params.put("auth_info", auth_info);
    
    
    
    track["ext-video-capture"] = this->track.ext_video_capture;    /* allow video track to use external capture */
    track["ext-audio-capture"] = this->track.ext_audio_capture;    /* allow audio track to use external capture */
    track["ext-audio-playback"] = this->track.ext_audio_playback;   /* allow audio track to use external playback */
    params.put("track", track);
    
    if (this->videomaxbps > 0)
    {
        params["videomaxbps"] = this->videomaxbps;
    }
    
    return params;
}
//===========================================================================

xc_device_external_input_t::xc_device_external_input_t(xc_video_frame_t &&video)
{
    this->format = video.format;
    this->data = video.data;
    this->size = video.size;
    this->rotate = video.rotate;
    this->width = video.width;
    this->height = video.height;
}

xc_device_external_input_t::~xc_device_external_input_t()
{
    
}

bool xc_device_external_input_t::is_vailed() const
{
    if (this->data == nullptr || this->size == 0)
    {
        return false;
    }
    switch (this->format) {
        case xc_media_argb32:
        case xc_media_i420:
        case xc_media_layer:
        {
            if(this->rotate == xc_rotate_unknow || this->width == 0 || this->height == 0)
            {
                return false;
            }
        }
            break;
        case xc_media_pcm:
        case xc_media_aac:
        {
            if(this->sample_rate == 0 || this->channel_count != 1 || this->channel_count != 2 || this->bits != 8 || this->bits != 16)
            {
                return false;
            }
        }
            break;
        default:
            break;
    }
    return true;
}

xcast_data_t xc_device_external_input_t::to_xcast_data() const
{
    xcast_data_t data;
    if (this->data == nullptr || this->size == 0 || this->width == 0)
    {
        return data;
    }
    switch (this->format) {
        case xc_media_argb32:
        case xc_media_i420:
        case xc_media_layer:
        {
            if(this->rotate == xc_rotate_unknow)
            {
                return data;
            }
            data["format"] = (int32_t)(this->format);
            data.put_bytes("data", this->data, this->size);
            data["size"] = this->size;
            data["width"] = this->width;
            if (this->height != 0)
            {
                // TODO: 是否要手动计算出来？
                data["height"] = this->height;
            }
            data["rotate"] = (int32_t)(this->rotate);
        }
            
            
            break;
        case xc_media_pcm:
        case xc_media_aac:
        {
            data["format"] = (int32_t)(this->format);
            data.put_bytes("data", this->data, this->size);
            data["size"] = this->size;
            
            // TODO:需要确认width是否需要
            // data["width"] = this->width;
            
            data["sample-rate"] = this->sample_rate;
            data["channel-count"] = this->channel_count;
            data["bits"] = this->bits;
        }
            break;
        default:
            break;
    }
    
    return data;
}
//===========================================================================

bool xc_mediafile_setting_t::is_vailed() const
{
    return !(file.length() == 0 || pos == 0 || max_pos == 0);
}

xcast_data_t xc_mediafile_setting_t::to_xcast_data() const
{
    xcast_data_t data;
    if (this->is_vailed()) {
        
        data["file"] = this->file.c_str();
        data["restart"] = this->restart;
        switch (this->state) {
            case xc_mediafile_state_init:
                data["state"] = "init";
                break;
            case xc_mediafile_state_playing:
                data["state"] = "playing";
                break;
            case xc_mediafile_state_pause:
                data["state"] = "pause";
                break;
            default:
                break;
        }
        data["pos"] = this->pos;
        data["max-pos"] = this->max_pos;
    }
    return data;
}

//===========================================================================
bool xc_screencapture_setting_t::is_vailed() const
{
    // TODO:不知道各平如的情况
    return true;
}
xcast_data_t xc_screencapture_setting_t::to_xcast_data() const
{
    xcast_data_t data;
    
    if (is_vailed())
    {
        data["left"] = this->left;
        data["top"] = this->top;
        data["right"] = this->right;
        data["bottom"] = this->bottom;
        data["fps"] = this->fps;
        if (this->hwnd)
        {
            data["hwnd"] = this->hwnd;
        }
    }
    
    
    return data;
}
//===========================================================================
bool xc_accompany_setting_t::is_vailed() const
{
    return !(player.length() == 0 || file.length() == 0);
}
xcast_data_t xc_accompany_setting::to_xcast_data() const
{
    xcast_data_t data;
    
    data["player"] = this->player.c_str();
    data["file"] = this->file.c_str();
    
    switch (this->source) {
        case xc_accompany_source_none:
            data["source"] = "none";
            break;
        case xc_accompany_source_system:
            data["source"] = "system";
            break;
        case xc_accompany_source_process:
            data["source"] = "process";
            break;
            
        default:
            break;
    }
    return data;
}
//===========================================================================

bool xc_preference_camera_preview_setting_t::is_vailed() const{
    if (format != xc_media_argb32 && format != xc_media_i420)//|| width == 0 || height == 0)
    {
        return false;
    }
    
    return true;
}

xcast_data_t xc_preference_camera_preview_setting_t::to_xcast_data() const
{
    xcast_data_t data;
    if (is_vailed())
    {
        data["format"] = this->format;
//        data["width"] = this->width;
//        data["height"] = this->height;
    }
    return data;
}
//===========================================================================




//===========================================================================

bool xc_preference_video_watermark_t::is_vailed() const
{
    if (argbdata == nullptr || width == 0 || height == 0)
    {
        return false;
    }
    return true;
}

xcast_data_t xc_preference_video_watermark_t::to_xcast_data() const
{
    xcast_data_t data;
    if (this->is_vailed())
    {
        switch (this->type) {
            case xc_preference_video_watermark_type_320_240:
                data["type"] = "320*240";
                break;
            case xc_preference_video_watermark_type_480_360:
                data["type"] = "480*360";
                break;
            case xc_preference_video_watermark_type_640_480:
                data["type"] = "640*480";
                break;
            case xc_preference_video_watermark_type_640_368:
                data["type"] = "640*368";
                break;
            case xc_preference_video_watermark_type_960_540:
                data["type"] = "960*540";
                break;
                
            case xc_preference_video_watermark_type_1280_720:
                data["type"] = "1280*720";
                break;
            case xc_preference_video_watermark_type_192_144:
                data["type"] = "192*144";
                break;
            case xc_preference_video_watermark_type_320_180:
                data["type"] = "320*180";
                break;
                
            default:
                break;
                
        }
        
        data["width"] = this->width;
        data["height"] = this->height;
        data.put_bytes("argb-data", this->argbdata, this->width * this->height * 4);
    }
    
    
    return data;
}
//===========================================================================
xc_work_result_t::xc_work_result_t(bool rp)
{
    is_passive_return = rp;
}
xc_work_result_t::xc_work_result_t(const xc_work_result_t &ret)
{
    this->err = ret.err;
    this->err_msg = ret.err_msg;
    if (ret.result.get())
    {
        this->set_result(*ret.result);
    }
}
xc_work_result_t::xc_work_result_t(int32_t aerr, std::string  aerr_msg, bool rp)
{
    err = aerr;
    err_msg = aerr_msg;
    is_passive_return = rp;
}

xc_work_result_t::~xc_work_result_t()
{
    result.reset();
}


int32_t xc_work_result_t::to_int32()
{
    bool succ =  this->is_valied();
    if (succ && this->result.get())
    {
        return this->result->int32_val();
    }
    return -1;
}
uint32_t xc_work_result_t::to_uint32()
{
    bool succ = this->is_valied();
    if (succ && this->result.get())
    {
        return this->result->uint32_val();
    }
    return 0;
}
std::string xc_work_result_t::to_string()
{
    bool succ =  this->is_valied();
    if (succ && this->result.get())
    {
        const char *cstr = this->result->str_val();
        if (cstr)
        {
            return std::string(cstr);
        }
    }
    return "";
}
bool xc_work_result_t::to_bool()
{
    bool succ =  this->is_valied();
    if (succ && this->result.get())
    {
        return this->result->bool_val();
    }
    return false;
}
std::vector<std::string> xc_work_result_t::to_list()
{
    std::vector<std::string> list;
    bool succ = this->is_valied();
    if (succ && this->result.get())
    {
        uint32_t size = this->result->size();
        for(uint32_t i = 0; i < size; i++)
        {
            const char *ds = this->result->str_at(i);
            if (ds)
            {
                list.push_back(std::string(ds));
            }
        }
    }
    return list;
}


xc_mediafile_setting xc_work_result_t::to_mediafile_setting()
{
    xc_mediafile_setting setting;
    bool succ = this->is_valied();
    if (succ && this->result.get())
    {
        xcast_data_t data = *(this->result);
        const char *file = data["file"].str_val();
        setting.file = file ? file : "";
        
        const char *state = data["state"].str_val();
        if (state)
        {
            if(strcmp(state, "init") == 0)
            {
                setting.state = xc_mediafile_state_init;
            }
            else if(strcmp(state, "playing") == 0)
            {
                setting.state = xc_mediafile_state_playing;
            }
            else if(strcmp(state, "paused") == 0)
            {
                setting.state = xc_mediafile_state_pause;
            }
        }
        
        setting.pos = data["pos"];
        setting.max_pos = data["max-pos"];
    }
    return setting;
}
xc_mediafile_verify xc_work_result_t::to_mediafile_verify()
{
    xc_mediafile_verify setting;
    bool succ = this->is_valied();
    if (succ && this->result.get())
    {
        xcast_data_t data = *(this->result);
        setting.valid = data["valid"].bool_val();
        setting.audio_only = data["audio-only"].bool_val();
    }
    return setting;
}
xc_screencapture_setting xc_work_result_t::to_screencapture_setting()
{
    xc_screencapture_setting setting;
    bool succ = this->is_valied();
    if (succ && this->result.get())
    {
        xcast_data_t data = *(this->result);
        setting.left = data["left"].int32_val();
        setting.top = data["top"].int32_val();
        setting.right = data["right"].int32_val();
        setting.bottom = data["bottom"].int32_val();
        setting.fps = data["fps"].uint32_val();
        
        // TODO: window only
        setting.hwnd = data["hwnd"].int32_val();
    }
    return setting;
}
xc_accompany_setting xc_work_result_t::to_accompany_setting()
{
    xc_accompany_setting setting;
    bool  succ = this->is_valied();
    if (succ && this->result.get())
    {
        xcast_data_t data = *(this->result);
        
        setting.player = data["player"].str_val();
        setting.player = data["file"].str_val();
        
        const char *source = data["source"];
        if (strcmp(source, "none") == 0)
        {
            setting.source = xc_accompany_source_none;
        }
        else if (strcmp(source, "system") == 0)
        {
            setting.source = xc_accompany_source_system;
        }
        else if (strcmp(source, "process") == 0)
        {
            setting.source = xc_accompany_source_process;
        }
    }
    return setting;
    
}
xc_preference_camera_preview_setting xc_work_result_t::to_camera_preview_setting()
{
    xc_preference_camera_preview_setting setting;
    bool succ = this->is_valied();
    if (succ && this->result.get())
    {
        xcast_data_t data = *(this->result);
        setting.format = (xc_media_format)(data["format"].int32_val());
//        setting.width = data["width"].uint32_val();
//        setting.height = data["height"].uint32_val();
    }
    
    return setting;
}

void xc_work_result_t::set_result(xcast_data_t ret)
{
    this->result = std::make_shared<xcast_data_t>(new xcast_data_t());
    this->result->put("return", ret);
    bool succ = is_valied();
    if (!succ)
    {
        this->err = -1;
        this->err_msg = "the return data format error";
    }
}

bool xc_work_result_t::is_valied()
{
    if (this->valied)
    {
        return this->valied;
    }
    if(this->result.get())
    {
        int len = this->result->to_json(NULL, 0);
        char *str = new char[len + 1];
        this->result->to_json(str, len+1);
        bool succ = false;
        
        const char *c = strstr(str, "return");
        succ = (c != nullptr);
        if (succ)
        {
            xcast_data_t data = (*this->result)["return"];
            this->result = std::make_shared<xcast_data_t>(data);
        }
        
        delete []str;
        this->valied = succ;
        return succ;
    }
    // xcast_set_property没有对应的result，防止外部误用
    this->valied = true;
    return false;
}

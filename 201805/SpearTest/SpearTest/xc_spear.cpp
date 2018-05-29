//
//  xc_spear.cpp
//  xlive
//
//  Created by AlexiChen on 2018/4/27.
//

#include "xc_spear.h"

#include <sstream>
#include <sys/stat.h>
#include <unistd.h>

//{"audio":{"aec":1,"agc":1,"ans":1,"anti_dropout":1,"au_scheme":1,"channel":1,"codec_prof":4129,"frame":20,"kbps":30,"max_antishake_max":1000,"max_antishake_min":400,"min_antishake":80,"sample_rate":16000,"silence_detect":1},"is_default":1,"net":{"rc_anti_dropout":1,"rc_init_delay":-1,"rc_max_delay":-1},"role":"user","type":1,"video":{"anti_dropout":1,"codec_prof":-1,"format":-2,"format_fix_height":240,"format_fix_width":320,"format_max_height":-1,"format_max_width":-1,"fps":15,"fqueue_time":-1,"live_adapt":-1,"maxkbps":400,"maxqp":-1,"minkbps":400,"minqp":-1,"qclear":-1,"small_video_upload":-1}}


#define kSpearILVBItemFormat "{\"audio\":{\"aec\":1,\"agc\":1,\"ans\":1,\"anti_dropout\":1,\"au_scheme\":1,\"channel\":1,\"codec_prof\":4106,\"frame\":20,\"kbps\":30,\"max_antishake_max\":1000,\"max_antishake_min\":400,\"min_antishake\":80,\"sample_rate\":16000,\"silence_detect\":1},\"is_default\":0,\"net\":{\"rc_anti_dropout\":1,\"rc_init_delay\":-1,\"rc_max_delay\":-1},\"role\":\"%s\",\"type\":%d,\"video\":{\"anti_dropout\":1,\"codec_prof\":5,\"format\":-2,\"format_fix_height\":%d,\"format_fix_width\":%d,\"format_max_height\":-1,\"format_max_width\":-1,\"fps\":%d,\"fqueue_time\":-1,\"live_adapt\":-1,\"maxkbps\":%d,\"maxqp\":-1,\"minkbps\":%d,\"minqp\":-1,\"qclear\":-1,\"small_video_upload\":-1}}"

#define kSpearRTRCItemFormat "{\"audio\":{\"aec\":1,\"agc\":1,\"ans\":1,\"anti_dropout\":1,\"au_scheme\":1,\"channel\":1,\"codec_prof\":4129,\"frame\":20,\"kbps\":30,\"max_antishake_max\":1000,\"max_antishake_min\":400,\"min_antishake\":80,\"sample_rate\":16000,\"silence_detect\":1},\"is_default\":0,\"net\":{\"rc_anti_dropout\":1,\"rc_init_delay\":-1,\"rc_max_delay\":-1},\"role\":\"%s\",\"type\":%d,\"video\":{\"anti_dropout\":1,\"codec_prof\":5,\"format\":-2,\"format_fix_height\":%d,\"format_fix_width\":%d,\"format_max_height\":-1,\"format_max_width\":-1,\"fps\":%d,\"fqueue_time\":-1,\"live_adapt\":-1,\"maxkbps\":%d,\"maxqp\":-1,\"minkbps\":%d,\"minqp\":-1,\"qclear\":-1,\"small_video_upload\":-1}}"


//#define kSpearRCItemFormat "{\"audio\":{\"aec\":1,\"agc\":1,\"ans\":1,\"anti_dropout\":1,\"au_scheme\":1,\"channel\":1,\"codec_prof\":4102,\"frame\":20,\"kbps\":30,\"max_antishake_max\":1000,\"max_antishake_min\":400,\"min_antishake\":80,\"sample_rate\":16000,\"silence_detect\":1},\"is_default\":0,\"net\":{\"rc_anti_dropout\":1,\"rc_init_delay\":-1,\"rc_max_delay\":-1},\"role\":\"%s\",\"type\":1,\"video\":{\"anti_dropout\":1,\"codec_prof\":-1,\"format\":-2,\"format_fix_height\":%d,\"format_fix_width\":%d,\"format_max_height\":-1,\"format_max_width\":-1,\"fps\":%d,\"fqueue_time\":-1,\"live_adapt\":-1,\"maxkbps\":%d,\"maxqp\":-1,\"minkbps\":%d,\"minqp\":-1,\"qclear\":-1,\"small_video_upload\":-1}}"


//#define kSpearItemFormat "{\"audio\":{\"aec\":1,\"agc\":1,\"ans\":1,\"anti_dropout\":1,\"au_scheme\":1,\"channel\":1,\"codec_prof\":%d,\"frame\":20,\"kbps\":30,\"max_antishake_max\":1000,\"max_antishake_min\":400,\"min_antishake\":80,\"sample_rate\":16000,\"silence_detect\":1},\"is_default\":0,\"net\":{\"rc_anti_dropout\":1,\"rc_init_delay\":-1,\"rc_max_delay\":-1},\"role\":\"%s\",\"type\":1,\"video\":{\"anti_dropout\":1,\"codec_prof\":-1,\"format\":-2,\"format_fix_height\":%d,\"format_fix_width\":%d,\"format_max_height\":-1,\"format_max_width\":-1,\"fps\":%d,\"fqueue_time\":-1,\"live_adapt\":-1,\"maxkbps\":%d,\"maxqp\":-1,\"minkbps\":%d,\"minqp\":-1,\"qclear\":-1,\"small_video_upload\":-1}}"


#define kSpearFormat "{\"data\":{\"biz_id\":%d,\"conf\":[%s],\"platform\":%d,\"scheme\":%d,\"sequence\":0},\"errmsg\":\"success.\",\"retcode\":0}"




namespace xc_api
{
    bool get_roles_from_spear_json(const std::string& content, int appid, int scheme, std::map<std::string, std::string> &map)
    {
        Json::Reader reader;
        Json::Value root;
        bool parseRet = reader.parse(content, root);
        if (!parseRet) {
            //log("GetVersion read from local file invalid string");
            return false;
        }
        
        if (!root["data"].isNull())
        {
            
            int bizid = 0;
            Json::Value data = root["data"];
            if (!data["biz_id"].isNull()) {
                bizid = data["biz_id"].asInt();
            }
            
            if (bizid != appid)
            {
                return false;
            }
            
            int ascheme = 0;
            if (!data["scheme"].isNull()) {
                ascheme = data["scheme"].asInt();
            }
            if (ascheme != scheme)
            {
                return false;
            }
            
            if (!data["conf"].isNull() && data["conf"].isArray())
            {
                Json::Value confArray = data["conf"];
                Json::ArrayIndex count = confArray.size();
                Json::FastWriter fastWriter;
                for (Json::ArrayIndex i = 0; i < count; i++)
                {
                    Json::Value item = confArray[i];
                    if (item["role"].isNull())
                    {
                        return false;
                    }
                    
                    
                    
                    std::string role = item["role"].asString();
                    std::string content = fastWriter.write(item);
                    map.insert({role, content});
                }
            }
        }
        
        return true;
    }
    
    bool read_spear_from_file(const std::string& path, std::string& content){
        do {
            unsigned long filesize = -1;
            
            FILE* file = fopen(path.c_str(), "rb");
            if (!file)
                break;
            
            fseek (file , 0 , SEEK_END);
            filesize = ftell (file);
            rewind (file);
            
            char* buffer = (char*) malloc (sizeof(char)*filesize);
            if (buffer == NULL) {
                fclose(file);
                break;
            }
            
            size_t result = fread (buffer, 1, filesize, file);
            if (result != filesize) {
                free(buffer);
                fclose(file);
                break;
            }
            
            content = buffer;
            fclose(file);
            free(buffer);
            
            return true;
        } while (0);
        
        return false;
    }
    
    
    
    // absolute custom
    xc_spear_tool::xc_spear_tool(int asdkappid, xc_spear_scheme ascheme):is_vailed_tool(true), sdkappid(asdkappid), scheme(ascheme)
    {
        
    }
    
    xc_spear_tool::xc_spear_tool(int asdkappid, xc_spear_scheme ascheme, const std::string &spearpath):is_vailed_tool(true) , sdkappid(asdkappid), scheme(ascheme)
    {
        if (spearpath.length() == 0)
        {
            this->is_vailed_tool = false;
            return;
        }
        
        std::string content;
        if (read_spear_from_file(spearpath, content))
        {
            this->is_vailed_tool =  get_roles_from_spear_json(content, asdkappid, ascheme, this->roles);
        }
        else
        {
            this->is_vailed_tool = false;
        }
    }
    
    
    xc_spear_tool::~xc_spear_tool()
    {
        roles.clear();
    }
    
    bool xc_spear_tool::is_vailed()const
    {
        return this->is_vailed_tool;
    }
    
    bool xc_spear_tool::is_vailed_role(const std::string &role) const
    {
        if (!this->is_vailed_tool)
        {
            return false;
        }
        
        int len = (int)role.length();
        if ( len == 0 || len > 15)
        {
            return false;
        }
        
        return roles.find(role) == roles.end();
    }
    bool xc_spear_tool::add_role(const std::string &role, const xc_spear_camera_preset &preset, int fps, int minkbps, int maxkbps, int type)
    {
        if (!this->is_vailed_tool)
        {
            //errinfo = "this tool is invailed";
            return false;
        }
        bool has = this->is_vailed_role(role);
        if (!has)
        {
            //errinfo = "role is empty or role.length not in [1, 15] or had same role";
            return false;
        }
        
        bool fpsok = false;
        switch (this->scheme) {
            case xc_spear_trtc:
                fpsok = (fps >= 1 && fps <= 30);
                break;
            case xc_spear_ilvb:
                fpsok = (fps >= 1 && fps <= 30);
                break;
                
            default:
                break;
        }
        if (!fpsok)
        {
            //errinfo = "fps not in [1, 30]";
            return false;
        }
        
        bool kbpsok = false;
        
        switch (this->scheme) {
            case xc_spear_trtc:
                kbpsok = (maxkbps >= minkbps && minkbps >= 30 && minkbps <= 1500 && maxkbps >= 30 && maxkbps <= 1500);
                break;
            case xc_spear_ilvb:
                kbpsok = (maxkbps >= minkbps && minkbps >= 30 && minkbps <= 1500 && maxkbps >= 30 && maxkbps <= 1500);
                break;
                
            default:
                break;
        }
        if (!kbpsok)
        {
            // errinfo = "kbps not in [30, 1500]";
            return false;
        }
        
        int width = 640;
        int height = 480;
        switch (preset)
        {
            case xc_spear_camera_preset_320_240:
                width = 320;
                height = 240;
                break;
            case xc_spear_camera_preset_480_360:
                width = 480;
                height = 360;
                break;
            case xc_spear_camera_preset_640_368:
                width = 640;
                height = 368;
                break;
            case xc_spear_camera_preset_640_480:
                width = 640;
                height = 480;
                break;
            case xc_spear_camera_preset_960_540:
                width = 960;
                height = 540;
                break;
            case xc_spear_camera_preset_1280_720:
                width = 1280;
                height = 720;
                break;
            default:
                width = 640;
                height = 480;
                break;
        }
        
        bool typeok = true;
        switch (this->scheme) {
            case xc_spear_trtc:
                typeok = type == 1;
                break;
            case xc_spear_ilvb:
                typeok = (type >= 1 || type <= 3);
                break;
            default:
                break;
        }
        
        if (!typeok)
        {
            return false;
        }
        
        
        
        char *item = (char *)malloc(1024 * sizeof(char)) ;
        memset(item, 0, 1024 * sizeof(char));
        
        switch (this->scheme) {
            case xc_spear_trtc:
                sprintf(item, kSpearRTRCItemFormat, role.c_str(), type,height, width, fps, maxkbps, minkbps);
                break;
            case xc_spear_ilvb:
                sprintf(item, kSpearILVBItemFormat, role.c_str(), type,height, width, fps, maxkbps, minkbps);
                break;
            default:
                break;
        }
        
        
        
        std::string itemcontext = item;
        free(item);
        
        this->roles.insert({role, itemcontext});
        return true;
    }
    
    bool xc_spear_tool::delete_role(const std::string &role)
    {
        if (!this->is_vailed_tool)
        {
            return false;
        }
        this->roles.erase(role);
        return true;
    }
    
    bool xc_spear_tool::save_to_file(const std::string &dirname) const
    {
        if (!this->is_vailed_tool)
        {
            return false;
        }
        
        std::string path;
        path.resize(512);
        snprintf(&path[0], path.size(), "%s/%d.conf", dirname.c_str(), sdkappid);
        
        std::stringstream instr;
        auto it = this->roles.begin();
        int count = (int)this->roles.size();
        int index = 0;
        while (it != this->roles.end())
        {
            instr << it->second;
            if (index != count -1)
            {
                instr << ",";
            }
            index++;
            it++;
        }
        
#if defined(_OS_WIN_)
        int platform = 0;
#elif defined(_OS_IOS_)
        int platform = 1;
#elif defined(_OS_ANDROID_)
        int platform = 2;
#elif defined(_OS_MAC_)
        int platform = 4;
#else
        int platform = 8;
#endif
        
        unsigned long alllen = (unsigned long)strlen(kSpearFormat) + instr.str().length() + 256;
        char *spear = (char *)malloc(alllen * sizeof(char)) ;
        memset(spear, 0, 1024 * sizeof(char));
        sprintf(spear, kSpearFormat, this->sdkappid, instr.str().c_str(), platform, this->scheme);
        
        
        std::string temp_file_path = path + ".tmp";
        
        unlink(temp_file_path.c_str());
        
        FILE* file = fopen(temp_file_path.c_str(), "wb+");
        if (!file) {
            free(spear);
            return false;
        }
        
        long len_to_write = strlen(spear);
        
        long write = fwrite(spear, 1, len_to_write, file);
        if (write != len_to_write){
            free(spear);
            fclose(file);
            return false;
        }
        free(spear);
        fclose(file);
        
        if(rename(temp_file_path.c_str(), path.c_str()) != 0){
            return false;
        }
        return true;
        
    }
    
}

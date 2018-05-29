//
//  xc_spear.hpp
//  xlive
//
//  Created by AlexiChen on 2018/4/27.
//

#ifndef xc_spear_hpp
#define xc_spear_hpp

#include <string>
#include <map>
#include "json/json.h"

namespace xc_api
{
    
//    struct xc_spear_item
//    {
//    private:
//        std::string
//    public:
//        xc_spear_item();
//        ~xc_spear_item();
//    public:
//        void update(const std::string name, int videoWidth, int videoHeight, int fps, int minkbps, int maxkbps);
//    };
    
    enum xc_spear_scheme
    {
        xc_spear_trtc = 4,  // trtc
        xc_spear_ilvb = 1,  // ilvb
        //xc_spear_rc   = 0,  // 实时通信息
    };
    
    enum xc_spear_camera_preset
    {
        xc_spear_camera_preset_320_240,
        xc_spear_camera_preset_480_360,
        xc_spear_camera_preset_640_368,
        xc_spear_camera_preset_640_480,
        xc_spear_camera_preset_960_540,
        xc_spear_camera_preset_1280_720,
    };
    
    class xc_spear_tool
    {
        using xc_spear_map = std::map<std::string, std::string> ;
    private:
        bool                is_vailed_tool  = true;
        int                 sdkappid        = 0;
        xc_spear_scheme     scheme          = xc_spear_trtc;
        xc_spear_map        roles;
    public:
        xc_spear_tool(int asdkappid, xc_spear_scheme ascheme);
        xc_spear_tool(int asdkappid, xc_spear_scheme ascheme, const std::string &spearpath);
        
        xc_spear_tool() = delete;
        ~xc_spear_tool();
        
    public:
        bool is_vailed() const;
        bool is_vailed_role(const std::string &role) const;
        bool add_role(const std::string &role, const xc_spear_camera_preset &preset, int fps, int minkbps, int maxkbps, int type = 1);
        bool delete_role(const std::string &role);
        bool save_to_file(const std::string &dirname) const;
    };
}

#endif /* xc_spear_hpp */

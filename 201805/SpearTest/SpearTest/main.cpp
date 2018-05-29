//
//  main.m
//  SpearTest
//
//  Created by AlexiChen on 2018/4/28.
//  Copyright © 2018年 AlexiChen. All rights reserved.
//

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>

#include "xc_spear.h"

using namespace xc_api;
using namespace std;

int main(int argc, const char * argv[]) {
    
    xc_spear_tool tool(1400027849, xc_spear_trtc);
    
    
    bool succ = tool.add_role("user", xc_spear_camera_preset_640_480, 15, 400, 600);
    if (!succ) {
        cout << "add_role error" << endl;
    }
    
    succ = tool.add_role("user2", xc_spear_camera_preset_640_368, 20, 450, 650);
    if (!succ) {
       cout << "add_role error" << endl;
    }
    string dir = "/Users/alexichen/Desktop";
    tool.save_to_file(dir);
    
    succ = tool.delete_role("user2");
    if (!succ) {
        cout << "delete_role error" << endl;
    }
    succ = tool.save_to_file(dir);
    if (!succ) {
        cout << "save_to_file error" << endl;
    }
    
    
    xc_spear_tool local(1400027849, xc_spear_trtc, "/Users/alexichen/Desktop/SpearTest/1400027849.conf");
    
    if (local.is_vailed())
    {
        succ = local.add_role("user", xc_spear_camera_preset_640_480, 15, 400, 600);
        if (!succ) {
            cout << "add_role error" << endl;
        }
        
        succ = local.add_role("user2", xc_spear_camera_preset_640_480, 15, 400, 600);
        if (!succ) {
            cout << "add_role error" << endl;
        }
        
        succ = local.add_role("1280_720", xc_spear_camera_preset_640_480, 15, 400, 600);
        if (!succ) {
            cout << "add_role error" << endl;
        }
        
        succ = local.save_to_file("/Users/alexichen/Desktop/");
        if (!succ) {
            cout << "save_to_file error" << endl;
        }
    }
    
    
    
    return 0;
}

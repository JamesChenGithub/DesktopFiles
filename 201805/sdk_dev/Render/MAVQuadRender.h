//
//  QuadRenderer.hpp
//  QQ4Mac
//
//  Created by nickdai on 15/11/24.
//  Copyright © 2015年 tencent. All rights reserved.
//

#ifndef __MAVQuadRender_hpp__
#define __MAVQuadRender_hpp__

#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>


#define	MAV_MAX_WIDTH	2560
#define MAV_MAX_HEIGHT	1660
#define MAV_RGBA_SIZE	4

class MAVQuadRender
{
public:
    MAVQuadRender();
    ~MAVQuadRender();
    void updateTextureResource(unsigned char*rgb24, int width, int height, int angle,  int type);
    void Draw();
    
private:
    GLuint vertexBuffer;
    GLuint indexBuffer;
    GLuint shaderProgram;
    GLuint inputLayout;
    
private:
    GLuint textureId;
};



#endif /* QuadRenderer_hpp */

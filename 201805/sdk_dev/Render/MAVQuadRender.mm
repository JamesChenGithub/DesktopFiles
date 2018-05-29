//
//  QuadRenderer.cpp
//  QQ4Mac
//
//  Created by nickdai on 15/11/24.
//  Copyright © 2015年 tencent. All rights reserved.
//
#include "MAVQuadRender.h"
#import "MAVQuadRenderUtils.h"

typedef struct tagVertex
{
    float Position[3];
    float TextureCoord[2];
}Vertex;

const Vertex vertices[4] = {
    {{ 1.0f, -1.0f, 0.0f}, {1.0f, 1.0f}},
    {{ 1.0f,  1.0f, 0.0f}, {1.0f, 0.0f}},
    {{-1.0f,  1.0f, 0.0f}, {0.0f, 0.0f}},
    {{-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},
};


static GLuint CreateVertexBuffer()
{
    GLuint vertexBuffer = 0;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(vertices),
                 &vertices[0],
                 GL_STATIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return vertexBuffer;
}

typedef float MAVTextureCoord[2];
static void SwapTextureCoord(MAVTextureCoord &tcrd1,MAVTextureCoord &tcrd2)
{
    float x = tcrd1[0];
    float y = tcrd1[1];
    tcrd1[0] = tcrd2[0];
    tcrd1[1] = tcrd2[1];
    tcrd2[0] = x;
    tcrd2[1] = y;
}
static void AssaignTextureCoord(MAVTextureCoord &tcrd1,MAVTextureCoord &tcrd2)
{
    tcrd1[0] = tcrd2[0];
    tcrd1[1] = tcrd2[1];
}


static void UpdateVertexBuffer(int vertexBuffer, int angle)
{
    Vertex r_vertices[4];
    memcpy(r_vertices, vertices, sizeof(vertices));
    // 180 上下翻转
    if(angle == 2)
    {
        SwapTextureCoord(r_vertices[0].TextureCoord, r_vertices[1].TextureCoord);
        SwapTextureCoord(r_vertices[2].TextureCoord, r_vertices[3].TextureCoord);
    }
    //90
    else if(angle == 3)
    {
        MAVTextureCoord savedCoord = {r_vertices[0].TextureCoord[0],r_vertices[0].TextureCoord[1]};
        AssaignTextureCoord(r_vertices[0].TextureCoord, r_vertices[3].TextureCoord);
        AssaignTextureCoord(r_vertices[3].TextureCoord, r_vertices[2].TextureCoord);
        AssaignTextureCoord(r_vertices[2].TextureCoord, r_vertices[1].TextureCoord);
        AssaignTextureCoord(r_vertices[1].TextureCoord, savedCoord);
    }
    //270
    else if(angle == 1)
    {
        MAVTextureCoord savedCoord = {r_vertices[0].TextureCoord[0],r_vertices[0].TextureCoord[1]};
        AssaignTextureCoord(r_vertices[0].TextureCoord, r_vertices[1].TextureCoord);
        AssaignTextureCoord(r_vertices[1].TextureCoord, r_vertices[2].TextureCoord);
        AssaignTextureCoord(r_vertices[2].TextureCoord, r_vertices[3].TextureCoord);
        AssaignTextureCoord(r_vertices[3].TextureCoord, savedCoord);
    }
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(r_vertices),
                 &r_vertices[0],
                 GL_STATIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}





static GLuint CreateIndexBuffer()
{
    const unsigned short indices[6] = {
        0, 1, 2,
        2, 3, 0
    };
    
    GLuint indexBuffer = 0;
    
    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(indices),
                 &indices[0],
                 GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    return indexBuffer;
}

static GLuint CreateTexture()
{
    GLuint textid;
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &textid);
    glBindTexture(GL_TEXTURE_2D, textid);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
    return textid;
}


static GLuint LoadShader(GLenum type, const char *shaderSrc)
{
    GLuint shader;
    GLint compiled;
    
    if(shaderSrc == NULL){
        return 0;
    }
    shader = glCreateShader(type);
    if(shader == 0){
        return 0;
    }
    
    glShaderSource(shader, 1, &shaderSrc, NULL);
    glCompileShader(shader);
    
    //检查错误
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if(!compiled)
    {
        GLint infoLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
        
        if(infoLen > 1)
        {
            char* infoLog = (char*)malloc(sizeof(char) * infoLen);
            
            glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
            NSLog(@"MAV Error compiling shader:\n%s\n", infoLog);
            
            free(infoLog);
        }
        
        glDeleteShader(shader);
        return 0;
    }
    
    return shader;
}


static GLuint CreateShaderProgram()
{
    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint programObject;
    GLint linked = 0;
    IResourceStringFileManager * fileManager = createResourceStringFileManager();
    if(fileManager == NULL){
        return 0;
    }
    const char * vShaderStr = fileManager->getFileContent(@"mav_standard_vertex.sh");
    const char * fShaderStr = fileManager->getFileContent(@"mav_standard_fragment.sh");
    
    vertexShader = LoadShader ( GL_VERTEX_SHADER, vShaderStr );
    fragmentShader = LoadShader ( GL_FRAGMENT_SHADER, fShaderStr );
    
    programObject = glCreateProgram ( );
    if ( programObject == 0 )
    {
        return 0;
    }
    //链接着色器程序
    glAttachShader ( programObject, vertexShader );
    glAttachShader ( programObject, fragmentShader );
    glLinkProgram ( programObject );
    
    //检查错误
    glGetProgramiv ( programObject, GL_LINK_STATUS, &linked );
    if ( !linked )
    {
        GLint infoLen = 0;
        
        glGetProgramiv ( programObject, GL_INFO_LOG_LENGTH, &infoLen );
        
        if ( infoLen > 1 )
        {
            char *infoLog = (char*)malloc ( sizeof ( char ) * infoLen );
            
            glGetProgramInfoLog ( programObject, infoLen, NULL, infoLog );
            NSLog(@"MAV Error linking program:\n%s\n", infoLog );
            
            free ( infoLog );
        }
        
        glDeleteProgram ( programObject );
        return FALSE;
    }
    
    return  programObject;
}

//VAO
static GLuint CreateInputLayout(GLuint shaderProgram)
{
    GLuint inputLayout = 0;
    
    glGenVertexArrays(1, &inputLayout);
    
    glBindVertexArray(inputLayout);
    
    auto const attributePosition = glGetAttribLocation(shaderProgram, "Position");
    auto const attributeTextureCoord = glGetAttribLocation(shaderProgram, "TextureCoord");
    
    glEnableVertexAttribArray(attributePosition);
    glEnableVertexAttribArray(attributeTextureCoord);
    
    auto const uniformTextureId = glGetUniformLocation(shaderProgram, "s_texture");
    glUniform1i(uniformTextureId, 0);
    
    // unbind
    glBindVertexArray(0);
    
    return inputLayout;
}

#pragma mark construct and unconstruct
MAVQuadRender::MAVQuadRender()
{
    vertexBuffer = CreateVertexBuffer();
    indexBuffer = CreateIndexBuffer();
    shaderProgram = CreateShaderProgram();
    inputLayout = CreateInputLayout(shaderProgram);
    textureId = CreateTexture();
    glDisable(GL_CULL_FACE);
}

MAVQuadRender::~MAVQuadRender()
{
    glDeleteVertexArrays(1, &inputLayout);
    glDeleteProgram(shaderProgram);
    glDeleteBuffers(1, &indexBuffer);
    glDeleteBuffers(1, &vertexBuffer);
}

static GLubyte const* BufferOffset(size_t byteWidth)
{
    return reinterpret_cast<GLubyte const*>(0) + byteWidth;
}

void MAVQuadRender::Draw()
{
    glBindVertexArray(inputLayout);
    {
        GLint attributePosition = glGetAttribLocation(shaderProgram, "Position");
        GLint attributeTextureCoord = glGetAttribLocation(shaderProgram, "TextureCoord");
        
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        
        GLsizei const componentsPosition = 3;
        GLsizei const componentsTextureCoord = 2;
        GLsizei const strideBytes = sizeof(Vertex);
        glVertexAttribPointer(attributePosition, componentsPosition, GL_FLOAT, GL_FALSE, strideBytes,BufferOffset(0));
        glVertexAttribPointer(attributeTextureCoord, componentsTextureCoord, GL_FLOAT, GL_FALSE, strideBytes, BufferOffset(sizeof(float) * 3));
        glVertexAttribDivisor(0, 0);
    }
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId);
    
    glUseProgram(shaderProgram);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    
    GLsizei const indexCount = 6;
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_SHORT, NULL);
}

void MAVQuadRender::updateTextureResource(unsigned char*rgb24, int width, int height, int angle,  int type)
{
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, rgb24);
    glBindTexture(GL_TEXTURE_2D, 0);
    UpdateVertexBuffer(vertexBuffer, angle);
    //用完释放
//    free(rgb24);
}



#define STB_IMAGE_IMPLEMENTATION
#include "GL/glew.h"
#include "Texture.h"
#include "stb_image.h"
#include <iostream>
#include "Window.h"
Texture::Texture(const char* path): filepath(path),m_RendererID(0), m_Width(0), m_Height(0), m_BPP(0) {
    GLCall(glGenTextures(1, &m_RendererID));
    GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));
    GLCall(stbi_set_flip_vertically_on_load(1));
    // set the texture wrapping/filtering options (on the currently bound texture object)
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

    // load and generate the texture
    data = stbi_load(path, &m_Width, &m_Height, &m_BPP, 0);
    if (data)
    {
        GLenum format = GL_RGB;
        if (m_BPP == 1) format = GL_RED;
        else if (m_BPP == 3) format = GL_RGB;
        else if (m_BPP == 4) format = GL_RGBA;
        GLCall(glTexImage2D(GL_TEXTURE_2D, 0, format, m_Width, m_Height, 0, format, GL_UNSIGNED_BYTE, data));
        GLCall(glGenerateMipmap(GL_TEXTURE_2D));
    }
    else
    {
        std::cout << "Failed to load texture : " << filepath << std::endl;
    }
    stbi_image_free(data);

};
Texture::~Texture(){
    if (m_RendererID) GLCall(glDeleteTextures(1, &m_RendererID));
};

Texture::Texture(Texture&& other) noexcept
    : m_RendererID(other.m_RendererID), filepath(other.filepath),
      data(nullptr), m_Width(other.m_Width), m_Height(other.m_Height), m_BPP(other.m_BPP) {
    other.m_RendererID = 0;
}

Texture& Texture::operator=(Texture&& other) noexcept {
    if (this != &other) {
        if (m_RendererID) glDeleteTextures(1, &m_RendererID);
        m_RendererID = other.m_RendererID;
        filepath = other.filepath;
        data = nullptr;
        m_Width = other.m_Width;
        m_Height = other.m_Height;
        m_BPP = other.m_BPP;
        other.m_RendererID = 0;
    }
    return *this;
}
void Texture::Bind(unsigned int slot) const{
    GLCall(glActiveTexture(GL_TEXTURE0 + slot));
    GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));
};
void Texture::Unbind() const{
    GLCall(glBindTexture(GL_TEXTURE_2D, 0));
};



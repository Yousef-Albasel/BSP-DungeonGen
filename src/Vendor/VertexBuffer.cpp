#include "GL/glew.h"
#include "VertexBuffer.h"
#include "Window.h"

VertexBuffer::VertexBuffer(const void* data, unsigned int size){
    GLCall(glGenBuffers(1, &m_RendererID));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));

    GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));

};
VertexBuffer::~VertexBuffer(){
    if (m_RendererID) GLCall(glDeleteBuffers(1, &m_RendererID));
};


VertexBuffer::VertexBuffer(VertexBuffer&& other) noexcept
    : m_RendererID(other.m_RendererID) {
    other.m_RendererID = 0;
}

VertexBuffer& VertexBuffer::operator=(VertexBuffer&& other) noexcept {
    if (this != &other) {
        if (m_RendererID) glDeleteBuffers(1, &m_RendererID);
        m_RendererID = other.m_RendererID;
        other.m_RendererID = 0;
    }
    return *this;
}

void VertexBuffer::Bind() const{
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
};
void VertexBuffer::Unbind() const{
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
};
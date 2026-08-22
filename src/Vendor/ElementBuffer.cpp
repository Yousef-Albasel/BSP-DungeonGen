#include "GL/glew.h"
#include "ElementBuffer.h"
#include "Window.h"

ElementBuffer::ElementBuffer(const void* data, unsigned int size) {
    m_Count = size / sizeof(unsigned int);
    GLCall(glGenBuffers(1, &m_RendererID));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));

    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));

};
ElementBuffer::~ElementBuffer() {
    if (m_RendererID) GLCall(glDeleteBuffers(1, &m_RendererID));
};

ElementBuffer::ElementBuffer(ElementBuffer&& other) noexcept
    : m_RendererID(other.m_RendererID), m_Count(other.m_Count) {
    other.m_RendererID = 0;
    other.m_Count = 0;
}

ElementBuffer& ElementBuffer::operator=(ElementBuffer&& other) noexcept {
    if (this != &other) {
        if (m_RendererID) glDeleteBuffers(1, &m_RendererID);
        m_RendererID = other.m_RendererID;
        m_Count = other.m_Count;
        other.m_RendererID = 0;
        other.m_Count = 0;
    }
    return *this;
}

void ElementBuffer::Bind() const {
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));
};
void ElementBuffer::Unbind() const {
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
};
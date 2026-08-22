#pragma once
#include "VertexBuffer.h"

class VertexBufferLayout;
class VertexArray {
private:
	unsigned int m_RendererID; 

public:
	VertexArray();
	~VertexArray();

	// Non-copyable, movable
	VertexArray(const VertexArray&) = delete;
	VertexArray& operator=(const VertexArray&) = delete;
	VertexArray(VertexArray&& other) noexcept;
	VertexArray& operator=(VertexArray&& other) noexcept;

	void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& vl);
	void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& vl, int instanceDivisor); 

	void Bind() const;
	void Unbind() const;
};
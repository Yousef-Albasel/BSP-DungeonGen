#pragma once

#include <vector>
#include "vendor/VertexArray.h"
#include "vendor/VertexBuffer.h"
#include "vendor/VertexBufferLayout.h"
#include "vendor/ElementBuffer.h"

class Mesh
{
private:
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    VertexArray VAO;
    VertexBuffer VBO;
    ElementBuffer EBO;

public:
    Mesh() = default;
    Mesh(    const std::vector<float>& vertices,
    const std::vector<unsigned int>& indices
    );

    const std::vector<float>& getVertices() const
    {
        return vertices;
    }

    const std::vector<unsigned int>& getIndices() const
    {
        return indices;
    }
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    // Movable
    Mesh(Mesh&&) noexcept = default;
    Mesh& operator=(Mesh&&) noexcept = default;

    void Bind() const;
    void Unbind() const;

    unsigned int GetIndexCount() const;
};
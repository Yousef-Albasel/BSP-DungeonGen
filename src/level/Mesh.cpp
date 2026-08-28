#include "Mesh.h"

Mesh::Mesh(
    const std::vector<float>& vertices,
    const std::vector<unsigned int>& indices
)
    : vertices(vertices),
      indices(indices),
      VAO(),
      VBO(vertices.data(), vertices.size() * sizeof(float)),
      EBO(indices.data(), indices.size() * sizeof(unsigned int))
{
    VertexBufferLayout layout;
    layout.Push<float>(3);

    VAO.AddBuffer(VBO, layout);
}

Mesh::Mesh(
    const std::vector<float>& vertices,
    const std::vector<unsigned int>& indices,
    const VertexBufferLayout& layout
    )
    : vertices(vertices),
      indices(indices),
      VAO(),
      VBO(vertices.data(), vertices.size() * sizeof(float)),
      EBO(indices.data(), indices.size() * sizeof(unsigned int))
{
    VAO.Bind();
    VBO.Bind();
    EBO.Bind();

    VAO.AddBuffer(VBO, layout);

    VAO.Unbind();
}


void Mesh::Bind() const
{
    VAO.Bind();
    EBO.Bind();
}

void Mesh::Unbind() const
{
    EBO.Unbind();
    VAO.Unbind();
}

unsigned int Mesh::GetIndexCount() const
{
    return EBO.GetCount();
}
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
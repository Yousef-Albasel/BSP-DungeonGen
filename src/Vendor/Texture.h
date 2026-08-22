#pragma once


class Texture {

private:
    unsigned int m_RendererID;
    const char* filepath;
    unsigned char* data;
    int m_Width, m_Height, m_BPP;

public:
	Texture(const char* path);
	~Texture();

	// Non-copyable, movable
	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;
	Texture(Texture&& other) noexcept;
	Texture& operator=(Texture&& other) noexcept;
    void Bind(unsigned int slot = 0) const;
    void Unbind() const;

};


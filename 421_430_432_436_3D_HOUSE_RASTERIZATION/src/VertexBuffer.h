#pragma once

class VertexBuffer
{
private:
    unsigned int m_RendererID;

public:
    // Data for the data of vertex buffer and size is the size of it
    VertexBuffer(const void *data, unsigned int size);

	//~VertexBuffer();

	void Bind() const;

	void Unbind() const;
};

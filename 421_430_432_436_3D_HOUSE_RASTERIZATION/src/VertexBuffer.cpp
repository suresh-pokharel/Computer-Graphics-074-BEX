#include "includes.h"
#include "VertexBuffer.h"



VertexBuffer::VertexBuffer(const void *data,  unsigned int size)
{
	glGenBuffers(1, &m_RendererID);
	Bind();
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}


//
//VertexBuffer::~VertexBuffer() 
//{
//	glDeleteBuffers(1, &m_RendererID);
//}

	void VertexBuffer::Bind() const 
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	}

void VertexBuffer::Unbind() const 
{ 
	glBindBuffer(GL_ARRAY_BUFFER, 0); 
}


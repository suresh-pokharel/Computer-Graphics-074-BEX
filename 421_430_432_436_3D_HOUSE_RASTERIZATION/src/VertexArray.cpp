#include "VertexArray.h"

VertexArray::VertexArray()
{
	glGenVertexArrays(1, &m_RendererID);
}

//VertexArray::~VertexArray()
//{
//	glDeleteVertexArrays(1, &m_RendererID);
//}
void VertexArray::AddBuffer(const VertexBuffer &vb, const VertexBufferLayout &layout) 
{
	Bind();
    vb.Bind();
	// position attribute
	/*glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);*/
    const auto &elements = layout.GetElements();
    unsigned int offset = 0;
    for (unsigned int i = 0; i < elements.size(); ++i)
    {
		const VertexBufferElement& element = elements[i];
        glVertexAttribPointer(i, element.count, element.type,
			element.normalized, layout.GetStride(), reinterpret_cast<void *>(offset)); 
		glEnableVertexAttribArray(i);
        offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
    }
}

void VertexArray::Bind() const
{
	glBindVertexArray(m_RendererID);
}

void VertexArray::Unbind() const
{
	glBindVertexArray(0);
}

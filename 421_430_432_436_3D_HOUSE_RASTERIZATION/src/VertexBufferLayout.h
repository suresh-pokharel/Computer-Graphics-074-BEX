#pragma once

#include "includes.h"
#include <vector>

struct VertexBufferElement
{
    unsigned int type;
    // Number of floats defining the position
    unsigned int count;
    /*
    GL_TRUE is 1 and
    GL_FALSE is 0
    */
    bool normalized;

    // Returns the size of given type
    static unsigned int GetSizeOfType(unsigned int type) 
    {
        switch (type)
        {
        case GL_FLOAT:		   return sizeof(float);
        case GL_UNSIGNED_INT:  return sizeof(unsigned int);
        case GL_UNSIGNED_BYTE: return sizeof(unsigned char);
        default:
            throw std::invalid_argument("Type is not among float, unsigned int or unsigned char");
        }
    }
};

class VertexBufferLayout
{
private:
	std::vector<VertexBufferElement> m_Elements;
	unsigned int m_Stride;

public:
	VertexBufferLayout() 
		: m_Stride(0) {}



	template <typename T>
	void Push(unsigned int count)
	{
		static_assert(false); // we don't handle this
	}

	template <>
	void Push<float>(unsigned int count)
	{
		m_Elements.push_back({ GL_FLOAT, count, GL_FALSE });
		//m_Stride += count * VertexBufferElement::GetSizeOfType(GL_FLOAT);
		m_Stride += count * sizeof(float);
	}

	template <>
	void Push<unsigned int>(unsigned int count)
	{
		m_Elements.push_back({GL_UNSIGNED_INT, count, GL_FALSE });
		//m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT);
		m_Stride += count * sizeof(unsigned int);
	}

	template <>
	void Push<unsigned char>(unsigned int count)
	{
		m_Elements.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE });
		//m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE);
		m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE);
	}

	inline const std::vector<VertexBufferElement> GetElements() const { return m_Elements; }
	inline unsigned int GetStride() const { return m_Stride; }
};
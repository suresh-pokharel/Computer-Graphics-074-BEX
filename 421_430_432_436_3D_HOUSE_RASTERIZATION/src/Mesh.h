#pragma once

#include "includes.h"
#include <vector>
#include "Shader.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"

struct Vertex {
	// position
	glm::vec3 Position;
	// normal
	glm::vec3 Normal;
	// texCoords
	glm::vec2 TexCoords;
	// tangent
	glm::vec3 Tangent;
	// bitangent
	glm::vec3 Bitangent;

	glm::vec3 colorambient;
	glm::vec3 colordiffuse;
	glm::vec3 colorspecular;
	float shininess;
};

struct Texture {
	unsigned int id;
	std::string type;
	std::string path;
	unsigned int nrComponents;
};

class Mesh {
public:
	/*  Mesh Data  */

	std::vector<Vertex> m_vertices;
	std::vector<unsigned int> m_indices;
	std::vector<Texture> m_textures;
	

	/*  Functions  */
	// constructor
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
		:m_vertices(vertices),
		m_indices(indices), 
		m_textures(textures),
		m_va(),
		m_ib(&m_indices[0], indices.size()),
		m_vb(&m_vertices[0], vertices.size() * sizeof(Vertex))

	{
		// now that we have all the required data, set the vertex buffers and its attribute pointers.	
		setupMesh();
	}

	// render the mesh
	void Draw(Shader shader)
	{
		// bind appropriate textures
		unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;
		unsigned int normalNr = 1;
		unsigned int heightNr = 1;
		for (unsigned int i = 0; i < m_textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
			// retrieve texture number (the N in diffuse_textureN)
			std::string number;
			std::string name = m_textures[i].type;
			if (name == "texture_diffuse")
			{
				number = std::to_string(diffuseNr++); glUniform1i(glGetUniformLocation(shader.ID, "nrComponents_d"), m_textures[i].nrComponents);
			}
			else if (name == "texture_specular")
			{
				number = std::to_string(specularNr++); glUniform1i(glGetUniformLocation(shader.ID, "nrComponents_sp"), m_textures[i].nrComponents); // transfer unsigned int to stream
			}
			else if (name == "texture_normal")
				number = std::to_string(normalNr++); // transfer unsigned int to stream
			else if (name == "texture_height")
				number = std::to_string(heightNr++); // transfer unsigned int to stream

			// now set the sampler to the correct texture unit
			glUniform1i(glGetUniformLocation(shader.ID, ("material." + name + number).c_str()), i);
			// and finally bind the texture
			glBindTexture(GL_TEXTURE_2D, m_textures[i].id);
		}
		//cout << diffuseNr << "  " << specularNr << endl;
		glUniform1i(glGetUniformLocation(shader.ID, "diffuseN"), diffuseNr - 1);
		glUniform1i(glGetUniformLocation(shader.ID, "specularN"), specularNr - 1);
		m_va.Bind();
		m_ib.Bind();
		glDrawElements(GL_TRIANGLES, m_ib.GetCount(), GL_UNSIGNED_INT, nullptr);
		m_va.Unbind();
	

	}

private:
	/*  Render data  */
	/*unsigned int VBO, EBO;*/
	VertexBuffer m_vb;
	IndexBuffer m_ib;
	VertexArray m_va;
	/*  Functions    */
	// initializes all the buffer objects/arrays
	void setupMesh()
	{
		VertexBufferLayout layout;

		layout.Push<float>(3);//Positions
		layout.Push<float>(3);//Normals
		layout.Push<float>(2);//TexCoordinates
		layout.Push<float>(3);//Tangents
		layout.Push<float>(3);//Bitangents
		layout.Push<float>(3);//colorambient;
		layout.Push<float>(3);//colordiffuse;
		layout.Push<float>(3); //colorspecular;
		layout.Push<float>(1);//shininess;
		
		m_va.AddBuffer(m_vb, layout);
		
	}
};


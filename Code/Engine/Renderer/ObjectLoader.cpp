//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Renderer/ObjectLoader.hpp"
#include "Engine/Commons/StringUtils.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Math/Vertex_Lit.hpp"
#include "Engine/Renderer/CPUMesh.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include <vector>
#include <fstream>

//------------------------------------------------------------------------------------------------------------------------------
ObjectLoader::ObjectLoader()
{

}

//------------------------------------------------------------------------------------------------------------------------------
ObjectLoader* ObjectLoader::CreateMeshFromFile(RenderContext* renderContext, const std::string& fileName)
{
	ObjectLoader* object = new ObjectLoader();
	object->m_renderContext = renderContext;
	
	//Check the file extention
	std::vector<std::string> strings = SplitStringOnDelimiter(fileName, '.');
	bool isDataDriven = false;
	if (strings.size() > 1)
	{
		if (strings[(strings.size() - 1)] == "mesh")
		{
			isDataDriven = true;
		}
	}

	//Open file and see what it says
	char* outData = nullptr;
	unsigned long bufferSize;
	if (isDataDriven)
	{
		//Load the models from xml;
	}
	else
	{
		//bufferSize = CreateFileTextBuffer(fileName, &outData);
		//object->CreateFromString(outData);

		object->CreateFromString(fileName.c_str());
	}

	return object;
}

//------------------------------------------------------------------------------------------------------------------------------
void ObjectLoader::CreateFromString(const char* data)
{
	std::ifstream fileStream;
	fileStream.open(data, std::ios::beg);
	//unsigned long fileSize = (unsigned long)fileStream.tellg();
	//Get the cursor back to the start of the file so we read the whole thing as opposed to reading nothing lol
	//fileStream.seekg(std::ios::beg);

	//char* outData = new char[fileSize];
	//fileStream.read(outData, fileSize);

	std::string lineString;
	
	
	//std::vector<std::string> lines = SplitStringOnDelimiter(std::string(data), '\n');
	
	while (std::getline(fileStream, lineString))
	{
		if (lineString.size() == 0 || lineString[0] == '#')
		{
			continue;
		}
		else if (lineString[0] == 'v' && lineString[1] == ' ')
		{
			//Read the vertex
			std::vector<std::string> tokens = SplitStringOnDelimiter(lineString, ' ');

			for (int i = 0; i < tokens.size(); i++)
			{
				if (tokens[i] == "" || tokens[i] == "v")
				{
					tokens.erase(tokens.begin() + i);
					i = -1;
				}
			}

			Vec3 value;
			std::string valueString(tokens[0]);
			valueString += ",";
			valueString += std::string(tokens[1]);
			valueString += ",";
			valueString += std::string(tokens[2]);

			value.SetFromText(valueString.c_str());
			DebuggerPrintf(std::to_string(value.x).c_str());
			DebuggerPrintf(std::to_string(value.y).c_str());
			DebuggerPrintf(std::to_string(value.z).c_str());

			m_positions.push_back(value);
		}
		else if (lineString[0] == 'v' && lineString[1] == 'n')
		{
			// read the normal
			std::vector<std::string> tokens = SplitStringOnDelimiter(lineString, ' ');

			Vec3 value;
			std::string valueString(tokens[1]);
			valueString += ",";
			valueString += std::string(tokens[2]);
			valueString += ",";
			valueString += std::string(tokens[3]);

			value.SetFromText(valueString.c_str());
			DebuggerPrintf(std::to_string(value.x).c_str());
			DebuggerPrintf(std::to_string(value.y).c_str());
			DebuggerPrintf(std::to_string(value.z).c_str());

			m_normals.push_back(value);
		}
		else if (lineString[0] == 'v' && lineString[1] == 't')
		{
			//Read the uv
			std::vector<std::string> tokens = SplitStringOnDelimiter(lineString, ' ');

			Vec2 value;
			std::string valueString(tokens[1]);
			valueString += ",";
			valueString += std::string(tokens[2]);

			value.SetFromText(valueString.c_str());
			value.y = 1 - value.y;
			DebuggerPrintf(std::to_string(value.x).c_str());
			DebuggerPrintf(std::to_string(value.y).c_str());

			m_uvs.push_back(value);
		}
		else if (lineString[0] == 'f')
		{
			//Read index for the face
			std::vector<std::string> tokens = SplitStringOnDelimiter(lineString, ' ');

			for (int i = 0; i < tokens.size(); i++)
			{
				if (tokens[i] == "" || tokens[i] == "f")
				{
					tokens.erase(tokens.begin() + i);
				}
			}

			int numIndices = (int)tokens.size();

			if (numIndices == 3)
			{
				//We have a tri so this is EZ
				this->AddIndexForMesh(tokens[0]);
				this->AddIndexForMesh(tokens[1]);
				this->AddIndexForMesh(tokens[2]);
			}
			else if (numIndices == 4)
			{
				//Bruh we have quad so better plit into tris
				this->AddIndexForMesh(tokens[0]);
				this->AddIndexForMesh(tokens[1]);
				this->AddIndexForMesh(tokens[2]);

				this->AddIndexForMesh(tokens[0]);
				this->AddIndexForMesh(tokens[2]);
				this->AddIndexForMesh(tokens[3]);
			}
			else
			{
				ERROR_AND_DIE("Obj file contains an n-gon");
			}
		}

	}

	/*
	for (int lineIndex = 0; lineIndex < (int)lines.size(); lineIndex++)
	{
		//Ignore all comments
		if (lines[lineIndex].size() == 0 || lines[lineIndex][0] == '#')
		{
			continue;
		}
		else if (lines[lineIndex][0] == 'v' && lines[lineIndex][1] == ' ')
		{
			//Read the vertex
			std::vector<std::string> tokens = SplitStringOnDelimiter(lines[lineIndex], ' ');

			Vec3 value;
			std::string valueString(tokens[2]);
			valueString += ",";
			valueString += std::string(tokens[3]);
			valueString += ",";
			valueString += std::string(tokens[4]);

			value.SetFromText(valueString.c_str());
			DebuggerPrintf(std::to_string(value.x).c_str());
			DebuggerPrintf(std::to_string(value.y).c_str());
			DebuggerPrintf(std::to_string(value.z).c_str());
			
			m_positions.push_back(value);
		}
		else if(lines[lineIndex][0] == 'v' && lines[lineIndex][1] == 'n')
		{
			// read the normal
			std::vector<std::string> tokens = SplitStringOnDelimiter(lines[lineIndex], ' ');

			Vec3 value;
			std::string valueString(tokens[1]);
			valueString += ",";
			valueString += std::string(tokens[2]);
			valueString += ",";
			valueString += std::string(tokens[3]);

			value.SetFromText(valueString.c_str());
			DebuggerPrintf(std::to_string(value.x).c_str());
			DebuggerPrintf(std::to_string(value.y).c_str());
			DebuggerPrintf(std::to_string(value.z).c_str());

			m_normals.push_back(value);
		}
		else if (lines[lineIndex][0] == 'v' && lines[lineIndex][1] == 't')
		{
			//Read the uv
			std::vector<std::string> tokens = SplitStringOnDelimiter(lines[lineIndex], ' ');

			Vec2 value;
			std::string valueString(tokens[1]);
			valueString += ",";
			valueString += std::string(tokens[2]);

			value.SetFromText(valueString.c_str());
			value.y = 1 - value.y;
			DebuggerPrintf(std::to_string(value.x).c_str());
			DebuggerPrintf(std::to_string(value.y).c_str());

			m_uvs.push_back(value);
		}
		else if (lines[lineIndex][0] == 'f')
		{
			//Read index for the face
			std::vector<std::string> tokens = SplitStringOnDelimiter(lines[lineIndex], ' ');

			int numIndices = (int)tokens.size() - 2;

			if (numIndices == 3)
			{
				//We have a tri so this is EZ
				this->AddIndexForMesh(tokens[1]);
				this->AddIndexForMesh(tokens[3]);
				this->AddIndexForMesh(tokens[2]);
			}
			else if (numIndices == 4)
			{
				//Bruh we have quad so better plit into tris
				this->AddIndexForMesh(tokens[1]);
				this->AddIndexForMesh(tokens[2]);
				this->AddIndexForMesh(tokens[3]);

				this->AddIndexForMesh(tokens[1]);
				this->AddIndexForMesh(tokens[3]);
				this->AddIndexForMesh(tokens[4]);
			}
			else
			{
				ERROR_AND_DIE("Obj file contains an n-gon");
			}


		}
	}
	*/

	CreateGPUMesh();
}

//------------------------------------------------------------------------------------------------------------------------------
void ObjectLoader::AddIndexForMesh(const std::string& indices)
{
	std::vector<std::string> values = SplitStringOnDelimiter(indices, '/');

	ObjIndex idx;
	idx.vertexIndex = atoi(values[0].c_str()) - 1;
	idx.uvIndex = atoi(values[1].c_str()) - 1;
	idx.normalIndex = atoi(values[2].c_str()) - 1;

	m_indices.push_back(idx);

	/*
	for (int i = 0; i < 3; i++)
	{
		
	}
	*/
}

//------------------------------------------------------------------------------------------------------------------------------
void ObjectLoader::CreateGPUMesh()
{
	int numIndices = (int)m_indices.size();

	std::vector<VertexMaster> vertices;
	std::vector<uint> indices;
	for (int index = 0; index < numIndices; index++)
	{
		VertexMaster vertex;

		vertex.m_position = m_positions[m_indices[index].vertexIndex];
		vertex.m_normal = m_normals[m_indices[index].normalIndex];
		vertex.m_uv = m_uvs[m_indices[index].uvIndex];

		vertices.push_back(vertex);
		indices.push_back(index);

	}

	m_mesh = new GPUMesh(m_renderContext);
	m_mesh->CopyVertexArray<Vertex_Lit>(vertices[0], (uint)vertices.size());
	m_mesh->CopyIndices(&indices[0], (uint)indices.size());
}

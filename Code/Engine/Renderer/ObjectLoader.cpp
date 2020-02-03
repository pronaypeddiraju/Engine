//------------------------------------------------------------------------------------------------------------------------------
#include "Engine/Renderer/ObjectLoader.hpp"
#include "Engine/Commons/StringUtils.hpp"
#include "Engine/Core/EventSystems.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/NamedProperties.hpp"
#include "Engine/Core/XMLUtils/XMLUtils.hpp"
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
ObjectLoader::~ObjectLoader()
{
	if (m_mesh != nullptr)
	{
		delete m_mesh;
	}
}

//------------------------------------------------------------------------------------------------------------------------------
STATIC ObjectLoader* ObjectLoader::CreateMeshFromFile(RenderContext* renderContext, const std::string& fileName, bool isDataDriven)
{
	ObjectLoader* object = new ObjectLoader();
	object->m_renderContext = renderContext;

	DebuggerPrintf("Loading: %s\n", fileName.c_str());

	//Open file and see what it says
	if (isDataDriven)
	{
		//Load the models from xml;
		object->LoadFromXML(fileName);
		object->CreateGPUMesh();
	}
	else
	{
		object->CreateFromString(fileName.c_str());
		object->CreateCPUMesh();
		object->CreateGPUMesh();
	}

	return object;
}

//------------------------------------------------------------------------------------------------------------------------------
void ObjectLoader::LoadFromXML(const std::string& fileName)
{
	//Open the xml file and parse it
	tinyxml2::XMLDocument meshDoc;
	meshDoc.LoadFile(fileName.c_str());

	if (meshDoc.ErrorID() != tinyxml2::XML_SUCCESS)
	{
	
		ERROR_AND_DIE(">> Error loading Mesh XML file ");
		return;
	}
	else
	{
		//We loaded the file successfully
		XMLElement* root = meshDoc.RootElement();

		if (root->FindAttribute("src"))
		{
			m_source = ParseXmlAttribute(*root, "src", m_source);
		}
		
		if (root->FindAttribute("invert"))
		{
			m_invert = ParseXmlAttribute(*root, "invert", false);
		}

		if (root->FindAttribute("tangents"))
		{
			m_tangents = ParseXmlAttribute(*root, "tangents", false);
		}

		if (root->FindAttribute("scale"))
		{
			m_scale = ParseXmlAttribute(*root, "scale", 1.f);
		}
		
		m_transform = ParseXmlAttribute(*root, "transform", "");

		CreateFromString((MODEL_PATH + m_source).c_str());
		CreateCPUMesh();

		XMLElement* elem = root->FirstChildElement("material");
		if (elem != nullptr)
		{
			//Set the default material path for this model from XML
			m_defaultMaterialPath = ParseXmlAttribute(*elem, "src", "");
		}

		elem = root->FirstChildElement("collision");
		while (elem != nullptr)
		{
			//We requested to create a static collider with this model so generate that using the PhysX System
			NamedProperties eventArgs;
			eventArgs.SetValue("id", ParseXmlAttribute(*root, "id", ""));
			eventArgs.SetValue("src", ParseXmlAttribute(*elem, "src", ""));

			g_eventSystem->FireEvent("ReadCollisionMeshFromData", eventArgs);

			elem = elem->NextSiblingElement("collision");
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void ObjectLoader::CreateFromString(const char* data)
{
	std::ifstream fileStream;
	fileStream.open(data, std::ios::beg);

	std::string lineString;
	
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

			for (int i = 0; i < (int)tokens.size(); i++)
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

			m_uvs.push_back(value);
		}
		else if (lineString[0] == 'f')
		{
			//Read index for the face
			std::vector<std::string> tokens = SplitStringOnDelimiter(lineString, ' ');

			for (int i = 0; i < (int)tokens.size(); i++)
			{
				if (tokens[i] == "" || tokens[i] == "f")
				{
					tokens.erase(tokens.begin() + i);
				}
			}

			int numIndices = (int)tokens.size();

			if (numIndices == 3)
			{
				if (!m_invert)
				{
					//We have a tri so this is EZ
					this->AddIndexForMesh(tokens[0]);
					this->AddIndexForMesh(tokens[1]);
					this->AddIndexForMesh(tokens[2]);
				}
				else
				{
					//We have a tri so this is EZ
					this->AddIndexForMesh(tokens[0]);
					this->AddIndexForMesh(tokens[2]);
					this->AddIndexForMesh(tokens[1]);
				}
			}
			else if (numIndices == 4)
			{
				if (!m_invert)
				{
					//We have quad so better plit into tris
					this->AddIndexForMesh(tokens[0]);
					this->AddIndexForMesh(tokens[1]);
					this->AddIndexForMesh(tokens[2]);

					this->AddIndexForMesh(tokens[0]);
					this->AddIndexForMesh(tokens[2]);
					this->AddIndexForMesh(tokens[3]);
				}
				else
				{
					//We have quad so better plit into tris
					this->AddIndexForMesh(tokens[0]);
					this->AddIndexForMesh(tokens[2]);
					this->AddIndexForMesh(tokens[1]);

					this->AddIndexForMesh(tokens[0]);
					this->AddIndexForMesh(tokens[3]);
					this->AddIndexForMesh(tokens[2]);
				}
			}
			else
			{
				DebuggerPrintf("Obj file contains an n-gon \n");
				if (!m_invert)
				{
					//We have 5 face so better plit into tris
					this->AddIndexForMesh(tokens[0]);
					this->AddIndexForMesh(tokens[1]);
					this->AddIndexForMesh(tokens[2]);

					this->AddIndexForMesh(tokens[0]);
					this->AddIndexForMesh(tokens[2]);
					this->AddIndexForMesh(tokens[3]);

					this->AddIndexForMesh(tokens[0]);
					this->AddIndexForMesh(tokens[3]);
					this->AddIndexForMesh(tokens[4]);
				}
				else
				{
					//We have 5 face so better plit into tris
					this->AddIndexForMesh(tokens[0]);
					this->AddIndexForMesh(tokens[2]);
					this->AddIndexForMesh(tokens[1]);

					this->AddIndexForMesh(tokens[0]);
					this->AddIndexForMesh(tokens[3]);
					this->AddIndexForMesh(tokens[2]);

					this->AddIndexForMesh(tokens[0]);
					this->AddIndexForMesh(tokens[4]);
					this->AddIndexForMesh(tokens[3]);
				}
			}
		}

	}
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

}

//------------------------------------------------------------------------------------------------------------------------------
void ObjectLoader::CreateCPUMesh()
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

	Matrix44 mat = Matrix44::IDENTITY;
	Vec3 vectors[3];

	//Setup transforms and scale
	if (m_transform != "")
	{
		bool negative = false;
		std::vector<std::string> tokens = SplitStringOnDelimiter(m_transform, ' ');
		for (int i = 0; i < 3; i++)
		{
			if (tokens[i].find('-') != std::string::npos)
			{
				//We did find a -ve
				negative = true;
			}

			if (tokens[i].find('x') != std::string::npos)
			{
				//We are x
				vectors[i] = (negative) ? Vec3(-1.0f, 0.f, 0.f) : Vec3(1.0f, 0.f, 0.f);
			}
			else if (tokens[i].find('y') != std::string::npos)
			{
				//We are x
				vectors[i] = (negative) ? Vec3(0.0f, -1.f, 0.f) : Vec3(0.0f, 1.f, 0.f);
			}
			else if (tokens[i].find('z') != std::string::npos)
			{
				//We are x
				vectors[i] = (negative) ? Vec3(0.0f, 0.f, -1.f) : Vec3(0.0f, 0.f, 1.f);
			}

			if (m_scale != 0.f)
			{
				vectors[i] *= m_scale;
			}
		}

		mat.SetIBasis(vectors[0]);
		mat.SetJBasis(vectors[1]);
		mat.SetKBasis(vectors[2]);

		for (int vertexIndex = 0; vertexIndex < (int)vertices.size(); vertexIndex++)
		{
			vertices[vertexIndex].m_position = mat.TransformPosition3D(vertices[vertexIndex].m_position);
			vertices[vertexIndex].m_normal = mat.TransformPosition3D(vertices[vertexIndex].m_normal);
		}

	}

	m_cpuMesh = new CPUMesh();
	for (int vertIndex = 0; vertIndex < (int)vertices.size(); ++vertIndex)
	{
		m_cpuMesh->AddVertex(vertices[vertIndex]);
		m_cpuMesh->AddIndex(vertIndex);
	}
}

//------------------------------------------------------------------------------------------------------------------------------
void ObjectLoader::CreateGPUMesh()
{
	m_mesh = new GPUMesh(m_renderContext);
	m_mesh->CreateFromCPUMesh<Vertex_Lit>(m_cpuMesh);
	m_mesh->m_defaultMaterial = m_defaultMaterialPath;
}
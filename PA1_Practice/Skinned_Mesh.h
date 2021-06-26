#ifndef SKINNED_MESH_H
#define	SKINNED_MESH_H

#include <map>
#include <unordered_map>
#include <vector>
#include <glm/gtx/transform.hpp>
#include<glm/gtx/string_cast.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>    
#include <assimp/postprocess.h> 
#include <iostream>
#include "ogldev_math_3d.h"
#include "MemUtil.h"
#include "Material.h"
#include "stb_image.h"
#include "Shader.h"

using namespace glm;

// Utilities for converting between ASSIMP and GLM
static inline glm::vec3 Vector3f_cast(const aiVector3D &v) { return glm::vec3(v.x, v.y, v.z); }
static inline glm::vec2 vec2_cast(const aiVector3D &v) { return glm::vec2(v.x, v.y); }
static inline glm::quat quat_cast(const aiQuaternion &q) { return glm::quat(q.w, q.x, q.y, q.z); }
static inline glm::mat4 Matrix4f_cast(const aiMatrix4x4 &m) { return glm::transpose(glm::make_mat4(&m.a1)); }
static inline glm::mat4 Matrix4f_cast(const aiMatrix3x3 &m) { return glm::transpose(glm::make_mat3(&m.a1)); }

class SkinnedMesh
{
public:



	SkinnedMesh();

	~SkinnedMesh();

	bool LoadMesh(const std::string& Filename);
	void setMaterial(Material M) { extraMaterial = M; }
	void Render(GLuint primitiveType, Shader & shader, bool overrideModelTexture = false);

	uint NumBones() const
	{
		return m_NumBones;
	}

	void BoneTransform(float TimeInSeconds, std::vector<Matrix4f>& Transforms);

private:

	Material extraMaterial;

#define NUM_BONES_PER_VERTEX 4

	struct BoneInfo
	{
		Matrix4f BoneOffset;
		Matrix4f FinalTransformation;

		BoneInfo()
		{
			BoneOffset.SetZero();
			FinalTransformation.SetZero();
		}
	};

	struct VertexBoneData
	{
		uint IDs[NUM_BONES_PER_VERTEX];
		float Weights[NUM_BONES_PER_VERTEX];
		VertexBoneData()
		{
			Reset();
		};

		void Reset()
		{
			ZERO_MEM(IDs);
			ZERO_MEM(Weights);
		}

		void AddBoneData(uint BoneID, float Weight);
	};

	void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	uint FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
	uint FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
	uint FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
	const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const  std::string NodeName);
	void ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const Matrix4f& ParentTransform);
	bool InitFromScene(const aiScene* pScene, const  std::string& Filename);
	void InitMesh(uint MeshIndex,
		const aiMesh* paiMesh,
		std::vector<Vector3f>& Positions,
		std::vector<Vector3f>& Normals,
		std::vector<Vector2f>& TexCoords,
		std::vector<VertexBoneData>& Bones,
		std::vector<unsigned int>& Indices);
	void LoadBones(uint MeshIndex, const aiMesh* paiMesh, std::vector<VertexBoneData>& Bones);
	bool InitMaterials(const aiScene* pScene, const  std::string& Filename);
	void Clear();
	
	
#define INVALID_MATERIAL 0xFFFFFFFF

	enum VB_TYPES {
		INDEX_BUFFER,
		POS_VB,
		NORMAL_VB,
		TEXCOORD_VB,
		BONE_VB,
		NUM_VBs
	};

	GLuint m_VAO;
	GLuint m_Buffers[NUM_VBs];

	struct MeshEntry {
		MeshEntry()
		{
			NumIndices = 0;
			BaseVertex = 0;
			BaseIndex = 0;
			MaterialIndex = INVALID_MATERIAL;
		}

		unsigned int NumIndices;
		unsigned int BaseVertex;
		unsigned int BaseIndex;
		unsigned int MaterialIndex;
		Material Mat;
		unsigned int texture_diffuse_ID;
		unsigned int texture_specular_ID;
		unsigned int texture_normal_ID;

		bool loadInOurTexture(std::string path, unsigned int & IDToSet) {

			glGenTextures(1, &IDToSet);
			glBindTexture(GL_TEXTURE_2D, IDToSet);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


			//path.insert(0, "../Textures/");
			const char * filePath = path.c_str();
			int Texture_Width, Texture_Height, Texture_Nr_Chans;
			stbi_set_flip_vertically_on_load(true);
			unsigned char *Tile_Texture_Image_Data_Loaded = stbi_load(filePath, &Texture_Width, &Texture_Height, &Texture_Nr_Chans, STBI_rgb);
			if (Tile_Texture_Image_Data_Loaded)
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Texture_Width, Texture_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, Tile_Texture_Image_Data_Loaded);
				glGenerateMipmap(GL_TEXTURE_2D);
				stbi_image_free(Tile_Texture_Image_Data_Loaded);
				std::cout << "Successfully loaded texture at " + path << std::endl;
				return true;
			}
			else
			{
				std::cout << "Failed to load texture" << std::endl;
				stbi_image_free(Tile_Texture_Image_Data_Loaded);
				return false;
			}
		}
	};

	class Texture {
	private:
		unsigned int id;
		std::string type;
		std::string path;
	public:
		Texture(unsigned int id, std::string path) {
			this->id = id;
			this->path = path;

		}

		// Method that loads the textures for the models generated in the world.
		bool load() {

			glGenTextures(1, &id);
			glBindTexture(GL_TEXTURE_2D, id);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


			path.insert(0, "../Textures/");
			const char * filePath = path.c_str();
			int Texture_Width, Texture_Height, Texture_Nr_Chans;
			stbi_set_flip_vertically_on_load(true);
			unsigned char *Tile_Texture_Image_Data_Loaded = stbi_load(filePath, &Texture_Width, &Texture_Height, &Texture_Nr_Chans, STBI_rgb);
			if (Tile_Texture_Image_Data_Loaded)
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Texture_Width, Texture_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, Tile_Texture_Image_Data_Loaded);
				glGenerateMipmap(GL_TEXTURE_2D);
				stbi_image_free(Tile_Texture_Image_Data_Loaded);

				return true;
			}
			else
			{
				stbi_image_free(Tile_Texture_Image_Data_Loaded);

				return false;
			}


		}
	};

	std::vector<MeshEntry> m_Entries;
	std::vector<Texture*> m_Textures;
	Matrix4f Identity;

	std::unordered_map<std::string, uint> m_BoneMapping; // maps a bone name to its index
	uint m_NumBones;
	std::vector<BoneInfo> m_BoneInfo;
	Matrix4f m_GlobalInverseTransform;
	
	// aiNodeAnim * map
	std::unordered_map <std::string, aiNodeAnim* > nodeAnimMap;

	const aiScene* m_pScene;
	Assimp::Importer m_Importer;
};


#endif	


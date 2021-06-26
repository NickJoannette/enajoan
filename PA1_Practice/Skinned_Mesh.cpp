


#include "Skinned_Mesh.h"

#define POSITION_LOCATION    0
#define NORMAL_LOCATION      1
#define TEX_COORD_LOCATION   2
#define BONE_ID_LOCATION     3
#define BONE_WEIGHT_LOCATION 4

void SkinnedMesh::VertexBoneData::AddBoneData(uint BoneID, float Weight)
{

	for (uint i = 0; i < ARRAY_SIZE_IN_ELEMENTS(IDs); i++) {
		if (Weights[i] == 0.0) {
			IDs[i] = BoneID;
			Weights[i] = Weight;
			return;
		}
	}

	// should never get here - more bones than we have space for
}

SkinnedMesh::SkinnedMesh()
{

	m_VAO = 0;
	ZERO_MEM(m_Buffers);
	m_NumBones = 0;
	m_pScene = NULL;
	Identity.InitIdentity();

}


SkinnedMesh::~SkinnedMesh()
{
	Clear();
}


void SkinnedMesh::Clear()
{
	for (uint i = 0; i < m_Textures.size(); i++) {
		SAFE_DELETE(m_Textures[i]);
	}

	if (m_Buffers[0] != 0) {
		glDeleteBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);
	}

	if (m_VAO != 0) {
		glDeleteVertexArrays(1, &m_VAO);
		m_VAO = 0;
	}
}


bool SkinnedMesh::LoadMesh(const std::string& Filename)
{
	// Release the previously loaded mesh (if it exists)
	Clear();

	// Create the VAO
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	// Create the buffers for the vertices attributes
	glGenBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);

	bool Ret = false;

	m_pScene = m_Importer.ReadFile(Filename.c_str(), ASSIMP_LOAD_FLAGS);

	if (m_pScene) {
		m_GlobalInverseTransform = m_pScene->mRootNode->mTransformation;
		m_GlobalInverseTransform.Inverse();
		Ret = InitFromScene(m_pScene, Filename);
	}
	else {
		printf("Error parsing '%s': '%s'\n", Filename.c_str(), m_Importer.GetErrorString());
	}

	// Make sure the VAO is not changed from the outside
	glBindVertexArray(0);

	return Ret;
}


bool SkinnedMesh::InitFromScene(const aiScene* pScene, const  std::string& Filename)
{
	m_Entries.resize(pScene->mNumMeshes);
	m_Textures.resize(pScene->mNumMaterials);

	std::vector<Vector3f> Positions;
	std::vector<Vector3f> Normals;
	std::vector<Vector2f> TexCoords;
	std::vector<VertexBoneData> Bones;
	std::vector<uint> Indices;





	uint NumVertices = 0;
	uint NumIndices = 0;






	// Count the number of vertices and indices
	for (uint i = 0; i < m_Entries.size(); ++i) { // For every mesh in the scene
		m_Entries[i].MaterialIndex = pScene->mMeshes[i]->mMaterialIndex;
		m_Entries[i].NumIndices = pScene->mMeshes[i]->mNumFaces * 3;
		m_Entries[i].BaseVertex = NumVertices;
		m_Entries[i].BaseIndex = NumIndices;


		NumVertices += pScene->mMeshes[i]->mNumVertices;
		NumIndices += m_Entries[i].NumIndices;
		aiMaterial * material = pScene->mMaterials[m_Entries[i].MaterialIndex];

		// Get mesh materials
		aiColor3D a(0.f, 0.f, 0.f);
		aiColor3D d(0.f, 0.f, 0.f);
		aiColor3D s(0.f, 0.f, 0.f);
		float shn;
		material->Get(AI_MATKEY_COLOR_AMBIENT, a);
		material->Get(AI_MATKEY_COLOR_DIFFUSE, d);
		material->Get(AI_MATKEY_COLOR_SPECULAR, s);
		material->Get(AI_MATKEY_SHININESS_STRENGTH, shn);
		m_Entries[i].Mat = Material(a.r, a.g, a.b, d.r, d.g, d.b, s.r, s.g, s.b, shn);

		// We should also get the mesh textures

		// Get diffuse textures
		for (unsigned int j = 0; j < material->GetTextureCount(aiTextureType_DIFFUSE); ++j)
		{
			aiString path;
			material->GetTexture(aiTextureType_DIFFUSE, j, &path);
			std::string stringVersionOfTexturePath(path.data);
			unsigned int locationOfLastFrontSlash = stringVersionOfTexturePath.find_last_of("\\");
			unsigned int locationOfLastFileEndingDelimiter = stringVersionOfTexturePath.find_last_of(".");
			std::string trueRelativePath = "../Textures/" + stringVersionOfTexturePath.substr(locationOfLastFrontSlash + 1, 
				locationOfLastFileEndingDelimiter - locationOfLastFrontSlash - 1) + ".png";


			m_Entries[i].loadInOurTexture(trueRelativePath, m_Entries[i].texture_diffuse_ID);


		}	
		
		for (unsigned int j = 0; j < material->GetTextureCount(aiTextureType_SPECULAR); ++j)
		{
			aiString path;
			material->GetTexture(aiTextureType_SPECULAR, j, &path);
			std::string stringVersionOfTexturePath(path.data);
			unsigned int locationOfLastFrontSlash = stringVersionOfTexturePath.find_last_of("\\");
			unsigned int locationOfLastFileEndingDelimiter = stringVersionOfTexturePath.find_last_of(".");
			std::string trueRelativePath = "../Textures/" + stringVersionOfTexturePath.substr(locationOfLastFrontSlash + 1, 
				locationOfLastFileEndingDelimiter - locationOfLastFrontSlash - 1) + ".png";
			m_Entries[i].loadInOurTexture(trueRelativePath, m_Entries[i].texture_specular_ID);
		}
				
		for (unsigned int j = 0; j < material->GetTextureCount(aiTextureType_NORMALS); ++j)
		{
			aiString path;
			material->GetTexture(aiTextureType_NORMALS, j, &path);
			std::string stringVersionOfTexturePath(path.data);
			unsigned int locationOfLastFrontSlash = stringVersionOfTexturePath.find_last_of("\\");
			unsigned int locationOfLastFileEndingDelimiter = stringVersionOfTexturePath.find_last_of(".");
			std::string trueRelativePath = "../Textures/" + stringVersionOfTexturePath.substr(locationOfLastFrontSlash + 1, 
				locationOfLastFileEndingDelimiter - locationOfLastFrontSlash - 1) + ".png";
			m_Entries[i].loadInOurTexture(trueRelativePath, m_Entries[i].texture_specular_ID);
		}


	}





	// Reserve space in the vectors for the vertex attributes and indices
	Positions.reserve(NumVertices);
	Normals.reserve(NumVertices);
	TexCoords.reserve(NumVertices);
	Bones.resize(NumVertices);
	Indices.reserve(NumIndices);

	// Initialize the meshes in the scene one by one
	for (uint i = 0; i < m_Entries.size(); i++) {
		const aiMesh* paiMesh = pScene->mMeshes[i];
		InitMesh(i, paiMesh, Positions, Normals, TexCoords, Bones, Indices);
	}

	if (!InitMaterials(pScene, Filename)) {
		return false;
	}

	// Generate and populate the buffers with vertex attributes and the indices
	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POS_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Positions[0]) * Positions.size(), &Positions[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(POSITION_LOCATION);
	glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[TEXCOORD_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexCoords[0]) * TexCoords.size(), &TexCoords[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(TEX_COORD_LOCATION);
	glVertexAttribPointer(TEX_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[NORMAL_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Normals[0]) * Normals.size(), &Normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(NORMAL_LOCATION);
	glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[BONE_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Bones[0]) * Bones.size(), &Bones[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(BONE_ID_LOCATION);
	glVertexAttribIPointer(BONE_ID_LOCATION, 4, GL_INT, sizeof(VertexBoneData), (const GLvoid*)0);
	glEnableVertexAttribArray(BONE_WEIGHT_LOCATION);
	glVertexAttribPointer(BONE_WEIGHT_LOCATION, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (const GLvoid*)(16));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[INDEX_BUFFER]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices[0]) * Indices.size(), &Indices[0], GL_STATIC_DRAW);

	return GLCheckError();
}


void SkinnedMesh::InitMesh(uint MeshIndex,
	const aiMesh* paiMesh,
	std::vector<Vector3f>& Positions,
	std::vector<Vector3f>& Normals,
	std::vector<Vector2f>& TexCoords,
	std::vector<VertexBoneData>& Bones,
	std::vector<uint>& Indices)
{

	const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

	// Populate the vertex attribute vectors
	for (uint i = 0; i < paiMesh->mNumVertices; i++) {
		const aiVector3D* pPos = &(paiMesh->mVertices[i]);
		const aiVector3D* pNormal = &(paiMesh->mNormals[i]);
		const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;

		Positions.push_back(Vector3f(pPos->x, pPos->y, pPos->z));
		Normals.push_back(Vector3f(pNormal->x, pNormal->y, pNormal->z));
		TexCoords.push_back(Vector2f(pTexCoord->x, pTexCoord->y));
	}

	LoadBones(MeshIndex, paiMesh, Bones);

	// Populate the index buffer
	for (uint i = 0; i < paiMesh->mNumFaces; i++) {
		const aiFace& Face = paiMesh->mFaces[i];
		Indices.push_back(Face.mIndices[0]);
		Indices.push_back(Face.mIndices[1]);
		Indices.push_back(Face.mIndices[2]);
	}

	// Map the node animations

	if (m_pScene->HasAnimations()) {
		aiAnimation* pAnimation = m_pScene->mAnimations[0];

		for (uint j = 0; j < pAnimation->mNumChannels; j++) {
			aiNodeAnim* pNodeAnim = pAnimation->mChannels[j];
			std::pair<std::string, aiNodeAnim*> pair(std::string(pNodeAnim->mNodeName.data), pNodeAnim);
			nodeAnimMap.insert(pair);
		}
	}

}

void SkinnedMesh::LoadBones(uint MeshIndex, const aiMesh* pMesh, std::vector<VertexBoneData>& Bones)
{
	for (uint i = 0; i < pMesh->mNumBones; i++) {
		uint BoneIndex = 0;
		uint ParentIndex = 0;
		std::string BoneName(pMesh->mBones[i]->mName.data);

		if (m_BoneMapping.find(BoneName) == m_BoneMapping.end()) {
			// Allocate an index for a new bone
			BoneIndex = m_NumBones;
			
			m_NumBones++;
			BoneInfo bi;
			m_BoneInfo.push_back(bi);
			m_BoneInfo[BoneIndex].BoneOffset = pMesh->mBones[i]->mOffsetMatrix;
			m_BoneMapping[BoneName] = BoneIndex;
		}
		else {
			BoneIndex = m_BoneMapping[BoneName];
		}

		for (uint j = 0; j < pMesh->mBones[i]->mNumWeights; j++) {
			uint VertexID = m_Entries[MeshIndex].BaseVertex + pMesh->mBones[i]->mWeights[j].mVertexId;
			float Weight = pMesh->mBones[i]->mWeights[j].mWeight;
			Bones[VertexID].AddBoneData(BoneIndex, Weight);
		}
	}
}


bool SkinnedMesh::InitMaterials(const aiScene* pScene, const std::string& Filename)
{

	// Extract the directory part from the file name
	std::string::size_type SlashIndex = Filename.find_last_of("/");
	std::string Dir;

	if (SlashIndex == std::string::npos) {
		Dir = ".";
	}
	else if (SlashIndex == 0) {
		Dir = "/";
	}
	else {
		Dir = Filename.substr(0, SlashIndex);
	}

	bool Ret = true;

	// Initialize the materials
	for (uint i = 0; i < pScene->mNumMaterials; i++) {
		const aiMaterial* pMaterial = pScene->mMaterials[i];

		m_Textures[i] = NULL;

		if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
			aiString Path;

			if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
				std::string p(Path.data);
				std::string::size_type lastBackSlash = p.find_last_of("\\");
				std::string::size_type tgaType = p.find_last_of(".tag");
				std::string actualTextureFileName = p.substr(lastBackSlash + 1,tgaType);
				std::string actualTextureFilePath = "../Models/" + actualTextureFileName + ".png";

				if (p.substr(0, 2) == ".\\") {
					p = p.substr(2, p.size() - 2);
				}

				std::string FullPath = actualTextureFilePath;//Dir + "/" + p;

				m_Textures[i] = new Texture(GL_TEXTURE_2D, FullPath.c_str());

				if (!m_Textures[i]->load()) {
					delete m_Textures[i];
					m_Textures[i] = NULL;
				}
				else {
					printf("%d - loaded texture '%s'\n", i, FullPath.c_str());
				}
			}
		}
	}

	return Ret;
}


void SkinnedMesh::Render(GLuint primitiveType, Shader & shader, bool overrideModelTexture)
{
	glBindVertexArray(m_VAO);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	for (uint i = 0; i < m_Entries.size(); i++) {

		// Does the mesh have textures? Bind them according to the amount and organization
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_Entries[i].texture_diffuse_ID);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_Entries[i].texture_specular_ID);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, m_Entries[i].texture_normal_ID);

		shader.setMaterial("baseMaterial", m_Entries[i].Mat);
		if (overrideModelTexture) {
			shader.setMaterial("extraMaterial", extraMaterial);
			shader.setBool("useExtraMaterial", true);
		} else 
			shader.setBool("useExtraMaterial", false);

		glDrawElementsBaseVertex(primitiveType,
			m_Entries[i].NumIndices,
			GL_UNSIGNED_INT,
			(void*)(sizeof(uint) * m_Entries[i].BaseIndex),
			m_Entries[i].BaseVertex);


		glBindTexture(GL_TEXTURE_2D, 0);
	}
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	// Make sure the VAO is not changed from the outside    
	glBindVertexArray(0);
}


uint SkinnedMesh::FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	for (uint i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
			return i;
		}
	}

//	assert(0);

	return 0;
}


uint SkinnedMesh::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumRotationKeys > 0);

	for (uint i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
			return i;
		}
	}

	//assert(0);

	return 0;
}


uint SkinnedMesh::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumScalingKeys > 0);

	for (uint i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
			return i;
		}
	}

	//assert(0);

	return 0;
}


void SkinnedMesh::CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumPositionKeys == 1) {
		Out = pNodeAnim->mPositionKeys[0].mValue;
		return;
	}

	uint PositionIndex = FindPosition(AnimationTime, pNodeAnim);
	uint NextPositionIndex = (PositionIndex + 1);
	assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
	float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
//	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
	const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}


void SkinnedMesh::CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	// we need at least two values to interpolate...
	if (pNodeAnim->mNumRotationKeys == 1) {
		Out = pNodeAnim->mRotationKeys[0].mValue;
		return;
	}

	uint RotationIndex = FindRotation(AnimationTime, pNodeAnim);
	uint NextRotationIndex = (RotationIndex + 1);
	assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
	float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
	//assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
	aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
	Out = Out.Normalize();
}


void SkinnedMesh::CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumScalingKeys == 1) {
		Out = pNodeAnim->mScalingKeys[0].mValue;
		return;
	}

	uint ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
	uint NextScalingIndex = (ScalingIndex + 1);
	assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
	float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
//	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
	const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}


void SkinnedMesh::ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const Matrix4f& ParentTransform)
{
		std::string NodeName(pNode->mName.data);

		const aiAnimation* pAnimation = m_pScene->mAnimations[0];

		Matrix4f NodeTransformation(pNode->mTransformation);

		const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);

		if (pNodeAnim) {
			// Interpolate scaling and generate scaling transformation matrix	
			aiVector3D Scaling;
			CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
			Matrix4f ScalingM;
			ScalingM.InitScaleTransform(Scaling.x, Scaling.y, Scaling.z);

			// Interpolate rotation and generate rotation transformation matrix	
			aiQuaternion RotationQ;
			CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
			Matrix4f RotationM = Matrix4f(RotationQ.GetMatrix());
			// Interpolate translation and generate translation transformation matrix	
			aiVector3D Translation;
			CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
			Matrix4f TranslationM;
			TranslationM.InitTranslationTransform(Translation.x, Translation.y, Translation.z);

			// Combine the above transformations	
			NodeTransformation = TranslationM * RotationM * ScalingM;
		}

		Matrix4f GlobalTransformation = ParentTransform * NodeTransformation;

		if (m_BoneMapping.find(NodeName) != m_BoneMapping.end()) {
			uint BoneIndex = m_BoneMapping[NodeName];
			m_BoneInfo[BoneIndex].FinalTransformation = m_GlobalInverseTransform * GlobalTransformation * m_BoneInfo[BoneIndex].BoneOffset;
		}

		for (uint i = 0; i < pNode->mNumChildren; i++) {
			ReadNodeHeirarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation);
		}
	}

void SkinnedMesh::BoneTransform(float TimeInSeconds, std::vector<Matrix4f>& Transforms)
{
	float TicksPerSecond = (float)(m_pScene->mAnimations[0]->mTicksPerSecond != 0 ? m_pScene->mAnimations[0]->mTicksPerSecond : 25.0f);
	float TimeInTicks = TimeInSeconds * TicksPerSecond;
	float AnimationTime = fmod(TimeInTicks, (float)m_pScene->mAnimations[0]->mDuration);
	ReadNodeHeirarchy(AnimationTime, m_pScene->mRootNode, Identity);
	Transforms.resize(m_NumBones);

	for (uint i = 0; i < m_NumBones; i++) {
		Transforms[i] = m_BoneInfo[i].FinalTransformation;
	}
}


const aiNodeAnim* SkinnedMesh::FindNodeAnim(const aiAnimation* pAnimation, const std::string NodeName)
{
	for (uint i = 0; i < pAnimation->mNumChannels; i++) {
		const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

		if (std::string(pNodeAnim->mNodeName.data) == NodeName) {
			return pNodeAnim;
		}
	}

	return NULL;
}
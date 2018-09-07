//
//  Model.cpp
//  ogl4
//
//  Created by Philipp Lensing on 21.09.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#include "Model.h"
#include "phongshader.h"
#include <list>
#include <fstream>

#define SCALE_FACTOR 5.f

Model::Model() : pMeshes(NULL), MeshCount(0), pMaterials(NULL), MaterialCount(0), type(NULL)
{

}

Model::Model(const char* ModelFile, bool FitSize) : pMeshes(NULL), MeshCount(0), pMaterials(NULL), MaterialCount(0), type(NULL)
{
	bool ret = load(ModelFile, FitSize);
	if (!ret)
		throw std::exception();
}

Model::~Model()
{
	if (this->MeshCount > 0)
		delete[] this->pMeshes;

	if (this->MaterialCount > 0)
	{
		for (size_t i = 0; i < this->MaterialCount; ++i)
			delete this->pMaterials[i].DiffTex;

		delete[] this->pMaterials;
	}

	deleteNodes(&RootNode);
}

void Model::deleteNodes(Node* pNode)
{
	if (!pNode)
		return;
	for (unsigned int i = 0; i < pNode->ChildCount; ++i)
		deleteNodes(&(pNode->Children[i]));
	if (pNode->ChildCount > 0)
		delete[] pNode->Children;
	if (pNode->MeshCount > 0)
		delete[] pNode->Meshes;
}

bool Model::load(const char* ModelFile, bool FitSize)
{
	const aiScene* pScene = aiImportFile(ModelFile, aiProcessPreset_TargetRealtime_Fast | aiProcess_TransformUVCoords);

	if (pScene == NULL || pScene->mNumMeshes <= 0)
		return false;

	Filepath = ModelFile;
	Path = Filepath;
	size_t pos = Filepath.rfind('/');
	if (pos == std::string::npos)
		pos = Filepath.rfind('\\');
	if (pos != std::string::npos)
		Path.resize(pos + 1);

	loadMeshes(pScene, FitSize);
	loadMaterials(pScene);
	loadNodes(pScene);

	return true;
}

void Model::loadMeshes(const aiScene* pScene, bool FitSize)
{
	// assimp documentation: http://assimp.sourceforge.net/lib_html
	// Model structure: https://docs.google.com/drawings/d/16T4XX06bKyfj9tY_9_MsmjgynnrYTl4xFXq6nu-0YSY/preview?h=600&hl=en&w=800
	// Loading meshes in OpenGL: https://badvertex.com/2014/04/13/loading-meshes-using-assimp-in-opengl.html

#pragma region some error catching
	if (pScene == NULL)
		throw std::exception("No scene found!\n");

	if (pScene->mRootNode == NULL)
		throw std::exception("Scene root node not found!\n");

	if (!pScene->HasMeshes())
		throw std::exception("Scene has no meshes!\n");
#pragma endregion

	// meshes count from pScene
	this->MeshCount = pScene->mNumMeshes;

	// initialize pMeshes with the correct size
	this->pMeshes = new Mesh[this->MeshCount];

	// for each mesh in pScene
	for (unsigned int mesh = 0; mesh < this->MeshCount; ++mesh)
	{
		// get address of mesh item from meshes array to save data to
		Mesh& outMesh = this->pMeshes[mesh];

		// create aiMesh pointer for the loop and get mesh
		const aiMesh* inMesh = pScene->mMeshes[mesh];

		// save material index
		outMesh.MaterialIdx = inMesh->mMaterialIndex;

		// not needed to cache, but less overhead
		// get vertices count
		unsigned int verticesCount = inMesh->mNumVertices;
		// get faces count
		unsigned int facesCount = inMesh->mNumFaces;

		// ------------------------------------------------------------------------
		// Vertex Buffer Data
		// ------------------------------------------------------------------------

		outMesh.VB.begin();

		// get vertex positions
		if (inMesh->HasPositions())
		{
			Vector vertPos = Vector();

			for (size_t vert = 0; vert < verticesCount; ++vert)
			{
				vertPos = this->convert(inMesh->mVertices[vert]);
				outMesh.VB.addVertex(vertPos);
			}
		}

		// get vertex normals
		if (inMesh->HasNormals())
		{
			Vector vertNormal = Vector();

			for (size_t vert = 0; vert < verticesCount; ++vert)
			{
				vertNormal = Vector(this->convert(inMesh->mNormals[vert]));
				outMesh.VB.addNormal(vertNormal);
			}
		}

		// get UV coordinates
		if (inMesh->HasTextureCoords(0))
		{
			float s0, t0;

			for (size_t vert = 0; vert < verticesCount; ++vert)
			{
				s0 = inMesh->mTextureCoords[0][vert].x;
				t0 = inMesh->mTextureCoords[0][vert].y;

				outMesh.VB.addTexcoord0(s0, 1 - t0);
			}
		}

		// Praktikum 6 - Exercise 3
		if (inMesh->HasTangentsAndBitangents())
		{
			for (size_t vert = 0; vert < verticesCount; ++vert)
			{
				// get tangents and bitangents
				outMesh.VB.addTexcoord1(inMesh->mTangents[vert].x, inMesh->mTangents[vert].y);
				outMesh.VB.addTexcoord2(inMesh->mBitangents[vert].x, inMesh->mBitangents[vert].y);
			}
		}

		outMesh.VB.end();

		// ------------------------------------------------------------------------
		// Index Buffer Data
		// ------------------------------------------------------------------------

		outMesh.IB.begin();

		// get face indices
		if (inMesh->HasFaces())
		{
			for (size_t face = 0; face < inMesh->mNumFaces; ++face)
			{
				// Make sure the face is triangulated before loading the data
				assert(inMesh->mFaces[face].mNumIndices == 3);

				outMesh.IB.addIndex(inMesh->mFaces[face].mIndices[0]);
				outMesh.IB.addIndex(inMesh->mFaces[face].mIndices[1]);
				outMesh.IB.addIndex(inMesh->mFaces[face].mIndices[2]);
			}
		}

		outMesh.IB.end();

		// not needed anymore and doesn't seem to work this way
		// using the reference to a mesh element in pMeshes directly, now
		// finally, save mesh to meshes array
		//this->pMeshes[mesh] = outMesh;
	}

	// calculate bounding box
	this->calcBoundingBox(pScene, this->BoundingBox);

	if (FitSize)
	{
		Matrix scalingMatrix;
		float scaleFactor = Vector(SCALE_FACTOR, SCALE_FACTOR, SCALE_FACTOR).length();
		scaleFactor = scaleFactor / this->BoundingBox.size().length();

		// scale the scaling matrix
		scalingMatrix.scale(scaleFactor);

		// scale model
		this->transform(scalingMatrix);
	}
}

void Model::loadMaterials(const aiScene* pScene)
{
	if (pScene->HasMaterials())
	{
		// material count from pScene
		this->MaterialCount = pScene->mNumMaterials;

		// initialize pMaterials with the correct size
		this->pMaterials = new Material[this->MaterialCount];

		// load material data
		for (unsigned int mat = 0; mat < this->MaterialCount; ++mat)
		{
			// get address of material item from materials array to save data to
			Material& outMaterial = this->pMaterials[mat];

			// create material pointer for the loop and get material
			const aiMaterial* inMaterial = pScene->mMaterials[mat];

			// use assimp keys to get the data and write it to a material of our format
			// Get() is C++-style, aiGetMaterialColor() is C-style
			// http://assimp.sourceforge.net/lib_html/structai_material.html
			// http://assimp.sourceforge.net/lib_html/material_8h.html

			// get color values
			aiColor4D color = aiColor4D();

			// Use checks to make sure the material has the value we ask for
			if (inMaterial->Get(AI_MATKEY_COLOR_AMBIENT, color) == AI_SUCCESS)
				outMaterial.AmbColor = this->convert(color);

			if (inMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)
				outMaterial.DiffColor = this->convert(color);

			if (inMaterial->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS)
				outMaterial.SpecColor = this->convert(color);

			// get material shininess
			float shininess = 0.f;
			if (inMaterial->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS)
				outMaterial.SpecExp = shininess;

			// get texture path
			aiString texturePath;

			// "aiTextureType_DIFFUSE should be used with aiGetMaterialString to retrieve the diffuse texture name"
			// from this post: http://forum.lwjgl.org/index.php?topic=6549.msg34697#msg34697

			// Use checks to make sure the material has a diffuse texture
			if (inMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0 && inMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS)
			{
				outMaterial.DiffTex = Texture::LoadShared((this->Path + texturePath.data).c_str());

				// Praktikum 6 - Exercise 3
				// create path for normal map file
				std::string textureNormalPath = this->Path + texturePath.C_Str();
				textureNormalPath.replace(textureNormalPath.find_last_of("."), 1, "_n.");

				// check if normal map is available for this texture
				std::ifstream ifs;
				ifs.open(textureNormalPath);

				// if file exists, load normal map
				if (ifs.good())
					outMaterial.NormalTex = Texture::LoadShared(textureNormalPath.c_str());
			}
		}
	}
}

void Model::calcBoundingBox(const aiScene* pScene, AABB& Box)
{
	// calculate bounding box
	// outer box that encapsulates the whole model by using
	// the furthest out vertex positions of the model for each direction (-/+)
	// so, furthest out:
	// -x/+x vertex position
	// -y/+y vertex position
	// -z/+z vertex position

	Vector min, max = min = Vector(0.f, 0.f, 0.f);

	for (size_t mesh = 0; mesh < this->MeshCount; ++mesh)
	{
		for (size_t vert = 0; vert < pScene->mMeshes[mesh]->mNumVertices; ++vert)
		{
			Vector currentPos = this->convert(pScene->mMeshes[mesh]->mVertices[vert]);

			// check for new min x,y,z values
			if (currentPos.X < min.X)
				min.X = currentPos.X;
			if (currentPos.Y < min.Y)
				min.Y = currentPos.Y;
			if (currentPos.Z < min.Z)
				min.Z = currentPos.Z;

			// check for new max x,y,z values
			if (currentPos.X > max.X)
				max.X = currentPos.X;
			if (currentPos.Y > max.Y)
				max.Y = currentPos.Y;
			if (currentPos.Z > max.Z)
				max.Z = currentPos.Z;
		}
	}

	// save vectors for bounding box
	Box.Min = min;
	Box.Max = max;
}

void Model::loadNodes(const aiScene* pScene)
{
	deleteNodes(&RootNode);
	copyNodesRecursive(pScene->mRootNode, &RootNode);
}

void Model::copyNodesRecursive(const aiNode* paiNode, Node* pNode)
{
	pNode->Name = paiNode->mName.C_Str();
	pNode->Trans = convert(paiNode->mTransformation);

	if (paiNode->mNumMeshes > 0)
	{
		pNode->MeshCount = paiNode->mNumMeshes;
		pNode->Meshes = new int[pNode->MeshCount];
		for (unsigned int i = 0; i < pNode->MeshCount; ++i)
			pNode->Meshes[i] = (int)paiNode->mMeshes[i];
	}

	if (paiNode->mNumChildren <= 0)
		return;

	pNode->ChildCount = paiNode->mNumChildren;
	pNode->Children = new Node[pNode->ChildCount];
	for (unsigned int i = 0; i < paiNode->mNumChildren; ++i)
	{
		copyNodesRecursive(paiNode->mChildren[i], &(pNode->Children[i]));
		pNode->Children[i].Parent = pNode;
	}
}

void Model::applyMaterial(unsigned int index)
{
	if (index >= MaterialCount)
		return;

	PhongShader* pPhong = dynamic_cast<PhongShader*>(shader());
	if (!pPhong) {
		//std::cout << "Model::applyMaterial(): WARNING Invalid shader-type. Please apply PhongShader for rendering models.\n";
		return;
	}

	Material* pMat = &pMaterials[index];
	pPhong->ambientColor(pMat->AmbColor);
	pPhong->diffuseColor(pMat->DiffColor);
	pPhong->specularExp(pMat->SpecExp);
	pPhong->specularColor(pMat->SpecColor);
	pPhong->diffuseTexture(pMat->DiffTex);
	pPhong->normalTexture(pMat->NormalTex);
}

void Model::draw(const BaseCamera& Cam)
{
	if (!pShader) {
		std::cout << "BaseModel::draw() no shader found" << std::endl;
		return;
	}
	pShader->modelTransform(transform());

	std::list<Node*> DrawNodes;
	DrawNodes.push_back(&RootNode);

	while (!DrawNodes.empty())
	{
		Node* pNode = DrawNodes.front();
		Matrix GlobalTransform;

		if (pNode->Parent != NULL)
			pNode->GlobalTrans = pNode->Parent->GlobalTrans * pNode->Trans;
		else
			pNode->GlobalTrans = transform() * pNode->Trans;

		pShader->modelTransform(pNode->GlobalTrans);

		for (unsigned int i = 0; i < pNode->MeshCount; ++i)
		{
			Mesh& mesh = pMeshes[pNode->Meshes[i]];
			mesh.VB.activate();
			mesh.IB.activate();
			applyMaterial(mesh.MaterialIdx);
			pShader->activate(Cam);
			glDrawElements(GL_TRIANGLES, mesh.IB.indexCount(), mesh.IB.indexFormat(), 0);
			mesh.IB.deactivate();
			mesh.VB.deactivate();
		}
		for (unsigned int i = 0; i < pNode->ChildCount; ++i)
			DrawNodes.push_back(&(pNode->Children[i]));

		DrawNodes.pop_front();
	}
}

Matrix Model::convert(const aiMatrix4x4& m)
{
	return Matrix(m.a1, m.a2, m.a3, m.a4,
		m.b1, m.b2, m.b3, m.b4,
		m.c1, m.c2, m.c3, m.c4,
		m.d1, m.d2, m.d3, m.d4);
}

Vector Model::convert(const aiVector3D& v)
{
	return Vector(v.x, v.y, v.z);
}

Color Model::convert(const aiColor4D& c)
{
	return Color(c.r, c.g, c.b);
}

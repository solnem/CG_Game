//
//  Model.hpp
//  ogl4
//
//  Created by Philipp Lensing on 21.09.16.
//  Copyright © 2016 Philipp Lensing. All rights reserved.
//

#ifndef Model_hpp
#define Model_hpp

#include <stdio.h>
#include "basemodel.h"
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "vertexbuffer.h"
#include "indexbuffer.h"
#include "texture.h"
#include "aabb.h"
#include <string>

class Model : public BaseModel
{
public:
	Model();
	Model(const char* ModelFile, bool FitSize=true);
	virtual ~Model();

	bool load(const char* ModelFile, bool FitSize=true);
	virtual void draw(const BaseCamera& Cam);
	const AABB& boundingBox() const { return BoundingBox; }
	const char* getModelType() const { return type; }
	void setModelType(char* Type) { type = Type; }
	
protected: // protected types
	struct Mesh
	{
		Mesh()  {}
		VertexBuffer VB;
		IndexBuffer IB;
		int MaterialIdx;
	};
	struct Material
	{
		Material() : DiffTex(NULL), NormalTex(NULL), DiffColor(1,1,1),SpecColor(0.3f,0.3f,0.3f), AmbColor(0,0,0), SpecExp(10) {}
		Color DiffColor;
		Color SpecColor;
		Color AmbColor;
		float SpecExp;
		const Texture* DiffTex;
		const Texture* NormalTex;
	};
	struct Node
	{
		Node() : Parent(NULL), Children(NULL), ChildCount(0), MeshCount(0), Meshes(NULL) {}
		Matrix Trans;
		Matrix GlobalTrans;
		int* Meshes;
		unsigned int MeshCount;
		Node* Parent;
		Node* Children;
		unsigned int ChildCount;
		std::string Name;
	};
	
protected: // protected methods
	void loadMeshes(const aiScene* pScene, bool FitSize);
	void loadMaterials(const aiScene* pScene);
	void calcBoundingBox( const aiScene* pScene, AABB& Box);

	void loadNodes(const aiScene* pScene);
	void copyNodesRecursive(const aiNode* paiNode, Node* pNode);
	Matrix convert(const aiMatrix4x4& m);
	Vector convert(const aiVector3D& v);
	Color convert(const aiColor4D& c);
	void applyMaterial( unsigned int index);
	void deleteNodes(Node* pNode);

protected: // protected member variables
	Mesh* pMeshes;
	unsigned int MeshCount;
	Material* pMaterials;
	unsigned int MaterialCount;
	AABB BoundingBox;
	
	std::string Filepath; // stores pathname and filename
	std::string Path; // stores path without filename
	Node RootNode;

	char* type;
};

#endif /* Model_hpp */

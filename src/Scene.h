//
//  Scene.h
//  RealtimeRending
//
//  Created by Philipp Lensing on 06.11.14.
//  Copyright (c) 2014 Philipp Lensing. All rights reserved.
//

#ifndef __RealtimeRending__Scene__
#define __RealtimeRending__Scene__

#include <iostream>
#include "matrix.h"
#include <set>
#include <vector>
#include <map>
#include "basemodel.h"
#include "phongshader.h"
#include "constantshader.h"

class Model;


class SceneNode
{
public:
	SceneNode();
	SceneNode(const std::string& Name, const Vector& Translation, const Vector& RotationAxis, const float RotationAngle, const Vector& Scale, SceneNode* pParent, Model* pModel);

	//getter
	const Matrix& getLocalTransform() const;
	Matrix getGlobalTransform() const;
	const SceneNode* getParent() const;
	const Model* getModel() const;
	const std::string& getName() const;
	const Vector& getScaling() const;

	//setter
	void setLocalTransform(const Vector& Translation, const Vector& RotationAxis, const float RotationAngle);
	void setLocalTransform(const Matrix& LocalTransform);
	void setParent(SceneNode* pNode);
	const std::set<SceneNode*>& getChildren() const;
	void addChild(SceneNode* pChild);
	void removeChild(SceneNode* pChild);
	void setModel(Model* pModel);
	void setName(const std::string& Name);
	void setScaling(const Vector& Scaling);
	void draw(const BaseCamera& Cam);
protected:
	std::string m_Name;
	Model* m_pModel;
	SceneNode* m_pParent;
	std::set<SceneNode*> m_Children;
	Matrix m_LocalTransform;
	Vector m_Scaling;
};

class Scene : public BaseModel
{
public:
	Scene();
	virtual ~Scene();
	bool addSceneFile(const char* Scenefile);
	virtual void draw(const BaseCamera& Cam);
protected:
	void draw(SceneNode* pNode);
	SceneNode m_Root;
	std::map<std::string, Model*> m_Models;
};

#endif /* defined(__RealtimeRending__Scene__) */

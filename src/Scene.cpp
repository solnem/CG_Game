//
//  Scene.cpp
//  RealtimeRending
//
//  Created by Philipp Lensing on 06.11.14.
//  Copyright (c) 2014 Philipp Lensing. All rights reserved.
//

#include "Scene.h"
#include <string.h>
#include "model.h"
#include <assert.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <stdlib.h>

#ifdef WIN32
#define ASSET_DIRECTORY "../../assets/"
#else
#define ASSET_DIRECTORY "../assets/"
#endif

using namespace std;

SceneNode::SceneNode() : m_pParent(NULL), m_pModel(NULL), m_Scaling(1, 1, 1)
{
	// TODO: Not sure here
	this->m_Name = "NULL";
	this->m_LocalTransform = Matrix();
	this->m_Children = set<SceneNode*>();
}

SceneNode::SceneNode(const std::string& Name, const Vector& Translation, const Vector& RotationAxis, const float RotationAngle, const Vector& Scale, SceneNode* pParent, Model* pModel)
{
	setName(Name);
	setLocalTransform(Translation, RotationAxis, RotationAngle);
	setScaling(Scale);
	setParent(pParent);
	setModel(pModel);
}

const Matrix& SceneNode::getLocalTransform() const
{
	return m_LocalTransform;
}

void SceneNode::setLocalTransform(const Vector& Translation, const Vector& RotationAxis, const float RotationAngle)
{
	// TODO: Not sure here
	this->m_LocalTransform.translation(Translation);
	this->m_LocalTransform.rotationAxis(RotationAxis, RotationAngle);
}

void SceneNode::setLocalTransform(const Matrix& LocalTransform)
{
	m_LocalTransform = LocalTransform;
}

Matrix SceneNode::getGlobalTransform() const
{
	// TODO: Add your code
	// get upper parent node transform?
	Matrix globalTransform, scalingMatrix = globalTransform = Matrix();
	scalingMatrix.scale(this->getScaling());
	globalTransform = this->getLocalTransform();

	// as long as there are more parent nodes to take into account
	const SceneNode* temp = this;
	while (temp->getParent() != NULL)
	{
		temp = temp->getParent();
		globalTransform *= temp->getLocalTransform();
	}

	globalTransform *= scalingMatrix;

	return globalTransform;
}

const SceneNode* SceneNode::getParent() const
{
	return m_pParent;
}

void SceneNode::setParent(SceneNode* pNode)
{
	// TODO: Add your code
	this->m_pParent = pNode;
}

const std::set<SceneNode*>& SceneNode::getChildren() const
{
	return m_Children;
}

void SceneNode::addChild(SceneNode* pChild)
{
	m_Children.insert(pChild);
}

void SceneNode::removeChild(SceneNode* pChild)
{
	m_Children.erase(pChild);
}

void SceneNode::setModel(Model* pModel)
{
	m_pModel = pModel;
}

const Model* SceneNode::getModel() const
{
	return m_pModel;
}

void SceneNode::setName(const std::string& Name)
{
	m_Name = Name;
}

const std::string& SceneNode::getName() const
{
	return m_Name;
}

const Vector& SceneNode::getScaling() const
{
	return m_Scaling;
}
void SceneNode::setScaling(const Vector& Scaling)
{
	m_Scaling = Scaling;
}

void SceneNode::draw(const BaseCamera& Cam)
{
	// TODO: Add your code
	const set<SceneNode*> children = this->getChildren();

	for (set<SceneNode*>::iterator it = children.begin(); it != children.end(); ++it)
	{
		// apply global transform
		(*it)->m_pModel->transform((*it)->getGlobalTransform());

		// draw current node
		(*it)->m_pModel->draw(Cam);

		SceneNode* temp = (*it);

		// check for child nodes
		while (temp->m_Children.size() > 0)
		{
			// get child nodes
			set<SceneNode*>::iterator childIt = temp->getChildren().begin();

			//if (childIt != temp->getChildren().end())
			//{
			//	printf("Node %s: There is more than one child node!", (*childIt)->getName());
			//}

			temp = (*childIt);

			// apply global transform
			(*childIt)->m_pModel->transform((*childIt)->getGlobalTransform());

			// draw child node
			(*childIt)->m_pModel->draw(Cam);
		}

		//SceneNode* temp = *it;

		//while (temp->m_Children.size() == 1)
		//{


			//const set<SceneNode*> tempChildren = temp->getChildren();

			//for (set<SceneNode*>::iterator it = tempChildren.begin(); it != tempChildren.end(); ++it)
			//{
			//	(*it)->draw(Cam);
			//}
		//}

	}
}


Scene::Scene()
{
	// TODO: Add your code   
	this->m_Root.setLocalTransform(Vector(0, 0, 0), Vector(1, 1, 1), 0.f);
	this->m_Root.setModel(NULL);
	this->m_Root.setName("RootNode");
	this->m_Root.setParent(NULL);
	this->m_Root.setScaling(Vector(1, 1, 1));

	this->m_Models = map<string, Model*>();
	this->m_Models.clear();
}

Scene::~Scene()
{
	// TODO: Add your code
	this->m_Models.clear();
}

bool Scene::addSceneFile(const char* Scenefile)
{
	string line;
	ifstream file(Scenefile);

	if (file.is_open())
	{
		// dirty workaround probably
		SceneNode* prevNode = NULL;

		// while scene objects available
		while (getline(file, line))
		{
			vector<string> object;

			stringstream ss(line);
			string temp;

			while (ss >> temp)
				object.push_back(temp);

			for (size_t i = 1; i < object.size(); ++i)
			{
				size_t position = object[i].find("=");

				object[i] = object[i].substr(position + 1);
			}

			// model saving
			// MODEL(0)
			// ID(1), FILE(2)
			if (!object[0].compare("MODEL"))
			{
				//BaseModel* pModel;
				//PhongShader* pPhongShader;

				//pTankBot = new Model(ASSET_DIRECTORY "tank_bottom.dae", false);
				//pPhongShader = new PhongShader();
				//pTankBot->shader(pPhongShader, true);
				//// add to render list
				//Models.push_back(pTankBot);

				Model* pModel;
				PhongShader* pPhongShader;
				
				pModel = new Model((ASSET_DIRECTORY + object[2]).c_str(), false);
				pPhongShader = new PhongShader();
				pModel->shader(pPhongShader, true);

				//std::map<std::string, Model*>
				//mymap.insert(std::pair<char, int>('a', 100));
				this->m_Models[object[1]] = pModel;
			}


			// node saving
			// NODE(0)
			// ID(1), PARENTID(2), MODELID(3),
			// TRANSLATION_X, TRANSLATION_Y, TRANSLATION_Z,
			// ROTATIONAXIS_X, ROTATIONAXIS_Y, ROTATIONAXIS_Z,
			// ROTATIONANGLE
			// SCALE_X, SCALE_Y, SCALE_Z
			if (!object[0].compare("NODE"))
			{
				SceneNode* pNode = new SceneNode();

				pNode->setName(object[1]);

				// no parent node
				if (!object[2].compare("NULL"))
				{
					// add as child of root node
					this->m_Root.addChild(pNode);

					// add root as parent node
					pNode->setParent(&this->m_Root);
				}

				if (prevNode != NULL)
				{
					// is child of previous node?
					if (!prevNode->getName().compare(object[2]))
					{
						prevNode->addChild(pNode);
						pNode->setParent(prevNode);
					}
				}

				// model loading
				Model* pModel;
				pModel = this->m_Models[object[3]];
				pNode->setModel(pModel);

				// model local transform
				Vector translation = Vector((float)atof(object[4].c_str()), (float)atof(object[5].c_str()), (float)atof(object[6].c_str()));
				Vector rotationAxis = Vector((float)atof(object[7].c_str()), (float)atof(object[8].c_str()), (float)atof(object[9].c_str()));
				float angle = (float)atof(object[10].c_str());

				Matrix localTransform;
				localTransform.rotationAxis(rotationAxis, angle);
				localTransform.translation(translation);

				pNode->setLocalTransform(localTransform);

				// model scaling
				Vector scaling = Vector((float)atof(object[11].c_str()), (float)atof(object[12].c_str()), (float)atof(object[13].c_str()));

				pNode->setScaling(scaling);

				// save current node reference
				prevNode = pNode;
			}
		}
	}

	return true;
}

void Scene::draw(const BaseCamera& Cam)
{
	// TODO: Add your code

	this->m_Root.draw(Cam);
}

//void Scene::draw(SceneNode* pNode)
//{
//	pNode->getModel()->draw(Cam)
//}

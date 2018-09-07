//#include "Weapon.h"
//
//#define M_PI 3.14159265358979323846
//
//Weapon::Weapon()
//{
//	this->attackPower = 5.f;
//	this->isThrown = false;
//}
//
//Weapon::~Weapon()
//{
//	delete(this->mWeapon);
//}
//
//bool Weapon::loadModels(const char * File)
//{
//	mWeapon = new Model(File, false);
//
//	mWeapon->shader(this->pShader, true);
//
//	//Matrix rotate = rotate.rotationY(0.5f * M_PI);
//	mWeapon->transform();
//
//	return true;
//}
//
//void Weapon::update(float dtime)
//{
//	Matrix transformMatrix = this->mWeapon->transform();
//	Matrix translationMatrix, rotationMatrix;
//}
//
//void Weapon::draw(const BaseCamera & Cam)
//{
//
//}
//
//void Weapon::melee()
//{
//
//}
//
//void Weapon::ranged()
//{
//
//}

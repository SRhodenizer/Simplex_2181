#include "MyEntityManager.h"
using namespace Simplex;
//  MyEntityManager
MyEntityManager* MyEntityManager::m_pInstance = nullptr;
void MyEntityManager::Init(void)
{
	m_uEntityCount = 0;//sets the count to 0
	m_entityList.clear();//makes sure the list is empty
}
void MyEntityManager::Release(void)
{
	for (int i = 0; i < m_uEntityCount; ++i)//for all the entities 
	{
		//delets them
		MyEntity* pEntity = m_entityList[i];
		SafeDelete(pEntity);
	}
	m_uEntityCount = 0;//sets count to 0
	m_entityList.clear();//clears list
}
MyEntityManager* MyEntityManager::GetInstance()
{
	if(m_pInstance == nullptr)//if the instance is null
	{
		m_pInstance = new MyEntityManager();//make a new one 
	}
	return m_pInstance;//else return the one you have 
}
void MyEntityManager::ReleaseInstance()
{
	if(m_pInstance != nullptr)//if it's not nullptr
	{
		//delete it
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}
int Simplex::MyEntityManager::GetEntityIndex(String a_sUniqueID)
{
	//loop through to find the right name
	for (int  i = 0; i < m_uEntityCount; ++i)
	{
		//if the name is right
		if (a_sUniqueID == m_entityList[i]->GetUniqueID()) 
		{
			return i;//return the id for it 
		}
	}
	//if not found return -1
	return -1;
}
//Accessors
Model* Simplex::MyEntityManager::GetModel(uint a_uIndex)
{
	//if the list is empty return nullptr
	if (m_entityList.size() == 0) 
	{
		return nullptr;
	}

	// if out of bounds
	if (a_uIndex >= m_uEntityCount) 
	{
		a_uIndex = m_uEntityCount - 1;//fix the index till you're not 
	}

	return m_entityList[a_uIndex]->GetModel();//return the model at that index 
}
Model* Simplex::MyEntityManager::GetModel(String a_sUniqueID)
{
	//Get the entity
	MyEntity* tempMod = MyEntity::GetEntity(a_sUniqueID);

	//if the entity exists
	if (tempMod)
	{
		return tempMod->GetModel();//return its model
	}
	return nullptr;//else return nullptr 
}
RigidBody* Simplex::MyEntityManager::GetRigidBody(uint a_uIndex)
{
	//if the list is empty return nullptr 
	if (m_entityList.size() == 0) 
	{
		return nullptr;
	}

	// if out of bounds fix the index 
	if (a_uIndex >= m_uEntityCount) 
	{
		a_uIndex = m_uEntityCount - 1;
	}	

	return m_entityList[a_uIndex]->GetRigidBody();//return the rigid body at the index 
}
RigidBody* Simplex::MyEntityManager::GetRigidBody(String a_sUniqueID)
{
	//Get the entity
	MyEntity* tempRig = MyEntity::GetEntity(a_sUniqueID);
	//if the entity exists
	if (tempRig)
	{
		return tempRig->GetRigidBody();//return its rigid body
	}
	return nullptr;//else return nullptr
}
matrix4 Simplex::MyEntityManager::GetModelMatrix(uint a_uIndex)
{
	//if the list is empty return the identity matrix
	if (m_entityList.size() == 0) 
	{
		return IDENTITY_M4;
	}

	// if out of bounds fix the index 
	if (a_uIndex >= m_uEntityCount) 
	{
		a_uIndex = m_uEntityCount - 1;
	}

	return m_entityList[a_uIndex]->GetModelMatrix();//return the correct model matrix 
}
matrix4 Simplex::MyEntityManager::GetModelMatrix(String a_sUniqueID)
{
	//Get the entity
	MyEntity* tempMat = MyEntity::GetEntity(a_sUniqueID);
	//if the entity exists
	if (tempMat)
	{
		return tempMat->GetModelMatrix();//return its model matrix
	}
	return IDENTITY_M4;//else return the identity 
}
void Simplex::MyEntityManager::SetModelMatrix(matrix4 a_m4ToWorld, String a_sUniqueID)
{
	//Get the entity
	MyEntity* pTemp = MyEntity::GetEntity(a_sUniqueID);
	//if the entity exists
	if (pTemp)
	{
		pTemp->SetModelMatrix(a_m4ToWorld);
	}
}

void Simplex::MyEntityManager::SetModelMatrix(matrix4 a_m4ToWorld, uint a_uIndex)
{
	//if the list is empty return blank
	if (m_entityList.size() == 0)
		return;

	// if out of bounds
	if (a_uIndex >= m_uEntityCount) 
	{
		a_uIndex = m_uEntityCount - 1;
	}
		

	m_entityList[a_uIndex]->SetModelMatrix(a_m4ToWorld);
}
//The big 3
MyEntityManager::MyEntityManager(){Init();}
MyEntityManager::MyEntityManager(MyEntityManager const& other){ }
MyEntityManager& MyEntityManager::operator=(MyEntityManager const& other) { return *this; }
MyEntityManager::~MyEntityManager(){Release();};
// other methods
void Simplex::MyEntityManager::Update(void)
{
	//check collisions
	for (uint i = 0; i < m_uEntityCount - 1; i++)//for all the entities in the list
	{
		for (uint j = i + 1; j < m_uEntityCount; j++)//check if colliding with all the others 
		{
			m_entityList[i]->IsColliding(m_entityList[j]);
		}
	}
}
void Simplex::MyEntityManager::AddEntity(String a_sFileName, String a_sUniqueID)
{
	//Create a new entity to store the object
	MyEntity* ent = new MyEntity(a_sFileName, a_sUniqueID);
	//if I was able to generate it add it to the list
	if (ent->IsInitialized())
	{
		m_entityList.push_back(ent);
		m_uEntityCount = m_entityList.size();
	}
}
void Simplex::MyEntityManager::RemoveEntity(uint a_uIndex)
{
	//if the list is empty return
	if (m_entityList.size() == 0) 
	{
		return;
	}

	// if out of bounds choose the last one
	if (a_uIndex >= m_uEntityCount) 
	{
		a_uIndex = m_uEntityCount - 1;
	}

	// if the entity is not the very last we swap it for the last one
	if (a_uIndex != m_uEntityCount - 1)
	{
		std::swap(m_entityList[a_uIndex], m_entityList[m_uEntityCount - 1]);
	}
	
	//and then pop the last one, and deletes the pointer 
	MyEntity* temp = m_entityList[m_uEntityCount - 1];
	SafeDelete(temp);
	m_entityList.pop_back();
	--m_uEntityCount;//lowers the count 
	return;
}
void Simplex::MyEntityManager::RemoveEntity(String a_sUniqueID)
{
	//trantitions to the id based remove method
	int nIndex = GetEntityIndex(a_sUniqueID);
	RemoveEntity((uint)nIndex);
}
String Simplex::MyEntityManager::GetUniqueID(uint a_uIndex)
{
	//if the list is empty return blank
	if (m_entityList.size() == 0) 
	{
		return "";
	}
	//if the index is larger than the number of entries we are asking for the last one
	if (a_uIndex >= m_entityList.size()) 
	{
		a_uIndex = m_entityList.size() - 1;
	}
	return m_entityList[a_uIndex]->GetUniqueID();//return the correct ID
}
MyEntity* Simplex::MyEntityManager::GetEntity(uint a_uIndex)
{
	//if the list is empty return blank
	if (m_entityList.size() == 0) 
	{
		return nullptr;
	}
	//if the index is larger than the number of entries we are asking for the last one
	if (a_uIndex >= m_entityList.size()) 
	{
		a_uIndex = m_entityList.size() - 1;
	}	

	return m_entityList[a_uIndex];//return the right entity
}
void Simplex::MyEntityManager::AddEntityToRenderList(uint a_uIndex, bool a_bRigidBody)
{
	//if index is less than 0, load all of them 
	if (a_uIndex >= m_uEntityCount)
	{
		//add for each one in the entity list
		for (a_uIndex = 0; a_uIndex < m_uEntityCount; ++a_uIndex)
		{
			m_entityList[a_uIndex]->AddToRenderList(a_bRigidBody);
		}
	}
	else //only load 1 
	{
		m_entityList[a_uIndex]->AddToRenderList(a_bRigidBody);
	}
}
void Simplex::MyEntityManager::AddEntityToRenderList(String a_sUniqueID, bool a_bRigidBody)
{
	//Get the entity
	MyEntity* temp = MyEntity::GetEntity(a_sUniqueID);
	//if the entity exists
	if (temp)
	{
		temp->AddToRenderList(a_bRigidBody);
	}
}
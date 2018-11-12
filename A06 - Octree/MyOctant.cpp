#include "MyOctant.h"
using namespace Simplex;

uint MyOctant::m_uOctantCount = 0;
uint MyOctant::m_uMaxLevel = 0;
uint MyOctant::m_uIdealEntityCount = 0;

void MyOctant::Init(void)
{
	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = EntityManager::GetInstance();
}

//constructor
MyOctant::MyOctant(uint a_nMaxLevel, uint a_nIdealEntityCount) 
{
	m_uMaxLevel = a_nMaxLevel;//sets max level 
	m_uIdealEntityCount = a_nIdealEntityCount;//sets ideal entity count

	if (m_uOctantCount == 0)//if this is the first octant make it the root  
	{
		m_uID = 0;//set id to 0
		m_pRoot = this;
	}
	else //if it's not add it to it's list of children
	{
		m_uID++;//increment the ID num
		m_lChild.push_back(this);
	}
	Init();
	m_uOctantCount++;//adds another octant to the count 
}

MyOctant::MyOctant(vector3 a_v3Center, float a_fSize)
{
	m_v3Center = a_v3Center;//sets the center 
	m_fSize = a_fSize;//sets the size

	//sets min and max for the octant 
	m_v3Max = m_v3Center + a_fSize;
	m_v3Min = m_v3Center - a_fSize;
	
	if (m_uOctantCount == 0)//if this is the first octant make it the root  
	{
		m_uID = 0;//set id to 0
		m_pRoot = this;
	}
	else //if it's not add it to it's list of children
	{
		m_uID++;//increment the ID num
		m_lChild.push_back(this);
	}
	Init();
	m_uOctantCount++;//adds another octant to the count 
}

//copy constructor
MyOctant::MyOctant(MyOctant const& other) 
{

}

//copy assignment operator
MyOctant& MyOctant::operator=(MyOctant const& other) 
{
	m_uID = other.m_uID;
	m_uLevel = other.m_uLevel;
	m_uChildren = other.m_uChildren;
	m_fSize = other.m_fSize;
	*m_pParent = *other.m_pParent;
	for (int i = 0; i < 0; i++) 
	{
		*m_pChild[i] = *other.m_pChild[i];
	}
	m_EntityList = other.m_EntityList;
}

//destructor
MyOctant::~MyOctant(void) 
{

}

void MyOctant::Swap(MyOctant& other) 
{

}

//returns the MyOctant size
float MyOctant::GetSize(void) 
{
	return m_fSize;
}

//return the center point in global space
vector3 MyOctant::GetCenterGlobal(void) 
{
	return m_v3Center;
}

//returns the min point in global space 
vector3 MyOctant::GetMinGlobal(void) 
{
	return m_v3Min;
}

//return the max point in global space
vector3 MyOctant::GetMaxGlobal(void) 
{
	return m_v3Max;
}

bool MyOctant::IsColliding(uint a_uRBIndex) 
{
	return false;
}

void MyOctant::Display(uint a_nIndex, vector3 a_v3Color) 
{
	matrix4 octBox = glm::translate(m_v3Center) * glm::scale(vector3(m_v3Max.x-m_v3Min.x, m_v3Max.y - m_v3Min.y, m_v3Max.z - m_v3Min.z));
	m_pMeshMngr->AddWireCubeToRenderList(octBox, a_v3Color, 1);
}

void MyOctant::Display(vector3 a_v3Color) 
{
	matrix4 octBox = glm::translate(m_v3Center) * glm::scale(vector3(m_v3Max.x - m_v3Min.x, m_v3Max.y - m_v3Min.y, m_v3Max.z - m_v3Min.z));
	m_pMeshMngr->AddWireCubeToRenderList(octBox, a_v3Color, 1);
}

void MyOctant::DisplayLeafs(vector3 a_v3Color) 
{

}

void MyOctant::ClearEntityList(void) 
{

}

void MyOctant::Subdivide(void) 
{
	if (IsLeaf()) //if the octant is a leaf 
	{
		m_uChildren = 8;//set number of children to 8
		//for each of the nex 8 children
		for (int i = 0; i < 8; i++) 
		{
			m_pChild[i] = new MyOctant((uint) 2, (uint) 5);
			m_pChild[i]->m_uLevel++;//makes the new octants in the next level
			m_pChild[i]->m_pParent = this;//sets the parent to this object
		}
	}
}

//returns the child at the specified index 
MyOctant* MyOctant::GetChild(uint a_nChild) 
{
	if (m_uChildren == 0)//if the octant has no children 
	{
		return nullptr;//return null
	}
	if (a_nChild > 7) //if the index is too high
	{
		a_nChild = 7;//set to max index
	}
	
	return m_pChild[a_nChild];//return the indexed child
}

//returns the parent of the octant
MyOctant* MyOctant::GetParent(void) 
{
	return m_pParent;//returns the parent
}

//checks if the octant contains children
bool MyOctant::IsLeaf(void) 
{
	//if there are no children
	if (m_uChildren == 0) 
	{
		return true;//yes
	}
	
	return false;//else no
}

bool MyOctant::ContainsMoreThan(uint a_nEntities) 
{
	return false;
}

//deletes all children and the children's children :(
void MyOctant::KillBranches(void) 
{
	for each(MyOctant* child in m_pChild)//for all children in the octree 
	{
		if (child->IsLeaf() == true)//if there are no children
		{
			SafeDelete(child);//delete this
		}
		else 
		{
			child->KillBranches();//else repeat for children's children
		}
	}
}

void MyOctant::ConstructTree(uint a_nMaxLevel) 
{

}

void MyOctant::AssignIDtoEntity(void) 
{

}

//returns the octant count 
uint MyOctant::GetOctantCount(void) 
{
	return m_uOctantCount;
}

//destroy everything 
void MyOctant::Release(void) 
{
	//kills all the children
	m_pRoot->KillBranches();
	//kills the root 
	SafeDelete(m_pRoot);
}


void MyOctant::ConstructList(void) 
{

}
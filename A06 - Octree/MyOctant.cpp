#include "MyOctant.h"
using namespace Simplex;

//sets static members to avoid errors 
uint MyOctant::m_uOctantCount;
uint MyOctant::m_uMaxLevel;
uint MyOctant::m_uIdealEntityCount;
uint MyOctant::currentMaxLevel;
uint MyOctant::numCreated;
uint MyOctant::layers;
uint MyOctant::totalOcts;

//initialize member variables 
void MyOctant::Init(void)
{
	//initialize Mesh and Entity Managers 
	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();
}

//constructor for the root octant
MyOctant::MyOctant(uint a_nMaxLevel, uint a_nIdealEntityCount) 
{
	m_uMaxLevel = a_nMaxLevel;//sets max level 
	m_uIdealEntityCount = a_nIdealEntityCount;//sets ideal entity count

	Init();//initialize the singleton entity manager and mesh manager 

	if (m_uOctantCount == 0)//if this is the first octant make it the root  
	{
		m_uID = 0;//set id to 0
		m_uLevel = 0;//sets level to 0
		m_pRoot = this;
		m_v3Center = vector3(0,0,0);//sets the center 
		m_fSize = 35.0f;//sets the size
		currentMaxLevel = 0;
		numCreated = 0;
		layers = 0;

		//sets min and max for the octant 
		m_v3Max = m_v3Center + m_fSize;
		m_v3Min = m_v3Center - m_fSize;

		//for each of the indexes in the entity manager
		for (int i = 0; i < m_pEntityMngr->GetEntityCount(); i++) 
		{
			m_EntityList.push_back(i);//add the index 
		}
		
	}
	else //if it's not add it to it's list of children
	{
		m_uID++;//increment the ID num
		m_lChild.push_back(this);
	}
	m_uOctantCount++;//adds another octant to the count 

	//if we are over the desired entity count and under the max level 
	if (m_EntityList.size() > m_uIdealEntityCount && m_uLevel < m_uMaxLevel)
	{
		currentMaxLevel++;
		
		Subdivide();//make more octants 
	}

}

//makes the branch octants 
MyOctant::MyOctant(vector3 a_v3Center, float a_fSize)
{
	Init();//initialize the singleton entity manager and mesh manager 

	//sets the level for the octant 
	m_uLevel = currentMaxLevel;

	if (m_uLevel == m_uMaxLevel) 
	{
		numCreated++;
		//layers++;
	}

	if (numCreated == 8) 
	{
		currentMaxLevel--;
		totalOcts++;
		numCreated = 0;
	}

	if (totalOcts == 8)
	{
		currentMaxLevel--;
		layers++;
		totalOcts = 0;
	}
	
	if (layers == 8)
	{
		currentMaxLevel--;
		layers = 0;
	}
	

	m_v3Center = a_v3Center;//sets the center 
	m_fSize = a_fSize;//sets the size

	//sets min and max for the octant 
	m_v3Max = m_v3Center + a_fSize;
	m_v3Min = m_v3Center - a_fSize;
	
	m_uID++;//increment the ID num
	//if the octant is a child of the root add it to the right list
	if (m_uOctantCount > 0 && m_uOctantCount < 9)
	{
		m_lChild.push_back(this);//add it to the list of children
	}
	
	m_uOctantCount++;//adds another octant to the count 
	
	//for each of the indexes in the entity manager - NEEDS CHANGING
	for (int i = 0; i < 6; i++)
	{
		m_EntityList.push_back(i);//add the index 
	}

	//if we are over the desired entity count and under the max level 
 	if (m_EntityList.size() > m_uIdealEntityCount && m_uLevel < m_uMaxLevel)
	{
		currentMaxLevel++;
		Subdivide();//make more octants
		std::cout << "OctCount: "<<m_uOctantCount<<"\n";
	}
}

//copy constructor
MyOctant::MyOctant(MyOctant const& other) 
{
	Init();
	m_uID = other.m_uID;
	m_uLevel = other.m_uLevel;
	m_uChildren = other.m_uChildren;
	m_fSize = other.m_fSize;
	*m_pParent = *other.m_pParent;
	*m_pRoot = *other.m_pRoot;
	for (int i = 0; i < 0; i++)
	{
		*m_pChild[i] = *other.m_pChild[i];
	}
	m_EntityList = other.m_EntityList;
	
	
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
	return (MyOctant&)other;
}

//destructor
MyOctant::~MyOctant(void) 
{
	if (!IsLeaf()) 
	{
		KillBranches();
	}
	//m_uOctantCount--;
	
	currentMaxLevel = 0;
	numCreated = 0;
	m_uOctantCount = 0;
	totalOcts = 0;
	layers = 0;
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

//checks in indexed rigidbody is colliding with rigidbodies in the octant 
bool MyOctant::IsColliding(uint a_uRBIndex) 
{
	//for each model in this octant's list 
	for each(uint num in m_EntityList) 
	{
		//if the provided index is colliding with them
		if (m_pEntityMngr->GetRigidBody(a_uRBIndex)->IsColliding(m_pEntityMngr->GetRigidBody(num)))
		{
			return true;//yes
		}
	}
	
	return false;//else no
}

//displays the octant box 
void MyOctant::Display(uint a_nIndex, vector3 a_v3Color) 
{
	matrix4 octBox = glm::translate(m_v3Center) * glm::scale(vector3(m_v3Max.x-m_v3Min.x, m_v3Max.y - m_v3Min.y, m_v3Max.z - m_v3Min.z));
	m_pMeshMngr->AddWireCubeToRenderList(octBox, a_v3Color, 1);
}

//displays the octant box 
void MyOctant::Display(vector3 a_v3Color) 
{
	matrix4 octBox = glm::translate(m_v3Center) * glm::scale(vector3(m_v3Max.x - m_v3Min.x, m_v3Max.y - m_v3Min.y, m_v3Max.z - m_v3Min.z));
	m_pMeshMngr->AddWireCubeToRenderList(octBox, a_v3Color, 1);

	for(int i = 0; i < 8;i++) 
	{
		if (m_pChild[i] != nullptr) 
		{
			m_pChild[i]->Display(a_v3Color);
		}
	}
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
		//centerpoints for the 8 children
		vector3 centerPoints[8];
		centerPoints[0] = vector3(m_v3Center.x + (m_fSize / 2.0f), m_v3Center.y + (m_fSize / 2.0f), m_v3Center.z + (m_fSize / 2.0f));
		centerPoints[1] = vector3(m_v3Center.x - (m_fSize / 2.0f), m_v3Center.y + (m_fSize / 2.0f), m_v3Center.z + (m_fSize / 2.0f));
		centerPoints[2] = vector3(m_v3Center.x + (m_fSize / 2.0f), m_v3Center.y + (m_fSize / 2.0f), m_v3Center.z - (m_fSize / 2.0f));
		centerPoints[3] = vector3(m_v3Center.x - (m_fSize / 2.0f), m_v3Center.y + (m_fSize / 2.0f), m_v3Center.z - (m_fSize / 2.0f));
		centerPoints[4] = vector3(m_v3Center.x + (m_fSize / 2.0f), m_v3Center.y - (m_fSize / 2.0f), m_v3Center.z + (m_fSize / 2.0f));
		centerPoints[5] = vector3(m_v3Center.x - (m_fSize / 2.0f), m_v3Center.y - (m_fSize / 2.0f), m_v3Center.z + (m_fSize / 2.0f));
		centerPoints[6] = vector3(m_v3Center.x + (m_fSize / 2.0f), m_v3Center.y - (m_fSize / 2.0f), m_v3Center.z - (m_fSize / 2.0f));
		centerPoints[7] = vector3(m_v3Center.x - (m_fSize / 2.0f), m_v3Center.y - (m_fSize / 2.0f), m_v3Center.z - (m_fSize / 2.0f));

		m_uChildren = 8;//set number of children to 8

		//for each of the nex 8 children
		for (int i = 0; i < 8; i++) 
		{
			//makes one of the correct size at a nex center point 
			m_pChild[i] = new MyOctant(centerPoints[i], (m_fSize/2.0f));
			m_pChild[i]->m_pParent = this;//sets the parent to this object
			std::cout << "Child Level: " << m_pChild[i]->m_uLevel << "\n";
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
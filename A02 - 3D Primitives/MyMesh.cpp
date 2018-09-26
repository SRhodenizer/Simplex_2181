#include "MyMesh.h"
void MyMesh::Init(void)
{
	m_bBinded = false;
	m_uVertexCount = 0;

	m_VAO = 0;
	m_VBO = 0;

	m_pShaderMngr = ShaderManager::GetInstance();
}
void MyMesh::Release(void)
{
	m_pShaderMngr = nullptr;

	if (m_VBO > 0)
		glDeleteBuffers(1, &m_VBO);

	if (m_VAO > 0)
		glDeleteVertexArrays(1, &m_VAO);

	m_lVertex.clear();
	m_lVertexPos.clear();
	m_lVertexCol.clear();
}
MyMesh::MyMesh()
{
	Init();
}
MyMesh::~MyMesh() { Release(); }
MyMesh::MyMesh(MyMesh& other)
{
	m_bBinded = other.m_bBinded;

	m_pShaderMngr = other.m_pShaderMngr;

	m_uVertexCount = other.m_uVertexCount;

	m_VAO = other.m_VAO;
	m_VBO = other.m_VBO;
}
MyMesh& MyMesh::operator=(MyMesh& other)
{
	if (this != &other)
	{
		Release();
		Init();
		MyMesh temp(other);
		Swap(temp);
	}
	return *this;
}
void MyMesh::Swap(MyMesh& other)
{
	std::swap(m_bBinded, other.m_bBinded);
	std::swap(m_uVertexCount, other.m_uVertexCount);

	std::swap(m_VAO, other.m_VAO);
	std::swap(m_VBO, other.m_VBO);

	std::swap(m_lVertex, other.m_lVertex);
	std::swap(m_lVertexPos, other.m_lVertexPos);
	std::swap(m_lVertexCol, other.m_lVertexCol);

	std::swap(m_pShaderMngr, other.m_pShaderMngr);
}
void MyMesh::CompleteMesh(vector3 a_v3Color)
{
	uint uColorCount = m_lVertexCol.size();
	for (uint i = uColorCount; i < m_uVertexCount; ++i)
	{
		m_lVertexCol.push_back(a_v3Color);
	}
}
void MyMesh::AddVertexPosition(vector3 a_v3Input)
{
	m_lVertexPos.push_back(a_v3Input);
	m_uVertexCount = m_lVertexPos.size();
}
void MyMesh::AddVertexColor(vector3 a_v3Input)
{
	m_lVertexCol.push_back(a_v3Input);
}
void MyMesh::CompileOpenGL3X(void)
{
	if (m_bBinded)
		return;

	if (m_uVertexCount == 0)
		return;

	CompleteMesh();

	for (uint i = 0; i < m_uVertexCount; i++)
	{
		//Position
		m_lVertex.push_back(m_lVertexPos[i]);
		//Color
		m_lVertex.push_back(m_lVertexCol[i]);
	}
	glGenVertexArrays(1, &m_VAO);//Generate vertex array object
	glGenBuffers(1, &m_VBO);//Generate Vertex Buffered Object

	glBindVertexArray(m_VAO);//Bind the VAO
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);//Bind the VBO
	glBufferData(GL_ARRAY_BUFFER, m_uVertexCount * 2 * sizeof(vector3), &m_lVertex[0], GL_STATIC_DRAW);//Generate space for the VBO

	// Position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)0);

	// Color attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)(1 * sizeof(vector3)));

	m_bBinded = true;

	glBindVertexArray(0); // Unbind VAO
}
void MyMesh::Render(matrix4 a_mProjection, matrix4 a_mView, matrix4 a_mModel)
{
	// Use the buffer and shader
	GLuint nShader = m_pShaderMngr->GetShaderID("Basic");
	glUseProgram(nShader); 

	//Bind the VAO of this object
	glBindVertexArray(m_VAO);

	// Get the GPU variables by their name and hook them to CPU variables
	GLuint MVP = glGetUniformLocation(nShader, "MVP");
	GLuint wire = glGetUniformLocation(nShader, "wire");

	//Final Projection of the Camera
	matrix4 m4MVP = a_mProjection * a_mView * a_mModel;
	glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(m4MVP));
	
	//Solid
	glUniform3f(wire, -1.0f, -1.0f, -1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);  

	//Wire
	glUniform3f(wire, 1.0f, 0.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(-1.f, -1.f);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);
	glDisable(GL_POLYGON_OFFSET_LINE);

	glBindVertexArray(0);// Unbind VAO so it does not get in the way of other objects
}
void MyMesh::AddTri(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft)
{
	//C
	//| \
	//A--B
	//This will make the triangle A->B->C 
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);
}
void MyMesh::AddQuad(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft, vector3 a_vTopRight)
{
	//C--D
	//|  |
	//A--B
	//This will make the triangle A->B->C and then the triangle C->B->D
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);

	AddVertexPosition(a_vTopLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopRight);
}
void MyMesh::GenerateCube(float a_fSize, vector3 a_v3Color)
{
	if (a_fSize < 0.01f)
		a_fSize = 0.01f;

	Release();
	Init();

	float fValue = a_fSize * 0.5f;
	//3--2
	//|  |
	//0--1

	vector3 point0(-fValue,-fValue, fValue); //0
	vector3 point1( fValue,-fValue, fValue); //1
	vector3 point2( fValue, fValue, fValue); //2
	vector3 point3(-fValue, fValue, fValue); //3

	vector3 point4(-fValue,-fValue,-fValue); //4
	vector3 point5( fValue,-fValue,-fValue); //5
	vector3 point6( fValue, fValue,-fValue); //6
	vector3 point7(-fValue, fValue,-fValue); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCuboid(vector3 a_v3Dimensions, vector3 a_v3Color)
{
	Release();
	Init();

	vector3 v3Value = a_v3Dimensions * 0.5f;
	//3--2
	//|  |
	//0--1
	vector3 point0(-v3Value.x, -v3Value.y, v3Value.z); //0
	vector3 point1(v3Value.x, -v3Value.y, v3Value.z); //1
	vector3 point2(v3Value.x, v3Value.y, v3Value.z); //2
	vector3 point3(-v3Value.x, v3Value.y, v3Value.z); //3

	vector3 point4(-v3Value.x, -v3Value.y, -v3Value.z); //4
	vector3 point5(v3Value.x, -v3Value.y, -v3Value.z); //5
	vector3 point6(v3Value.x, v3Value.y, -v3Value.z); //6
	vector3 point7(-v3Value.x, v3Value.y, -v3Value.z); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCone(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	//creates the cone
	vector3 baseLeft;
	vector3 baseRight = vector3(a_fRadius * std::cos(0),  -.5f*a_fHeight ,.5f * a_fRadius *std::sin(0)); //the right point in the tri to draw, initialized to be at the 0th angle interval  
	vector3 baseCenter = vector3(0, -.5f*a_fHeight, 0);//the center point of the base circle 
	vector3 topCenter = vector3(0, .5*a_fHeight, 0);//the center point at the top of the cone 


	float angle = 360.0f / a_nSubdivisions; //gets the standard deviation in angles for the number of subdivisions 

	for (int i = 0; i <= a_nSubdivisions; i++) //for each sub division
	{
		//gets the current degree interval 
		float currentAngle = angle * i;

		baseLeft = baseRight;//changes the left point to the previous right point
		baseRight = vector3(a_fRadius * std::cos(currentAngle * 3.14159f / 180), -.5f*a_fHeight, a_fRadius *std::sin(currentAngle* 3.14159f / 180));//changes the right point to be one angle interval over 

		AddTri(baseLeft, baseRight, baseCenter);//draws the tri at the base
		AddTri(baseRight, baseLeft, topCenter);//draws the tri for the cone wall
	}

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCylinder(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	//creates the cylinder
	vector3 baseLeft;
	vector3 baseRight = vector3(a_fRadius * std::cos(0), -.5 * a_fHeight, a_fRadius *std::sin(0)); //the right point in the tri to draw, initialized to be at the 0th angle interval  
	vector3 baseCenter = vector3(0, -.5 * a_fHeight, 0);//the center point of the base circle 
	vector3 topLeft;
	vector3 topRight = vector3(a_fRadius * std::cos(0), .5 * a_fHeight, a_fRadius *std::sin(0));
	vector3 topCenter = vector3(0, .5 * a_fHeight, 0);//the center point of the top circle


	float angle = 360.0f / a_nSubdivisions; //gets the standard deviation in angles for the number of subdivisions 

	for (int i = 0; i <= a_nSubdivisions; i++) //for each sub division
	{
		//gets the current degree interval 
		float currentAngle = angle * i;

		//sets points for the base circle's triangle 
		baseLeft = baseRight;//changes the left point to the previous right point
		baseRight = vector3(a_fRadius * std::cos(currentAngle * 3.14159f / 180), -.5 * a_fHeight, a_fRadius *std::sin(currentAngle* 3.14159f / 180));//changes the right point to be one angle interval over 

		//sets points for the top circle's triangle 
		topLeft = topRight;//changes the left point to the previous right point
		topRight = vector3(a_fRadius * std::cos(currentAngle * 3.14159f / 180), .5 * a_fHeight, a_fRadius *std::sin(currentAngle * 3.14159f / 180));//changes the right point to be one angle interval over 

		
		AddTri(topRight, topLeft,  topCenter);//draws the tri for the top circle
		AddQuad(topLeft, topRight, baseLeft, baseRight);//draws the quad in between the base triangles
		AddTri(baseLeft, baseRight,   baseCenter);//draws the tri at the base circle
	}

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTube(float a_fOuterRadius, float a_fInnerRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	//for the inner circle
	vector3 iBaseLeft;
	vector3 iBaseRight = vector3(a_fInnerRadius * std::cos(0), -.5 * a_fHeight, a_fInnerRadius *std::sin(0)); //the right point in the tri to draw, initialized to be at the 0th angle interval  
	vector3 iBaseCenter = vector3(0, -.5 * a_fHeight, 0);//the center point of the base circle 
	vector3 iTopLeft;
	vector3 iTopRight = vector3(a_fInnerRadius * std::cos(0), .5 * a_fHeight, a_fInnerRadius *std::sin(0));
	vector3 iTopCenter = vector3(0, .5 * a_fHeight, 0);//the center point of the top circle

	//for the outer circle
	vector3 oBaseLeft;
	vector3 oBaseRight = vector3(a_fOuterRadius * std::cos(0), -.5 * a_fHeight, a_fOuterRadius *std::sin(0)); //the right point in the tri to draw, initialized to be at the 0th angle interval  
	vector3 oBaseCenter = vector3(0, -.5 * a_fHeight, 0);//the center point of the base circle 
	vector3 oTopLeft;
	vector3 oTopRight = vector3(a_fOuterRadius * std::cos(0), .5 * a_fHeight, a_fOuterRadius *std::sin(0));
	vector3 oTopCenter = vector3(0, .5 * a_fHeight, 0);//the center point of the top circle

	float angle = 360.0f / a_nSubdivisions; //gets the standard deviation in angles for the number of subdivisions 

	for (int i = 0; i <= a_nSubdivisions; i++) //for each sub division
	{
		//gets the current degree interval 
		float currentAngle = angle * i;

		//inner circle

		//sets points for the base circle's triangle 
		iBaseLeft = iBaseRight;//changes the left point to the previous right point
		iBaseRight = vector3(a_fInnerRadius * std::cos(currentAngle * 3.14159f / 180), -.5 * a_fHeight, a_fInnerRadius *std::sin(currentAngle* 3.14159f / 180));//changes the right point to be one angle interval over 

		//sets points for the top circle's triangle 
		iTopLeft = iTopRight;//changes the left point to the previous right point
		iTopRight = vector3(a_fInnerRadius * std::cos(currentAngle * 3.14159f / 180), .5 * a_fHeight, a_fInnerRadius *std::sin(currentAngle * 3.14159f / 180));//changes the right point to be one angle interval over 
																																							  
		//inner circle

		//sets points for the base circle's triangle 
		oBaseLeft = oBaseRight;//changes the left point to the previous right point
		oBaseRight = vector3(a_fOuterRadius * std::cos(currentAngle * 3.14159f / 180), -.5 * a_fHeight, a_fOuterRadius *std::sin(currentAngle* 3.14159f / 180));//changes the right point to be one angle interval over 

		//sets points for the top circle's triangle 
		oTopLeft = oTopRight;//changes the left point to the previous right point
		oTopRight = vector3(a_fOuterRadius * std::cos(currentAngle * 3.14159f / 180), .5 * a_fHeight, a_fOuterRadius *std::sin(currentAngle * 3.14159f / 180));//changes the right point to be one angle interval over 

		//draw quads in between the inner and outer circles
		AddQuad(oBaseLeft, oBaseRight, iBaseLeft, iBaseRight);
		AddQuad(iTopLeft, iTopRight, oTopLeft, oTopRight);

		//draw quads to connect the top and bottom
		AddQuad(iBaseLeft, iBaseRight, iTopLeft,iTopRight);
		AddQuad(oBaseRight, oBaseLeft, oTopRight, oTopLeft);
		
	}
	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTorus(float a_fOuterRadius, float a_fInnerRadius, int a_nSubdivisionsA, int a_nSubdivisionsB, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_nSubdivisionsA < 3)
		a_nSubdivisionsA = 3;
	if (a_nSubdivisionsA > 360)
		a_nSubdivisionsA = 360;

	if (a_nSubdivisionsB < 3)
		a_nSubdivisionsB = 3;
	if (a_nSubdivisionsB > 360)
		a_nSubdivisionsB = 360;

	Release();
	Init();

	vector4 cylinCenter;//the center point of cylinder faces 
	std::vector<std::vector<vector3>> shapeList;//a list of lists of points in the cylinder faces 
	//ex a 7/7 torus would have 7 lists of 7 points each showing the points in one of the heptagons

	float cylinderRadius = .5f * (a_fOuterRadius - a_fInnerRadius);//radius of the cylinder
	float toCenter = a_fOuterRadius - cylinderRadius;//the distance from center of torus to centerpoint of the cylinder parts 
	
	//get the angles for ring and cylinder creation
	float outerAngle = 360.0f / a_nSubdivisionsA;
	float innerAngle = 360.0f / a_nSubdivisionsB;

	//sets up a matrix for rotating points 
	matrix4 rotation = matrix4();//rotation to make sure all the faces are standing up
	matrix4 rotation2 = matrix4();//the rotation to make sure all the disks face where they need to be

	rotation = glm::rotate((3.14159f/2) ,vector3(1,0,0));//rotate 90 degrees or pi/2 radians

	for (int i = 0; i < a_nSubdivisionsA; i++)//for each torus subdivision
	{
		float currentOAngle = outerAngle * i;//sets the current angle for rotation
		rotation2 = glm::rotate((-currentOAngle * (3.14159f / 180)), vector3(0, 1.0f, 0));//rotate the points currentOuterAngle degrees 
		cylinCenter = vector4(toCenter *std::cos(currentOAngle*(3.14159f / 180)), 0, toCenter*std::sin(currentOAngle * (3.14159f / 180)),0);//sets the center point for the current cylinder segment

		vector4 cylinLeft;//sets up left point for circle math
		vector4 cylinRight = vector4(cylinderRadius*(1 / 180.0f), 0, 0,0);//sets up right point for circle math

		std::vector<vector3> shapePoints;//vector to push points into 
		shapeList.push_back(shapePoints);//pushes that vector into shapeList
		

		for (int j = 0; j <= a_nSubdivisionsB; j++)//for each cylinder subdivision
		{
			float currentIAngle = innerAngle * j;//sets current angle for rotation
			cylinLeft = cylinRight;//sets the left point to the old right point 
			cylinRight = cylinCenter + vector4(cylinderRadius * std::cos(currentIAngle * (3.14159f/180)),0,cylinderRadius*std::sin(currentIAngle*(3.14159f/180)),0);//moves the right point over another subdivision

			//rotates the right point to the correct orientation
			cylinRight = rotation * (cylinRight - cylinCenter);
			cylinRight = rotation2 * (cylinRight + cylinCenter);

			if (j == 0)//for the first time only  
			{
				shapeList[i].push_back(cylinLeft);//push back the left point 
			}
				
			shapeList[i].push_back(cylinRight);//push back the right point 
			
		}
	}

	//draws quads to connect the points 
	for (int i = 0; i < a_nSubdivisionsA; i++)
	{
		for (int j = 0; j < a_nSubdivisionsB; j++)
		{
			//makes the next values in the vectors safe to use 
			int nextI = (i + 1) % a_nSubdivisionsA;
			int nextJ = (j + 1) % a_nSubdivisionsB;

			//draws the quad
			AddQuad(shapeList[i][j], shapeList[i][nextJ] , shapeList[nextI][j], shapeList[nextI][nextJ]);
		}
	}
	

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateSphere(float a_fRadius, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	//Sets minimum and maximum of subdivisions
	if (a_nSubdivisions < 1)
	{
		GenerateCube(a_fRadius * 2.0f, a_v3Color);
		return;
	}
	if (a_nSubdivisions > 6)
		a_nSubdivisions = 6;

	Release();
	Init();

	//creates the cone
	vector3 baseLeft;
	vector3 baseRight = vector3(a_fRadius * std::cos(0), -1*a_fRadius + (2* a_fRadius/a_nSubdivisions), .5f * a_fRadius *std::sin(0)); //the right point in the tri to draw, initialized to be at the 0th angle interval  
	vector3 baseCenter = vector3(0, -1 * a_fRadius, 0);//the center point of the base circle 

	vector3 topLeft;
	vector3 topRight = vector3(a_fRadius * std::cos(0), a_fRadius, .5f * a_fRadius *std::sin(0)); //the right point in the tri to draw, initialized to be at the 0th angle interval  
	vector3 topCenter = vector3(0,  a_fRadius, 0);//the center point of the top circle 


	float angle = 360.0f / a_nSubdivisions; //gets the standard deviation in angles for the number of subdivisions 

	for (int i = 0; i <= a_nSubdivisions; i++) //for each sub division
	{
		//gets the current degree interval 
		float currentAngle = angle * i;

		baseLeft = baseRight;//changes the left point to the previous right point
		baseRight = vector3(a_fRadius * std::cos(currentAngle * 3.14159f / 180), -1 * a_fRadius + (a_fRadius / a_nSubdivisions), a_fRadius *std::sin(currentAngle* 3.14159f / 180));//changes the right point to be one angle interval over 
		
		topLeft = topRight;//changes the left point to the previous right point
		topRight = vector3(a_fRadius * std::cos(currentAngle * 3.14159f / 180), a_fRadius - (a_fRadius / a_nSubdivisions), a_fRadius *std::sin(currentAngle* 3.14159f / 180));//changes the right point to be one angle interval over 

		AddTri(baseLeft, baseRight, baseCenter);//draws the tris at the base circle
	
		if (i != 0) 
		{
			AddTri(topRight, topLeft, topCenter);//draws the tri at the top circle
		}


		for (int j = 1; j < a_nSubdivisions; j++)
		{
			vector3 qPBaseLeft = vector3(baseLeft.x, baseLeft.y + j * (a_fRadius / a_nSubdivisions), baseLeft.z );
			vector3 qPBaseRight = vector3(baseRight.x, baseRight.y + j * (a_fRadius / a_nSubdivisions), baseRight.z);


			vector3 qPTopLeft = vector3(topLeft.x, topLeft.y - j*(a_fRadius / a_nSubdivisions), topLeft.z );
			vector3 qPTopRight = vector3(topRight.x, topRight.y - j*(a_fRadius / a_nSubdivisions), topRight.z );

			AddQuad(qPBaseRight, qPBaseLeft, qPTopRight, qPTopLeft);
		}
	}

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
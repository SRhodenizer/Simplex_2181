#include "AppClass.h"

std::vector<MyMesh*> meshList;

void Application::InitVariables(void)
{
	//init the mesh
	for (float i = 0; i < 46; i++)//the number of cubes needed to make the alien
	{
		 m_pMesh = new MyMesh();
		 m_pMesh->GenerateCube(1, C_BLACK);
		 meshList.push_back(m_pMesh);
	}
	
	//m_pMesh->GenerateCube(1.0f, C_WHITE);
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	matrix4 m4View = m_pCameraMngr->GetViewMatrix();
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix();
	
	matrix4 m4Scale = glm::scale(IDENTITY_M4, vector3(2.0f,2.0f,2.0f));
	static float value = 0.0f;
	matrix4 m4Translate = glm::translate(IDENTITY_M4, vector3(value, 2.0f, 3.0f));
	value += 0.01f;

	//matrix4 m4Model = m4Translate * m4Scale;
	matrix4 m4Model = m4Scale;// *m4Translate;

	//matrix for getting the cubes in the shape of the alien
	matrix4 translation = matrix4();
	for (int i = 0; i < 46; i++)
	{
		meshList[i]->Render(m4Projection, m4View, m4Model);//makes all the shapes 
		translation = glm::translate(vector3(i, 0, 0));//translation matrix that moves them so they are not on top of each other 
		m4Model = m4Scale * translation;//applies first matrix
		m4Model = m4Model * m4Translate;//applies the move right matrix
	}


	m_pMesh->Render(m4Projection, m4View, m4Model);
	
	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();
	
	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();
	
	//draw gui
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	SafeDelete(m_pMesh);

	//release GUI
	ShutdownGUI();
}
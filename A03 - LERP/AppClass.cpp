#include "AppClass.h"

//the list of stop lists
std::vector<std::vector<vector3>> stops;
static std::vector<int> numStop;

void Application::InitVariables(void)
{
	//Change this to your name and email
	m_sProgrammer = "Stephen Rhodenizer - scr3876@rit.edu";
	
	//Set the position and target of the camera
	//(I'm at [0,0,10], looking at [0,0,0] and up is the positive Y axis)
	m_pCameraMngr->SetPositionTargetAndUpward(AXIS_Z * 20.0f, ZERO_V3, AXIS_Y);

	//if the light position is zero move it
	if (m_pLightMngr->GetPosition(1) == ZERO_V3)
		m_pLightMngr->SetPosition(vector3(0.0f, 0.0f, 3.0f));

	//if the background is cornflowerblue change it to black (its easier to see)
	if (vector3(m_v4ClearColor) == C_BLUE_CORNFLOWER)
	{
		m_v4ClearColor = vector4(ZERO_V3, 1.0f);
	}
	
	//if there are no segments create 7
	if(m_uOrbits < 1)
		m_uOrbits = 7;

	float fSize = 1.0f; //initial size of orbits

	//creating a color using the spectrum 
	uint uColor = 650; //650 is Red
	//prevent division by 0
	float decrements = 250.0f / (m_uOrbits > 1? static_cast<float>(m_uOrbits - 1) : 1.0f); //decrement until you get to 400 (which is violet)
	/*
		This part will create the orbits, it start at 3 because that is the minimum subdivisions a torus can have
	*/
	uint uSides = 3; //start with the minimal 3 sides


	int num = 0;//the current list we are modifying;
	for (uint i = uSides; i < m_uOrbits + uSides; i++)
	{
		vector3 v3Color = WaveLengthToRGB(uColor); //calculate color based on wavelength
		m_shapeList.push_back(m_pMeshMngr->GenerateTorus(fSize, fSize - 0.1f, 3, i, v3Color)); //generate a custom torus and add it to the meshmanager
		
		uColor -= static_cast<uint>(decrements); //decrease the wavelength

		//list of stops in the shape
		std::vector<vector3> shape;
		stops.push_back(shape);//push the list to the list of lists 

		float angle = 360 / i;

		//make the list of stops for each shape 
		for (int u = 0; u < i; u++) 
		{
			float currentAngle = angle * u;

			stops[num].push_back(vector3(fSize * std::cos(currentAngle * 3.14159f / 180),fSize * std::sin(currentAngle * 3.14159f / 180),0));//pushes the stop to the list 
		}
		num++;//increments the num so we use the next list 
		fSize += 0.5f; //increment the size for the next orbit
		numStop.push_back(0);//helps with reset fixing 
	}
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

	matrix4 m4View = m_pCameraMngr->GetViewMatrix(); //view Matrix
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix(); //Projection Matrix
	matrix4 m4Offset = IDENTITY_M4; //offset of the orbits, starts as the global coordinate system
	/*
		The following offset will orient the orbits as in the demo, start without it to make your life easier.
	*/
	//m4Offset = glm::rotate(IDENTITY_M4, 1.5708f, AXIS_Z);

	//the size of the orbit 
	float orbitSize = 1.0f;

	//Get a timer
	static float fTimer = 0;//store the new timer
	static uint uClock = m_pSystem->GenClock(); //generate a new clock for that timer
	fTimer += m_pSystem->GetDeltaTime(uClock); //get the delta time for that timer

	//get the percentage
	float fTimeBetweenStops = 2.0;//in seconds
	//map the value to be between 0.0 and 1.0
	float fPercentage = MapValue(fTimer, 0.0f, fTimeBetweenStops, 0.0f, 1.0f);


	// draw the shapes
	
	for (uint i = 0; i < m_uOrbits; ++i) //the number of which shape we are currently using 
	{
		vector3 v3CurrentPos;
		m_pMeshMngr->AddMeshToRenderList(m_shapeList[i], glm::rotate(m4Offset, 1.5708f, AXIS_X));
		matrix4 m4Model;

		//changes the position for drawing the sphere 
		vector3 startPos = stops[i][numStop[i] % stops[i].size()];
		vector3 endPos = stops[i][(numStop[i] + 1) % stops[i].size()];

		v3CurrentPos = glm::lerp(startPos, endPos, fPercentage);//lerps there 

		orbitSize += .5f;//changes the values so all spheres line up with all the orbitals
			
		m4Model = glm::translate(m4Offset, v3CurrentPos);
		
		//draw spheres
		m_pMeshMngr->AddSphereToRenderList(m4Model * glm::scale(vector3(0.1)), C_WHITE);
	}

	if (fPercentage >= 1.0f)
	{
		for (int j = 0; j < numStop.size();j++) //loops the route for all shapes 
		{
			//go to the next route
			numStop[j]++;
		}
		fTimer = m_pSystem->GetDeltaTime(uClock);//restart the clock
	}
	

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
	//release GUI
	ShutdownGUI();
}
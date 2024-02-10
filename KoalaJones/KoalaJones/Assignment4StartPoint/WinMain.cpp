
#include "MyProject.h"
#include <Windowsx.h> // for GET__LPARAM macros
#include <SimpleMath.h>

//----------------------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nShowCmd)
{
	MyProject application(hInstance);    // Create the class variable

	// Initialize the window, if all is well show and update it so it displays
	if (application.InitWindowsApp(L"Assignment 4 - Time to Create!", nShowCmd) == false)
		return 0;                   // Error creating the window, terminate application

	// Set some DirectX object settings and initialize 
	application.SetClearColor(DirectX::Colors::BurlyWood.v);	// Set the background colour of the windo
	application.SetDepthStencil(false);						// Tell DirectX object to create and maintain a depth stencil buffer
	if (application.InitializeDirect3D())
	{
		application.InitalizeTextures();
		application.InitalizeSprites();
		application.MessageLoop();				// Window has been successfully created, start the application message loop
	}

	return 0;
}

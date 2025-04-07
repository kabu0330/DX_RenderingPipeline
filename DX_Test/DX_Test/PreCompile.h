#pragma once

#include "framework.h"
#include "resource.h"
#include <Base/EngineTimer.h>
#include <Base/EngineMath.h>
#include <Base/EngineFile.h>
#include <Base/EngineDirectory.h>
#include <Base/EnginePath.h>
#include <Base/EngineString.h>
#include <Base/EngineSerializer.h>
#include <Base/EngineDebug.h>
#include <list>
#include <vector>
#include <map>
#include <Windows.h>

#include <d3d11_4.h> 
#include <d3dcompiler.h> 

// Debug
#include <dxgidebug.h>
//#pragma comment(lib, "dxguid.lib")

#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler") 
#pragma comment(lib, "dxguid")

#pragma comment(lib, "DXGI") 


#include "DX_Test.h"
#include "Core.h"
#include "Renderer.h"
#include "Level.h"
#include "Actor.h"
#include "GraphicsDevice.h"

extern HINSTANCE hInst;
extern HWND hWnd;
extern HDC hdc;


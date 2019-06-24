#pragma once

/*
    ###################################################################################
    stdafx.h
    Include file for standard system include files, or project specific include files
    that are used frequently, but are changed infrequently.
    Used by gfxcore2.
    Made by PR00F88
    EMAIL : PR0o0o0o0o0o0o0o0o0o0oF88@gmail.com
    ###################################################################################
*/

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

#include <string>
#include <vector>

// GL headers, never changed
#include "../../PGE/PRRE/gl/GL.h"
#include "../../PGE/PRRE/gl/glu.h"
#include "../../PGE/PRRE/gl/glext.h"
#include "../../PGE/PRRE/gl/wglext.h"

// TODO: reference additional headers your program requires here
#include "../../CConsole/src/CConsole.h"
#include "../../PFL/PFL.h"
#include "../../PGE/PRRE/PR00FsReducedRenderingEngine.h"
#include "../../PGE/PRRE/PRREVector.h"
#include "../../PGE/PRRE/PRREMatrix.h"

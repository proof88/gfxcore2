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

#include <string>
#include <vector>

#include "targetver.h"
#ifndef WINPROOF88_ALLOW_CONTROLS_AND_DIALOGS
#define WINPROOF88_ALLOW_CONTROLS_AND_DIALOGS
#endif
#ifndef WINPROOF88_ALLOW_MSG_USER_WINMESSAGES
#define WINPROOF88_ALLOW_MSG_USER_WINMESSAGES
#endif
#include "../../../PFL/PFL/winproof88.h"

// GL headers, never changed
#include "../../../PGE/PGE/Pure/include/internal/gl/GL.h"
#include "../../../PGE/PGE/Pure/include/internal/gl/glu.h"
#include "../../../PGE/PGE/Pure/include/internal/gl/glext.h"
#include "../../../PGE/PGE/Pure/include/internal/gl/wglext.h"

// TODO: reference additional headers your program requires here
#include "../../../Console/CConsole/src/CConsole.h"
#include "../../../PFL/PFL/PFL.h"
#include "../../../PGE/PGE/Pure/include/external/PR00FsUltimateRenderingEngine.h"
#include "../../../PGE/PGE/Pure/include/external/Render/PureRendererHWfixedPipe.h"
#include "../../../PGE/PGE/Pure/include/external/Math/PureVector.h"
#include "../../../PGE/PGE/Pure/include/external/Math/PureMatrix.h"

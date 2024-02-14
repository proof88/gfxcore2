/*
  ###################################################################################
  gfxcore2.cpp
  Main source file of gfxcore2.
  This DLL is to be used by the legacy PR00FPS made in 2007.
  This DLL is only a wrapper so the old PR00FPS can utilize my new engine: PURE.
  Aim is to avoid any modification to PR00FPS code, the change is totally transparent.
  Made by PR00F88
  EMAIL : PR0o0o0o0o0o0o0o0o0o0oF88@gmail.com
  ###################################################################################
*/

#include "stdafx.h"  // PCH
#include "gfxcore2.h"

/*
   Internal variables
   ###########################################################################
*/

static std::vector<std::string> vReturnedStrings;

static PGEcfgProfiles* cfgProfiles = NULL;
static PGEInputHandler* inputHandler = NULL;

static PR00FsUltimateRenderingEngine* pure = NULL;
static PureImageManager*    imgmgr = NULL;
static PureTextureManager*  texmgr = NULL;
static PureObject3DManager* objmgr = NULL;
static PureCamera* camera = NULL;

static PureTexture* texLastCreateBlank = NULL;

static bool bDebugEnabled = false;

static DELPHI_BYTE   nMBlurUpdateRate = 0;
static DELPHI_TRGBA  clrMBlurColor;
static DELPHI_TRGBA  clrBgColor;

static TPURE_XYZ     vLegacyCameraPos;    // we store camera pos XYZ values as set by caller legacy project and we also return this value, see details at related functions
static TPURE_XYZ     vLegacyCameraAngle;  // we store camera angle XYZ values as set by caller legacy project and we also return this value, see details at related functions

// default texture settings for external objects
static DELPHI_BOOLEAN  bExtMipmapping = true;
static DELPHI_TGLCONST glExtFiltering = GL_LINEAR_MIPMAP_LINEAR;
static DELPHI_TGLCONST glExtEnvmode = GL_DECAL;
static DELPHI_BOOLEAN  bExtBorder = false;
static DELPHI_BOOLEAN  bExtCompressed = true;
static DELPHI_TGLCONST glExtWrapS = GL_REPEAT;
static DELPHI_TGLCONST glExtWrapT = GL_REPEAT;

/*
   Internal functions
   ###########################################################################
*/

/**
    Converts a received Delphi-string to C-string.
*/
static void StrConvDelphiStrToCStr(const DELPHI_BYTE* sDelphiStr, DELPHI_TSTR40 sDestStr)
{
    // char #0 holds the actual length of the string, so we shift char array left by 1 char
    // then make sure that string is terminated with nullchar at correct position as stated by Delphi.
    const int nOriginalLengthFromDelphi = sDelphiStr[0];

    strncpy((char*) sDestStr, (const char*) sDelphiStr+1, sizeof(DELPHI_BYTE)*40);
    sDestStr[nOriginalLengthFromDelphi] = '\0';
}

/**
    Converts a C-String to a Delphi-string before passing it to Delphi.
*/
static void StrConvStrToDelphiStr(const std::string& sCStr, DELPHI_TSTR40_RET& sDestStr)
{
    // we have to shift the char array right by 1 char, so we can put string length into char #0
    strncpy((char*) sDestStr.str+1, sCStr.c_str(), sizeof(DELPHI_BYTE)*sCStr.length());
    sDestStr.str[0] = (DELPHI_BYTE) sCStr.length();
}


/**
    COPIED FROM PureObject3D::PureObject3DImpl, HOPEFULLY LATER WE CAN INVOKE IT FROM MATERIALMANAGER.

    Gets the appropriate Pure blend factor for the given GL enum.
    @return The appropriate Pure blend factor for the given GL enum.
            PURE_ZERO for invalid GL enum.
*/
static TPURE_BLENDFACTOR getPureBlendFromGLBlend(GLenum glb)
{
    switch( glb )
    {
    case GL_ZERO                : return PURE_ZERO;
    case GL_ONE                 : return PURE_ONE;
    case GL_SRC_COLOR           : return PURE_SRC_COLOR;
    case GL_ONE_MINUS_SRC_COLOR : return PURE_ONE_MINUS_SRC_COLOR;
    case GL_DST_COLOR           : return PURE_DST_COLOR;
    case GL_ONE_MINUS_DST_COLOR : return PURE_ONE_MINUS_DST_COLOR;
    case GL_SRC_ALPHA           : return PURE_SRC_ALPHA;
    case GL_ONE_MINUS_SRC_ALPHA : return PURE_ONE_MINUS_SRC_ALPHA;
    case GL_DST_ALPHA           : return PURE_DST_ALPHA;         
    case GL_ONE_MINUS_DST_ALPHA : return PURE_ONE_MINUS_DST_ALPHA;
    case GL_SRC_ALPHA_SATURATE  : return PURE_SRC_ALPHA_SATURATE;
    default                     : return PURE_ZERO;
    }
}


/**
    This is not yet implemented in Pure.
    HOPEFULLY LATER WE CAN INVOKE IT FROM MATERIALMANAGER.
*/
static TPURE_TEX_WRAPPING getPuretexWrappingFromGLtexWrapping(GLenum glw)
{
    switch (glw)
    {
    case GL_CLAMP           : return PURE_TW_CLAMP;
    case GL_REPEAT          : return PURE_TW_REPEAT;
    case GL_MIRRORED_REPEAT : return PURE_TW_MIRRORED_REPEAT;
    case GL_CLAMP_TO_BORDER : return PURE_TW_CLAMP_TO_BORDER;
    case GL_CLAMP_TO_EDGE   : return PURE_TW_CLAMP_TO_EDGE;
    default:
        CConsole::getConsoleInstance().EOLn("getPuretexWrappingFromGLtexWrapping() wrapping branched to default!");
        return PURE_TW_REPEAT;
    }    
}


static bool createPrerequisitePgeObjects()
{
    if (!cfgProfiles)
    {
        cfgProfiles = new PGEcfgProfiles("");
        if (!cfgProfiles)
        {
            return false;
        }
    }
    
    inputHandler = &PGEInputHandler::createAndGet(*cfgProfiles);

    return true;
}


/*
   Exported function definitions
   ###########################################################################
*/

GFXCORE2_API DELPHI_BOOLEAN __stdcall tmcsInitialized()
{
    return pure->isInitialized();
}


/**
    @return 0 on success, >0 otherwise.
*/
GFXCORE2_API DELPHI_BYTE __stdcall tmcsInitGraphix(HWND wnd, DELPHI_BOOLEAN fs, DELPHI_INTEGER freq, DELPHI_INTEGER cdepth, DELPHI_INTEGER zdepth, DELPHI_BOOLEAN vsyncstate, DELPHI_INTEGER shading)
{
    if (!createPrerequisitePgeObjects())
    {
        return 1;
    }

    pure = &PR00FsUltimateRenderingEngine::createAndGet(*cfgProfiles, *inputHandler);

    /* HACK: since shading is practically used for nothing, we indicate the renderer in this parameter now: GL_FLAT means SW renderer, other means HW renderer */
    TPURE_RENDERER renderer = ( (shading == GL_FLAT) ? PURE_RENDERER_SW : PURE_RENDERER_HW_FP );
    
    /* WA: if the window is already created outside by caller, its width and height should be our new width and height */
    /* this workaround is for PR00FPS and other legacy projects using tmcsgfxlib where window is created first outside and then grafix init happens with specifying already existing window */
    TPureUInt width = 0;
    TPureUInt height = 0;
    if ( wnd != NULL )
    {
        // theoretically we always end up here because legacy projects using tmcsgfxlib always create window outside first
        RECT rect;
        GetClientRect(wnd, &rect);
        width = (TPureUInt) rect.right - rect.left;
        height = (TPureUInt) rect.bottom - rect.top;
    }

    DELPHI_BYTE nRetVal = (DELPHI_BYTE) pure->initialize(renderer, width, height, fs ? PURE_FULLSCREEN : PURE_WINDOWED, freq, cdepth, zdepth, 0, 0, wnd);

    if ( nRetVal == 0 )
    {
        pure->getScreen().setVSyncEnabled( vsyncstate );

        PureWindow& window = pure->getWindow();
        window.SetAutoCleanupOnQuitOn(false);
        window.SetCaption( "alma" );
        window.ShowFull();
        window.WriteSettings();

        imgmgr = &pure->getImageManager();
        texmgr = &pure->getTextureManager();
        objmgr = &pure->getObject3DManager();
        camera = &pure->getCamera();
    }

    return nRetVal;
}

GFXCORE2_API void __stdcall tmcsRender()
{
    pure->getRenderer()->RenderScene();
}

GFXCORE2_API void __stdcall tmcsRestoreOriginalDisplayMode()
{
    // apply the same display settings as were set originally BEFORE the initialization stage
    // example: user quits from the game, user ALT+TABs to another app, user minimizes game window, etc ...
    pure->getScreen().ResetDisplaySettings();
}

GFXCORE2_API void __stdcall tmcsRestoreDisplayMode()
{
    // unimplemented in PURE: apply the same display settings as were applied at the initialization stage
    // example: when user reactivates the game window, fullscreen settings should be re-applied, like screen resolution
}


/**
    @return 0 on success, 1 otherwise.
*/
GFXCORE2_API DELPHI_BYTE __stdcall tmcsShutdownGraphix()
{
    imgmgr = NULL;
    texmgr = NULL;
    objmgr = NULL;
    camera = NULL;
    texLastCreateBlank = NULL;

    // dont delete cfgProfiles because static inputHandler and other stuff may still use it during destruction,
    // those should be also changed into NOT static into future to properly resolve this dependency.

    return pure->shutdown() ? 0 : 1;
}

GFXCORE2_API void __stdcall tmcsEnableDebugging()
{
    if ( !bDebugEnabled )
    {
        bDebugEnabled = true;
        #ifdef PGE_CCONSOLE_IS_ENABLED
        // HACK: shouldn't grab cconsole this way but currently there is no other way to do it ...
        CConsole::getConsoleInstance().Initialize("gfxcore2 log", true);
        CConsole::getConsoleInstance().SetFGColor( FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE, "999999" );
        CConsole::getConsoleInstance().SetIntsColor( FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY, "FFFF00" );
        CConsole::getConsoleInstance().SetStringsColor( FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY, "FFFFFF" );
        CConsole::getConsoleInstance().SetFloatsColor( FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY, "FFFF00" );
        CConsole::getConsoleInstance().SetBoolsColor( FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY, "00FFFF" );
        #endif

        if (!createPrerequisitePgeObjects())
        {
            return;
        }
        PR00FsUltimateRenderingEngine::createAndGet(*cfgProfiles, *inputHandler).SetAutoWriteStatsAtShutdown(true);
    }
}

GFXCORE2_API void __stdcall tmcsDisableDebugging()
{
    if ( bDebugEnabled )
    {
        bDebugEnabled = false;
        #ifdef PGE_CCONSOLE_IS_ENABLED
        CConsole::getConsoleInstance().Deinitialize();
        #endif

        if (!createPrerequisitePgeObjects())
        {
            return;
        }
        PR00FsUltimateRenderingEngine::createAndGet(*cfgProfiles, *inputHandler).SetAutoWriteStatsAtShutdown(false);
    }
}

GFXCORE2_API void __stdcall tmcsSetGamma(DELPHI_INTEGER r, DELPHI_INTEGER g, DELPHI_INTEGER b)
{
    // unimplemented in PURE
}

GFXCORE2_API void  __stdcall tmcsEnableMotionBlur(DELPHI_WORD width, DELPHI_WORD height)
{
    // unimplemented in PURE
}

GFXCORE2_API void  __stdcall tmcsDisableMotionBlur()
{
    // unimplemented in PURE
}

GFXCORE2_API void  __stdcall tmcsFreeMotionBlurResources()
{
    // unimplemented in PURE
}

GFXCORE2_API void  __stdcall tmcsSetMotionBlurUpdateRate(DELPHI_BYTE rate)
{
    // unimplemented in PURE
    nMBlurUpdateRate = rate;
}

GFXCORE2_API DELPHI_TRGBA __stdcall tmcsGetMotionBlurColor()
{
    return clrMBlurColor;
}

GFXCORE2_API void  __stdcall tmcsSetMotionBlurColor(DELPHI_BYTE red, DELPHI_BYTE green, DELPHI_BYTE blue, DELPHI_BYTE alpha)
{
    clrMBlurColor.red   = red;
    clrMBlurColor.green = green;
    clrMBlurColor.blue  = blue;
    clrMBlurColor.alpha = alpha;
}


GFXCORE2_API void __stdcall tmcsSetBgColor(DELPHI_BYTE r, DELPHI_BYTE g, DELPHI_BYTE b, DELPHI_BYTE a)
{
    clrBgColor.red   = r;
    clrBgColor.green = g;
    clrBgColor.blue  = b;
    clrBgColor.alpha = a;
    camera->getBackgroundColor().Set(clrBgColor.red, clrBgColor.green, clrBgColor.blue, clrBgColor.alpha);
}

GFXCORE2_API void __stdcall tmcsInitCamera(
    DELPHI_SINGLE x, DELPHI_SINGLE y, DELPHI_SINGLE z, DELPHI_SINGLE ax, DELPHI_SINGLE ay, DELPHI_SINGLE az,
    DELPHI_DOUBLE fov, DELPHI_DOUBLE aspect, DELPHI_DOUBLE znear, DELPHI_DOUBLE zfar )
{
    tmcsSetCameraPos(x, y, z);
    tmcsSetCameraAngle(ax, ay, az);
    tmcsSetCameraFov(fov);
    tmcsSetCameraAspect(aspect);
    tmcsSetCameraNearPlane(znear);
    tmcsSetCameraFarPlane(zfar);
}

GFXCORE2_API void __stdcall tmcsSetviewport(DELPHI_INTEGER x, DELPHI_INTEGER y, DELPHI_INTEGER w, DELPHI_INTEGER h)
{
    camera->SetViewport(x, y, w, h);
}

GFXCORE2_API DELPHI_SINGLE __stdcall tmcsGetCameraX()
{
    return vLegacyCameraPos.x;
}

GFXCORE2_API DELPHI_SINGLE __stdcall tmcsGetCameraY()
{
    return vLegacyCameraPos.y;
}

GFXCORE2_API DELPHI_SINGLE __stdcall tmcsGetCameraZ()
{
    return vLegacyCameraPos.z;
}

GFXCORE2_API DELPHI_SINGLE __stdcall tmcsGetCameraAngleX()
{
    return vLegacyCameraAngle.x;
}

GFXCORE2_API DELPHI_SINGLE __stdcall tmcsGetCameraAngleY()
{
    return vLegacyCameraAngle.y;
}

GFXCORE2_API DELPHI_SINGLE __stdcall tmcsGetCameraAngleZ()
{
    return vLegacyCameraAngle.z;
}

GFXCORE2_API DELPHI_DOUBLE __stdcall tmcsGetCameraFov()
{
    return camera->getFieldOfView();
}

GFXCORE2_API DELPHI_DOUBLE __stdcall tmcsGetCameraAspect()
{
    return camera->getAspectRatio();
}

GFXCORE2_API DELPHI_DOUBLE __stdcall tmcsGetCameraNearPlane()
{
    return camera->getNearPlane();
}

GFXCORE2_API DELPHI_DOUBLE __stdcall tmcsGetCameraFarPlane()
{
    return camera->getFarPlane();
}

GFXCORE2_API DELPHI_SINGLE __stdcall tmcsWrapAngle(DELPHI_SINGLE f)
{
    if ( (f >= 0.0f) && (f < 360.0f) )
        return f;

    f -= floor( f / 360.0f ) * 360.0f;

    if ( f >= 360.0f )
        f -= 360.0f;
    if ( f < 0.0f )
        f += 360.0f;

    return f;
}

GFXCORE2_API DELPHI_SINGLE __stdcall tmcsGetNewX(DELPHI_SINGLE x, DELPHI_SINGLE angle, DELPHI_SINGLE factor)
{
    return x - cos((angle+90)*PFL::PI/180)/200*factor;
}

GFXCORE2_API DELPHI_SINGLE __stdcall tmcsGetNewZ(DELPHI_SINGLE z, DELPHI_SINGLE angle, DELPHI_SINGLE factor)
{
    return z + sin((angle+90)*PFL::PI/180)/200*factor;
}

GFXCORE2_API void __stdcall tmcsSetCameraX(DELPHI_SINGLE posx)
{
    // for compatibility with legacy projects, we need to negate input pos X and pos Z
    // and we store the input values so we can easily return them when a legacy project queries ...
    vLegacyCameraPos.x = posx;

    const TPureFloat dx = -posx - camera->getPosVec().getX();
    camera->getPosVec().SetX(camera->getPosVec().getX() + dx);
    camera->getTargetVec().SetX(camera->getTargetVec().getX() + dx);
}

GFXCORE2_API void __stdcall tmcsSetCameraY(DELPHI_SINGLE posy)
{
    // no compatibility issue with camera Y pos so far, but I still save it in legacy variable ...
    vLegacyCameraPos.y = posy;

    const TPureFloat dy = posy - camera->getPosVec().getY();
    camera->getPosVec().SetY(camera->getPosVec().getY() + dy);
    camera->getTargetVec().SetY(camera->getTargetVec().getY() + dy);
}

GFXCORE2_API void __stdcall tmcsSetCameraZ(DELPHI_SINGLE posz)
{
    // for compatibility with legacy projects, we need to negate input pos X and pos Z
    // and we store the input values so we can easily return them when a legacy project queries ...
    vLegacyCameraPos.z = posz;

    const TPureFloat dz = -posz - camera->getPosVec().getZ();
    camera->getPosVec().SetZ(camera->getPosVec().getZ() + dz);
    camera->getTargetVec().SetZ(camera->getTargetVec().getZ() + dz);
}

GFXCORE2_API void __stdcall tmcsSetCameraPos(DELPHI_SINGLE posx, DELPHI_SINGLE posy, DELPHI_SINGLE posz)
{
    tmcsSetCameraX(posx);
    tmcsSetCameraY(posy);
    tmcsSetCameraZ(posz);
}

GFXCORE2_API void __stdcall tmcsSetCameraAngleX(DELPHI_SINGLE anglex)
{
    // for compatibility with legacy projects, we need to negate input angle X and angle Z, and subtract 180° from angle Y
    // and we store the input values so we can easily return them when a legacy project queries ...
    // and also we need to limit angle x to (-90;+90)° exclusive otherwise PURE engine lookat() may fail to properly set the view.

    if ( anglex > 89.9f )
        anglex = 89.9f;
    else if ( anglex < -89.9f )
        anglex = -89.9f;

    vLegacyCameraAngle.x = anglex;
    camera->SetRotation(-vLegacyCameraAngle.x, vLegacyCameraAngle.y-180, -vLegacyCameraAngle.z);
}

GFXCORE2_API void __stdcall tmcsSetCameraAngleY(DELPHI_SINGLE angley)
{
    // for compatibility with legacy projects, we need to negate input angle X and angle Z, and subtract 180° from angle Y
    // and we store the input values so we can easily return them when a legacy project queries ...

    vLegacyCameraAngle.y = angley;
    camera->SetRotation(-vLegacyCameraAngle.x, vLegacyCameraAngle.y-180, -vLegacyCameraAngle.z);
}

GFXCORE2_API void __stdcall tmcsSetCameraAngleZ(DELPHI_SINGLE anglez)
{
    // for compatibility with legacy projects, we need to negate input angle X and angle Z, and subtract 180° from angle Y
    // and we store the input values so we can easily return them when a legacy project queries ...

    vLegacyCameraAngle.z = anglez;
    camera->SetRotation(-vLegacyCameraAngle.x, vLegacyCameraAngle.y-180, -vLegacyCameraAngle.z);
}

GFXCORE2_API void __stdcall tmcsSetCameraAngle(DELPHI_SINGLE anglex, DELPHI_SINGLE angley, DELPHI_SINGLE anglez)
{
    tmcsSetCameraAngleX(anglex);
    tmcsSetCameraAngleY(angley);
    tmcsSetCameraAngleZ(anglez);
}

GFXCORE2_API void __stdcall tmcsXRotateCamera(DELPHI_SINGLE angle)
{
    tmcsSetCameraAngleX(tmcsGetCameraAngleX() + angle);    
}

GFXCORE2_API void __stdcall tmcsYRotateCamera(DELPHI_SINGLE angle)
{
    tmcsSetCameraAngleY(tmcsGetCameraAngleY() + angle);
}

GFXCORE2_API void __stdcall tmcsZRotateCamera(DELPHI_SINGLE angle)
{
    tmcsSetCameraAngleZ(tmcsGetCameraAngleZ() + angle);
}

GFXCORE2_API void __stdcall tmcsSetCameraFov(DELPHI_DOUBLE fov)
{
    camera->SetFieldOfView((TPureFloat) fov);
}

GFXCORE2_API void __stdcall tmcsSetCameraAspect(DELPHI_DOUBLE aspect)
{
    camera->SetAspectRatio((TPureFloat)aspect);
}

GFXCORE2_API void __stdcall tmcsSetCameraNearPlane(DELPHI_DOUBLE znear)
{
    camera->SetNearPlane((TPureFloat)znear);
}

GFXCORE2_API void __stdcall tmcsSetCameraFarPlane(DELPHI_DOUBLE zfar)
{
    camera->SetFarPlane((TPureFloat)zfar);
}


GFXCORE2_API DELPHI_WORD __stdcall tmcsGetTotalObjects()
{
    return (DELPHI_WORD) objmgr->getCount();
}

GFXCORE2_API DELPHI_INTEGER __stdcall tmcsGetNumSubObjects(DELPHI_WORD num)
{
    const PureObject3D* const object = (PureObject3D*) objmgr->getAttachedAt(num);
    return object ? object->getCount() : 0;
}

GFXCORE2_API DELPHI_INTEGER __stdcall tmcsCreatePlane(DELPHI_SINGLE w, DELPHI_SINGLE h)
{
    // to mimic legacy engine, by default we create object in CLIENT memory, for that we need to select DYNAMIC modification habit
    const PureObject3D* const obj = objmgr->createPlane(w, h, PURE_VMOD_DYNAMIC, PURE_VREF_INDEXED, true);
    return obj ? objmgr->getAttachedIndex(*obj) : -1;
}

GFXCORE2_API DELPHI_INTEGER __stdcall tmcsCreateCube(DELPHI_SINGLE a)
{
    // to mimic legacy engine, by default we create object in CLIENT memory, for that we need to select DYNAMIC modification habit
    const PureObject3D* const obj = objmgr->createCube(a, PURE_VMOD_DYNAMIC, PURE_VREF_INDEXED, true);
    return obj ? objmgr->getAttachedIndex(*obj) : -1;
}

GFXCORE2_API DELPHI_INTEGER __stdcall tmcsCreateBox(DELPHI_SINGLE a, DELPHI_SINGLE b, DELPHI_SINGLE c)
{
    // to mimic legacy engine, by default we create object in CLIENT memory, for that we need to select DYNAMIC modification habit
    const PureObject3D* const obj = objmgr->createBox(a, b, c, PURE_VMOD_DYNAMIC, PURE_VREF_INDEXED, true);
    return obj ? objmgr->getAttachedIndex(*obj) : -1;
}

GFXCORE2_API DELPHI_INTEGER __stdcall tmcsCreateObjectFromFile(DELPHI_TSTR40 fname, DELPHI_BOOLEAN compiled)
{
    DELPHI_INTEGER retVal = -1;
    DELPHI_TSTR40 strTmpBuffer;
    StrConvDelphiStrToCStr(fname, strTmpBuffer);

    // save current texmanager settings before setting external object texture settings
    const TPURE_ISO_TEX_FILTERING fTexIsoFilteringMin = texmgr->getDefaultMinFilteringMode();
    const TPURE_ISO_TEX_FILTERING fTexIsoFilteringMag = texmgr->getDefaultMagFilteringMode();
    const TPURE_TEX_COMPRESSION_MODE compTex = texmgr->getDefaultCompressionMode();
    const TPureBool bBorder = texmgr->getDefaultBorder();
    const TPURE_TEX_WRAPPING twS = texmgr->getDefaultTextureWrappingModeS();
    const TPURE_TEX_WRAPPING twT = texmgr->getDefaultTextureWrappingModeT();

    // load external object texture settings   
    // TODO: following few attribs still not supported by PURE
    /*
        DELPHI_TGLCONST glExtEnvmode = GL_DECAL;
    */
    TPURE_ISO_TEX_FILTERING fNewTexIsoFilteringMin = bExtMipmapping ? (glExtFiltering == GL_LINEAR_MIPMAP_LINEAR ? PURE_ISO_LINEAR_MIPMAP_LINEAR : PURE_ISO_LINEAR_MIPMAP_NEAREST) : PURE_ISO_LINEAR;
    texmgr->setDefaultMinFilteringMode( fNewTexIsoFilteringMin );
    texmgr->setDefaultMagFilteringMode( PURE_ISO_LINEAR );
    texmgr->setDefaultCompressionMode( bExtCompressed ? PURE_TC_AUTO : PURE_TC_NONE );
    texmgr->setDefaultBorder( bExtBorder );
    texmgr->setDefaultTextureWrappingMode( getPuretexWrappingFromGLtexWrapping(glExtWrapS), getPuretexWrappingFromGLtexWrapping(glExtWrapT) );

    // need to set dynamic vmod habit to make sure bForceClientMemory = true case takes effect
    PureObject3D* const obj = objmgr->createFromFile((char*) strTmpBuffer, compiled ? PURE_VMOD_STATIC : PURE_VMOD_DYNAMIC, PURE_VREF_INDEXED, !compiled);
    if ( obj )
    {
        // need to flip along X axis because I noticed that with new engine the objects appear mirrored along X-axis compared to how they appear in old proofps engine
        obj->SetScaling( PureVector(-1.0f, 1.0f, 1.0f) );
        retVal = objmgr->getAttachedIndex(*obj);
    }


    // load original texmanager settings
    texmgr->setDefaultMinFilteringMode( fTexIsoFilteringMin );
    texmgr->setDefaultMagFilteringMode( fTexIsoFilteringMag );
    texmgr->setDefaultCompressionMode( compTex );
    texmgr->setDefaultBorder( bBorder );
    texmgr->setDefaultTextureWrappingMode( twS, twT );
    
    return retVal;
}

GFXCORE2_API DELPHI_INTEGER __stdcall tmcsCreateClonedObject(DELPHI_INTEGER refersto)
{
    PureObject3D* const referredobj = (PureObject3D*) objmgr->getAttachedAt(refersto);
    if ( !referredobj )
        return -1;

    PureObject3D* const obj = objmgr->createCloned( *referredobj );
    if ( obj )
    {
        return objmgr->getAttachedIndex(*obj);
    }
    else
        return -1;
}

GFXCORE2_API void __stdcall tmcsSetObjectMultiTextured(DELPHI_WORD index)
{
    // nothing to do in new engine, as in the new engine an object's material is implicitly considered as multitextured when
    // number of layers is more than 1.
}

GFXCORE2_API void __stdcall tmcsMultiTexAssignObject(DELPHI_WORD index1, DELPHI_WORD index2)
{
    PureObject3D* const obj1 = (PureObject3D*) objmgr->getAttachedAt(index1);
    const PureObject3D* const obj2 = (PureObject3D*) objmgr->getAttachedAt(index2);

    if ( !obj1 || !obj2 )
        return;

    // number of subobjects must be also equal
    if ( obj1->getCount() != obj2->getCount() )
        return;

    for (TPureUInt i = 0; i < obj1->getCount(); i++)
    {
        PureObject3D* const obj1Sub = (PureObject3D*) obj1->getAttachedAt(i);
        // assuming that obj2 has the same subobject- and vertex count as obj1
        PureObject3D* const obj2Sub = (PureObject3D*) obj2->getAttachedAt(i);
        if ( obj1Sub && obj2Sub )
        {
            // copying lightmap data into obj1 material's 2nd layer
            obj1Sub->getMaterial(false).copyFromMaterial(obj2Sub->getMaterial(false), 1, 0);
            // setBlendFuncs() is not recommended to be called from this lib because it doesn't invoke setDestinationBlendFunc()
            // if setSourceBlendFunc() is failed prior to it. And PR00FPS actually requests invalid source blend func sometimes ...
            // So we need to work around that by calling the 2 functions here separately.
            obj1Sub->getMaterial(false).setSourceBlendFunc(obj1Sub->getMaterial(false).getSourceBlendFunc(), 1);
            obj1Sub->getMaterial(false).setDestinationBlendFunc(obj1Sub->getMaterial(false).getDestinationBlendFunc(), 1);
        }
    }

    // WA: in old engine, object's blend state (true/false) was an explicit value, and only transparent object's had it as true.
    // But in new engine, it is implicit value, which evaluates to true depending on src and dst blending factors.
    // However, legacy projects set object blending factors NOT ONLY when an object should be transparent, but also when it is multitextured,
    // to tell the engine with what blending factors it should blend layer 2 over layer 1. In such case they dont set blend state to true at all.
    // In our new engine case, setting blending factors would mean the object is blended/transparent anyway, which is obviously not what we want
    // when legacy projects sets it for multitexturing purpose.
    // Not much idea on how to workaround it, so I just set the original object's blending mode here to default values meaning it is NOT blended.
    // This works because legacy PR00FPS always sets blending state to true first, then specifies blending modes, and finally calls this tmcsMultiTexAssignObject() for the object.
    // So after all here we have a chance to set blending funcs of the object back to default, since the blending funcs controlling how layer 2 
    // should be blended over layer 1 is already stored in subobject's material attribs in above loop. That will work fine even if we reset parent's
    // blending funcs.
    obj1->getMaterial(false).setBlendFuncs( PURE_ONE, PURE_ZERO );

    // At this point, we should be safe to delete obj2 since object's dtor calls material's dtor which doesn't free up the textures.
    // However, a mechanism is needed to be implemented to correctly handle this situation.
    // WA1: CopyFromMaterial() should hardcopy the textures also; deleting material should delete its textures too;
    // WA2: (better) textures should maintain refcount. Material deletion would decrement refcount and would effectively delete textures when refcount reaches 0.
    // But there is no need to delete anything now, as legacy engine didn't delete anything either as applications were responsible for that.
}

GFXCORE2_API void __stdcall tmcsDeleteObject(DELPHI_WORD index)
{
    /*
        Due to proofps 1.0 (2007) bug, we could not let application delete the first object, reasoning:
         - first object in proofps with index 0 is obj_mapsample alias loading screen;
         - after loading everything, proofps creates opening screen with 2 planes which move out of view in main loop;
         - after this, proofps sets boolean firstframe variable to true before entering main loop;
         - proofps enters main loop, 2 opening screen planes are moving away;
         - anytime when firstframe is true in proofps main loop, it deletes obj_mapsample and its texture because it doesn't need it anymore;
         - however, obj_mapsample stays 0 at this point which is important fact from bug point of view;
         - when user restarts the current game with in-game-menu option 4, opening screen planes are re-created and firstframe variable becomes true in proofps;
         - proofps enters main loop, 2 opening screen planes are moving away;
         - since firstframe is true again, it deletes obj_mapsample which has different result in new engine because:
           - with old engine the opening screen's upper moving plane had index > 100;
           - with new engine the opening screen's upper moving plane has index 0.
         -  the indices differ because old engine always added newly created objects at the end of the TList hence opening screen's upper moving plane never had index 0,
            but new engine may add newly created object wherever it finds an empty place such as at location 0 where originally the loading screen was;
         - so, after restarting current game, before main loop could properly move away both planes of opening screen, upper plane is deleted by proofps wrongly indexing,
           it shouldn't ever touch object index 0. Fix should come from application side: when it deletes obj_mapsample, it should also set obj_mapsample to -1, this way
           it couldn't delete another object having index 0 later.
         - unfortunately, this bug causes unplayable game after restarting current game, because mainloop won't call keyboard and bot handling routies when
           startpic_takeaway variable is true, and it forever stays true due to deleted upper screen won't ever reach screen upper bounds again ...
         - as the rule is NOT to modify the legacy game in any way, we workaround the problem in this wrapper here.

        Update: after testing proofps with not deleting object 0, it stopped leaking memory, and game was playable for multiple rounds.
        However, there were some glitches with some objects, so I rather stick to not deleting any object just hiding it. With that glitches were not found but memory leak is huge.
        Still playable for 2 consecutive rounds though.

        Update 2: fixed the problem in the game itself, no need for the WA anymore. Still keeping the above comment since that applies to the original PR00FPS 1.0.
    */
    PureObject3D* const obj = (PureObject3D*) objmgr->getAttachedAt(index);

    if ( obj )
    {
        objmgr->DeleteAttachedInstance(*obj);
    }
}

GFXCORE2_API void __stdcall tmcsDeleteObjects()
{
    objmgr->DeleteAll();
}

GFXCORE2_API void __stdcall tmcsSetExtObjectsTextureMode(DELPHI_BOOLEAN mipmapping, DELPHI_TGLCONST filtering, DELPHI_TGLCONST envmode, DELPHI_BOOLEAN border, DELPHI_BOOLEAN compressed, DELPHI_TGLCONST wrap_s, DELPHI_TGLCONST wrap_t)
{
    // currently there is no capability in PURE to set default texture mode for external loaded objects
    // so we store the passed parameters in wrapper lib, and use them in tmcsCreateObjectFromFile()

    bExtMipmapping = mipmapping;
    glExtFiltering = filtering;
    glExtEnvmode = envmode;
    bExtBorder = border;
    bExtCompressed = compressed;
    glExtWrapS = wrap_s;
    glExtWrapT = wrap_t;
}

GFXCORE2_API void __stdcall tmcsCompileObject(DELPHI_WORD index)
{
    PureObject3D* const obj = (PureObject3D*) objmgr->getAttachedAt(index);
    if ( obj )
        obj->setVertexTransferMode( PureVertexTransfer::selectVertexTransferMode(PURE_VMOD_STATIC, PURE_VREF_INDEXED, false) );
}

GFXCORE2_API void __stdcall tmcsShowObject(DELPHI_WORD index)
{
    PureObject3D* const obj = (PureObject3D*) objmgr->getAttachedAt(index);
    if ( obj )
        obj->Show();
}

GFXCORE2_API void __stdcall tmcsHideObject(DELPHI_WORD index)
{
    PureObject3D* const obj = (PureObject3D*) objmgr->getAttachedAt(index);
    if ( obj )
        obj->Hide();
}

GFXCORE2_API void __stdcall tmcsSetObjectWireframe(DELPHI_WORD index, DELPHI_BOOLEAN wf)
{
    PureObject3D* const obj = (PureObject3D*) objmgr->getAttachedAt(index);
    if ( obj )
        obj->SetWireframed(wf);
}

GFXCORE2_API void __stdcall tmcsSetObjectDoublesided(DELPHI_WORD index, DELPHI_BOOLEAN ds)
{
    PureObject3D* const obj = (PureObject3D*) objmgr->getAttachedAt(index);
    if ( obj )
        obj->SetDoubleSided(ds);
}

GFXCORE2_API void __stdcall tmcsSetObjectStickedState(DELPHI_WORD index, DELPHI_BOOLEAN state)
{
    PureObject3D* const obj = (PureObject3D*) objmgr->getAttachedAt(index);
    if ( obj )
        obj->SetStickedToScreen(state);
}

GFXCORE2_API void __stdcall tmcsSetObjectZBuffered(DELPHI_WORD index, DELPHI_BOOLEAN state)
{
    // TODO: check if SetTestingAgainstZBuffer() is enough or SetAffectingZBuffer() is needed as well
    PureObject3D* const obj = (PureObject3D*) objmgr->getAttachedAt(index);
    if ( obj )
        obj->SetTestingAgainstZBuffer(state);
}

GFXCORE2_API void __stdcall tmcsSetWiredCulling(DELPHI_BOOLEAN state)
{
    // this is a global setting in PR00FPS-engine, so apply it to all objects now
    for (TPureUInt i = 0; i < objmgr->getSize(); i++)
    {
        PureObject3D* const obj = (PureObject3D*) objmgr->getAttachedAt(i);
        if ( obj != PGENULL )
            obj->SetWireframedCulled(state);
    }
}

GFXCORE2_API DELPHI_SINGLE __stdcall tmcsGetXPos(DELPHI_WORD index)
{
    // for compatibility with legacy projects, we need to negate input pos X and pos Z for non-sticked objects
    // and return them negated when a legacy project queries so from outside they don't look negated at all.
    const PureObject3D* const obj = (PureObject3D*) objmgr->getAttachedAt(index);
    return obj ? (obj->isStickedToScreen() ? obj->getPosVec().getX() : -obj->getPosVec().getX()) : 0.f;
}

GFXCORE2_API DELPHI_SINGLE __stdcall tmcsGetYPos(DELPHI_WORD index)
{
    const PureObject3D* const obj = (PureObject3D*) objmgr->getAttachedAt(index);
    return obj ? obj->getPosVec().getY() : 0.f;
}

GFXCORE2_API DELPHI_SINGLE __stdcall tmcsGetZPos(DELPHI_WORD index)
{
    // for compatibility with legacy projects, we need to negate input pos X and pos Z for non-sticked objects
    // and return them negated when a legacy project queries so from outside they don't look negated at all.
    const PureObject3D* const obj = (PureObject3D*) objmgr->getAttachedAt(index);
    return obj ? (obj->isStickedToScreen() ? obj->getPosVec().getZ() : -obj->getPosVec().getZ()) : 0.f;
}

GFXCORE2_API DELPHI_SINGLE __stdcall tmcsGetSizeX(DELPHI_WORD index)
{
    const PureObject3D* const obj = (PureObject3D*) objmgr->getAttachedAt(index);
    return obj ? obj->getSizeVec().getX() : 0.f;
}

GFXCORE2_API DELPHI_SINGLE __stdcall tmcsGetSizeY(DELPHI_WORD index)
{
    const PureObject3D* const obj = (PureObject3D*) objmgr->getAttachedAt(index);
    return obj ? obj->getSizeVec().getY() : 0.f;
}

GFXCORE2_API DELPHI_SINGLE __stdcall tmcsGetSizeZ(DELPHI_WORD index)
{
    const PureObject3D* const obj = (PureObject3D*) objmgr->getAttachedAt(index);
    return obj ? obj->getSizeVec().getZ() : 0.f;
}

GFXCORE2_API DELPHI_SINGLE __stdcall tmcsGetAngleX(DELPHI_WORD index)
{
    // in legacy tmcsgfxlib, rotation was also problematic on X and Z axes, basically we need X-rotate on Z-axis in new engine,
    // with angle value negated, so here we need to return negated angle Z to give expected angle X to legacy projects.
    const PureObject3D* const obj = (PureObject3D*) objmgr->getAttachedAt(index);
    return obj ? -obj->getAngleVec().getZ() : 0.f;
}

GFXCORE2_API DELPHI_SINGLE __stdcall tmcsGetAngleY(DELPHI_WORD index)
{
    const PureObject3D* const obj = (PureObject3D*) objmgr->getAttachedAt(index);
    return obj ? obj->getAngleVec().getY() : 0.f;
}

GFXCORE2_API DELPHI_SINGLE __stdcall tmcsGetAngleZ(DELPHI_WORD index)
{
    // in legacy tmcsgfxlib, rotation was also problematic on X and Z axes, basically we need Z-rotate on X-axis in new engine,
    // with angle value negated, so here we need to return negated angle X to give expected angle Z to legacy projects.
    const PureObject3D* const obj = (PureObject3D*) objmgr->getAttachedAt(index);
    return obj ? -obj->getAngleVec().getX() : 0.f;
}

GFXCORE2_API DELPHI_BOOLEAN __stdcall tmcsIsVisible(DELPHI_WORD index)
{
    const PureObject3D* const obj = (PureObject3D*) objmgr->getAttachedAt(index);
    return obj ? obj->isRenderingAllowed() : false;
}

GFXCORE2_API DELPHI_SINGLE __stdcall tmcsGetScaling(DELPHI_WORD index)
{
    // uhh ... in legacy, scaling is just a single scalar value, applied on all axes.
    // but in the new engine, scaling can be set independently on the 3 axes, this means 3 different scalar values.
    // so what value to return here to legacy to keep compatibility?
    // well, since legacy behaves negatively on X- and Z-axes compared to new engine, and we may implicitly negate scaling as well on these 2 axes in this wrapper lib,
    // we should return Y scaling here because there is no diff on Y-axis behavior and we can use Y scaling value as valid for all 3 axes for legacy projects.

    const PureObject3D* const obj = (PureObject3D*) objmgr->getAttachedAt(index);
    return obj ? obj->getScaling().getY() * 100.0f : 0.f;
}

GFXCORE2_API DELPHI_TSTR40_RET __stdcall tmcsGetName(DELPHI_WORD index)
{
    DELPHI_TSTR40_RET retVal = {};
    const PureObject3D* const obj = (PureObject3D*) objmgr->getAttachedAt(index);

    if ( obj )
        StrConvStrToDelphiStr(obj->getName().c_str(), retVal);

    return retVal;
}

GFXCORE2_API void __stdcall tmcsSetXPos(DELPHI_WORD num, DELPHI_SINGLE factor)
{
    // for compatibility with legacy projects, we need to negate input pos X and pos Z for non-sticked objects
    PureObject3D* const obj = (PureObject3D*) objmgr->getAttachedAt(num);
    if ( obj )
        obj->getPosVec().SetX( obj->isStickedToScreen() ? factor : -factor);
    // we can safely rely on current sticked state since legacy projects always first set sticked state and then modified position
}

GFXCORE2_API void __stdcall tmcsSetYPos(DELPHI_WORD num, DELPHI_SINGLE factor)
{
    PureObject3D* const obj = (PureObject3D*) objmgr->getAttachedAt(num);
    if ( obj )
        obj->getPosVec().SetY(factor);
}

GFXCORE2_API void __stdcall tmcsSetZPos(DELPHI_WORD num, DELPHI_SINGLE factor)
{
    // for compatibility with legacy projects, we need to negate input pos X and pos Z for non-sticked objects
    PureObject3D* const obj = (PureObject3D*) objmgr->getAttachedAt(num);
    if ( obj )
        obj->getPosVec().SetZ(obj->isStickedToScreen() ? factor : -factor);
    // we can safely rely on current sticked state since legacy projects always first set sticked state and then modified position
}

GFXCORE2_API void __stdcall tmcsScaleObject(DELPHI_WORD num, DELPHI_SINGLE factor)
{
    PureObject3D* const obj = (PureObject3D*) objmgr->getAttachedAt(num);
    if ( obj )
        obj->Scale(factor / 100.0f);
}

GFXCORE2_API void __stdcall tmcsScaleObjectAbsolute(DELPHI_WORD num, DELPHI_SINGLE factor)
{
    PureObject3D* const obj = (PureObject3D*) objmgr->getAttachedAt(num);
    if ( obj )
        obj->SetScaling(factor / 100.0f);
}

GFXCORE2_API void __stdcall tmcsXRotateObject(DELPHI_WORD num, DELPHI_SINGLE factor)
{
    // in legacy tmcsgfxlib, rotation was also problematic on X and Z axes, basically input X factor
    // needs to be set on Z axis with negation!
    PureObject3D* const obj = (PureObject3D*) objmgr->getAttachedAt(num);
    if ( obj )
        obj->getAngleVec().SetZ( tmcsWrapAngle(obj->getAngleVec().getZ() -factor) );
}

GFXCORE2_API void __stdcall tmcsYRotateObject(DELPHI_WORD num, DELPHI_SINGLE factor)
{
    PureObject3D* const obj = (PureObject3D*) objmgr->getAttachedAt(num);

    // good thing we can call tmcsWrapAngle() because there is a glitch if not implemented:
    // in proofps, when a snail dies, its starts spinning around its Y axis by
    // tmcsYRotateObject() being called by proofps with factor = tmcsGetAngleY()+factor,
    // which basically results in Inf or NaN values after a few second due to float overflow.
    // It starts with 360° + 0.2, next frame 720.2° + 0.2, ...

    if ( obj )
        obj->getAngleVec().SetY( tmcsWrapAngle(obj->getAngleVec().getY() + factor) );
}

GFXCORE2_API void __stdcall tmcsZRotateObject(DELPHI_WORD num, DELPHI_SINGLE factor)
{
    // in legacy tmcsgfxlib, rotation was also problematic on X and Z axes, basically input Z factor
    // needs to be set on X axis with negation!
    PureObject3D* const obj = (PureObject3D*) objmgr->getAttachedAt(num);
    if ( obj )
        obj->getAngleVec().SetX( tmcsWrapAngle(obj->getAngleVec().getX() - factor) );
}

GFXCORE2_API void __stdcall tmcsSetAngleX(DELPHI_WORD num, DELPHI_SINGLE factor)
{
    // in legacy tmcsgfxlib, rotation was also problematic on X and Z axes, basically input X factor
    // needs to be set on Z axis with negation!
    PureObject3D* const obj = (PureObject3D*) objmgr->getAttachedAt(num);
    if ( obj )
        obj->getAngleVec().SetZ(tmcsWrapAngle(-factor));
}

GFXCORE2_API void __stdcall tmcsSetAngleY(DELPHI_WORD num, DELPHI_SINGLE factor)
{
    PureObject3D* const obj = (PureObject3D*) objmgr->getAttachedAt(num);
    if ( obj )
        obj->getAngleVec().SetY(tmcsWrapAngle(factor));
}

GFXCORE2_API void __stdcall tmcsSetAngleZ(DELPHI_WORD num, DELPHI_SINGLE factor)
{
    // in legacy tmcsgfxlib, rotation was also problematic on X and Z axes, basically input Z factor
    // needs to be set on X axis with negation!
    PureObject3D* const obj = (PureObject3D*) objmgr->getAttachedAt(num);
    if ( obj )
        obj->getAngleVec().SetX(tmcsWrapAngle(-factor));
}

GFXCORE2_API void __stdcall tmcsSetObjectRotationXZY(DELPHI_WORD num)
{
    PureObject3D* const obj = (PureObject3D*) objmgr->getAttachedAt(num);
    if ( obj )
        obj->SetRotationOrder(PURE_XZY);
}

GFXCORE2_API void __stdcall tmcsSetObjectRotationYXZ(DELPHI_WORD num)
{
    PureObject3D* const obj = (PureObject3D*) objmgr->getAttachedAt(num);
    if ( obj )
        obj->SetRotationOrder(/*PURE_YXZ proofps old*/PURE_YZX);
}

GFXCORE2_API void __stdcall tmcsSetName(DELPHI_WORD num, const DELPHI_TSTR40 name)
{
    PureObject3D* const obj = (PureObject3D*) objmgr->getAttachedAt(num);
    if ( obj )
    {
        DELPHI_TSTR40 strTmpBuffer;
        StrConvDelphiStrToCStr(name, strTmpBuffer);
        obj->SetName((char*)strTmpBuffer);
    }
}

GFXCORE2_API void __stdcall tmcsSetObjectLit(DELPHI_WORD num, DELPHI_BOOLEAN state)
{
    PureObject3D* const obj = (PureObject3D*) objmgr->getAttachedAt(num);
    if ( obj )
        obj->SetLit(state);
}

GFXCORE2_API void __stdcall tmcsSetObjectColor(DELPHI_WORD num, DELPHI_BYTE r, DELPHI_BYTE g, DELPHI_BYTE b)
{
    PureObject3D* const obj = (PureObject3D*) objmgr->getAttachedAt(num);
    if ( !obj )
        return;

    // currently new engine doesn't support vertex colors, so let's just set texture env color
    obj->getMaterial(false).getTextureEnvColor().Set(r, g, b, obj->getMaterial(false).getTextureEnvColor().getAlpha());

    for (TPureUInt i = 0; i < obj->getCount(); i++)
    {
        PureObject3D* const subobj = (PureObject3D*) obj->getAttachedAt(i);
        if ( subobj )
        {
            PureMaterial& const mat = subobj->getMaterial(false);
            for (TPureUInt j = 0; j < mat.getColorsCount(); j++)
            {
                mat.getColors()[j].red = r;
                mat.getColors()[j].green = g;
                mat.getColors()[j].blue = b;
            }
        }
    }
}

GFXCORE2_API DELPHI_TRGBA  __stdcall tmcsGetObjectColorKey(DELPHI_WORD num)
{
    PureObject3D* const obj = (PureObject3D*) objmgr->getAttachedAt(num);
    DELPHI_TRGBA clrRet;
    if ( obj )
    {
        clrRet.red = obj->getMaterial(false).getTextureEnvColor().getRed();
        clrRet.green = obj->getMaterial(false).getTextureEnvColor().getGreen();
        clrRet.blue = obj->getMaterial(false).getTextureEnvColor().getBlue();
        clrRet.alpha = obj->getMaterial(false).getTextureEnvColor().getAlpha();
    }
    else
    {
        clrRet.red = clrRet.green = clrRet.blue = clrRet.alpha = 0;
    }

    return clrRet;
}

GFXCORE2_API void __stdcall tmcsSetObjectColorKey(DELPHI_WORD num, DELPHI_BYTE r, DELPHI_BYTE g, DELPHI_BYTE b, DELPHI_BYTE a)
{
    PureObject3D* const obj = (PureObject3D*) objmgr->getAttachedAt(num);

    if ( obj )
        obj->getMaterial(false).getTextureEnvColor().Set(r, g, b, a);
}

GFXCORE2_API void __stdcall tmcsSetObjectAlpha(DELPHI_WORD num, DELPHI_BYTE a)
{
    PureObject3D* const obj = (PureObject3D*) objmgr->getAttachedAt(num);
    if ( !obj )
        return;

    for (TPureUInt i = 0; i < obj->getCount(); i++)
    {
        PureObject3D* const subobj = (PureObject3D*) obj->getAttachedAt(i);
        if ( subobj )
        {
            PureMaterial& const mat = subobj->getMaterial(false);
            for (TPureUInt j = 0; j < mat.getColorsCount(); j++)
                mat.getColors()[j].alpha = a;
        }
    }
}

GFXCORE2_API void __stdcall tmcsSetObjectBlendMode(DELPHI_WORD num, DELPHI_TGLCONST sfactor, DELPHI_TGLCONST dfactor)
{
    PureObject3D* const obj = (PureObject3D*) objmgr->getAttachedAt(num);
    if ( obj )
    {
        // setBlendFuncs() is not recommended to be called from this lib because it doesn't invoke setDestinationBlendFunc()
        // if setSourceBlendFunc() is failed prior to it. And PR00FPS actually requests invalid source blend func sometimes ...
        // So we need to work around that by calling the 2 functions here separately.
        obj->getMaterial(false).setSourceBlendFunc(getPureBlendFromGLBlend((GLenum) sfactor));
        obj->getMaterial(false).setDestinationBlendFunc(getPureBlendFromGLBlend((GLenum) dfactor));
    }
}

GFXCORE2_API void __stdcall tmcsSetObjectBlending(DELPHI_WORD num, DELPHI_BOOLEAN state)
{
    // Not much to do here because in new engine boolean blending state is not explicitly stored,
    // enabling blending happens automatically based on object's source and destination blending factors.
    // However, I noticed that legacy projects may just set blending state to FALSE to turn off blending,
    // so here if state is FALSE then I set non-blending blendmodes.
    PureObject3D* const obj = (PureObject3D*) objmgr->getAttachedAt(num);
    if ( obj && !state )
        obj->getMaterial(false).setBlendFuncs(PURE_ONE, PURE_ZERO);

    return;
}


GFXCORE2_API DELPHI_SINGLE __stdcall tmcsGetSubXPos(DELPHI_WORD num1, DELPHI_WORD num2)
{
    const PureObject3D* const obj = (PureObject3D*) objmgr->getAttachedAt(num1);
    if ( obj )
    {
        const PureObject3D* const subobj = (PureObject3D*) obj->getAttachedAt(num2);
        return subobj ? subobj->getPosVec().getX() : 0.f;
    }
    return 0.f;
}

GFXCORE2_API DELPHI_SINGLE __stdcall tmcsGetSubYPos(DELPHI_WORD num1, DELPHI_WORD num2)
{
    const PureObject3D* const obj = (PureObject3D*) objmgr->getAttachedAt(num1);
    if ( obj )
    {
        const PureObject3D* const subobj = (PureObject3D*) obj->getAttachedAt(num2);
        return subobj ? subobj->getPosVec().getY() : 0.f;
    }
    return 0.f;
}

GFXCORE2_API DELPHI_SINGLE __stdcall tmcsGetSubZPos(DELPHI_WORD num1, DELPHI_WORD num2)
{
    const PureObject3D* const obj = (PureObject3D*) objmgr->getAttachedAt(num1);
    if ( obj )
    {
        const PureObject3D* const subobj = (PureObject3D*) obj->getAttachedAt(num2);
        return subobj ? subobj->getPosVec().getZ() : 0.f;
    }
    return 0.f;
}

GFXCORE2_API DELPHI_BOOLEAN __stdcall tmcsSubIsVisible(DELPHI_WORD num1, DELPHI_WORD num2)
{
    const PureObject3D* const obj = (PureObject3D*) objmgr->getAttachedAt(num1);
    if ( obj )
    {
        const PureObject3D* const subobj = (PureObject3D*) obj->getAttachedAt(num2);
        return subobj ? subobj->isRenderingAllowed() : false;
    }
    return false;
}

GFXCORE2_API DELPHI_SINGLE __stdcall tmcsGetSubSizeX(DELPHI_WORD num1, DELPHI_WORD num2)
{
    const PureObject3D* const obj = (PureObject3D*) objmgr->getAttachedAt(num1);
    if ( obj )
    {
        const PureObject3D* const subobj = (PureObject3D*) obj->getAttachedAt(num2);
        return subobj ? subobj->getSizeVec().getX() : 0.f;
    }
    return 0.f;
}

GFXCORE2_API DELPHI_SINGLE __stdcall tmcsGetSubSizeY(DELPHI_WORD num1, DELPHI_WORD num2)
{
    const PureObject3D* const obj = (PureObject3D*) objmgr->getAttachedAt(num1);
    if ( obj )
    {
        const PureObject3D* const subobj = (PureObject3D*) obj->getAttachedAt(num2);
        return subobj ? subobj->getSizeVec().getY() : 0.f;
    }
    return 0.f;
}

GFXCORE2_API DELPHI_SINGLE __stdcall tmcsGetSubSizeZ(DELPHI_WORD num1, DELPHI_WORD num2)
{
    const PureObject3D* const obj = (PureObject3D*) objmgr->getAttachedAt(num1);
    if ( obj )
    {
        const PureObject3D* const subobj = (PureObject3D*) obj->getAttachedAt(num2);
        return subobj ? subobj->getSizeVec().getZ() : 0.f;
    }
    return 0.f;
}

GFXCORE2_API DELPHI_TSTR40_RET __stdcall tmcsGetSubName(DELPHI_WORD num1, DELPHI_WORD num2)
{
    DELPHI_TSTR40_RET retVal = {};
    const PureObject3D* const obj = (PureObject3D*) objmgr->getAttachedAt(num1);
    if ( obj )
    {
        const PureObject3D* const subobj = (PureObject3D*) obj->getAttachedAt(num2);
        if ( subobj )
            StrConvStrToDelphiStr(subobj->getName().c_str(), retVal);
    }
    return retVal;
}

GFXCORE2_API void __stdcall tmcsSetSubXPos(DELPHI_WORD num1, DELPHI_WORD num2, DELPHI_SINGLE factor)
{
    const PureObject3D* const obj = (PureObject3D*) objmgr->getAttachedAt(num1);
    if ( obj )
    {
        PureObject3D* const subobj = (PureObject3D*) obj->getAttachedAt(num2);
        if ( subobj )
            subobj->getPosVec().SetX(factor);
    }
}

GFXCORE2_API void __stdcall tmcsSetSubYPos(DELPHI_WORD num1, DELPHI_WORD num2, DELPHI_SINGLE factor)
{
    const PureObject3D* const obj = (PureObject3D*) objmgr->getAttachedAt(num1);
    if ( obj )
    {
        PureObject3D* const subobj = (PureObject3D*) obj->getAttachedAt(num2);
        if ( subobj )
            subobj->getPosVec().SetY(factor);
    }
}

GFXCORE2_API void __stdcall tmcsSetSubZPos(DELPHI_WORD num1, DELPHI_WORD num2, DELPHI_SINGLE factor)
{
    const PureObject3D* const obj = (PureObject3D*) objmgr->getAttachedAt(num1);
    if ( obj )
    {
        PureObject3D* const subobj = (PureObject3D*) obj->getAttachedAt(num2);
        if ( subobj )
            subobj->getPosVec().SetZ(factor);
    }
}

GFXCORE2_API void __stdcall tmcsShowSubObject(DELPHI_WORD num1, DELPHI_WORD num2)
{
    const PureObject3D* const obj = (PureObject3D*) objmgr->getAttachedAt(num1);
    if ( obj )
    {
        PureObject3D* const subobj = (PureObject3D*) obj->getAttachedAt(num2);
        if ( subobj )
            subobj->Show();
    }
}

GFXCORE2_API void __stdcall tmcsHideSubObject(DELPHI_WORD num1, DELPHI_WORD num2)
{
    const PureObject3D* const obj = (PureObject3D*) objmgr->getAttachedAt(num1);
    if ( obj )
    {
        PureObject3D* const subobj = (PureObject3D*) obj->getAttachedAt(num2);
        if ( subobj )
            subobj->Hide();
    }
}

GFXCORE2_API void __stdcall tmcsSetSubName(DELPHI_WORD num1, DELPHI_WORD num2, DELPHI_TSTR40 name)
{
    const PureObject3D* const obj = (PureObject3D*) objmgr->getAttachedAt(num1);
    if ( obj )
    {
        DELPHI_TSTR40 strTmpBuffer;
        PureObject3D* const subobj = (PureObject3D*) obj->getAttachedAt(num2);
        if ( subobj )
        {
            StrConvDelphiStrToCStr(name, strTmpBuffer);
            subobj->SetName((char*)strTmpBuffer);
        }
    }
}


GFXCORE2_API void __stdcall tmcsEnableLights()
{
    // unimplemented in PURE
}

GFXCORE2_API void __stdcall tmcsDisableLights()
{
    // unimplemented in PURE
}

GFXCORE2_API void __stdcall tmcsEnableAmbientLight()
{
    // unimplemented in PURE
}

GFXCORE2_API void __stdcall tmcsDisableAmbientLight()
{
    // unimplemented in PURE
}

GFXCORE2_API void __stdcall tmcsSetAmbientLight(DELPHI_SINGLE r, DELPHI_SINGLE g, DELPHI_SINGLE b)
{
    // unimplemented in PURE
}


GFXCORE2_API DELPHI_INTEGER __stdcall tmcsCreateTextureFromFile(DELPHI_TFILENAME filename, DELPHI_BOOLEAN mipmapped, DELPHI_BOOLEAN border, DELPHI_BOOLEAN compressed, DELPHI_TGLCONST filtering, DELPHI_TGLCONST envmode, DELPHI_TGLCONST wrap_s, DELPHI_TGLCONST wrap_t)
{
    // filtering param is the min filtering; mag filtering was always LINEAR in the old engine
    // we can ignore the mipmapped param because Pure figures out it based on the given filtering param anyways
    // TODO: envmode is material-related in Pure


    // save current texmanager settings before setting external object texture settings
    const TPURE_ISO_TEX_FILTERING fTexIsoFilteringMin = texmgr->getDefaultMinFilteringMode();
    const TPURE_ISO_TEX_FILTERING fTexIsoFilteringMag = texmgr->getDefaultMagFilteringMode();
    const TPURE_TEX_COMPRESSION_MODE compTex = texmgr->getDefaultCompressionMode();
    const TPureBool bBorder = texmgr->getDefaultBorder();
    const TPURE_TEX_WRAPPING twS = texmgr->getDefaultTextureWrappingModeS();
    const TPURE_TEX_WRAPPING twT = texmgr->getDefaultTextureWrappingModeT();

    const TPURE_ISO_TEX_FILTERING finalMagFiltering = PURE_ISO_LINEAR;
          TPURE_ISO_TEX_FILTERING finalMinFiltering;
    
    switch (filtering)
    {
    case GL_NEAREST:
        finalMinFiltering = PURE_ISO_NEAREST;  break;
    case GL_LINEAR :
        finalMinFiltering = PURE_ISO_LINEAR;  break;
    case GL_LINEAR_MIPMAP_NEAREST:
        finalMinFiltering = PURE_ISO_LINEAR_MIPMAP_NEAREST;  break;
    case GL_LINEAR_MIPMAP_LINEAR:
        finalMinFiltering = PURE_ISO_LINEAR_MIPMAP_LINEAR;  break;
    case GL_NEAREST_MIPMAP_NEAREST:
        finalMinFiltering = PURE_ISO_NEAREST_MIPMAP_NEAREST;  break;
    case GL_NEAREST_MIPMAP_LINEAR:
        finalMinFiltering = PURE_ISO_NEAREST_MIPMAP_LINEAR;  break;
    default:
        finalMinFiltering = PURE_ISO_LINEAR;
        CConsole::getConsoleInstance().EOLn("tmcsCreateTextureFromFile() filtering branched to default!");
    }

    TPURE_TEX_WRAPPING finalTW_S, finalTW_T;

    switch (wrap_s)
    {
    case GL_CLAMP:
        finalTW_S = PURE_TW_CLAMP;  break;
    case GL_REPEAT:
        finalTW_S = PURE_TW_REPEAT;  break;
    case GL_MIRRORED_REPEAT:
        finalTW_S = PURE_TW_MIRRORED_REPEAT;  break;
    case GL_CLAMP_TO_BORDER:
        finalTW_S = PURE_TW_CLAMP_TO_BORDER;  break;
    case GL_CLAMP_TO_EDGE:
        finalTW_S = PURE_TW_CLAMP_TO_EDGE;  break;
    default:
        finalTW_S = PURE_TW_REPEAT;
        CConsole::getConsoleInstance().EOLn("tmcsCreateTextureFromFile() wrapping S branched to default!");
    }

    switch (wrap_t)
    {
    case GL_CLAMP:
        finalTW_T = PURE_TW_CLAMP;  break;
    case GL_REPEAT:
        finalTW_T = PURE_TW_REPEAT;  break;
    case GL_MIRRORED_REPEAT:
        finalTW_T = PURE_TW_MIRRORED_REPEAT;  break;
    case GL_CLAMP_TO_BORDER:
        finalTW_T = PURE_TW_CLAMP_TO_BORDER;  break;
    case GL_CLAMP_TO_EDGE:
        finalTW_T = PURE_TW_CLAMP_TO_EDGE;  break;
    default:
        finalTW_T = PURE_TW_REPEAT;
        CConsole::getConsoleInstance().EOLn("tmcsCreateTextureFromFile() wrapping T branched to default!");
    }
    
    texmgr->setDefaultIsoFilteringMode(finalMinFiltering, finalMagFiltering);
    texmgr->setDefaultCompressionMode(compressed ? PURE_TC_AUTO : PURE_TC_NONE);
    texmgr->setDefaultTextureWrappingMode(finalTW_S, finalTW_T);
    texmgr->setDefaultBorder(border);

    DELPHI_TSTR255 strTmpBuffer;
    StrConvDelphiStrToCStr(filename, strTmpBuffer);

    const PureTexture* const tex = texmgr->createFromFile((char*) strTmpBuffer);

    // load original texmanager settings
    texmgr->setDefaultMinFilteringMode( fTexIsoFilteringMin );
    texmgr->setDefaultMagFilteringMode( fTexIsoFilteringMag );
    texmgr->setDefaultCompressionMode( compTex );
    texmgr->setDefaultBorder( bBorder );
    texmgr->setDefaultTextureWrappingMode( twS, twT );

    return tex ? texmgr->getAttachedIndex(*tex) : -1;
}

GFXCORE2_API void  __stdcall tmcsFrameBufferToTexture(DELPHI_INTEGER texnum)
{
    PureTexture* const tex = (PureTexture*) texmgr->getAttachedAt(texnum);

    if ( !tex )
        return;

    pure->CopyScreenToTexture(*tex);
}

GFXCORE2_API DELPHI_INTEGER __stdcall tmcsCreateBlankTexture(DELPHI_INTEGER width, DELPHI_INTEGER height, DELPHI_TGLCONST filtering, DELPHI_TGLCONST envmode, DELPHI_TGLCONST wrap_s, DELPHI_TGLCONST wrap_t)
{
    // filtering param is the min filtering; mag filtering was always LINEAR in the old engine
    // we can ignore the mipmapped param because Pure figures out it based on the given filtering param anyways
    // TODO: envmode is material-related in Pure


    // save current texmanager settings before setting external object texture settings
    const TPURE_ISO_TEX_FILTERING fTexIsoFilteringMin = texmgr->getDefaultMinFilteringMode();
    const TPURE_ISO_TEX_FILTERING fTexIsoFilteringMag = texmgr->getDefaultMagFilteringMode();
    const TPURE_TEX_WRAPPING twS = texmgr->getDefaultTextureWrappingModeS();
    const TPURE_TEX_WRAPPING twT = texmgr->getDefaultTextureWrappingModeT();

    const TPURE_ISO_TEX_FILTERING finalMagFiltering = PURE_ISO_LINEAR;
          TPURE_ISO_TEX_FILTERING finalMinFiltering;
    
    switch (filtering)
    {
    case GL_NEAREST:
        finalMinFiltering = PURE_ISO_NEAREST;  break;
    case GL_LINEAR :
        finalMinFiltering = PURE_ISO_LINEAR;  break;
    case GL_LINEAR_MIPMAP_NEAREST:
        finalMinFiltering = PURE_ISO_LINEAR_MIPMAP_NEAREST;  break;
    case GL_LINEAR_MIPMAP_LINEAR:
        finalMinFiltering = PURE_ISO_LINEAR_MIPMAP_LINEAR;  break;
    case GL_NEAREST_MIPMAP_NEAREST:
        finalMinFiltering = PURE_ISO_NEAREST_MIPMAP_NEAREST;  break;
    case GL_NEAREST_MIPMAP_LINEAR:
        finalMinFiltering = PURE_ISO_NEAREST_MIPMAP_LINEAR;  break;
    default:
        finalMinFiltering = PURE_ISO_LINEAR;
        CConsole::getConsoleInstance().EOLn("tmcsCreateBlankTexture() filtering branched to default!");
    }

    TPURE_TEX_WRAPPING finalTW_S, finalTW_T;

    switch (wrap_s)
    {
    case GL_CLAMP:
        finalTW_S = PURE_TW_CLAMP;  break;
    case GL_REPEAT:
        finalTW_S = PURE_TW_REPEAT;  break;
    case GL_MIRRORED_REPEAT:
        finalTW_S = PURE_TW_MIRRORED_REPEAT;  break;
    case GL_CLAMP_TO_BORDER:
        finalTW_S = PURE_TW_CLAMP_TO_BORDER;  break;
    case GL_CLAMP_TO_EDGE:
        finalTW_S = PURE_TW_CLAMP_TO_EDGE;  break;
    default:
        finalTW_S = PURE_TW_REPEAT;
        CConsole::getConsoleInstance().EOLn("tmcsCreateBlankTexture() wrapping S branched to default!");
    }

    switch (wrap_t)
    {
    case GL_CLAMP:
        finalTW_T = PURE_TW_CLAMP;  break;
    case GL_REPEAT:
        finalTW_T = PURE_TW_REPEAT;  break;
    case GL_MIRRORED_REPEAT:
        finalTW_T = PURE_TW_MIRRORED_REPEAT;  break;
    case GL_CLAMP_TO_BORDER:
        finalTW_T = PURE_TW_CLAMP_TO_BORDER;  break;
    case GL_CLAMP_TO_EDGE:
        finalTW_T = PURE_TW_CLAMP_TO_EDGE;  break;
    default:
        finalTW_T = PURE_TW_REPEAT;
        CConsole::getConsoleInstance().EOLn("tmcsCreateBlankTexture() wrapping T branched to default!");
    }

    PureImage* const img = imgmgr->createBlank(width, height, 24);
    if ( img == PGENULL )
    {
        CConsole::getConsoleInstance().EOLn("tmcsCreateBlankTexture() failed on imgmgr->createBlank(%d, %d, %d) !", width, height, 24);
        return -1;
    }
    
    texmgr->setDefaultIsoFilteringMode(finalMinFiltering, finalMagFiltering);
    texmgr->setDefaultTextureWrappingMode(finalTW_S, finalTW_T);

    PureTexture* const tex = texmgr->createTextureFromImage(*img);

    // we need to save this texture so tmcsAdjustPlaneCoordsToViewport() will work properly ... this is due to legacy issue described in tmcsAdjustPlaneCoordsToViewport().
    texLastCreateBlank = tex;

    // load original texmanager settings
    texmgr->setDefaultMinFilteringMode( fTexIsoFilteringMin );
    texmgr->setDefaultMagFilteringMode( fTexIsoFilteringMag );
    texmgr->setDefaultTextureWrappingMode( twS, twT );

    return tex ? texmgr->getAttachedIndex(*tex) : -1;
}

GFXCORE2_API void  __stdcall tmcsDeleteTexture(DELPHI_WORD num)
{
    // due to legacy project bug, we cannot let application delete the first texture, see details in tmcsDeleteObject()
    // update: bug has been fixed on application side, see updates in tmcsDeleteObject()
    /*if ( num == 0 )
        return;*/

    PureTexture* const tex = (PureTexture*) texmgr->getAttachedAt(num);
    if ( tex ) {
        texmgr->DeleteAttachedInstance(*tex);
    }
}

GFXCORE2_API void  __stdcall tmcsDeleteTextures()
{
    texmgr->DeleteAll();
}

GFXCORE2_API void  __stdcall tmcsTextureObject(DELPHI_WORD num, DELPHI_WORD num2)
{
    // num = object index
    // num2 = texture index

    PureObject3D* const obj = (PureObject3D*) objmgr->getAttachedAt(num);
    if ( obj )
    {
        PureTexture* const tex = (PureTexture*) texmgr->getAttachedAt(num2);
        if ( tex )
        {
            obj->getMaterial().setTexture(tex);
        }
    }
}

GFXCORE2_API DELPHI_INTEGER __stdcall tmcsGetTextureWidth(DELPHI_WORD num)
{
    const PureTexture* const tex = (PureTexture*) texmgr->getAttachedAt(num);
    if ( tex )
        return tex->getWidth();
    else
        return 0;
}

GFXCORE2_API DELPHI_INTEGER __stdcall tmcsGetTextureHeight(DELPHI_WORD num)
{
    const PureTexture* const tex = (PureTexture*) texmgr->getAttachedAt(num);
    if ( tex )
        return tex->getHeight();
    else
        return 0;
}

GFXCORE2_API DELPHI_CARDINAL __stdcall tmcsGetTextureInternalNum(DELPHI_WORD num)
{
    const PureTexture* const tex = (PureTexture*) texmgr->getAttachedAt(num);
    if ( tex )
        return tex->getInternalNum();
    else
        return 0;
}

GFXCORE2_API DELPHI_INTEGER __stdcall tmcsGetObjectTexture(DELPHI_WORD num)
{
    const PureObject3D* const obj = (PureObject3D*) objmgr->getAttachedAt(num);
    if ( obj )
    {
        const PureTexture* const tex = obj->getMaterial().getTexture();
        if ( tex )
        {
            return texmgr->getAttachedIndex(*tex);
        }
    }
    return -1;
}

GFXCORE2_API DELPHI_INTEGER __stdcall tmcsGetSubObjectTexture(DELPHI_WORD num1, DELPHI_WORD num2)
{
    const PureObject3D* const obj = (PureObject3D*) objmgr->getAttachedAt(num1);
    if ( obj )
    {
        const PureObject3D* const subobj = (PureObject3D*) obj->getAttachedAt(num2);
        if ( subobj )
        {
            const PureTexture* const tex = subobj->getMaterial().getTexture();
            if ( tex )
            {
                return texmgr->getAttachedIndex(*tex);
            }
        }
    }
    return -1;
}

GFXCORE2_API void  __stdcall tmcsMultiplyUVCoords(DELPHI_WORD num, DELPHI_SINGLE factorw, DELPHI_SINGLE factorh)
{
    PureObject3D* const obj = (PureObject3D*) objmgr->getAttachedAt(num);
    if ( !obj )
        return;

    for (TPureUInt i = 0; i < obj->getCount(); i++)
    {
        PureObject3D* const subobj = (PureObject3D*) obj->getAttachedAt(i);
        if ( subobj )
        {
            PureMaterial& const mat = subobj->getMaterial(false);
            for (TPureUInt j = 0; j < mat.getTexcoordsCount(); j++)
            {
                mat.getTexcoords()[j].u *= factorw;
                mat.getTexcoords()[j].v *= factorh;
            }
        }
    }
}

GFXCORE2_API void  __stdcall tmcsAdjustUVCoords(DELPHI_WORD num, DELPHI_SINGLE factor)
{
    PureObject3D* const obj = (PureObject3D*) objmgr->getAttachedAt(num);
    if ( !obj )
        return;

    for (TPureUInt i = 0; i < obj->getCount(); i++)
    {
        PureObject3D* const subobj = (PureObject3D*) obj->getAttachedAt(i);
        if ( subobj )
        {
            PureMaterial& const mat = subobj->getMaterial(false);
            for (TPureUInt j = 0; j < mat.getTexcoordsCount(); j++)
            {
                if ( mat.getTexcoords()[j].u == 0.0f )
                    mat.getTexcoords()[j].u += factor;
                else if ( mat.getTexcoords()[j].u == 1.0f )
                    mat.getTexcoords()[j].u -= factor;

                if ( mat.getTexcoords()[j].v == 0.0f )
                    mat.getTexcoords()[j].v += factor;
                else if ( mat.getTexcoords()[j].v == 1.0f )
                    mat.getTexcoords()[j].v -= factor;
            }
        }
    }
}

GFXCORE2_API void  __stdcall tmcsAdjustPlaneCoordsToViewport(DELPHI_WORD num, DELPHI_WORD num2)
{
    // Note: this is funny: although even legacy tmcsgfxlib defines prototype of this function like this,
    // but the function definition doesn't have the 2nd parameter at all! So the signature of the
    // function definition and declaration is different! The 2nd param is not used at all.
    // Instead of that, an internal texture (texLastCreateBlank) is referenced here.

    PureObject3D* obj = (PureObject3D*) objmgr->getAttachedAt(num);

    if ( !obj )
        return;

    if ( obj->getCount() != 1 )
        return;

    // Since we have only 1 subobject, obj->getVertices() will return the vertices of that subobject by design.
    if ( obj->getVerticesCount() != 4 )
        return;

    // And obj->getMaterial() will return the material of that only one subobject.
    obj->getMaterial().getTexcoords()[0].u = 0.0f;                                     /* U = 0.0f */
    obj->getMaterial().getTexcoords()[0].v = 0.0f;                                     /* V = 0.0f */
    obj->getVertices()[0].x = -pure->getCamera().getViewport().size.width / 2.0f;      /* X = -1024.0f / 2.0f = -512.0f */
    obj->getVertices()[0].y = -pure->getCamera().getViewport().size.height / 2.0f;     /* Y = - 768.0f / 2.0f = -384.0f */

    // Another funny glitch: legacy tmcsgfxlib references texture^.width but doesn't even set
    // texture pointer in this function at all! That pointer has its value previously set by
    // tmcsCreateBlankTexture(). So this was pretty dangerous API ... 
    // So we need to prepare our tmcsCreateBlankTexture() implementation to properly set texLastCreateBlank for us.
    // Remember, originally the aim of this legacy function must had been to properly set vertex- and texture coordinates
    // of object referenced by num BASED ON THE ATTRIBUTES of texture referenced by num2 !

    obj->getMaterial().getTexcoords()[1].u = pure->getCamera().getViewport().size.width / (TPureFloat) texLastCreateBlank->getWidth();  /* U = 1024.0f / 1024 = 1.0f */
    obj->getMaterial().getTexcoords()[1].v = 0.0f;                                                                                      /* V =                  0.0f */
    obj->getVertices()[1].x =  pure->getCamera().getViewport().size.width / 2.0f;                                                       /* X = 1024.0f / 2.0f =  512.0f */
    obj->getVertices()[1].y = -pure->getCamera().getViewport().size.height / 2.0f;                                                      /* Y = -768.0f / 2.0f = -384.0f */

    obj->getMaterial().getTexcoords()[2].u = pure->getCamera().getViewport().size.width / (TPureFloat) texLastCreateBlank->getWidth();   /* U = 1024.0f / 1024 = 1.0f */
    obj->getMaterial().getTexcoords()[2].v = pure->getCamera().getViewport().size.height / (TPureFloat) texLastCreateBlank->getWidth();  /* V =  768.0f / 1024 = 0.75f */
    obj->getVertices()[2].x = pure->getCamera().getViewport().size.width / 2.0f;                                                         /* X = 1024.0f / 2.0f = 512.0f */
    obj->getVertices()[2].y = pure->getCamera().getViewport().size.height / 2.0f;                                                        /* Y =  768.0f / 2.0f = 384.0f */

    obj->getMaterial().getTexcoords()[3].u = 0.0f;                                                                                       /* U =                  0.0f */
    obj->getMaterial().getTexcoords()[3].v = pure->getCamera().getViewport().size.height / (TPureFloat) texLastCreateBlank->getWidth();  /* V =  768.0f / 1024 = 0.75f */
    obj->getVertices()[3].x = -pure->getCamera().getViewport().size.width / 2.0f;                                                        /* X = -1024.0f / 2.0f = -512.0f */
    obj->getVertices()[3].y =  pure->getCamera().getViewport().size.height / 2.0f;                                                       /* Y =   768.0f / 2.0f =  384.0f */
}


GFXCORE2_API DELPHI_INTEGER __stdcall tmcsGetTotalVertices(DELPHI_WORD num)
{
    // TODO: unimplemented in PURE
    return 0;
}

GFXCORE2_API DELPHI_SINGLE __stdcall tmcsGetVertexX(DELPHI_WORD num, DELPHI_WORD num2)
{
    // TODO: unimplemented in PURE
    return 0.f;
}

GFXCORE2_API DELPHI_SINGLE __stdcall tmcsGetVertexY(DELPHI_WORD num, DELPHI_WORD num2)
{
    // TODO: unimplemented in PURE
    return 0.f;
}

GFXCORE2_API DELPHI_SINGLE __stdcall tmcsGetVertexZ(DELPHI_WORD num, DELPHI_WORD num2)
{
    // TODO: unimplemented in PURE
    return 0.f;
}

GFXCORE2_API void __stdcall tmcsSetVertex(DELPHI_WORD num, DELPHI_WORD num2, DELPHI_SINGLE x, DELPHI_SINGLE y, DELPHI_SINGLE z)
{
    // TODO: unimplemented in PURE
}

GFXCORE2_API DELPHI_SINGLE __stdcall tmcsGetNormalX(DELPHI_WORD num, DELPHI_WORD num2)
{
    // TODO: unimplemented in PURE
    return 0.f;
}

GFXCORE2_API DELPHI_SINGLE __stdcall tmcsGetNormalY(DELPHI_WORD num, DELPHI_WORD num2)
{
    // TODO: unimplemented in PURE
    return 0.f;
}

GFXCORE2_API DELPHI_SINGLE __stdcall tmcsGetNormalZ(DELPHI_WORD num, DELPHI_WORD num2)
{
    // TODO: unimplemented in PURE
    return 0.f;
}

GFXCORE2_API void __stdcall tmcsSetNormal(DELPHI_WORD num, DELPHI_WORD num2, DELPHI_SINGLE nx, DELPHI_SINGLE ny, DELPHI_SINGLE nz)
{
    // TODO: unimplemented in PURE
}


GFXCORE2_API void __stdcall tmcsLoadFontInfo(DELPHI_TSTR255 path, DELPHI_TSTR255 name)
{
    // TODO: unimplemented in PURE
}

GFXCORE2_API void __stdcall tmcsText(DELPHI_TSTR255 text, DELPHI_WORD x, DELPHI_WORD y, DELPHI_WORD fontheight, DELPHI_WORD scaling)
{
    // as legacy proofps project may continuously call this function for items' text even when alpha is 0, 
    // check against current text alpha value and return immediately to avoid unnecessary resource eatup.
    if ( pure->getUImanager().getDefaultColorLegacy().getAlphaAsFloat() < 0.1f )
        return;

    DELPHI_TSTR255 strTmpBuffer;
    StrConvDelphiStrToCStr(text, strTmpBuffer);
    std::string textStr = (char*) strTmpBuffer;

    pure->getUImanager().textTemporalLegacy(textStr, x, y, pure->getUImanager().getDefaultFontFaceLegacy(), (int) (fontheight*(scaling/60.0f)), false, false, false, false);
}

GFXCORE2_API void __stdcall tmcsSetTextColor(DELPHI_BYTE r, DELPHI_BYTE g, DELPHI_BYTE b, DELPHI_BYTE a)
{
    pure->getUImanager().getDefaultColorLegacy().Set(r, g, b, a);
}

GFXCORE2_API void __stdcall tmcsSetTextBlendingState(DELPHI_BOOLEAN state)
{
    // unimplemented in PURE: new engine doesn't yet support text blending; we will hide text having alpha < 0.1f by enabling alpha-testing
}

GFXCORE2_API void __stdcall tmcsSetTextBlendMode(DELPHI_TGLCONST sfactor, DELPHI_TGLCONST dfactor)
{
    // unimplemented in PURE: new engine doesn't yet support text blending; we will hide text having alpha < 0.1f by enabling alpha-testing
}

GFXCORE2_API DELPHI_INTEGER __stdcall tmcsGetTextWidth(DELPHI_TSTR255 text, DELPHI_WORD fontheight, DELPHI_WORD scaling)
{
    // TODO: proofps continuously invoking this even when no text is displayed ... find way to speed this up
    DELPHI_TSTR255 strTmpBuffer;
    StrConvDelphiStrToCStr(text, strTmpBuffer);
    std::string textStr = (char*) strTmpBuffer;

    // create a temporary text just to retrieve the needed width ... no need to delete, uiManager will delete anyway after next frame;
    // dummy positions are given to make sure it is not visible within viewport
    PureUiText* tmpText = pure->getUImanager().textTemporalLegacy(textStr, 0, -500, pure->getUImanager().getDefaultFontFaceLegacy(), (int) (fontheight*(scaling/60.0f)), false, false, false, false);

    if ( tmpText )
        return (DELPHI_INTEGER) tmpText->getWidth() - tmpText->getText().length()*2;  /* small correction for legacy projects which also contain some negative correction */
    else
        return 0;
}

GFXCORE2_API void __stdcall tmcsSetRenderPath(DELPHI_WORD renderPath)
{
    if ( bDebugEnabled )
    {
        CConsole::getConsoleInstance().SetLoggingState("4LLM0DUL3S", true);
    }

    std::string sRenderPath;
    switch (renderPath)
    {
    case 0: sRenderPath = "Legacy PR00FPS";
        break;
    case 1: sRenderPath = "Distance Ordered";
        break;
    default: sRenderPath = "Occlusion Culled";
    }
    CConsole::getConsoleInstance().OLn("%s: %u: %s", __FUNCTION__, renderPath, sRenderPath.c_str());

    TPURE_RENDER_HINT renderHints = pure->getRenderer()->getRenderHints();
    BITF_SET(renderHints, renderPath, PURE_RH_RENDER_PATH_BITS, 3);
    CConsole::getConsoleInstance().OI();
    pure->getRenderer()->SetRenderHints(renderHints);
    CConsole::getConsoleInstance().OO();

    if ( bDebugEnabled )
    {
        CConsole::getConsoleInstance().SetLoggingState("4LLM0DUL3S", false);
    }
}

GFXCORE2_API void __stdcall tmcsSetOcclusionCullingMethod(DELPHI_WORD ocMethod)
{
    if ( bDebugEnabled )
    {
        CConsole::getConsoleInstance().SetLoggingState("4LLM0DUL3S", true);
    }

    std::string sOcMethod;
    switch (ocMethod)
    {
    case 0: sOcMethod = "Sync";
        break;
    default: sOcMethod = "ASync";
    }
    CConsole::getConsoleInstance().OLn("%s: %u: %s", __FUNCTION__, ocMethod, sOcMethod.c_str());

    TPURE_RENDER_HINT renderHints = pure->getRenderer()->getRenderHints();
    BITF_SET(renderHints, ocMethod, PURE_RH_OQ_METHOD_BITS, 2);
    CConsole::getConsoleInstance().OI();
    pure->getRenderer()->SetRenderHints(renderHints);
    CConsole::getConsoleInstance().OO();

    if ( bDebugEnabled )
    {
        CConsole::getConsoleInstance().SetLoggingState("4LLM0DUL3S", false);
    }
}

GFXCORE2_API void __stdcall tmcsSetOcclusionCullingBoundingBoxes(DELPHI_BOOLEAN state)
{
    if ( bDebugEnabled )
    {
        CConsole::getConsoleInstance().SetLoggingState("4LLM0DUL3S", true);
    }

    CConsole::getConsoleInstance().OLn("%s: %b", __FUNCTION__, state);

    TPURE_RENDER_HINT renderHints = pure->getRenderer()->getRenderHints();
    BITF_SET(renderHints, state, PURE_RH_OQ_DRAW_BOUNDING_BOXES_BIT, 1);
    CConsole::getConsoleInstance().OI();
    pure->getRenderer()->SetRenderHints(renderHints);
    CConsole::getConsoleInstance().OO();

    if ( bDebugEnabled )
    {
        CConsole::getConsoleInstance().SetLoggingState("4LLM0DUL3S", false);
    }
}

GFXCORE2_API void __stdcall tmcsSetOcclusionCullingDrawIfPending(DELPHI_BOOLEAN state)
{
    if ( bDebugEnabled )
    {
        CConsole::getConsoleInstance().SetLoggingState("4LLM0DUL3S", true);
    }

    CConsole::getConsoleInstance().OLn("%s: %b", __FUNCTION__, state);

    TPURE_RENDER_HINT renderHints = pure->getRenderer()->getRenderHints();
    BITF_SET(renderHints, state, PURE_RH_OQ_DRAW_IF_QUERY_PENDING_BIT, 1);
    CConsole::getConsoleInstance().OI();
    pure->getRenderer()->SetRenderHints(renderHints);
    CConsole::getConsoleInstance().OO();

    if ( bDebugEnabled )
    {
        CConsole::getConsoleInstance().SetLoggingState("4LLM0DUL3S", false);
    }
}

GFXCORE2_API void __stdcall tmcsSetOrderingByDistance(DELPHI_BOOLEAN state)
{
    if ( bDebugEnabled )
    {
        CConsole::getConsoleInstance().SetLoggingState("4LLM0DUL3S", true);
    }

    CConsole::getConsoleInstance().OLn("%s: %b", __FUNCTION__, state);

    TPURE_RENDER_HINT renderHints = pure->getRenderer()->getRenderHints();
    BITF_SET(renderHints, state, PURE_RH_ORDERING_BY_DISTANCE_BIT, 1);
    CConsole::getConsoleInstance().OI();
    pure->getRenderer()->SetRenderHints(renderHints);
    CConsole::getConsoleInstance().OO();

    if ( bDebugEnabled )
    {
        CConsole::getConsoleInstance().SetLoggingState("4LLM0DUL3S", false);
    }
}

GFXCORE2_API void __stdcall tmcsResetStatistics()
{
    if ( bDebugEnabled )
    {
        CConsole::getConsoleInstance().SetLoggingState("4LLM0DUL3S", true);
    }

    CConsole::getConsoleInstance().OLn("%s", __FUNCTION__);
    CConsole::getConsoleInstance().OI();
    pure->getRenderer()->ResetStatistics();
    CConsole::getConsoleInstance().OO();

    if ( bDebugEnabled )
    {
        CConsole::getConsoleInstance().SetLoggingState("4LLM0DUL3S", false);
    }
}

GFXCORE2_API void __stdcall tmcsEngineDump()
{
    if ( bDebugEnabled )
    {
        CConsole::getConsoleInstance().SetLoggingState("4LLM0DUL3S", true);
    }

    pure->WriteList();

    if ( bDebugEnabled )
    {
        CConsole::getConsoleInstance().SetLoggingState("4LLM0DUL3S", false);
    }
}



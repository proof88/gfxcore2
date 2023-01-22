#pragma once

/*
    ###################################################################################
    gfxcore2.h
    Main header of gfxcore2.
    This DLL is to be used by the legacy PR00FPS made in 2007.
    This DLL is only a wrapper so the old PR00FPS can utilize my new engine: Pure.
    Aim is to avoid any modification to PR00FPS code, the change is totally transparent.
    Made by PR00F88
    EMAIL : PR0o0o0o0o0o0o0o0o0o0oF88@gmail.com
    ###################################################################################
*/

/**
    If PGE_CCONSOLE_IS_ENABLED macro is defined, there will be console window created and printouts will be visible, otherwise not.
    Note that CConsole library should be built console-enabled as well.
*/
#ifndef PGE_CCONSOLE_IS_ENABLED
#define PGE_CCONSOLE_IS_ENABLED
#endif

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the GFXCORE2_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// GFXCORE2_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef GFXCORE2_EXPORTS
#define GFXCORE2_API extern "C" __declspec(dllexport)
#else
#define GFXCORE2_API extern "C" __declspec(dllimport)
#endif


typedef bool               DELPHI_BOOLEAN;
typedef unsigned char      DELPHI_BYTE;
typedef unsigned short int DELPHI_WORD;
typedef int                DELPHI_INTEGER;
typedef unsigned int       DELPHI_CARDINAL;
typedef float              DELPHI_SINGLE;
typedef double             DELPHI_DOUBLE;
typedef DELPHI_WORD        DELPHI_TGLCONST;

typedef DELPHI_BYTE                    DELPHI_TSTR40[41];      /* Byte 0 holds the length of the string in Delphi */
typedef struct { DELPHI_TSTR40 str; }  DELPHI_TSTR40_RET;      /* Use this type for returning DELPHI_TSTR40! */
typedef DELPHI_BYTE                    DELPHI_TSTR255[256];
typedef struct { DELPHI_TSTR255 str; } DELPHI_TSTR255_RET;     /* Use this type for returning DELPHI_TSTR255! */
typedef DELPHI_TSTR255                 DELPHI_TFILENAME;

typedef struct
{
    DELPHI_SINGLE x, y, z;
} DELPHI_TXYZ;

typedef struct
{
    DELPHI_BYTE red, green, blue;
} DELPHI_TRGB;

typedef struct
{
    DELPHI_BYTE red, green, blue, alpha;
} DELPHI_TRGBA;



/*
    Original gfxcore.dll exported functions as defined in gfxCore.pas:

    { Inicializálás, renderelés, debuggolás, finalizálás }
    function tmcsInitialized(): boolean; stdcall; external 'gfxcore.dll';
    function tmcsInitGraphix(wnd: HWND; fs: boolean; freq: integer; cdepth,zdepth: integer; vsyncstate: boolean; shading: integer): byte; stdcall; external 'gfxcore.dll';
    procedure tmcsRender(); stdcall; external 'gfxcore.dll';
    procedure tmcsRestoreOriginalDisplayMode; stdcall; external 'gfxcore.dll';
    procedure tmcsRestoreDisplayMode; stdcall; external 'gfxcore.dll';
    function tmcsShutdownGraphix(): byte; stdcall; external 'gfxcore.dll';
    procedure tmcsEnableDebugging(); stdcall; external 'gfxcore.dll';
    procedure tmcsDisableDebugging(); stdcall; external 'gfxcore.dll';
    procedure tmcsSetGamma(r,g,b: integer); stdcall; external 'gfxcore.dll';
    function tmcsGetRenderedVertices(): integer; stdcall; external 'gfxcore.dll';
    function tmcsGetRenderedFaces(): integer; stdcall; external 'gfxcore.dll';
    function tmcsGetNumFuncCalls(): integer; stdcall; external 'gfxcore.dll';
*/

GFXCORE2_API DELPHI_BOOLEAN __stdcall tmcsInitialized();
GFXCORE2_API DELPHI_BYTE    __stdcall tmcsInitGraphix(HWND wnd, DELPHI_BOOLEAN fs, DELPHI_INTEGER freq, DELPHI_INTEGER cdepth, DELPHI_INTEGER zdepth, DELPHI_BOOLEAN vsyncstate, DELPHI_INTEGER shading);
GFXCORE2_API void           __stdcall tmcsRender();
GFXCORE2_API void           __stdcall tmcsRestoreOriginalDisplayMode();
GFXCORE2_API void           __stdcall tmcsRestoreDisplayMode();
GFXCORE2_API DELPHI_BYTE    __stdcall tmcsShutdownGraphix();
GFXCORE2_API void           __stdcall tmcsEnableDebugging();
GFXCORE2_API void           __stdcall tmcsDisableDebugging();
GFXCORE2_API void           __stdcall tmcsSetGamma(DELPHI_INTEGER r, DELPHI_INTEGER g, DELPHI_INTEGER b);



/*
    Original gfxcore.dll exported functions as defined in gfxCore.pas:

    { Motion Blur }
    procedure tmcsEnableMotionBlur(width, height: word); stdcall; external 'gfxcore.dll';
    procedure tmcsDisableMotionBlur(); stdcall; external 'gfxcore.dll';
    procedure tmcsFreeMotionBlurResources(); stdcall; external 'gfxcore.dll';
    procedure tmcsSetMotionBlurUpdateRate(rate: byte); stdcall; external 'gfxcore.dll';
    function tmcsGetMotionBlurColor(): TRGBA; stdcall; external 'gfxcore.dll';
    procedure tmcsSetMotionBlurColor(red, green, blue, alpha: byte); stdcall; external 'gfxcore.dll';
*/

GFXCORE2_API void         __stdcall tmcsEnableMotionBlur(DELPHI_WORD width, DELPHI_WORD height);
GFXCORE2_API void         __stdcall tmcsDisableMotionBlur();
GFXCORE2_API void         __stdcall tmcsFreeMotionBlurResources();
GFXCORE2_API void         __stdcall tmcsSetMotionBlurUpdateRate(DELPHI_BYTE rate);
GFXCORE2_API DELPHI_TRGBA __stdcall tmcsGetMotionBlurColor();
GFXCORE2_API void         __stdcall tmcsSetMotionBlurColor(DELPHI_BYTE red, DELPHI_BYTE green, DELPHI_BYTE blue, DELPHI_BYTE alpha);



/*
    Original gfxcore.dll exported functions as defined in gfxCore.pas:

    { Kamera }
    procedure tmcsSetCameraTarget(index: integer); stdcall; external 'gfxcore.dll';
    procedure tmcsSetBgColor(r,g,b,a: byte); stdcall; external 'gfxcore.dll';
    procedure tmcsInitCamera(x,y,z,ax,ay,az: single; fov,aspect,znear,zfar: double); stdcall; external 'gfxcore.dll';
    procedure tmcsSetviewport(x,y,w,h: integer); stdcall; external 'gfxcore.dll';
    function tmcsGetCameraX(): single; stdcall; external 'gfxcore.dll';
    function tmcsGetCameraY(): single; stdcall; external 'gfxcore.dll';
    function tmcsGetCameraZ(): single; stdcall; external 'gfxcore.dll';
    function tmcsGetCameraAngleX(): single; stdcall; external 'gfxcore.dll';
    function tmcsGetCameraAngleY(): single; stdcall; external 'gfxcore.dll';
    function tmcsGetCameraAngleZ(): single; stdcall; external 'gfxcore.dll';
    function tmcsGetCameraFov(): double; stdcall; external 'gfxcore.dll';
    function tmcsGetCameraAspect(): double; stdcall; external 'gfxcore.dll';
    function tmcsGetCameraNearPlane(): double; stdcall; external 'gfxcore.dll';
    function tmcsGetCameraFarPlane(): double; stdcall; external 'gfxcore.dll';
    function tmcsWrapAngle(f: single): single; stdcall; external 'gfxcore.dll';
    function tmcsGetNewX(x: single; angle: single; factor: single): single; stdcall; external 'gfxcore.dll';
    function tmcsGetNewZ(z: single; angle: single; factor: single): single; stdcall; external 'gfxcore.dll';
    procedure tmcsSetCameraX(posx: single); stdcall; external 'gfxcore.dll';
    procedure tmcsSetCameraY(posy: single); stdcall; external 'gfxcore.dll';
    procedure tmcsSetCameraZ(posz: single); stdcall; external 'gfxcore.dll';
    procedure tmcsSetCameraPos(posx,posy,posz: single); stdcall; external 'gfxcore.dll';
    procedure tmcsMoveCamera(factor: single); stdcall; external 'gfxcore.dll';
    procedure tmcsSetCameraAngleX(anglex: single); stdcall; external 'gfxcore.dll';
    procedure tmcsSetCameraAngleY(angley: single); stdcall; external 'gfxcore.dll';
    procedure tmcsSetCameraAngleZ(anglez: single); stdcall; external 'gfxcore.dll';
    procedure tmcsSetCameraAngle(anglex,angley,anglez: single); stdcall; external 'gfxcore.dll';
    procedure tmcsXRotateCamera(angle: single); stdcall; external 'gfxcore.dll';
    procedure tmcsYRotateCamera(angle: single); stdcall; external 'gfxcore.dll';
    procedure tmcsZRotateCamera(angle: single); stdcall; external 'gfxcore.dll';
    procedure tmcsSetCameraFov(fov: double); stdcall; external 'gfxcore.dll';
    procedure tmcsSetCameraAspect(aspect: double); stdcall; external 'gfxcore.dll';
    procedure tmcsSetCameraNearPlane(znear: double); stdcall; external 'gfxcore.dll';
    procedure tmcsSetCameraFarPlane(zfar: double); stdcall; external 'gfxcore.dll';
*/


GFXCORE2_API void          __stdcall tmcsSetBgColor(DELPHI_BYTE r, DELPHI_BYTE g, DELPHI_BYTE b, DELPHI_BYTE a);
GFXCORE2_API void          __stdcall tmcsInitCamera(DELPHI_SINGLE x, DELPHI_SINGLE y, DELPHI_SINGLE z, DELPHI_SINGLE ax, DELPHI_SINGLE ay, DELPHI_SINGLE az,
                                                    DELPHI_DOUBLE fov, DELPHI_DOUBLE aspect, DELPHI_DOUBLE znear, DELPHI_DOUBLE zfar);
GFXCORE2_API void          __stdcall tmcsSetviewport(DELPHI_INTEGER x, DELPHI_INTEGER y, DELPHI_INTEGER w, DELPHI_INTEGER h);
GFXCORE2_API DELPHI_SINGLE __stdcall tmcsGetCameraX();
GFXCORE2_API DELPHI_SINGLE __stdcall tmcsGetCameraY();
GFXCORE2_API DELPHI_SINGLE __stdcall tmcsGetCameraZ();
GFXCORE2_API DELPHI_SINGLE __stdcall tmcsGetCameraAngleX();
GFXCORE2_API DELPHI_SINGLE __stdcall tmcsGetCameraAngleY();
GFXCORE2_API DELPHI_SINGLE __stdcall tmcsGetCameraAngleZ();
GFXCORE2_API DELPHI_DOUBLE __stdcall tmcsGetCameraFov();
GFXCORE2_API DELPHI_DOUBLE __stdcall tmcsGetCameraAspect();
GFXCORE2_API DELPHI_DOUBLE __stdcall tmcsGetCameraNearPlane();
GFXCORE2_API DELPHI_DOUBLE __stdcall tmcsGetCameraFarPlane();
GFXCORE2_API DELPHI_SINGLE __stdcall tmcsWrapAngle(DELPHI_SINGLE f);
GFXCORE2_API DELPHI_SINGLE __stdcall tmcsGetNewX(DELPHI_SINGLE x, DELPHI_SINGLE angle, DELPHI_SINGLE factor);
GFXCORE2_API DELPHI_SINGLE __stdcall tmcsGetNewZ(DELPHI_SINGLE z, DELPHI_SINGLE angle, DELPHI_SINGLE factor);
GFXCORE2_API void          __stdcall tmcsSetCameraX(DELPHI_SINGLE posx);
GFXCORE2_API void          __stdcall tmcsSetCameraY(DELPHI_SINGLE posy);
GFXCORE2_API void          __stdcall tmcsSetCameraZ(DELPHI_SINGLE posz);
GFXCORE2_API void          __stdcall tmcsSetCameraPos(DELPHI_SINGLE posx, DELPHI_SINGLE posy, DELPHI_SINGLE posz);
GFXCORE2_API void          __stdcall tmcsSetCameraAngleX(DELPHI_SINGLE anglex);
GFXCORE2_API void          __stdcall tmcsSetCameraAngleY(DELPHI_SINGLE angley);
GFXCORE2_API void          __stdcall tmcsSetCameraAngleZ(DELPHI_SINGLE anglez);
GFXCORE2_API void          __stdcall tmcsSetCameraAngle(DELPHI_SINGLE anglex, DELPHI_SINGLE angley, DELPHI_SINGLE anglez);
GFXCORE2_API void          __stdcall tmcsXRotateCamera(DELPHI_SINGLE angle);
GFXCORE2_API void          __stdcall tmcsYRotateCamera(DELPHI_SINGLE angle);
GFXCORE2_API void          __stdcall tmcsZRotateCamera(DELPHI_SINGLE angle);
GFXCORE2_API void          __stdcall tmcsSetCameraFov(DELPHI_DOUBLE fov);
GFXCORE2_API void          __stdcall tmcsSetCameraAspect(DELPHI_DOUBLE aspect);
GFXCORE2_API void          __stdcall tmcsSetCameraNearPlane(DELPHI_DOUBLE znear);
GFXCORE2_API void          __stdcall tmcsSetCameraFarPlane(DELPHI_DOUBLE zfar);



/*
    Original gfxcore.dll exported functions as defined in gfxCore.pas:

    { Objektumok }
    function tmcsGetTotalObjects(): word; stdcall; external 'gfxcore.dll';
    function tmcsGetNumSubObjects(num: word): integer; stdcall; external 'gfxcore.dll';
    function tmcsCreatePlane(w,h: single): integer; stdcall; external 'gfxcore.dll';
    function tmcsCreateCube(a: single): integer; stdcall; external 'gfxcore.dll';
    function tmcsCreateBox(a,b,c: single): integer; stdcall; external 'gfxcore.dll';
    function tmcsCreateObjectFromFile(fname: TSTR40; compiled: boolean): integer; stdcall; external 'gfxcore.dll';
    function tmcsCreateClonedObject(refersto: integer): integer; stdcall; external 'gfxcore.dll';
    procedure tmcsSetObjectMultiTextured(index: word); stdcall; external 'gfxcore.dll';
    procedure tmcsMultiTexAssignObject(index1, index2: word); stdcall; external 'gfxcore.dll';
    procedure tmcsDeleteObject(index: word); stdcall; external 'gfxcore.dll';
    procedure tmcsDeleteObjects(); stdcall; external 'gfxcore.dll';
    procedure tmcsSetExtObjectsTextureMode(mipmapping: boolean; filtering,envmode: TGLConst; border,compressed: boolean; wrap_s,wrap_t: TGLConst); stdcall;  external 'gfxcore.dll';
    procedure tmcsCompileObject(index: word); stdcall; external 'gfxcore.dll';
    procedure tmcsShowObject(num: word); stdcall; external 'gfxcore.dll';
    procedure tmcsHideObject(num: word); stdcall; external 'gfxcore.dll';
    procedure tmcsSetObjectWireframe(num: word; wf: boolean); stdcall; external 'gfxcore.dll';
    procedure tmcsSetObjectDoublesided(num: word; ds: boolean); stdcall; external 'gfxcore.dll';
    procedure tmcsSetObjectStickedState(num: word; state: boolean); stdcall; external 'gfxcore.dll';
    procedure tmcsSetObjectZBuffered(num: word; state: boolean); stdcall; external 'gfxcore.dll';
    procedure tmcsSetWiredCulling(state: boolean); stdcall; external 'gfxcore.dll';
    function tmcsGetXPos(num: word): single; stdcall; external 'gfxcore.dll';
    function tmcsGetYPos(num: word): single; stdcall; external 'gfxcore.dll';
    function tmcsGetZPos(num: word): single; stdcall; external 'gfxcore.dll';
    function tmcsGetSizeX(num: word): single; stdcall; external 'gfxcore.dll';
    function tmcsGetSizeY(num: word): single; stdcall; external 'gfxcore.dll';
    function tmcsGetSizeZ(num: word): single; stdcall; external 'gfxcore.dll';
    function tmcsGetAngleX(num: word): single; stdcall; external 'gfxcore.dll';
    function tmcsGetAngleY(num: word): single; stdcall; external 'gfxcore.dll';
    function tmcsGetAngleZ(num: word): single; stdcall; external 'gfxcore.dll';
    function tmcsIsVisible(num: word): boolean; stdcall; external 'gfxcore.dll';
    function tmcsGetScaling(num: word): single; stdcall; external 'gfxcore.dll';
    function tmcsGetName(num: word): TSTR40; stdcall; external 'gfxcore.dll';
    procedure tmcsSetXPos(num: word; factor: single); stdcall; external 'gfxcore.dll';
    procedure tmcsSetYPos(num: word; factor: single); stdcall; external 'gfxcore.dll';
    procedure tmcsSetZPos(num: word; factor: single); stdcall; external 'gfxcore.dll';
    procedure tmcsScaleObject(num: word; factor: single); stdcall; external 'gfxcore.dll';
    procedure tmcsScaleObjectAbsolute(num: word; factor: single); stdcall; external 'gfxcore.dll';
    procedure tmcsXRotateObject(num: word; factor: single); stdcall; external 'gfxcore.dll';
    procedure tmcsYRotateObject(num: word; factor: single); stdcall; external 'gfxcore.dll';
    procedure tmcsZRotateObject(num: word; factor: single); stdcall; external 'gfxcore.dll';
    procedure tmcsSetAngleX(num: word; factor: single); stdcall; external 'gfxcore.dll';
    procedure tmcsSetAngleY(num: word; factor: single); stdcall; external 'gfxcore.dll';
    procedure tmcsSetAngleZ(num: word; factor: single); stdcall; external 'gfxcore.dll';
    procedure tmcsSetObjectRotationXZY(num: word); stdcall; external 'gfxcore.dll';
    procedure tmcsSetObjectRotationYXZ(num: word); stdcall; external 'gfxcore.dll';
    procedure tmcsSetName(num: word; name: TSTR40); stdcall; external 'gfxcore.dll';
    procedure tmcsSetObjectLit(num: word; state: boolean); stdcall; external 'gfxcore.dll';
    procedure tmcsSetObjectColor(num: word; r,g,b: byte); stdcall; external 'gfxcore.dll';
    function tmcsGetObjectColorKey(num: word): TRGBA; stdcall; external 'gfxcore.dll';
    procedure tmcsSetObjectColorKey(num: word; r,g,b,a: byte); stdcall; external 'gfxcore.dll';
    procedure tmcsSetObjectAlpha(num: word; a: byte); stdcall; external 'gfxcore.dll';
    procedure tmcsSetObjectBlendMode(num: word; sfactor,dfactor: TGLConst); stdcall; external 'gfxcore.dll';
    procedure tmcsSetObjectBlending(num: word; state: boolean); stdcall; external 'gfxcore.dll';
*/

GFXCORE2_API DELPHI_WORD    __stdcall tmcsGetTotalObjects();
GFXCORE2_API DELPHI_INTEGER __stdcall tmcsGetNumSubObjects(DELPHI_WORD num);
GFXCORE2_API DELPHI_INTEGER __stdcall tmcsCreatePlane(DELPHI_SINGLE w, DELPHI_SINGLE h);
GFXCORE2_API DELPHI_INTEGER __stdcall tmcsCreateCube(DELPHI_SINGLE a);
GFXCORE2_API DELPHI_INTEGER __stdcall tmcsCreateBox(DELPHI_SINGLE a, DELPHI_SINGLE b, DELPHI_SINGLE c);
GFXCORE2_API DELPHI_INTEGER __stdcall tmcsCreateObjectFromFile(DELPHI_TSTR40 fname, DELPHI_BOOLEAN compiled);
GFXCORE2_API DELPHI_INTEGER __stdcall tmcsCreateClonedObject(DELPHI_INTEGER refersto);
GFXCORE2_API void           __stdcall tmcsSetObjectMultiTextured(DELPHI_WORD index);
GFXCORE2_API void           __stdcall tmcsMultiTexAssignObject(DELPHI_WORD index1, DELPHI_WORD index2);
GFXCORE2_API void           __stdcall tmcsDeleteObject(DELPHI_WORD index);
GFXCORE2_API void           __stdcall tmcsDeleteObjects();
GFXCORE2_API void           __stdcall tmcsSetExtObjectsTextureMode(DELPHI_BOOLEAN mipmapping, DELPHI_TGLCONST filtering, DELPHI_TGLCONST envmode, DELPHI_BOOLEAN border, DELPHI_BOOLEAN compressed, DELPHI_TGLCONST wrap_s, DELPHI_TGLCONST wrap_t);
GFXCORE2_API void           __stdcall tmcsCompileObject(DELPHI_WORD index);
GFXCORE2_API void           __stdcall tmcsShowObject(DELPHI_WORD index);
GFXCORE2_API void           __stdcall tmcsHideObject(DELPHI_WORD index);
GFXCORE2_API void           __stdcall tmcsSetObjectWireframe(DELPHI_WORD index, DELPHI_BOOLEAN wf);
GFXCORE2_API void           __stdcall tmcsSetObjectDoublesided(DELPHI_WORD index, DELPHI_BOOLEAN ds);
GFXCORE2_API void           __stdcall tmcsSetObjectStickedState(DELPHI_WORD index, DELPHI_BOOLEAN state);
GFXCORE2_API void           __stdcall tmcsSetObjectZBuffered(DELPHI_WORD index, DELPHI_BOOLEAN state);
GFXCORE2_API void           __stdcall tmcsSetWiredCulling(DELPHI_BOOLEAN state);
GFXCORE2_API DELPHI_SINGLE  __stdcall tmcsGetXPos(DELPHI_WORD index);
GFXCORE2_API DELPHI_SINGLE  __stdcall tmcsGetYPos(DELPHI_WORD index);
GFXCORE2_API DELPHI_SINGLE  __stdcall tmcsGetZPos(DELPHI_WORD index);
GFXCORE2_API DELPHI_SINGLE  __stdcall tmcsGetSizeX(DELPHI_WORD index);
GFXCORE2_API DELPHI_SINGLE  __stdcall tmcsGetSizeY(DELPHI_WORD index);
GFXCORE2_API DELPHI_SINGLE  __stdcall tmcsGetSizeZ(DELPHI_WORD index);
GFXCORE2_API DELPHI_SINGLE  __stdcall tmcsGetAngleX(DELPHI_WORD index);
GFXCORE2_API DELPHI_SINGLE  __stdcall tmcsGetAngleY(DELPHI_WORD index);
GFXCORE2_API DELPHI_SINGLE  __stdcall tmcsGetAngleZ(DELPHI_WORD index);
GFXCORE2_API DELPHI_BOOLEAN __stdcall tmcsIsVisible(DELPHI_WORD index);
GFXCORE2_API DELPHI_SINGLE  __stdcall tmcsGetScaling(DELPHI_WORD index);
GFXCORE2_API DELPHI_TSTR40_RET   __stdcall tmcsGetName(DELPHI_WORD index);
GFXCORE2_API void           __stdcall tmcsSetXPos(DELPHI_WORD num, DELPHI_SINGLE factor);
GFXCORE2_API void           __stdcall tmcsSetYPos(DELPHI_WORD num, DELPHI_SINGLE factor);
GFXCORE2_API void           __stdcall tmcsSetZPos(DELPHI_WORD num, DELPHI_SINGLE factor);
GFXCORE2_API void           __stdcall tmcsScaleObject(DELPHI_WORD num, DELPHI_SINGLE factor);
GFXCORE2_API void           __stdcall tmcsScaleObjectAbsolute(DELPHI_WORD num, DELPHI_SINGLE factor);
GFXCORE2_API void           __stdcall tmcsXRotateObject(DELPHI_WORD num, DELPHI_SINGLE factor);
GFXCORE2_API void           __stdcall tmcsYRotateObject(DELPHI_WORD num, DELPHI_SINGLE factor);
GFXCORE2_API void           __stdcall tmcsZRotateObject(DELPHI_WORD num, DELPHI_SINGLE factor);
GFXCORE2_API void           __stdcall tmcsSetAngleX(DELPHI_WORD num, DELPHI_SINGLE factor);
GFXCORE2_API void           __stdcall tmcsSetAngleY(DELPHI_WORD num, DELPHI_SINGLE factor);
GFXCORE2_API void           __stdcall tmcsSetAngleZ(DELPHI_WORD num, DELPHI_SINGLE factor);
GFXCORE2_API void           __stdcall tmcsSetObjectRotationXZY(DELPHI_WORD num);
GFXCORE2_API void           __stdcall tmcsSetObjectRotationYXZ(DELPHI_WORD num);
GFXCORE2_API void           __stdcall tmcsSetName(DELPHI_WORD num, const DELPHI_TSTR40 name);
GFXCORE2_API void           __stdcall tmcsSetObjectLit(DELPHI_WORD num, DELPHI_BOOLEAN state);
GFXCORE2_API void           __stdcall tmcsSetObjectColor(DELPHI_WORD num, DELPHI_BYTE r, DELPHI_BYTE g, DELPHI_BYTE b);
GFXCORE2_API DELPHI_TRGBA   __stdcall tmcsGetObjectColorKey(DELPHI_WORD num);
GFXCORE2_API void           __stdcall tmcsSetObjectColorKey(DELPHI_WORD num, DELPHI_BYTE r, DELPHI_BYTE g, DELPHI_BYTE b, DELPHI_BYTE a);
GFXCORE2_API void           __stdcall tmcsSetObjectAlpha(DELPHI_WORD num, DELPHI_BYTE a);
GFXCORE2_API void           __stdcall tmcsSetObjectBlendMode(DELPHI_WORD num, DELPHI_TGLCONST sfactor, DELPHI_TGLCONST dfactor);
GFXCORE2_API void           __stdcall tmcsSetObjectBlending(DELPHI_WORD num, DELPHI_BOOLEAN state);



/*
    Original gfxcore.dll exported functions as defined in gfxCore.pas:

    { Alobjektumok }
    function tmcsGetSubXPos(num1, num2: word): single; stdcall; external 'gfxcore.dll';
    function tmcsGetSubYPos(num1, num2: word): single; stdcall; external 'gfxcore.dll';
    function tmcsGetSubZPos(num1, num2: word): single; stdcall; external 'gfxcore.dll';
    function tmcsSubIsVisible(num1, num2: word): boolean; stdcall; external 'gfxcore.dll';
    function tmcsGetSubSizeX(num1, num2: word): single; stdcall; external 'gfxcore.dll';
    function tmcsGetSubSizeY(num1, num2: word): single; stdcall; external 'gfxcore.dll';
    function tmcsGetSubSizeZ(num1, num2: word): single; stdcall; external 'gfxcore.dll';
    function tmcsGetSubName(num1, num2: word): TSTR40; stdcall; external 'gfxcore.dll';
    procedure tmcsSetSubXPos(num1, num2: word; factor: single); stdcall; external 'gfxcore.dll';
    procedure tmcsSetSubYPos(num1, num2: word; factor: single); stdcall; external 'gfxcore.dll';
    procedure tmcsSetSubZPos(num1, num2: word; factor: single); stdcall; external 'gfxcore.dll';
    procedure tmcsShowSubObject(num1, num2: word); stdcall; external 'gfxcore.dll';
    procedure tmcsHideSubObject(num1, num2: word); stdcall; external 'gfxcore.dll';
    procedure tmcsSetSubobjectColor(num1, num2: word; r,g,b: byte); stdcall; external 'gfxcore.dll';
    procedure tmcsSetSubName(num1, num2: word; name: TSTR40); stdcall; external 'gfxcore.dll';
*/

GFXCORE2_API DELPHI_SINGLE  __stdcall tmcsGetSubXPos(DELPHI_WORD num1, DELPHI_WORD num2);
GFXCORE2_API DELPHI_SINGLE  __stdcall tmcsGetSubYPos(DELPHI_WORD num1, DELPHI_WORD num2);
GFXCORE2_API DELPHI_SINGLE  __stdcall tmcsGetSubZPos(DELPHI_WORD num1, DELPHI_WORD num2);
GFXCORE2_API DELPHI_BOOLEAN __stdcall tmcsSubIsVisible(DELPHI_WORD num1, DELPHI_WORD num2);
GFXCORE2_API DELPHI_SINGLE  __stdcall tmcsGetSubSizeX(DELPHI_WORD num1, DELPHI_WORD num2);
GFXCORE2_API DELPHI_SINGLE  __stdcall tmcsGetSubSizeY(DELPHI_WORD num1, DELPHI_WORD num2);
GFXCORE2_API DELPHI_SINGLE  __stdcall tmcsGetSubSizeZ(DELPHI_WORD num1, DELPHI_WORD num2);
GFXCORE2_API DELPHI_TSTR40_RET  __stdcall tmcsGetSubName(DELPHI_WORD num1, DELPHI_WORD num2);
GFXCORE2_API void           __stdcall tmcsSetSubXPos(DELPHI_WORD num1, DELPHI_WORD num2, DELPHI_SINGLE factor);
GFXCORE2_API void           __stdcall tmcsSetSubYPos(DELPHI_WORD num1, DELPHI_WORD num2, DELPHI_SINGLE factor);
GFXCORE2_API void           __stdcall tmcsSetSubZPos(DELPHI_WORD num1, DELPHI_WORD num2, DELPHI_SINGLE factor);
GFXCORE2_API void           __stdcall tmcsShowSubObject(DELPHI_WORD num1, DELPHI_WORD num2);
GFXCORE2_API void           __stdcall tmcsHideSubObject(DELPHI_WORD num1, DELPHI_WORD num2);
GFXCORE2_API void           __stdcall tmcsSetSubName(DELPHI_WORD num1, DELPHI_WORD num2, DELPHI_TSTR40 name);



/*
    Original gfxcore.dll exported functions as defined in gfxCore.pas:

    { Fények }
    procedure tmcsEnableLights(); stdcall; external 'gfxcore.dll';
    procedure tmcsDisableLights(); stdcall; external 'gfxcore.dll';
    procedure tmcsEnableAmbientLight(); stdcall; external 'gfxcore.dll';
    procedure tmcsDisableAmbientLight(); stdcall; external 'gfxcore.dll';
    procedure tmcsSetAmbientLight(r,g,b: single); stdcall; external 'gfxcore.dll';
*/

GFXCORE2_API void __stdcall tmcsEnableLights();
GFXCORE2_API void __stdcall tmcsDisableLights();
GFXCORE2_API void __stdcall tmcsEnableAmbientLight();
GFXCORE2_API void __stdcall tmcsDisableAmbientLight();
GFXCORE2_API void __stdcall tmcsSetAmbientLight(DELPHI_SINGLE r, DELPHI_SINGLE g, DELPHI_SINGLE b);



/*
    Original gfxcore.dll exported functions as defined in gfxCore.pas:

    { Textúrák }
    function tmcsCreateTextureFromFile(fname: TFileName; mipmapped,border,compressed: boolean; filtering,envmode,wrap_s,wrap_t: TGLConst): integer; stdcall; external 'gfxcore.dll';
    procedure tmcsFrameBufferToTexture(texnum: integer); stdcall; external 'gfxcore.dll';
    function tmcsCreateBlankTexture(width, height: integer; filtering,envmode,wrap_s,wrap_t: TGLConst): integer; stdcall; external 'gfxcore.dll';
    procedure tmcsDeleteTexture(num: word); stdcall; external 'gfxcore.dll';
    procedure tmcsDeleteTextures(); stdcall; external 'gfxcore.dll';
    procedure tmcsTextureObject(num: word; num2: word); stdcall; external 'gfxcore.dll';
    function tmcsGetTextureWidth(num: word): integer; stdcall; external 'gfxcore.dll';
    function tmcsGetTextureHeight(num: word): integer; stdcall; external 'gfxcore.dll';
    function tmcsGetTextureInternalnum(num: word): cardinal; stdcall; external 'gfxcore.dll';
    function tmcsGetObjectTexture(num: word): integer; stdcall; external 'gfxcore.dll';
    function tmcsGetSubObjectTexture(num1: word; num2: word): integer; stdcall; external 'gfxcore.dll';
    procedure tmcsMultiplyUVCoords(num: word; factorw: single; factorh: single); stdcall; external 'gfxcore.dll';
    procedure tmcsAdjustUVCoords(num: word; factor: single); stdcall; external 'gfxcore.dll';
    procedure tmcsAdjustPlaneCoordsToViewport(num: word; num2: word); stdcall; external 'gfxcore.dll';
*/

GFXCORE2_API DELPHI_INTEGER  __stdcall tmcsCreateTextureFromFile(DELPHI_TFILENAME filename, DELPHI_BOOLEAN mipmapped, DELPHI_BOOLEAN border, DELPHI_BOOLEAN compressed, DELPHI_TGLCONST filtering, DELPHI_TGLCONST envmode, DELPHI_TGLCONST wrap_s, DELPHI_TGLCONST wrap_t);
GFXCORE2_API void            __stdcall tmcsFrameBufferToTexture(DELPHI_INTEGER texnum);
GFXCORE2_API DELPHI_INTEGER  __stdcall tmcsCreateBlankTexture(DELPHI_INTEGER width, DELPHI_INTEGER height, DELPHI_TGLCONST filtering, DELPHI_TGLCONST envmode, DELPHI_TGLCONST wrap_s, DELPHI_TGLCONST wrap_t);
GFXCORE2_API void            __stdcall tmcsDeleteTexture(DELPHI_WORD num);
GFXCORE2_API void            __stdcall tmcsDeleteTextures();
GFXCORE2_API void            __stdcall tmcsTextureObject(DELPHI_WORD num, DELPHI_WORD num2);
GFXCORE2_API DELPHI_INTEGER  __stdcall tmcsGetTextureWidth(DELPHI_WORD num);
GFXCORE2_API DELPHI_INTEGER  __stdcall tmcsGetTextureHeight(DELPHI_WORD num);
GFXCORE2_API DELPHI_CARDINAL __stdcall tmcsGetTextureInternalNum(DELPHI_WORD num);
GFXCORE2_API DELPHI_INTEGER  __stdcall tmcsGetObjectTexture(DELPHI_WORD num);
GFXCORE2_API DELPHI_INTEGER  __stdcall tmcsGetSubObjectTexture(DELPHI_WORD num1, DELPHI_WORD num2);
GFXCORE2_API void            __stdcall tmcsMultiplyUVCoords(DELPHI_WORD num, DELPHI_SINGLE factorw, DELPHI_SINGLE factorh);
GFXCORE2_API void            __stdcall tmcsAdjustUVCoords(DELPHI_WORD num, DELPHI_SINGLE factor);
GFXCORE2_API void            __stdcall tmcsAdjustPlaneCoordsToViewport(DELPHI_WORD num, DELPHI_WORD num2);



/*
    Original gfxcore.dll exported functions as defined in gfxCore.pas:

    { Vertexszintû manipulálás }
    function tmcsGetTotalVertices(num: word): integer; stdcall; external 'gfxcore.dll';
    function tmcsGetVertexX(num: word; num2: word): single; stdcall; external 'gfxcore.dll';
    function tmcsGetVertexY(num: word; num2: word): single; stdcall; external 'gfxcore.dll';
    function tmcsGetVertexZ(num: word; num2: word): single; stdcall; external 'gfxcore.dll';
    procedure tmcsSetVertex(num: word; num2: word; x,y,z: single); stdcall; external 'gfxcore.dll';
    function tmcsGetNormalX(num: word; num2: word): single; stdcall; external 'gfxcore.dll';
    function tmcsGetNormalY(num: word; num2: word): single; stdcall; external 'gfxcore.dll';
    function tmcsGetNormalZ(num: word; num2: word): single; stdcall; external 'gfxcore.dll';
    procedure tmcsSetNormal(num: word; num2: word; nx,ny,nz: single); stdcall; external 'gfxcore.dll';
*/

GFXCORE2_API DELPHI_INTEGER __stdcall tmcsGetTotalVertices(DELPHI_WORD num);
GFXCORE2_API DELPHI_SINGLE  __stdcall tmcsGetVertexX(DELPHI_WORD num, DELPHI_WORD num2);
GFXCORE2_API DELPHI_SINGLE  __stdcall tmcsGetVertexY(DELPHI_WORD num, DELPHI_WORD num2);
GFXCORE2_API DELPHI_SINGLE  __stdcall tmcsGetVertexZ(DELPHI_WORD num, DELPHI_WORD num2);
GFXCORE2_API void           __stdcall tmcsSetVertex(DELPHI_WORD num, DELPHI_WORD num2, DELPHI_SINGLE x, DELPHI_SINGLE y, DELPHI_SINGLE z);
GFXCORE2_API DELPHI_SINGLE  __stdcall tmcsGetNormalX(DELPHI_WORD num, DELPHI_WORD num2);
GFXCORE2_API DELPHI_SINGLE  __stdcall tmcsGetNormalY(DELPHI_WORD num, DELPHI_WORD num2);
GFXCORE2_API DELPHI_SINGLE  __stdcall tmcsGetNormalZ(DELPHI_WORD num, DELPHI_WORD num2);
GFXCORE2_API void           __stdcall tmcsSetNormal(DELPHI_WORD num, DELPHI_WORD num2, DELPHI_SINGLE nx, DELPHI_SINGLE ny, DELPHI_SINGLE nz);



/*
    Original gfxcore.dll exported functions as defined in gfxCore.pas:

    { Szövegek kiírásának kezelése }
    procedure tmcsLoadFontInfo(path,name: tstr255); stdcall; external 'gfxcore.dll';
    procedure tmcsText(text: tstr255; x,y: word; fontheight: word; scaling: word); stdcall; external 'gfxcore.dll';
    procedure tmcsSetTextColor(r,g,b,a: byte); stdcall; external 'gfxcore.dll';
    procedure tmcsSetTextBlendingState(state: boolean); stdcall; external 'gfxcore.dll';
    procedure tmcsSetTextBlendMode(sfactor, dfactor: TGLConst); stdcall; external 'gfxcore.dll';
    function tmcsGetTextWidth(text: tstr255; fontheight: word; scaling: word): integer; stdcall; external 'gfxcore.dll';
*/

GFXCORE2_API void           __stdcall tmcsLoadFontInfo(DELPHI_TSTR255 path, DELPHI_TSTR255 name);
GFXCORE2_API void           __stdcall tmcsText(DELPHI_TSTR255 text, DELPHI_WORD x, DELPHI_WORD y, DELPHI_WORD fontheight, DELPHI_WORD scaling);
GFXCORE2_API void           __stdcall tmcsSetTextColor(DELPHI_BYTE r, DELPHI_BYTE g, DELPHI_BYTE b, DELPHI_BYTE a);
GFXCORE2_API void           __stdcall tmcsSetTextBlendingState(DELPHI_BOOLEAN state);
GFXCORE2_API void           __stdcall tmcsSetTextBlendMode(DELPHI_TGLCONST sfactor, DELPHI_TGLCONST dfactor);
GFXCORE2_API DELPHI_INTEGER __stdcall tmcsGetTextWidth(DELPHI_TSTR255 text, DELPHI_WORD fontheight, DELPHI_WORD scaling);

GFXCORE2_API void __stdcall tmcsSetRenderPath(DELPHI_WORD renderPath);
GFXCORE2_API void __stdcall tmcsSetOcclusionCullingMethod(DELPHI_WORD ocMethod);
GFXCORE2_API void __stdcall tmcsSetOcclusionCullingBoundingBoxes(DELPHI_BOOLEAN state);
GFXCORE2_API void __stdcall tmcsSetOcclusionCullingDrawIfPending(DELPHI_BOOLEAN state);
GFXCORE2_API void __stdcall tmcsSetOrderingByDistance(DELPHI_BOOLEAN state);
GFXCORE2_API void __stdcall tmcsResetStatistics();
GFXCORE2_API void __stdcall tmcsEngineDump();
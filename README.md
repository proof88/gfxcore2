# gfxcore2
C++ adapter/wrapper lib for the original Delphi-based tmcsgfxlib2 which was used by the legacy PR00FPS made in 2007.

I wrote this wrapper lib in 2019 in order to be able to run the old Delphi-based legacy PR00FPS with my NEW C++ -based graphics engine (PURE) without the need of any modification of the original code of PR00FPS.

This wrapper lib is sitting in between legacy PR00FPS and the new PURE graphics engine, mimicing the behavior of the old Delphi-based tmcsgfxlib2 graphics library towards PR00FPS.

**Requires**: I keep this lib up-to-date, so in theory, anytime you checkout the **latest PURE graphics library**, it can be used to compile this library without any modification into a .dll file, that can be placed into PR00FPS directory to run PR00FPS with.

The source of the new PURE graphics library (part of PGE game engine) can be checked here:
https://github.com/proof88/PGE

To **build PGE and this gfxcore2 projects**, please follow the instructions in [PGE-WoW.txt](https://github.com/proof88/PGE-misc/blob/master/src/PGE-WoW.txt).

The source of the original tmcsgfxlib2 written in Delphi can be checked here:
https://github.com/proof88/tmcsgfxlib2

And the source code of PR00FPS written in Delphi is here:
https://github.com/proof88/PR00FPS

The Visual Studio project file is included.<br/>
However, you may be interested in the Visual Studio solution including other projects as well in [PGE-misc](https://github.com/proof88/PGE-misc) repo, including the abovementioned new PURE graphics library as well.


# Design Notes

Some design and implementation notes since the devlog was getting too big.

## Global classes

* AppRunner - initializes and creates the application
* Package - holds application and project state inforamtion.  Contains all scenes, models, meshes and textures and packages them into one executable.
* Scene - A scene for all items in the currently loaded game world.  Areas are separated into scenes in order to minimize memory footprint.  In the case of the designed game, scene differentiation is really unnecessary, as the world is "minecraft" infinite and it loads dynamically.  However, for a more linear game model the Scene would be used to separate game areas in order to reduce memory footprint.
* Gu - Global utility class used to access commonly used pieces of the engine (textures, meshes) through simple static methods.
* GLContext - OpenGL context.  There may be more than one OpenGL context in the engine.  "Sharable" items are shared across contexts, including textures, meshes and shaders.

## Hierarchy Overview

* Gu
    * OpenGL Contexts
	* Window Manager
* AppRunner
    * _Initializes globals and loads Package, creates scene from package, creates window, attaches main window to scene_
	* GraphicsWindow
	* Scene


## Additional Notes

Package loading is done automatically right now since this engine is being programmed specifically for the specified game design.  It's possible, however, to load individual packages in the case there are multiple games.


# Design Notes

## Overview

BR2 ('Mine-Maker') started off as a small game that used Minecraft-like world-building.  It incorporates a fusion of many kinds of games.  The graphics engine grew out of this development.  Of course, since the game itself
has somewhat been put on hiatus, for the purpose of getting this engine to a working, and stable state, all systems that are added are added with the purpose of completing the game.  Meaning, that any unnecessary
game systems are going to be left for later development.  

Globally, the game is contained in an application Package.  The Package is an application of which all game data is appended to the end of the executable.  Packages are equivalent to project files.   Each package
represents a different game, so there is one package per game. Package configuration data are stored in Package.xml in the *game root folder*. The *game root folder* is the folder in which the application is currently
executing.  During development, we keep all of the  game's data in this folder, '/data/'. These directories can be changed by changing the Package.xml.  Additionally, any engine-specific configurations can be changed
by updating engine.xml in the root folder.

BR2's systems are controlled by a set of managers.  Each manager controls a set of objects.  The engine is separated into a set of windows.  Each window contains its own OpenGL context, and a Rendering Pipeline.  

#### Scenes
The game world is separated into Scenes.   Each scene is a 'scenegraph', and is a directed acyclic graph.  Rendering is performed by running a PVS, Frustum pruning, and collection routine after the physics
system has settled all the physical objects in the scene.  For the purpose of the game design, we are using an infinite-world type of system.  Sort of like Minecraft.  The world is separated into a massive
grid of cube-blocks.  Unlike Minecraft (AFAIK) our grid is infinite across all 3 axes.  This means, there is no limit to how deep you can mine in the game.  

Scenes can be moved in between Windows, which gives us the ability to render the same scene across multiple windows.  This is currently the
goal for Phase 1 of the engine.   Each scene contains a user interface defined by a UiScreen.  Each window user interfaces is separate from each other window's.  The UI is currently under development, and the goal is to get multiple windows 
to render alternate user interfaces seamlessly, in separate contexts'

The game also contains a physics system.  The physics system runs an iterative solver which computes a 'best fit' for the game objects, within some parameters.  The physics engine is definitely going 
to be ported to the GPU, in the future, in another rendering context.  There are many enhancements that can be made to the current state of the physics engine itself, but because the game design is 
mostly tile-based, an excessive level of detail in the iterative solving engine is unnecessary.

## Details

|  Folder | Contents|
|---------|----------------------------------------------------------------------------------------------------------
|  base   | graphs, generic systems, memory, buffers, networking, events and OS interface.							|
|  ext    | *Lightweight* external libraries.  (Large API's reside in ./external).									|
|  gfx    | GPU, graphics classes, image manipulation,  materials, lighting, framebuffers, shaders.					|
|  math   | vectors, matrices, boxes, geometry, hulls, algorithms.													|
|  model  | meshes, animation, models, characters, skeletons, bones.												|
|  world  | physics, scenegraph. 																					|
|  bottle  | contains world rendering data.                                                                         |

| Class            | Purpose                                                                                                                                                                                                                                                                                                                                                                                                                                             |
|------------------|---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
|     Package      |  There is only 1 package at a time in the BR2 engine.  Application and project state inforamtion are stored here.  Contains all scenes, models, meshes and textures and packages them into one executable.                                                                                                                                                                                                                                                                                                              |
|     Window       |  Windows are the root of the rendering system. Each window gets its own rendering pipeline, and graphics context.						|
|     Context      | Rendering context.  1 Global context, and multiple additional contexts are possible.  Items shared across contexts include textures, meshes and shaders.                                                 |
|     Scene        | Scenegraph for all items in the currently loaded game world.  Areas are separated into scenes in order to minimize memory footprint.  In the case of the designed game, scene differentiation is really unnecessary, as the world is "minecraft" infinite and it loads dynamically.  However, for a more linear game model the Scene would be used to separate game areas in order to reduce memory footprint.                                                       |
|     Gu           | Global utility class used to access commonly used pieces of the engine (textures, meshes) through simple static methods.                                                      |

## Class Hierarchy

* Gu
	* Texture Manager (1, GLOBAL)
	* Mesh Manager (1, GLOBAL)
	* ShaderManager (1, GLOBAL)
    * Package (1, GLOBAL)
	* GraphicsApi (1, GLOBAL)
		* Context (1+)
			* GraphicsWindow (1)
				* FrameSync (1)
				* Delta (1)
				* RenderPipe (1)
					* Picker (1)
				* UiScreen (1)
				* PhysicsWorld (1)
				* Scene (1)
					* LightNode (1*)
						* :PointLight
						* :DirLight
					* CameraNode (1*, only 1 active at a time)
					* PhysicsNode (1*) 
					* MeshNode (1*)
					* ModelNode (1*)
					* SceneNode (1*)
						* Component (1*)
							* BottleScript (1)
								* World25 (1)

## Notes

* Package loading is done automatically right now since this engine is being programmed specifically for the specified game design.  It's possible, however, to load individual packages in the case there are multiple games.
* Multiple Rendering in a single area is going to be important.
* PBOs allow for async texture rendering.  PBO stands for Pixel Buffer Object. It allows you to upload a texture to GL in a asynchronous fashion which means that calls to glTexImage2D are instantaneous. 
"https://www.khronos.org/opengl/wiki/OpenGL_and_multithreading"
* GL contexts can share most data.  VAOs and Framebuffers cannot be shared.  For this reason, RenderPipeline, and Picker (which uses a pipe buffer), exist only on GraphicsWindow.

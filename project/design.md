
# Design Notes

## Overview

BR2 started off as a small game design that was dependent on Minecraft-like world-building, incorporating a fusion of many kinds of games.  The graphics engine grew out of this development.  Of course, the game itself
has somewhat been put on hiatus, for the purpose of getting this engine to a working, and stable state.  Yet, all systems that are added are added with the purpose of completing the game.  Meaning, that most generic
game systems (such as the ability to create multiple game projects) are going to be left for later development.  

Globally, the game is contained in an application Package.  The Package is a packed application of which all necessary data is appended to the end of the executable.  Packages are equivalent to project files.   Each package
represents a different game, so there is one package per game. Package configuration data are stored in Package.xml in the *game root folder*.  

BR2's systems are controlled by a set
of managers.  Each manager controls a set of objects.  The engine is separated into a set of windows.  Each window contains its own OpenGL context, and a Rendering Pipeline.  

The game world is separated into Scenes.   Each scene is a scenegraph is a directed acyclic graph data structure.  Rendering is performed by running a pruning, and collection routine after the physics
system has settled all the physical objects in the scene. Scenes can be moved in between Windows, which gives us the ability to render the same scene across multiple windows.  This is currently the
goal for Phase 1 of the engine.  Each scene contains a user interface defined by a UiScreen.  Each window user interfaces is separate from each other window's.  The UI is currently under development, and the goal is to get multiple windows 
to render alternate user interfaces seamlessly, in separate contexts'

The game also contains a physics system.  The physics system runs an iterative solver which computes a 'best fit' for the game objects, within some parameters.  The physics engine is definitely worthy
of being promoted to the GPU, and perhaps in an asynchronous rendering context.  There are many enhancements that can be made to the current state of the physics engine.

## Details

|  Folder | Contents|
|---------|----------------------------------------------------------------------------------------------------------
|  base   | graphs, generic systems, memory, buffers, networking, events and OS interface.
|  ext    | *Lightweight* external libraries.  (Large API's reside in ./external).
|  gfx    | GPU, graphics classes, image manipulation,  materials, lighting, framebuffers, shaders.
|  math   | vectors, matrices, boxes, geometry, hulls, algorithms.
|  model  | meshes, animation, models, characters, skeletons, bones.
|  world  | physics, scenegraph. 

| Class            | Purpose                                                                                                                                                                                                                                                                                                                                                                                                                                             |
|------------------|---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
|     AppRunner    |  Initializes and creates the application                                                                                                                                                                                                                                                                                                                                                                                                                                                 |
|     Package      |    Holds application and project state inforamtion.  Contains all scenes, models, meshes and textures and packages them into one executable.                                                                                                                                                                                                                                                                                                              |
|     Scene         | A scene for all items in the currently loaded game world.  Areas are separated into scenes in order to minimize memory footprint.  In the case of the designed game, scene differentiation is really unnecessary, as the world is "minecraft" infinite and it loads dynamically.  However, for a more linear game model the Scene would be used to separate game areas in order to reduce memory footprint.                                                       |
|     Gu            | Global utility class used to access commonly used pieces of the engine (textures, meshes) through simple static methods.                                                      |
|     GLContext     | OpenGL context.  There may be more than one OpenGL context in the engine.  "Sharable" items are shared across contexts, including textures, meshes and shaders.                                                 |
|   Managers | All managers control their respective system.  PhysicsManager, InputManager, WorldManager, ModelManger, TexCache (TextureManager), ShaderMaker (ShaderManager) .. etc.

## Classes

* Gu
    * Package (1)
    * GLContext (1+)
	    * Texture Manager (1, static)
		* Mesh Manager (1, static)
        * ShaderManager (1, static)
        * FrameSync (1)
        * Delta (1)
        * RenderPipeline (1) 
	* Window Manager (1)
        * GraphicsWindow (1+)
		    * GLContext (1, ref)
            * Scene (1)
                * <GameObject> (1*)
					*  WorldObject 
						* <Component>
							* CollisionShape
							* CSharpScript
						* :PhysicsNode (inherited)
							* :BaseNode (inherited)
								* :TreeNode (inherited)
									* :Drawable (inherited)
										* :VMS (inherited)
                * UiScreen (1)
                * PhysicsWorld (1)
         * `step(delta) \\Steps the graphics context, engine, and simulation`

## Notes

* Package loading is done automatically right now since this engine is being programmed specifically for the specified game design.  It's possible, however, to load individual packages in the case there are multiple games.
* Multiple Rendering in a single area is going to be important.
* PBOs allow for async texture rendering.  PBO stands for Pixel Buffer Object. It allows you to upload a texture to GL in a asynchronous fashion which means that calls to glTexImage2D are instantaneous. 
"https://www.khronos.org/opengl/wiki/OpenGL_and_multithreading"
* GL contexts can share most data.  VAOs and Framebuffers cannot be shared.  For this reason, RenderPipeline, and Picker (which uses a pipe buffer), exist only on GraphicsWindow.

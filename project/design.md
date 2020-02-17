
# Design Notes

## Overview

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

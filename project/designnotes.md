
# Design Notes

Some design and implementation notes since the devlog was getting too big.

## Main Classes

| Class            | Purpose                                                                                                                                                                                                                                                                                                                                                                                                                                             |
|------------------|---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
|     AppRunner    |  Initializes and creates the application                                                                                                                                                                                                                                                                                                                                                                                                                                                 |
|     Package      |    Holds application and project state inforamtion.  Contains all scenes, models, meshes and textures and packages them into one executable.                                                                                                                                                                                                                                                                                                              |
|     Scene         | A scene for all items in the currently loaded game world.  Areas are separated into scenes in order to minimize memory footprint.  In the case of the designed game, scene differentiation is really unnecessary, as the world is "minecraft" infinite and it loads dynamically.  However, for a more linear game model the Scene would be used to separate game areas in order to reduce memory footprint.                                                       |
|     Gu            | Global utility class used to access commonly used pieces of the engine (textures, meshes) through simple static methods.                                                      |
|     GLContext     | OpenGL context.  There may be more than one OpenGL context in the engine.  "Sharable" items are shared across contexts, including textures, meshes and shaders.                                                 |

## Hierarchy Overview

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
                * UiScreen (1)
                * PhysicsWorld (1)
         * `step(delta) \\Steps the graphics context, engine, and simulation`

## Notes

Package loading is done automatically right now since this engine is being programmed specifically for the specified game design.  It's possible, however, to load individual packages in the case there are multiple games.


Curent problem: I have 1 camera, rendering to 2 different windows, or surfaces.  How do I give it the viewport if the viewport is different per surface?  
It needs a viewport driver of some kind.  The AR of the cameras are all fixed to a specific AR.

* So, mouse projection must be done on the window, using the current Scene camera. Each window has 1 scene.  The scene has 1 active camera.
* Cameras will then use Aspect Ratio to determine the frustum data.
* To render, we must pass in a RenderTarget, like a window, to the pipeline's RenderScene method.

Gu owns the window manager.  Window manager owns windows.  Windows point to scenes.
	Window ->
		Scene->
			<GameObject>
			PhysicsWorld
				PhysicsNode : GameObject

	Scene owns the camera (currently, window owns camera).  Camera is a property on the RenderTarget.

	RenderPipe is part of a rendertarget.  A rendertarget can be a window, or buffer.
	GraphicsWindow :: RenderTarget
	RenderTarget
		RenderPipe 


	Every window (the system window blender runs in) has a screen property. Every screen in turn has a scene property which defines the active editable scene.

## General Notes
* Multiple Rendering in a single area is going to be important.
* PBOs allow for async texture rendering.  PBO stands for Pixel Buffer Object. It allows you to upload a texture to GL in a asynchronous fashion which means that calls to glTexImage2D are instantaneous. 
"https://www.khronos.org/opengl/wiki/OpenGL_and_multithreading"
* VAOs and Framebuffers cannot be shared.

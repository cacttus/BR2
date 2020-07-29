
# Design Notes

## Overview

The compiled game is contained in an "application package" but this is not yet fully implemented. Game's data in this folder, '/data/'. 
These directories can be changed by changing the Package.xml.  Any engine-specific configurations can be changed
by updating engine.xml in the root folder. BR2's systems are controlled by a set of managers.  Each manager controls a set of objects.  
The engine is separated into a set of windows. Each window contains its own OpenGL context, and a Rendering Pipeline.  

The game world is separated into Scenes. Each scene is a scenegraph, and is a directed acyclic graph. Rendering is performed by running a PVS, 
frustum pruning, and collection routine after the physics system has settled all the physical objects in the scene. Scenes can be moved in between Windows, 
which gives us the ability to render the same scene across multiple windows.  This is currently the goal for Phase 1 of the engine. Each scene contains a 
user interface. The UI is currently under development, and the goal is to get multiple windows to render alternate user interfaces in separate contexts.

### Folders

|  Folder | Contents|
|---------|----------------------------------------------------------------------------------------------
|  base   | graphs, generic systems, memory, buffers, networking, events and OS interface.				|
|  ext    | *Lightweight* external libraries.  (Large API's reside in ./external).						|
|  gfx    | GPU, graphics classes, image manipulation,  materials, lighting, framebuffers, shaders.		|
|  math   | vectors, matrices, boxes, geometry, hulls, algorithms.										|
|  model  | meshes, animation, models, characters, skeletons, bones.									|
|  world  | physics, scenegraph. 																		|
|  bottle | contains world rendering data.                                                              |

| Class     | Purpose                                                                                                                                                                                                                                                                                                                                                                                                                                             |
|-----------|---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
|  Package  | Application and project state inforamtion.  Contains all scenes, models, meshes and textures and packages them into one executable.                                                                                                                                                                                                                                                                                                              |
|  Window   | Each window gets its own rendering pipeline, and graphics context.						|
|  Context  | Rendering context.  1 Global context, and multiple additional contexts are possible.  Items shared across contexts include textures, meshes and shaders.                |
|  Scene    | Scenegraph for all items in the currently loaded game world.  Areas are separated into scenes in order to minimize memory footprint.        |
|  Gu       | Global static class to access commonly used pieces of the engine (textures, meshes) through static methods.                                                      |

### Hierarchy

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

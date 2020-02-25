# Mine City Task Log

## BR0 to BR2 Upgrade
- [x] Remove /base/ - place in /base/
	- [x] Compile, Run.
- [x] Code Cleanup
	- [x] Remove empty H/CPP files
	- [x] Fix Comments 
	- [x] Remove Name
	- [x] Fix spacing
	- [x] Organize public members first.
	- [x] Compile, Run.
- [x] Engine Rename - Rename engine to BR2
	- [x] Rename "bro.." to "br.." in...
		- [x] Exceptions
		- [x] math
	- [x] Compile, Run.
- [ ] File Additions, and Replacements (from broken version)	
	- [x] Remove most of Globalincludes.
		- [x] Add SDLIncludes
	- [x] Compile, run.
	- [x] Rename t_string to string_t and remove string_t
	- [x] Replace OperatingSystem file
	- [x] Replace BuildConfig
	- [ ] Rename Fingers -> InputManager
	- [ ] Rename Party -> ParticleManager
	- [ ] Rename ObjectFile to ObjFile
	- [ ] Compile, Run.
- [ ] "App" Removal for Scene
	- [x] Move /app into /base/
	- [ ] Add Scene (from broken version)
	* For some reason our old files got mixed in with the new...ugh.
	- [ ] Remove App, AppBase
	- [ ] Place this stuff on scene
	- [ ] Add ApplicationPackage (from broken version), remove Package 
- [x] Engine Rename - Rename engine to BR2
	- [x] Rename "bro.." to "br.." in...
		- [x] Exceptions
		- [x] math
		- [x] Compile, Run.
	- [x] Rename ObjectFile to ObjFile
	- [x] Compile, Run.
- [ ] Squashed viewport Bug
	- [ ] Fix SQUASHED text rendering.  Text must be rendered at the same w/h ratio no matter window size.

## Bottle World
- [ ] CSharp inline *minimal* scripts.
	* This should fix the camera, so we can easily add the bottle topologizer.
	- [ ] Update FlyCameraControls (flycam) from previous release and use CSharpScript to control it.
- [ ] Add Bottle World
	- [ ] Add the foundation (globs, awareness)
	- [ ] Test and render the Bottle Globs.
- [ ] Move object creation in PhysicsManager to Scene.
	- [ ] Compile, Run.

## Model & Scene Updates
* The goal here is to create a more 'common' game engine architecture, similar to what Blender and Unity use.  We want to be able to quickly add nodes
to scenes.  Our current "Spec" system for creating node clones is sloppy, and doesn't make sense as a lot of the "spec" data are not resource intensive
and don not need to be shared.  Secondly, we want to have a component-based system, where we may have multiple 'nodes' per WorldObject (GameObject in Unity).
The system will be rewritten in 3 areas:
	1. Remove all Data classes, besides MeshData.  MeshData will be referred to as a "slot" in the "MeshComponent"
		* Add NodeData to SceneNode, not copying it.
	2. Implement `clone()` and `copy()` on SceneNode
	3. Add a generic component class, which is subclassed by
		* CSharpScript (ScriptComponent)
		* Mesh (MeshComponent)
	4. Integration of "PhysicsNode" with the nodes themselves. (it makes no sense do deprive a node of velocity).
		* Remove Velocity from "PhysicsNode" and place the Velocity on SceneNode.
		* Create a ColliderComponent
		* Place Mass, Shape, and all other PhysicsNode properties on the ColliderComponent
		* Subclass ColliderComponent into SphereComponent, and HullComponent.
			* We will only have 2 colliders for this game, there doesn't need to be more (yet).

- [ ] Implement GLTF file loader. (Replace MBI files with GLTF binary)
	- [ ] Test, by using a GLTF model from Blender.
- [ ] Data Class Separation. Replace Node/Spec with a clone() and copy() system.
	- [ ] Remove WorldObject inheritence, and use Component model.
	- [ ] Rename BaseNode -> SceneNode
	- [ ] Remove NodeData - place on the SceneNode.  It's not shared data, so ther's no point.
	- [ ] Move Serialize/Deserialize from _Data_ to _Node_ classes.
	- [ ] Implement `clone()` and `copy()` on nodes.
	- [ ] Move complex methods from all *data* classes to their respective Node classes.
	- [ ] Remove inheritence from data classes.
	- [ ] Implement some kind of management system that prevents NodeData name collisions. (they must be unique as name and hashedname are the GUIDs for the nodedata).
	- [ ] Update Scene File
		- [ ] Create a data model that holds all nodes in the game.  Currently, there's no 'scene' file, but we have the glob/world files.
			- [ ] *Recursive Serialization* of nodes, instead of the linear Serialization.  
			- [ ] Control this serialization through a new type of manager file.
	- [ ] Completely remove "Model" and either, 
		* use some kind of Component interface for models, or just place Model's information on WorldObject.
		* or just place Model's information on WorldObject.
		* or just place Model's information on WorldObject
		* or Subclass MeshNode with SkinnedMeshNode
	- [ ] Componentize "Model" class, and put it in a logical place, such as on the Armature, or as a "component"
	- [ ] *Dynamic skinning* where, mesh skin is a *component* and their *skin* is a separate component on WorldObject that points to the given mesh. We should copy Blender's data format.  Armature is a child of the object.

## Mine City, Enhancements, Phase I
- [ ] Simplify the UI to work with the UI design for this game.  (Which should update UI performance)\
- [ ] Implement hard coded x/y locations.  
- [ ] Remove percentages.
- [ ] Asynchronous UI layout routine.

- [ ] **Story**: Create a new Peon.	
		
- [ ] **Story**: Be able to click on a toolbelt item and drag it from your inventory to your toolbelt.
	- [ ] Tab Bar
		- [ ] TabItem
		- [ ] Temporary Tab vs Permanent Tab
	- [ ] Inventory Window.
		- [ ] Scrollbars, for more info
		- [ ] Organize Inventory Items.
	- [ ] ToolBelt Slots

- [ ] **Story**: Be able to exit the game.
	- [ ] Hamburger Menu (top right): 
		- [ ] Settings
			- [ ] Fullscreen toggle button
		- [ ] Exit Game

- [ ] **Story**: Be able to dig the ground, and place items on the ground.
	- [ ] Mine Tool.
		- [ ] Move mine tool from inventory to toolbelt

- [ ] **Story**: Be able to build paths for the peons to navigate.
	- [ ] Path Builder Tab & Path Builder Toolbar item

## Engine Enhancements, Unrelated to Game
- [ ] CMake integration. Test on iOS, Linux, Android.
- [ ] Instanced Rendering. Merge all the uniform buffers, skin joint buffers. Reference by gl_InstanceID. (see PhysicsManager)
- [ ] Move window update logic from AppRunner to GraphicsWindow so they can run async.

## Delayed / Shelved 
- [ ] ~~Multiple Window Rendering~~
	* This isn't necessary for the purpose of completing the game.
	- [ ] ~~Remove Vulkan~~
	- [ ] ~~Hard code GL context. Remove GraphicsContext~~
	- [ ] ~~Add GL Context to all required Classes~~
	- [ ] ~~Compile, Run.~~
- [x] ~~Unlink the Window viewport with the Camera Viewport.~~
	* Although 'logical' this is not possible with modern GPUs, as we'd end up having to create separate RenderPipe's per Camera, since, the Viewport Width/Height determines the RenderPipe Width/Height. *Nor* are framebuffers shared across GL contexts, *and* we are running these contexts asynchronously. Each renderpipe uses more than 8 1080p surfaces.  Obviously, we'd end up with memory or performance issues.  The solution, is to share camera viewport.  Maybe, in the future, when GPUs can handle such intensity.
- [ ] Combine RenderSettings with EngineConfig.  Puts all engine settings in one place.

### Wishlist (TODO tasks, not directly relevant to project)
- [ ] Integrate SDL_Touch with the engine, making 
- [ ] Instead of hard code vertex interleaved formats, allow us to supply multiple buffers for the vertex components (n,v,t..) for each component
- [ ] Move all STL header files DOWN into the respective CPP files that need them. This will speed-up compilation time a lot.
- [ ] detach Camera viewport from Window viewport so the camera can render independently (offscreen) of whichever window it's rendering to.  This would mean creating a CameraViewport class, and passing w/h into the camera.
- [ ] All strings use the same hash function which may cause collisions.  Integrate collision handling function in HashMap and in all places where STRHASH is used.
- [ ] Keyframe Bezier curve handles, and bezier interpolation (see KeyFrame)
- [ ] Remove WorldObject class inheritance, and favor composition like Unity does.  Managers will hold onto components.
	- [ ] WorldObject will be composited by other items and not inherited.


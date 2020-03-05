# Mine City Task Log

- [ ] Fix the onAddedToNode code to work only when the root scene() has been attached to the Window()

- [ ] (CRITICAL) Deleted the Async Logger on accident.  Add it back (previous checkin).

- [ ] Implement the STL math functions in MathHeader

## Controller System
- [ ] Fix issues with Input for multiple windows
	* Mouse wheel for example would get set in the input manager.
	- [ ] Remove Global Input Manager
	
- [ ] Make WorldSelect part of the UI system.  (Remove it from BottleScript.h) 
	* either remove the WorldSelect class, or turn it into a UI "script".

- [ ] The current system for controls isn't generic.  We directly map everything to WSAD. We need a 'controller' class like the Helix library has.
	* The goal of the Controller class is to move the Joystick, Dpad, etc, with the implementation, meaning if the user presses W on the keyboard, the Joystick will move directly up, since the keyboard lacks a joystick.
	* `class Controller`
	* `  Joystick _pJoystick` --> Maps to WSAD or a controller DPAd
	* `  DPad _pDPad` --> Maps to WSAD, or a controller DPAD
	* `class KeyboardController : Controller` --> the Joystick control would be driven by the mouse
	* `class GameController : Controller`

## World25 Integration
- [ ] Implement GodCam as a script.
- [ ] PhysicsWorld is now referenced in World25, this lets us turn BottleRoom into a script run on Scene.
- [ ] Make Atlas's "gsiz" paramater automatic.  Meaning, allow the atlas to automatically pack itself (like MegaTex does). Essentially turn Atlas into MegaTex.

- [ ] FIRST implement World25 INTO physicsworld directly, 
	- [ ] Compile, Run.. test
		 - [ ] Test model loading
		 - [ ] Test glob rendering
- [ ] Separate PhysicsWorld physics & collision routines into GameWorld & PhysicsManager
	* GameWorld responsible for the BVH hierarchy, globs, terrain creation, and multithreading the creation.
	* PhysicsManager responsible for collision detection and response (which may not be needed if we're using the envisioned game design).
	- [ ] Scene->GameWorld  Scene->PhysicsManager
		* PhysicsManager->getScene()->getGameWorld->getGlobs.
		* Or add the globs to the physicsManager as nodes..
- [ ] Replace BottleUtils constants with PhysicsWorldCreate paramater structure.
	- [ ] Reference this structure everywhere instead of BottleUtils::
- [ ] move "getcellorobjectunderray" straight back to scene

## Engine & Code Cleanup 

- [ ] Replace name in headers
- [ ] Combine Gu into ApplicationPackage.  They are basically the same thing. (reorg) 
- [ ] See if we can make ParticleManager part of PhysicsWorld (reorg)
- [ ] Remove Graphics and Config defines in Gu (reorg)
- [x] Remove AppMain, and AppBase, and Package (reorg)
	- [x] Merge Appbase & Scene
	- [x] Remove AppBase
	- [x] Add ApplicationPackage back.
	- [x] Create new Scene() 
	- [x] Attach Scene to GraphicsWindow.	
- [ ] move to /gfx  (reorg)
	- [ ] graphicswindow
	- [ ] glcontext
- [ ] Scene -> PhysicsWorld integration (reorg)
	* PhysicsWorld is *basically* Scene, but with extra stuff.  Their data are the same (nodes).
	- [x] Move all object creation to Scene
	- [ ] Move ALL drawing to Scene
	- [ ] Link PhysicsWorld and Scene objs (somehow), hopefully by NOT DUPLICATING DATA between PhysicsWorld and Scene
		* Pass Scene, into PhysicsWorld and LightManager to manage the objects.
	- [ ] Do the same with LightManager.

## Sprite Management

- [ ] SpriteBucket needs to be more of a global class.  Like SpriteManager.

## Models

- [ ] Test loading a model with the old system.

- [ ] **CRITICAL** Squashed viewport Bug. Fix Viewport scaling issue.
	- [ ] Fix UI text squashing bug.
	- [x] Implement the updated viewport class from the engine.
		- [x] Make sure it has width/height defined.
	- [x] Update everywhere where viewport is referenced.
	- [ ] Fixed render squishing by boxing the render viewport in windowed mode.
	- [ ] In fullscreen mode, force the w/h to be the screen.
	- [ ] Fixed the squashed UI image problems.
		- [ ] Fix SQUASHED text rendering.  Text must be rendered at the same w/h ratio no matter window size.

## Bottle World

- [ ] CSharp inline *minimal* scripts.
	* This should fix the camera, so we can easily add the bottle topologizer.
	- [x] Update FlyCameraControls (flycam) from previous release and use CSharpScript to control it.
	- [x] Test the c# lexer.
	- [ ] Implement a parser
	- [ ] Finish the C# class-based scripting implementation.
	- [ ] Test a script.
	- [ ] Update FlyingCameraControls so that it runs in the new script-component system.
		- [ ] Make Cameras work with the Scene() and get their update() methods called
		- [ ] This would make the flying controls run as a script IN the node as a component instead of AROUND
			*Currently FlyingCameraControls run on top of the camera 

- [ ] Add Bottle World
	- [ ] Add the foundation (globs, awareness)
	- [ ] Test and render the Bottle Globs.
- [ ] Move object creation in PhysicsManager to Scene.
	- [ ] Compile, Run.

	
## Bugs

- [ ] Figure out some way to prevent recursive node updating for SceneNode, this has been causing MULTILPE issues.


# Future Phases (May be out of date)

## Scene System

* The goal here is to create a more 'common' game engine architecture, similar to Blender and Unity design.  We want to be able to quickly add nodes
to scenes and reference their shared `data` components via instancing.  Our current "Spec" system for creating node clones is sloppy, and doesn't make sense as a lot of the "spec" data are not resource intensive
and don not need to be shared.  Secondly, we want to have a component-based system, where we may have multiple 'nodes' per WorldObject (GameObject in Unity). We
will have only 4 kinds of nodes then:
	* SceneNode (base, inherits TreeNode)
	* -> CameraNode
	* -> Scene
	* -> LightNode (Point and Directional)
	* -> WorldNode (A Glob in the game world)
The system will be rewritten in 3 areas:
	- [x] Remove Global Camera (Gu::getCamera())  Pass it around to rendering functions.
	- [ ] Remove all Data classes, besides MeshData.  MeshData will be referred to as a "slot" in the "MeshComponent"
		* Add NodeData to SceneNode, not copying it.
	- [ ] Implement `clone()` and `copy()` on SceneNode
	- [ ] Remove `serialize()` and `deserialize()` from all *Data's and place them on SceneNode and all Sub-Nodes.
	- [ ] Add a generic component class, which is subclassed by
		* CSharpScript (ScriptComponent)
		* Mesh (MeshComponent)
	- [x] Integration of "PhysicsNode" with the nodes themselves. (it makes no sense do deprive a node of velocity).
		* Remove Velocity from "PhysicsNode" and place the Velocity on SceneNode.
		* Create a ColliderComponent
		* Place Mass, Shape, and all other PhysicsNode properties on the ColliderComponent
		* Subclass ColliderComponent into SphereComponent, and HullComponent.
			* We will only have 2 colliders for this game, there doesn't need to be more (yet).

## Model system

- [ ] Model Loading
	- [ ] Change ModelCache to ModLoader
	- [ ] Move ModelCache's model load methods into ApplicationPackage (reorg)
- [ ] Implement GLTF file loader. (Replace MBI files with GLTF binary)
	- [ ] Test, by using a GLTF model from Blender.
- [ ] Remove WorldObject inheritence, and use Component model.
- [x] Rename BaseNode -> SceneNode
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


## Mine City Phase I

- [ ] UI System tasks
	- [ ] Simplify the UI to work with the UI design for this game.  (Which should update UI performance)\
	- [ ] Implement hard coded x/y locations.  
	- [ ] Remove percentages.
	- [ ] Asynchronous UI layout routine.

- [ ] **Story**: Create a Peon.	
	- [ ] Create a Peon Model in Blender, with at least 1 Animation
	- [ ] Load the Peon model from a GLTF file, created in Blender.
	- [ ] Associate at least one animation with the Peon Model
	- [ ] Have the Peon wander around the tiles in the game.
	- [ ] Make sure rendering is not slowed down by Peon.
		
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


# Backlog

## Engine Enhancements
- [ ] CMake integration. Test on iOS, Linux, Android.
- [ ] Instanced Rendering. Merge all the uniform buffers, skin joint buffers. Reference by gl_InstanceID. (see PhysicsManager)
- [ ] Move window update logic from AppRunner to GraphicsWindow so they can run async.
- [ ] Remove SDLGLIncludes from headers.  (It includes Windows.h)
- [ ] Further optimize engine building by implementing PIMPL in headers with big include files (such as SDL).
- [ ] Support line and point in UtilMesh, and remove glPolygonMode everywhere (also glLineWidth).
- [ ] Replace context->gl.. with GL.  This will be a static interface.  It allows us to remove all the proc junk from GLContext.
- [ ] For proper code page conversion, change all the following to use wint_t instead of "char"
	- [ ] remove t_char
	- [ ] TextParser
	- [ ] StringUtil
	- [ ] DiskFile
	- [ ] remove t_byte SHOULD keep a 'byte' type though.  Make good decisions here.

## Delayed / Shelved 
- [ ] Multiple Window Rendering
	* This isn't necessary for the purpose of completing the game.
	- [ ] ~~Hard code GL context. Remove GraphicsContext~~
	- [ ] ~~Add GL Context to all required Classes~~
	- [ ] ~~Compile, Run.~~
- [x] ~~Unlink the Window viewport with the Camera Viewport.~~
	* Although 'logical' this is not possible with modern GPUs, as we'd end up having to create separate RenderPipe's per Camera, since, the Viewport Width/Height determines the RenderPipe Width/Height. *Nor* are framebuffers shared across GL contexts, *and* we are running these contexts asynchronously. Each renderpipe uses more than 8 1080p surfaces.  Obviously, we'd end up with memory or performance issues.  The solution, is to share camera viewport.  Maybe, in the future, when GPUs can handle such intensity.
- [ ] Combine RenderSettings with EngineConfig.  Puts all engine settings in one place.

## Wishlist (TODO tasks, not directly relevant to project)
- [ ] Integrate SDL_Touch with the engine, making 
- [ ] Instead of hard code vertex interleaved formats, allow us to supply multiple buffers for the vertex components (n,v,t..) for each component
- [ ] Move all STL header files DOWN into the respective CPP files that need them. This will speed-up compilation time a lot.
- [ ] detach Camera viewport from Window viewport so the camera can render independently (offscreen) of whichever window it's rendering to.  This would mean creating a CameraViewport class, and passing w/h into the camera.
- [ ] All strings use the same hash function which may cause collisions.  Integrate collision handling function in HashMap and in all places where STRHASH is used.
- [ ] Keyframe Bezier curve handles, and bezier interpolation (see KeyFrame)
- [ ] Remove WorldObject class inheritance, and favor composition like Unity does.  Managers will hold onto components.
	- [ ] WorldObject will be composited by other items and not inherited.
- [ ] Vuklan support.  This is "half" implemented.  ALL GP systems need to be subclassed.  This is a very big effort.

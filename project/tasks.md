# Mine City Devlog

## Tasks
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
	- [ ] Compile, Run.
- [ ] File Additions, and Replacements (from broken version)	
	- [ ] Replace OperatingSystem file
	- [ ] Replace BuildConfig
	- [ ] Add Scene
	- [ ] Implement Package to -> ApplicationPackage 
	- [ ] Rename Fingers -> InputManager
	- [ ] Rename Party -> ParticleManager
	- [ ] Rename ObjectFile to ObjFile
	- [ ] Remove App
	- [ ] Compile, Run.
- [ ] Engine Rename - Rename engine to BR2
	- [ ] Rename "bro.." to "br.." in...
		- [ ] Exceptions
		- [ ] math
		- [ ] Compile, Run.
	- [ ] Rename ObjectFile to ObjFile
	- [ ] Compile, Run.
- [ ] Move object creation in PhysicsManager to Scene.
	- [ ] Compile, Run.
- [ ] Multiple Window Rendering
	- [ ] Remove Vulkan
	- [ ] Hard code GL context. Remove GraphicsContext
	- [ ] Add GL Context to all required Classes
	- [ ] Compile, Run.
- [ ] Squashed viewport Bug
	- [ ] Fix SQUASHED text rendering.  Text must be rendered at the same w/h ratio no matter window size.
- [ ] CSharp inline *minimal* scripts.
	- [ ] Update FlyCameraControls (flycam) from previous release and use CSharpScript to control it.
- [ ] Simplifying the UI to work with the UI design for this game.  Updating UI performance.
- [ ] Data Class Separation. Replace Node/Spec with a clone() and copy() system.
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
- [ ] Remove WorldObject inheritence, and use Component model.
- [ ] CMake integration. Test on iOS, Linux, Android.
- [ ] Instanced Rendering. Merge all the uniform buffers, skin joint buffers. Reference by gl_InstanceID. (see PhysicsManager)
- [ ] Move window update logic from AppRunner to GraphicsWindow so they can run async.
- [ ] Componentize these:
    - [ ] Remove "Model" class, and put it in a logical place, such as on the Armature, or as a "component"
	- [ ] *Dynamic skinning* where, mesh skin is a *component* and their *skin* is a separate component on WorldObject that points to the given mesh. We should copy Blender's data format.  Armature is a child of the object.

- [ ] Combine RenderSettings with EngineConfig.  Puts all engine settings in one place.

- [x] ~~Unlink the Window viewport with the Camera Viewport.~~
	* Although this is 'logical', this is not possible with modern GPUs, as we'd end up having to create separate RenderPipe's per Camera, since, the Viewport Width/Height determines the RenderPipe Width/Height. *Nor* are framebuffers shared across GL contexts, *and* we are running these contexts asynchronously. Each renderpipe uses more than 8 1080p surfaces.  Obviously, we'd end up with memory or performance issues.  The solution, is to share camera viewport.  Maybe, in the future, when GPUs can handle such intensity.
#### Wishlist (TODO future tasks)
- [ ] Instead of hard code vertex interleaved formats, allow us to supply multiple buffers for the vertex components (n,v,t..)
- [ ] Move all STL header files DOWN into the respective CPP files that need them. This will speed-up compilation time a lot.
- [ ] detach Camera viewport from Window viewport so the camera can render independently (offscreen) of whichever window it's rendering to.  This would mean creating a CameraViewport class, and passing w/h into the camera.
- [ ] All strings use the same hash function which may cause collisions.  Integrate collision handling function in HashMap and in all places where STRHASH is used.
- [ ] Keyframe Bezier curve handles, and bezier interpolation (see KeyFrame)
- [ ] Remove WorldObject class inheritance, and favor composition like Unity does.  Managers will hold onto components.
	- [ ] WorldObject will be composited by other items and not inherited.


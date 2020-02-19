## TODO list
* TODO - Instead of hard code vertex interleaved formats, allow us to supply multiple buffers for the vertex components (n,v,t..)
* TODO - Move all STL header files DOWN into the respective CPP files that need them. This will speed-up compilation time a lot.
* TODO - detach Camera viewport from Window viewport so the camera can render independently (offscreen) of whichever window it's rendering to.  This would mean creating a CameraViewport class, and passing w/h into the camera.
* TODO - All strings use the same hash function which may cause collisions.  Integrate collision handling function in HashMap and in all places where STRHASH is used.

## Phase I Roadmap
* I. Remove Spec System, use "data" system.
* II. CSharp inline *minimal* scripts.
* III. Remove WorldObject inheritence, and use Component model.
* IV. CMake integration. Test on iOS, Linux, Android.
* V. Instanced Rendering. Merge all the uniform buffers, skin joint buffers. Reference by gl_InstanceID. (see PhysicsManager)
* VI. Keyframe Bezier curve handles, and bezier interpolation (see KeyFrame)

## Currently Working On..
1. ~~Unlink the Window viewport with the Camera Viewport.~~
	* Although this is 'logical', this is not possible with modern GPUs, as we'd end up having to create separate RenderPipe's per Camera, since, the Viewport Width/Height determines the RenderPipe Width/Height. *Nor* are framebuffers shared across GL contexts, *and* we are running these contexts asynchronously. Each renderpipe uses more than 8 1080p surfaces.  Obviously, we'd end up with memory or performance issues.  The solution, is to share camera viewport.  Maybe, in the future, when GPUs can handle such intensity.
2. Data Class Sesparation
	* Implement clone() on nodes.
	* Move complex methods from all *data* classes to their respective Node classes.
	* Remove inheritence from data classes.
4. Remove WorldObject class inheritance, and favor composition like Unity does.  Managers will hold onto components.
	* WorldObject will be composited by other items and not inherited.
5. Simplifying the UI to work with the UI design for this game.  Updating UI performance.
6. Move window update logic from AppRunner to GraphicsWindow so they can run async.
7. Much of PhysicsManager must be moved to Scene, Object Creation.

*2/18/2020*
*Although we resolved to pull the old (working) version of BR2, it wouldn't make sense to go back to this change.  
	* GL Contexts *Must* be separate from Gu::getContext().  Since, to render multiple OpenGL windows Asynchronously, you need to have
	multiple contexts active at one time (wglMakeCurrent called on either of them).
* Removed inheritance for Node data classes.  Instead, we're going to use multiple data classes on each inherited node class.  This way, we can share multiple
data if we need to.
* Created the NamedData serializable 
* _Not_ creating initial class datas, per design.  This is to decrease deserialization time, and to prevent unwanted garbage data from becoming confusing.
* Data classes shouldn't have methods.  Our new goal is to clone objects using the data as a shared "user" system.
	* Separating the data classes is going to be a huge hurdle.
	* Data blocks should be almost method-less.
* It appears we must move serialization off of the Data classes, and move it onto the Node class. this appears to make sense.  This would also mean, we need
to make the node classes public.

* *Rationale Behind the new system*
* This is getting really destructive; the system isn't fully implemented yet, and still fails to compile.
* So let's say I have 2 Link models.
	* Each piece of the model is a node tree which has a name, matrix, box, etc.
	* Then there are armature, and bones, mesh, and "model".
		* Essentially "model" is unnecessary.  I just added it for convenience when creating the object.  It's essentially what's now WorldObject.
			* So really, instead of "ModelNode" we should place the animation operations as a WorldObject Component, or at least as part of armature.
			* Essentially, even meshes can have all kinds of animation. The "skin" becomes a modifier, as in most modelling systems.
	* So we want 2 link models. and then we want a link model where we can morph and deform.
	* First, we create an instance of the link model.
		* To do this, we need to clone the object, but share the data blocks.
			* so we create a shallow copy.
				* `std::shared_ptr<WorldObject> _pLinkObj = x;`
				* `_pLinkObj->copy(bool is_deep=false)`
					* `copy all data..`
					* `if(isdeepcopy){`
					* `	 _pModelData = _pModelData.clone(); // This is how we make a deep copy.`
					* `}`
					* `//NOTE* the NAME of the model must not match the previous, meaning, NodeData must not be shared in this case.`
					* SO WE MUST NOT HAVE NODEDATA, INSTEAD, MOVE ALL OF NODEDATA to SceneNode!
		* Now, we want our other Link model to modify, and create clones of.  So we make a _deep copy_.
			* As above, specify deep.  This will clone the data.
			* For objects to animate independently we need separate animation buffers.  This is already covered.git ba

*2/17/2020*

* We resolved to pull the old verison of BR2, as this has become intractible.
* Removed error checking from Gu.  Instead, error checking is done via the context: both GPU errors, and Operating system errors.

*2/16/2020*

* Renamed BaseNode to SceneNode (could also be SceneGraphNode, but Scenenode is more compact).

* II. part 2 Removal of Spec
* Removing init() from the node classes to make node construction a simple new-ing of the class, also removing static create() method, likewise.
* The goal here is to have a data segment for each node, instead of one data segment.  This gives us the ability to clone variuos data segments instead of 
requiring the entire data segment to be cloned.

*So here is a design dilemma of II.  We want to do things such as, reference ONLY a mesh's vertex data.  Or we want to reference ONLY this armature data.  But to do that
we need to separate the data classes into individual components.  Yet, the problem is a lot of the data components are shared with NodeData.  So it makes sense to inherit
the data.  We currently have a single NodeData that comprises all of the data for any node.  The only option I can think of is to simply duplicate this node data across each
respective node class in the hierarchy.  This makes no sense either, since you're duplciating the inherited information (name, invbind..).

*PhysicsData should be separate from ModelData.  Physics will only involve colliders now.

* We can have it like this:
	all *Node data's inherit from NodeData'
	MeshData is it's own respective class. no inheritance
	MaterialData is its own respective class, no inheritance.

* Commented out particlemaker, for later use.

*2/15/2020*
* Moved Model Rendering from PhysicsManager to Scene
* Moved Physics node creation to Scene.
* Removed BaseSpec, PhysicsSpec (part of (2))
    * We must remove all BaseSpec inheritence, and replace it with a new class member.
* By removing MeshData as a requirement for Meshnode, we create "empties" like most rendering engines, we can create an empty object without any mesh data.

*2/14/2020*

* Remove viewport from window.  Now, we will only use the camera viewport to update the clip region.
* Viewports are only on cameras now.  We now use 1 viewport per camera, since we may, want to render certain regions with different width/height settings.
* So the only issue here, is when we want fullscreen mode, or we need to place the viewport in the window, so the viewport needs the window's width, height, or both.
* The viewport may need window height
* Viewport can be: 
	Width & Height of window - this is the same as "fullscreen" regardless of whether the window is actually covering the screen.
* Change viewport's update to bind().  We bind() the viewport before rendering, in RenderPipe.
* Camera also needs access to screen buffers so, when window resizes we may also resize screen buffers, but since window is depenent on camera w/h so the
RenderPipeline will also be dependent on camera w/h
* Remove Window reference from RenderPipeline
* Fix the GraphisWindow:updateWidthheight
```
Camera->bind();
...viewport->bind();
scene->render();
```

* So here is the issue, we can only have 1 renderpipe per resolution.  We can't create a new renderpipe for each camera.
Either way, it appears you have to share the screen dimensions with the camera.  In the least, you need to share the RenderPipe.
If you didn't you'd end up creating more than 1 renderpipe.  Graphics isn't quite there yet, where we can establish
30 render surafaces *per camera*.  On the other hand, you'd have to resize the buffers each time you swap cameras.  Neither solution is optimal, so we must use
the same dimensions per camera.  I guess, one solution might be to use sub-areas of buffers.  This is sort of outside the scope of the GDD in the first place.
If you notice, Blender fixes all cameras to use the same dims, scales, and resolution, since there can only be one rendering pipeline.  It's not that it is logical, it's that
the logical solution is resource intensive.

*2/13/2020*

* Separating camera and window viewport.
	* The only reason camera uses the window viewport, is to update it.
	* The only reason frustum uses viewport is to calculate the frustum geometry, which can be calculated simply by passing the camera FOV, Near/far.
	* We must also consider that the game's camera aspect ratio is way off.  This is partially due to the window/camera viewport dependency.
	* Frustum can be a class off of viewport.
	* Or an independent class (probably better this way)
	* Frustums also used for shadow boxes.
* glViewport needs to be tied to the RenderPipe, not renderviewpoert
* Add pixelaspectX, pixelaspectY for non-square pixels.
* Must Fix the pixel aspect (squashing)
* Resolution options - Automatic, then a set of adjustable resolutions.  
	* Fullscreen- width + height are the height of the main display
	* Windowed 
	* - * Height Adjustment - the height resolution is set to the height of the window.Viewport is placed in the window Center if width does not equal window
	* 1920 x 1080
** Inherit camera with FlyCam, or use a script?
	Unity uses a script to control camera.  I'm not sure what to do here.  A script would be neat though.
* Created basic C# scripting, and moved the FlyCam controls into a script.

*2/12/2020*

* New file structure looks like this.
* /My Documents
    * /BR2
        * config.xml
            * /projects
            * /default
                * /cache
                * package.xml

* Removed the app package's cache, and config.  these should really be hard coded.
* Fully Replaced AppMain with Scene().
* Got a majority of the replaced code to compile.
* Unfortunately, OpenGL contexts require a Window in SDL.  This is similar functionality with Windows OpenGL which requires a DC.  A good reason, if any, to switch to Vulkan.
* Fully reimplemented project paths in the ApplicationPackage.
* Started a StringWrapper class, but abandoned.  *NOTE* StringWrapper was needed for hash algorithms. Without it, we're flying blind by possible hash collisions in _HashMap_. 
* Renamed engine, and namespace to "BR2" 

*2/11/2020*

* Removed "app"
* Moved contextual managers back into GLContext.
* Removed generic "GraphicsContext" in favor of using GLContext (sorry, vulkan, maybe later).
* Moved the app's filesystem directories into Package.
* Moved more stuff around.
* Renamed Package to ApplicationPackage.  It's an application, but it's also 'packed' so settled on AppPackage.

*2/7/2020*

* Scene
GLContext
	Shaders, texture, models, renderpipe, particle manager, PhysicsWorld (as its gpu accellerated), picker (uses buffers)

*2/6/2020*
* Use Screen to represent a UI window.
* Move Gui2d to Gu
* Undo changes.  Gui2d is actually Screen.
* Rename Gui2d to screen
* add Screen(s) to a new Gui2d class.

* TabButton - the basic of the UI
* TODO Make the ui text not shrink vertically based on window size.  
* TODO make the window not squash horizontally (keep the horizontal ratio)

* We need to make the render system "windows" more modular.  Like, we need to create multiple windows if needed.

* WindowManager
    * Get rid of "OpenGL Api" we don't need that.. right.

* Moved Window control to applciation (not in the main core)
* Removed Mob to Bin converter (apprunner.cpp)
* Removed Cycle Logging in Logger.cpp


*12/29/19*

* Began simple port to Make and CMake with gcc.  Fix this in the future.  The one caveat here is still debugging with VS. 
* VS allows a CMake project, but it doesn't appear as lightweight as a simple makefile.

* Previously we removed the custom app class instantiation in preparation for 
	1 - a scripting engine 
	2 - Multiple graphics windows 
	3 - Split window view (blender)

* We converted Shake thus to its own EXE.  It's no longer a library.

*8/23/19*

* Remove most of AppBase and move to ProjectFile.
* Move remaining AppBase contents into ProjectFile
* Remove "AppBase" or make it ProjectBase. Sth.
* Generic Windowing for UI popups.
* move RenderPipe to Window
* Rendering multiple windows
* need multiple contexts
* shaders & Everything need to be changed...

*8/14/19*

In FlyCam make the FlyCam a Script Run on top of CameraNode
	Remove FlyCam Class.

*8/11/19*

* Rebooted "BR2" Graphics Engine Project
* Adding a Window layer,
* replacing Viewport with WindowViewport to differentiate between window and render viewport 
* this is in preparation for multiple viewports for the UI system.
* having a problem with teh file SDL.h because it conflicts.
* added <SDL> from a more absolute path.
* Todo: merge engine and apprunner
* For split view need
* Image Dropdown like blender
* remove "udim"



# Mine City Devlog

*3/4/20*

* Undid the previous changes and added /bottle/ back to the solution (better than reintegrating all of it).
* Commenting out the BottleRoom main class CPP.  Most of this will be moved to scene and such.

* We will make PhysicsWorld be referenced by World25 instead of being inherited.
* Renamed BottleRoom -> BottleScript.
	* Scene will still create the base PhysicsManager
* Replaced many math functions with STL math functions in MathHeader using a define flag to turn them on/off
	* TODO: Test and make sure the STL matches what was there before.

*3/3/20*

* Round 2 of Bottle migration
	* Renamed ModelNode -> SceneNode for the combination of SceneNode and Model
	* Renamed ModelSpec -> SceneNodeSpec
		* Model node wasn't a needed structure, now we have a reason to have it.
		* TODO: make Armature, and mesh Components of the WorldObject
		* TODO: Remove WorldObj typedef.  This is a shortcut for the integraiton.
* We need to do away with the spec system, it adds extra overhead like simple node copying or bound box computation.
* I believe modelcache is necessary as we want to be able to preload models without rendering them (then again, we could just add them to the scene and hide them, but that feels sloppy).
* renamed getAssetsFolder -> getDataFolder

* Moved all game Creation to WorldMaker

*3/2/20*

* Implemented async logging.
	* Async logging decreases startup time by about 2 seconds, vs 4.
	* Async logging may cause loss of log messages when app fails critically, so you can disable it.
	* Essentially, it is only necessary for seriously needed performance gains.
	* If anything, async logging lets us halt the console window and let the app keep running.
* Removing console colors from the base header appears to have decreased compile time somewhat
* Added log_async=true to allow async logging in SERIOUS PERFORMANCE CASES ONLY.
	* We don't want async logging by default, could make errors difficult.
* --
* Added createNode back to PhysicsWorld from World25
* Added WorldMaker, GameFile.
* Moved many /bottle/ classes to /world/.  
* The system does not compile due to some architecture changes we need to make.  
	* It is likely better we split PhysicsWorld and World25 as their concerns are separated 
		* World25 - Concerned with efficiently generating a world mesh toplology, generating its structure, and managing the textures (grass,dirt..) that go with it.
		* PhysicsWorld - Concerned with colliding physical objects in the game world with themselves, and the BlockNodes.

*3/1/20*

* Fixed StringUtil's isDelim.
* Added a simple, silly bool unit test lambdas to the main method so we can easily drop a unit test in main() if needed.
* Tested the lexer and fixed one bug.  Lexer appears to work for simple CS scripts.
* Had context issues, so worked on creating valid contexts.
* Added forward compatible boolean to context.
* Fixed various GL compatibility issues.
* Tested on laptop and found support for 16-bit depth only.
* Made the context creation support a lot more contexts with more verbose context creation logging.
* **By default, we enabled forward compatibility.  It is supposed to imporove performance.**
* Fixed issues with core context methods getting called as well.
	* Note if enabling forward compat or core context, we curently don't support rendering line or point in UtilMesh.
* Fixed depth buffer problems.
* Fixed context creation for all subsystems.
	* The problem with context creation is that we needed to destroy an SDL window before attempting to create a new context.


*2/27/20*

* Deleted & removed AppBase.
* Deleted & removed Package.
* Deleted & removed AppMain.
* Added ApplicationPackage to replace all project directory methods in AppBase & AppMain.
* Moved Delta to GraphicsWindow.
* Moved FpsMEter to GraphicsWindow.
* Moved FrameSync to GraphicsWindow.
* Moving these is important since each window has its own frame rate (thread).
* Replaced XmlFile with old version.
* Moved Gu::getFrameNuamber to GraphicsWindow.
* Updated Global init with new cache directory method.
* Moved ParticleManager to Scene
* Internalized ApplicationPackage and made PackageConfiguration only in that file (allows us to exclude HashMap from XmlFile)
* Fixed HashMap, moved from the broken verion
* Deleted Script
* Changed /data/ folders to be full names: tex->textures, shr->shaders, fnt->fonts
* commit 2
* Added update routine to components (conseqauentially flying camera).
* commit 3
* Started brushing up the CSharpScript.
	* Removed scriptmanager it isn't needed due to the flexibility of the component system.
	* We are making a Quick and dirty c sharp script class that does basic stuff.
	* Scirpts will use class definitions and method definitions.
	* `CSharpClass ("Int32",{"int" /*Type Aliases*/}, { /*Methods*/`
	* `CSharpMethod({}), CSharpMethod({})`
	* `)`
	* compile() will be used on CSharpLanguage
	* execute() will be on CSharpRuntime
	* The recursive parse solution probably isn't the best approach since we want to see multiple compile errors, even if we hit one. So we need more of a whole-file approach.

*2/26/20*

* Refactored graphics api to "make more sense" giving us the ability to create multiple windows.
* Also fixed the Opengl Context creation issue.  Contexts now create OpenGL Windows
* Updated and fixed some Vulkan window creation routines
* Moved application update logic to GraphicsContext.
* Rendering works for screen images, and some UI features.
* Updated some of RenderPipe (from broken RenderPipe)
	* Updated Virtual Draw Methods to fit new RenderPipe method
* Unfortunately we can't move GraphicsWindow creation down and 'fully modularize' it.  The reason is that we MUST have a graphics context when we initialize the global managers, however,
in SDL, you must have a SDL_window to initialize the graphics context (since all contexts require surfaces).  So that means that we need to separate the *initial* window creation with its
render system initialization.

*2/25/20*

* WindowViewport (old) -> RenderViewport
	* Note many places we changed the viewport updateChanged to bind() which may impact rendering.
* removed flycam from graphicscontext (in place of scripted cameracontrols in Scene)
* Officially moving lightmanager to Scene
* Added RenderTarget class, Renamed RenderTarget to BufferRenderTarget, GraphicsWindow and BufferRenderTarget now share RenderTarget base class.
* Reimplemented RenderPipe to use the owning window context.
	* Added scene & camera passed in (no hard code scene). 
	* **We're going to need to update the Context & Window initialization code to set the window context**
		* **DONT FORGET TO SET WINDOW CONTEXT**
	* All Framebuffers, including RenderPipe, share the same context.  This is good for multiple context rendering, as framebuffers are the only things we use
		that can't share context.

* Removed ScriptManager
	* We do not need a script 'manager' since all scripts are executed when we update the objects. Scripts are components on objects.
* Script Usage & Design
	* `std::shared_ptr<CSharpScript> sc = CSharpCompiler::loadAndCompile(Gu::getPackage()->getScript("MyScript.cs"));`
	* `myNode->addComponent(sc);`
* Removed Gu::getCamera() - Camera is located relative to Scene and must use Scene itself.
* Added getActiveCamera to GLContext. 
	* This works because A Window is associated with ONE context.  A window is associated with ONE scene. A scene has ONE active camera.
* Added getCamera to RenderParams()
* Adding camera to UtilMesh and All UtilMesh's'
* Subclassed UtilMesh with GLFramework.

* Replaced ShadowFrustum::update with ::cull
* Created CullParams

LightNode updates frustum
	We need a camera with the update function in order to correctly cull all objects from the frustum.
	Another option, would be to NOT perform culling during update() phase.
	LightManager->updateShadows():

shadow frustums require the camera to cull, but we don't pass that in during node update()

We should probably cull shadow frustums asynchronously.

Cull camera nodes first.
camera->cullNodes
	Of course if the shadow casts over the camera, the node may not be in it, so we would still need them.
	We could have a max_shadow_distance where the node is a certain distance from the camera and we won't cast shadows if it is'
foreach light
	light_frustum->cullNodes(PhysicsWorld w) -> std::future<shard_ptr<RenderBucket>>


The idea is to do this stuff asynchronously.
Scene
	node->update()
	node->updateAnimation
PhysicsManager
	node->updatePhysics
LightManager
	node->updateLight()
		shadowFrustum->collect
			Physics->collectVisibleNodes **Slow

Desired Shadow Culling
bool future [light]
For each light
	std::async{
		collectnodes(light->frustum)
		future.set[light]
	}
	fence(future.lights = set)

*2/24/2020*

* Task list getting too large, so moved to new file /project/tasks.md.
* The "new" BR2
	* I don't see a reason to forcibly remove Vulkan like we did last time.
	* Refactoring the whole engine for multiple GL contexts isn't necessary right now.  We don't need it. In the future, we might.
* Removed Global Include Headers
* Reduced Compile time by about 3 minutes.
* Replaced t_stirng with string_t
* Replace OPeratingSystem with the broken build OperatingSystem
* Replaced BuildConfig
* Compile, Tested.
* Renamed Particle Manager, Input Manager.  Refactored Gu.
* Note: We are not using WorldObject per design, we're using SceneNode - which will be the main "WQorldObject" for the game.
* Added BaseCollider - for the collider components 
* Added Component
* Added Scene

*2/23/2020*

* Reset BR2 to the LKG.  We lost a lot of work... but sometimes this happens.
* Recoded all file headers.
* Removed /base and /baseAll
* Compile, Tested.

*2/21/2020*

* Commenting items out for the rebuild with **PHASE1**
* Moved GraphicsWindow, GLContext and RenderBucket to /gfx
* Removed Base.h and BaseAll.h
* Removing WorldObject, it's not necessary.  We only need SceneObject. See Design
	* I like "WorldObject" namme. We can always rename SceneNode in the future.  For now it's SceneNode.
* Changed MeshNode to MeshComponent
* Most, if not all nodes need a physics capability.  It makes sense to inherit SceneNode FROM physicsnode.  Yet, the reason we separate them, is because
we want to optimize the physics routine.  Not hard, since we cxan simply separate ones that have no colldier on them.  Yet, anyway SCENE is really the
same thing as PhysicsManager.  I just separated the collision handling code.
	* Decision: combine physicsNode with SceneNode
		* why? all nodes have a velocity, and thsu should get their positions updated.
		* why not?  Because not all nodes have friction, etc.
		I think physicsnode can be a part of the tree, rather than a component.
		You can have volume hierarchies.  Yet, those are usually computed not added manually.  So if there's a hierarchy we're not going to be editing it.
		so yes volumes should be components.

		So here's the plan.
		put Velocity on SceneNode
		Put the rest of thee PhysicsNode properties on Collider'

*2/19/2020*

* We need to serialize all data in one way, as nodes reference data.  
* All data needs unique identifiers.  (possibly, name).
* Removed BoneData
* Removed ArmatureData
* Commenting out the model system for right now, until we get the rest of the changes squared away.
* Deleted ModelCache.  ModelCache is no longer needed.  To access models, we're simply going to clone them from those in the scene.'

** Commented out RenderPipe!!  We removed this in order to prevent compilation of the scene system.

*2/18/2020*

* Although we resolved to pull the old (working) version of BR2, it wouldn't make sense to go back to this change. GL Contexts *Must* be separate from Gu::getContext().  Since, to render multiple OpenGL windows Asynchronously, you need to have multiple contexts active at one time (wglMakeCurrent called on either of them).
* Removed inheritance for Node data classes.  Instead, we're going to use multiple data classes on each inherited node class.  This way, we can share multiple data if we need to.
* Created the NamedData serializable. Then removed it.  It's nonsensical as we're removing serialization and inheritence from data classes.
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
* Viewport can be: Width & Height of window - this is the same as "fullscreen" regardless of whether the window is actually covering the screen.
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


## TODO list
* TODO - Instead of hard code vertex interleaved formats, allow us to supply multiple buffers for the vertex components (n,v,t..)
* TODO - Move all STL header files DOWN into the respective CPP files that need them. This will speed-up compilation time a lot.
* TODO - detach Camera viewport from Window viewport so the camera can render independently (offscreen) of whichever window it's rendering to.  This would mean creating a CameraViewport class, and passing w/h into the camera.

## Currently Working On..
1. Essentially our MeshNode/MeshSpec, should really be one class.  to create new meshes, we'd do a shallow copy of the class.
2. The same goes for all the Spec/Data model classes.
3. Additionally, we need to remove class inheritance, and favor composition like Unity does.
4. WorldObject will be composited by other items and not inherited.
5. Adding "SCENE" to the world.  
6. Simplifying the UI to work with the UI design for this game.
7. Camera needs to have a separate viewport from the window.
8. Replacing AppMain() with Scene()
9. Graphics Context - So, we can have a ContextManager, which has a STATIC graphics context in it.  When we call SwitchContext(ctx) the static is set.
	Then we can call global references to TexCache, and the like without having to get the context explicitly from the window (or something).
	*having context on Gu is simple, but things like RenderPipe need to have a specific context since they have their own data that isn't shared between contexts.
10. makeAssetPath - needs to be defined in the "project" or package level.

*2/11/2020*
Removed "app"
Moved contextual managers back into GLContext.
Removed generic "GraphicsContext" in favor of using GLContext (sorry, vulkan, maybe later).
Moved the app's filesystem directories into Package.
Moved more stuff around.
Renamed Package to ApplicationPackage.  It's an application, but it's also 'packed' so settled on AppPackage.

*2/7/2020*
Scene

GLContext
	Shaders, texture, models, renderpipe, particle manager, PhysicsWorld (as its gpu accellerated), picker (uses buffers)

*2/6/2020*
Use Screen to represent a UI window.
Move Gui2d to Gu
Undo changes.  Gui2d is actually Screen.
Rename Gui2d to screen
add Screen(s) to a new Gui2d class.

TabButton - the basic of the UI
TODO Make the ui text not shrink vertically based on window size.  
TODO make the window not squash horizontally (keep the horizontal ratio)

We need to make the render system "windows" more modular.  Like, we need to create multiple windows if needed.

*WindowManager
* Get rid of "OpenGL Api" we don't need that.. right.

Moved Window control to applciation (not in the main core)
Removed Mob to Bin converter (apprunner.cpp)
Removed Cycle Logging in Logger.cpp


*12/29/19*
Began simple port to Make and CMake with gcc.  Fix this in the future.  The one caveat here is still debugging with VS. 
VS allows a CMake project, but it doesn't appear as lightweight as a simple makefile.

**Previously we removed the custom app class instantiation in preparation for 
	1 - a scripting engine 
	2 - Multiple graphics windows 
	3 - Split window view (blender)

We converted Shake thus to its own EXE.  It's no longer a library.

*8/23/19*
Remove most of AppBase and move to ProjectFile.
Move remaining AppBase contents into ProjectFile
Remove "AppBase" or make it ProjectBase. Sth.
Generic Windowing for UI popups.
move RenderPipe to Window
Rendering multiple windows
need multiple contexts
shaders & Everything need to be changed...

# 8/14/19
In FlyCam make the FlyCam a Script Run on top of CameraNode
	Remove FlyCam Class.

*8/11/19*
Adding a Window layer,
replacing Viewport with WindowViewport to differentiate between window and render viewport 
this is in preparation for multiple viewports for the UI system.

having a problem with teh file SDL.h because it conflicts.
added <SDL> from a more absolute path.

Todo: merge engine and apprunner

Need:
	For split view need
	Image Dropdown like blender
	remove "udim"


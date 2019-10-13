# Vulkan Renderer
The repository I use for getting familiar with Vulkan (and computer graphics/3D maths in general)
Obviously very much in development (and far from being complete, if ever). Heavily based on the [vulkan tutorial](https://vulkan-tutorial.com),
as well as Sascha Willems' [examples](https://github.com/SaschaWillems/Vulkan).

Currently it is only displaying a single model with a texture and lighting cooked in. There are a lot of plans what I want to do with this,
and never enough time. :)

Depends on using C++17, assimp as model loader, Vulkan (and MoltenVK on OSX) obviously, and my [other repository](https://github.com/mdinim/Core.git) for some of the funcitonality
like file handling, logging etc.


Builds with CMake as a library, contains an executable target that uses the renderer.

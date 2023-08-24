# BJEngine

in progress

version 0
cube with lights

version 1
Added function to create background sphere and map (will be updated)
Created Log class 

version 1.1
Create SpotLight class. Added spotlight to the shader

version 1.1.1
The ability to create multiple light sources for a single object has been disabled.
Created a boolean constant buffer to determine the type of light object present.

version 1.1.2
Changed camera from first-person to free-look. Optimized memory usage in the Camera class.

version 1.1.3
The methods of the Render class: Close, Init, and Draw, are now virtual.

version 1.2
Added the Rasterize state to the Map class. Created the StaticModelOBJ class.

version 1.2.1
Added matrix-changing functions

version 1.2.2
Added functions to create Constant buffer, vertex buffer and index buffer.

version 1.3
Added class Model which use Assimp to load and draw scene (with normal map and texture)

version 1.3.1
fixed a bug with the lights and objects after changing their positions. Fixed a bug with loadind models without textures. (only obj models tested)

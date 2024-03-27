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

version 1.3.2
Added Multilighting (Max 5 lights) (Types: Spot or Point).

version 1.3.3
Added Frustum for models. Added the ability to create terrain from heightmap bitmap picture.

version 1.3.4
Recreated objects shaders. Added additional modifications to the models. 

version 1.4 
Added shadows from directional, spot and point light. ImGui was also integrated. Reshaping was added. Pixel shader was recreated. Added roughness texture map support

version 2.0
Mini engine =)

version 2.1
GUI and something new

version -1.9
Useless functions are deleted. Added deferred shading but with out light pre-pass. Fixed omnidirectional shadows

![изображение](https://github.com/JustBloodJoker/BJEngine/assets/113119625/20fbcd77-6a6d-4b70-9bae-6dfd3f6c6801)
![изображение](https://github.com/JustBloodJoker/BJEngine/assets/113119625/56d2e0f8-0c5c-413a-9328-859778a7c594)

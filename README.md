# Particles
Simulate millions of particles simultaneously

## Properties
* Allocates an OpenGL vertex buffer which is shared by OpenCL
* OpenCL kernels calculate particle position every frame
* No data is moved after initial setup (if energy is disabled), which gives significantly improved performance

## Performance
Computer specs:  
CPU: Ryzen 5 2600  
GPU: GTX 1060 6GB  

When calculating energy (particle lifetime), the buffer data has to be moved from the GPU to the CPU and back.  
Doing this every frame will cause large delays before the window can update.  
The table below shows the average framerate over a period of 30 seconds with and without energy calculation

|  Particles | Framerate | Frame Time | Energy? |
|:----------:|:---------:|:----------:|:-------:|
|    100 000 |    141.87 |     7.05ms |      no |
|    100 000 |   142.007 |     7.05ms |     yes |
|  1 000 000 |    142.29 |     7.02ms |      no |
|  1 000 000 |     58.36 |    17.13ms |     yes |
|  3 000 000 |    141.93 |     7.04ms |      no |
|  3 000 000 |     24.96 |    40.06ms |     yes |
|  5 000 000 |    101.83 |     9.81ms |      no |
| 10 000 000 |     55.23 |    18.10ms |      no |

On average, any particle count below 3 million yields the same performance.  

## Controls
* [M1] place gravitor
* [M2] place repulsor
* [WASD] scene rotation
* [P] toggle movement
* [C] toggle gravity
* [SCROLL] change zoom level
* [F+SCROLL] change hue
* [G+SCROLL] change saturation
* [H+SCROLL] change value
* [ESC] exit

## Features

## Libraries
The following libraries are required: 
* OpenCL
* GLFW
* GLAD
* GLM

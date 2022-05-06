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

The table below shows the average framerate for a given particle count.

|  Particles | Framerate |  Frametime | Energy? |
|:----------:|:---------:|:----------:|:-------:|
|    100 000 |   1370.69 |     0.73ms |      no |
|    100 000 |    359.85 |     2.78ms |     yes |
|  1 000 000 |    400.53 |     2.49ms |      no |
|  1 000 000 |     64.97 |    15.39ms |     yes |
|  3 000 000 |    155.32 |     6.44ms |      no |
|  3 000 000 |     26.87 |    37.22ms |     yes |
|  5 000 000 |     98.61 |    10.14ms |      no |
| 10 000 000 |     52.20 |    19.15ms |      no |

As you can see, moving data between the CPU and GPU causes a large performance loss, even for small amounts.  

## Controls
* [M1] place gravitor
* [M2] place repulsor
* [WASD] rotate scene
* [SCROLL] change zoom level
* [SCROLL+F] change hue
* [SCROLL+G] change saturation
* [SCROLL+H] change value
* [SCROLL+V] change background R
* [SCROLL+B] change background G
* [SCROLL+B] change background B
* [P] toggle movement
* [C] toggle gravity
* [ESC] exit

## Features

## Libraries
The following libraries are required: 
* OpenCL
* GLFW
* GLAD
* GLM

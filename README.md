# Software-Rasterizer

C++ Sofware Rasterizer using only standard libraries.  
Reads a single .obj model and a TGA image and writes the rendered image to a file.  
Loading of both file formats is rudimentary and only basic formats are supported.  
Apart from rasterization the rest of the Pipeline is implemented in a straighforward and unoptimized way. The math library is barebones and only implements what is needed.    
Rasterization is multithreaded by splitting the image into tiles, which are assigned to threads.  
The baryzentric coordinates are computed iteratively, as described in "A Parallel Algorithm for Polygon Rasterization".  
SIMD instructions are used to process four pixels at a time.

![](images/render.jpeg)
![](images/renderNormal.jpeg)

# Build instructions
Only tested on Windows. As only standard libraries are used the code should work on any C++ 17 compiler out of the box.  
Precompiled headers are setup to work with Visual Studio.  

The application relies on a "resources" folder. It has to contain the specified model and texture. The resulting image is written into the resource folder too. The folder is searched along the path of the working directory.  

Links to used [model](https://github.com/ssloy/tinyrenderer/blob/f6fecb7ad493264ecd15e230411bfb1cca539a12/obj/african_head.obj) and [texture](https://github.com/ssloy/tinyrenderer/raw/master/obj/african_head/african_head_diffuse.tga)  

# References
https://github.com/ssloy/tinyrenderer/wiki/Lesson-0:-getting-started  
https://www.scratchapixel.com/lessons/3d-basic-rendering/rasterization-practical-implementation  
"A Parallel Algorithm for Polygon Rasterization" Juan Pineda 1988  

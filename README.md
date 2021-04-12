# MyFragmentsLoader
My Utility for learn OpenGl stuff with [thebookofshaders](https://thebookofshaders.com/) and [shadertoy](https://www.shadertoy.com)
--------

It can load *multiple fragment shaders with one vertex shader in common*.   
You can select the current fragment to display and when you edit the fragment source code, current or not, the application reload the associated program.   

For example :   
You can start the application like ``./MyFragmentsLoader -f fragment_folder/ -r -t texture_forder/``  
The application load all fragments and texture images and you can select the current fragment to display with + or - ,   
then you can edit the fragment source code with an editor of your choise, like Visual Studio, and when you save the file the application reload the associated program.   
You can use this [fragment shader](https://github.com/musicrizz/MyFragmentsLoader/blob/main/example/fragment_template.glsl) as template.   
If the compilation is not successiful it will display an ERROR image and the errors will be printed on the console.   

PS:   
at the moment the errors are printed on the console, but I have to implement a text system for display directly in the ERROR Window.
   

This is a cpp project with Eclipse.  
Required lib :   
* GL
* GLEW
* glfw
* glm
   
also I used [stb_image.h](https://github.com/nothings/stb/blob/master/stb_image.h) for loading the texture images and [easylogging++](https://github.com/amrayn/easyloggingpp) for logger.





![todo](img.png)

![todo](img2.png)



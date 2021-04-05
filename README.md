# MyFragmentsLoader
My Utility for learn OpenGl stuff with https://thebookofshaders.com/ and https://www.shadertoy.com/
--------
It can load multiple glsl programs (" *multiple fragment shaders with one vertex shader in common* ") . You can select the current fragment to display and when you edit the fragment source, current or not, the application reload the associated program.

You can add fragment shaders in the folder of your choise, when the program start it load all shaders and you can switch them with + and -

for example you can run this app to view the result and edit the fragment shader with Visual Studio Code.

USAGE:   
$ ./MyFragmetsLoader -f *'fragment_shaders_folder'*  
	   -f   : specify folder where are located all fragment shaders  
	   -r   : (optional) set recursive search in subfolder of fragment folder  
	   -v   : (optional) specify your vertex shader file that will common to all fragment . ('overwrite the default')   


**WORK IN PROGRESS - not complete yet 

![shadertoy](img.png)

![shadertoy](img2.png)

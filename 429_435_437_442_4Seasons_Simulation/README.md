 First install the following libraries and link them properly
GLEW
GLM
SOIL
ASSIMP
GLFW

In linux type below command for compiling: 
g++ -o render render.cpp -lGL -lGLEW -lglfw -lSOIL -lassimp

In windows(Visual Studio) you can bind all the libraries and run the program

Under final_final you can find the required materials for spring and winter season
Under final, you can find the required material for point source light acting as street lights
Under des, you can find the required material for day and night modes 

*Remember to give appropriate file location for loading textures

Features:
You can switch between day and night light using ‘P’ and ‘O’ keys
You can switch between summer and winter season using ‘H’ and ‘J’ keys

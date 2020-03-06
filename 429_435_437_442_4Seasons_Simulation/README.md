First install the following libraries and link them properly
GLEW
GLM
SOIL
ASSIMP
GLFW

Firstly, since large files (.blend and .obj) couldn’t be uploaded in the publicly forked repo, you can go to the link of Google Drive below to find the models

https://drive.google.com/drive/folders/1dtIt801sfqbWa7zd5IL9jhh9zCAubL4_?usp=sharing

Download the 'Models' directory and copy the directories 'final_final','final' and 'des' (inside the Models) into this working directory and you're good to go!

In linux type below command for compiling:
g++ -o render render.cpp -lGL -lGLEW -lglfw -lSOIL -lassimp

And to execute:
./render

In windows(Visual Studio) you can bind all the libraries and run the program

Under final_final you can find the required materials for spring and winter season
Under final, you can find the required material for point source light acting as street lights
Under des, you can find the required material for day and night modes

Header files: model.h,mesh.h,stb_image.h,camera.h
VertexShader files: shader.vs lamp.vs
FragmentShader files: shader.frag lamp.frag book.frag

*Remember to give appropriate file location for loading textures

Features:
You can switch between day and night light using ‘P’ and ‘O’ keys
You can switch between summer and winter season using ‘H’ and ‘J’ keys

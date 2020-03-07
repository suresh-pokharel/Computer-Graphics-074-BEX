import glfw
from OpenGL.GL import *
from OpenGL.GL.shaders import compileProgram, compileShader
import pyrr

##Self-defined modules
from TextureLoader import load_texture
from ObjLoader import ObjLoader
from camera import Camera
import ShaderLoader

cam=Camera()
WIDTH,HEIGHT=1280,720
lastX,lastY=WIDTH/2, HEIGHT/2
first_mouse=True
left, right, forward, backward, Rotate=False,False,False,False, False

##initialising glfw library
if not glfw.init():
    raise Exception("GLFW CANNOT BE INITIALISED")

##Creating the window
w_height=800
w_width=600
window=glfw.create_window(w_height,w_width,"Dharara",None,None)

# check if window was created
if not window:
    glfw.terminate()
    raise Exception("glfw window can not be created!")

# set window's position
glfw.set_window_pos(window, 10, 20)
##make the context current
glfw.make_context_current(window)

#loading the 3d meshes
dh_indices, dh_buffer=ObjLoader.load_model("meshes/final.obj")
##dh_indices, dh_buffer=ObjLoader.load_model("meshes/cube.obj")

##shader=ShaderLoader.compile_shader("shaders/vert.vs","shaders/frag.vs")
shader=ShaderLoader.compile_shader("shaders/shader_vert.vs","shaders/shader_frag.fs")

##VAO AND VBO
VAO=glGenVertexArrays(1)
VBO=glGenBuffers(1)
##VAO
glBindVertexArray(VAO)
##Vertex Buffer Object
glBindBuffer(GL_ARRAY_BUFFER,VBO)
glBufferData(GL_ARRAY_BUFFER, dh_buffer.nbytes, dh_buffer, GL_STATIC_DRAW)

##Vertices
glEnableVertexAttribArray(0)
glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,dh_buffer.itemsize*8,ctypes.c_void_p(0))
##textures
glEnableVertexAttribArray(1)
glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,dh_buffer.itemsize*8,ctypes.c_void_p(12))
##normals
glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,dh_buffer.itemsize*8,ctypes.c_void_p(20))
glEnableVertexAttribArray(2)

textures=glGenTextures(1)
load_texture("meshes/final.png",textures)
##load_texture("meshes/metal.jpg",textures)
glEnable(GL_TEXTURE_2D)

glUseProgram(shader)
glClearColor(0.2,0.75,0.1,1)
glEnable(GL_DEPTH_TEST)
glEnable(GL_BLEND)
glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA)

##view = pyrr.matrix44.create_from_translation(pyrr.Vector3([0.0, 0.0, -3.0]))
view = pyrr.matrix44.create_from_translation(pyrr.Vector3([0.0, -7.0, -6.0]))
projection = pyrr.matrix44.create_perspective_projection_matrix(45.0, w_height / w_width, 0.1, 100.0)
model = pyrr.matrix44.create_from_translation(pyrr.Vector3([0.0, 0.0, 0.0]))

view_loc = glGetUniformLocation(shader, "view")
proj_loc = glGetUniformLocation(shader, "projection")
model_loc = glGetUniformLocation(shader, "model")
transform_loc = glGetUniformLocation(shader, "transform")
light_loc = glGetUniformLocation(shader, "light")

glUniformMatrix4fv(view_loc, 1, GL_FALSE, view)
glUniformMatrix4fv(proj_loc, 1, GL_FALSE, projection)
glUniformMatrix4fv(model_loc, 1, GL_FALSE, model)

while not glfw.window_should_close(window):
        glfw.poll_events()

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
        glClearColor(0.2,0.75,0.1,1)

        #rot_x = pyrr.Matrix44.from_x_rotation(0.5 * glfw.get_time() )
        rot_y = pyrr.Matrix44.from_y_rotation(0.8 * glfw.get_time() )

        glUniformMatrix4fv(transform_loc, 1, GL_FALSE, rot_y)
        glUniformMatrix4fv(light_loc, 1, GL_FALSE, rot_y)

        glDrawArrays(GL_TRIANGLES, 0, len(dh_indices))

        glfw.swap_buffers(window)

glfw.terminate()




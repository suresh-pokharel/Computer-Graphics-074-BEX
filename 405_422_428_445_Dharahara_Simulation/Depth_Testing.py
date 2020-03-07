import glfw
from OpenGL.GL import *
from OpenGL.GL.shaders import compileProgram, compileShader
import pyrr
from pyrr import Vector3, vector, vector3, matrix44

##Self-defined modules
from TextureLoader import load_texture
from ObjLoader import ObjLoader
from camera import Camera
import ShaderLoader

cam=Camera()
WIDTH,HEIGHT=1280,720
lastX,lastY=WIDTH/2, HEIGHT/2
first_mouse=True
left, right, forward, backward, Rotate, quake=False,False,False,False, False, False


##the keyboard input callback
def key_input_clb(window,key,scancode,action,mode):
    global left, right, forward, backward, Rotate, quake
    
    if key==glfw.KEY_ESCAPE and action==glfw.PRESS:
        glfw.set_window_should_close(window,True)

    if key==glfw.KEY_W and action==glfw.PRESS:
        forward=True
    elif key==glfw.KEY_W and action==glfw.RELEASE:
        forward=False
    if key==glfw.KEY_S and action==glfw.PRESS:
        backward=True
    elif key==glfw.KEY_S and action==glfw.RELEASE:
        backward=False
    if key==glfw.KEY_A and action==glfw.PRESS:
        left=True
    elif key==glfw.KEY_A and action==glfw.RELEASE:
        left=False
    if key==glfw.KEY_D and action==glfw.PRESS:
        right=True
    elif key==glfw.KEY_D and action==glfw.RELEASE:
        right=False
    if key==glfw.KEY_R and action==glfw.PRESS:
        Rotate= not Rotate
    if key==glfw.KEY_E and action==glfw.PRESS:
        quake= not quake
    

##do the movement, call this funcn in main loop
def do_movement():
    ##called in every loop
    steps=0.05
    if left:
        cam.process_keyboard("LEFT",steps)
    if right:
        cam.process_keyboard("RIGHT",steps)
    if forward:
        cam.process_keyboard("FORWARD",steps)
    if backward:
        cam.process_keyboard("BACKWARD",steps)
    if Rotate:
        ##print("Rotation called")
        pass
    if quake:
        ##cam.process_keyboard("QUAKE",steps)
        pass
        
        

##the mouse position callback function
def mouse_look_clb(window, xpos, ypos):
    
    global first_mouse, lastX, lastY

    if first_mouse:
        lastX=xpos
        lastY=ypos
        first_mouse=False

    xoffset=xpos-lastX   
    yoffset=lastY-ypos      ##because Y counting starts from bottom

    lastX=xpos
    lastY=ypos

    cam.process_mouse_movement(xoffset, yoffset)
        
##the scroll callback function
def scroll_clb(windows, xoff, yoff):
    cam.process_scroll(yoff)
    
##glfw callback functions
def window_resize(window, width, height):
    glViewport(0,0,width, height)
    ##*********change*******##
    projection=pyrr.matrix44.create_perspective_projection_matrix(45, width/height,
                                                                  0.1,100)
    glUniformMatrix4fv(proj_loc,1,GL_FALSE, projection)


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

##CALLBACK FUNCTIONS
##set the callback func for window resize
glfw.set_window_size_callback(window, window_resize)
##set the mouse position callback
glfw.set_cursor_pos_callback(window,mouse_look_clb)
##set the keyboard input callback
glfw.set_key_callback(window,key_input_clb)
##capture the mouse cursor
glfw.set_input_mode(window,glfw.CURSOR, glfw.CURSOR_DISABLED)
##call back for mouse scroll
glfw.set_scroll_callback(window, scroll_clb)

##make the context current
glfw.make_context_current(window)

##load here the 3d meshes

dh_indices, dh_buffer= ObjLoader.load_model("meshes/final.obj")
##dh_indices, dh_buffer=ObjLoader.load_model("meshes/cube.obj")
##shader = ShaderLoader.compile_shader("shaders/shader_vert.vs", "shaders/shader_frag.fs")
shader = ShaderLoader.compile_shader("shaders/shadow_mapping_depth.vs", "shaders/shadow_mapping_depth.fs")
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
glClearColor(0,0.1,0.1,1)
glEnable(GL_DEPTH_TEST)
glEnable(GL_BLEND)
glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA)

view = pyrr.matrix44.create_from_translation(pyrr.Vector3([0.0, -7.0, -6.0]))
projection = pyrr.matrix44.create_perspective_projection_matrix(45.0, w_height / w_width, 0.1, 100.0)
model = pyrr.matrix44.create_from_translation(pyrr.Vector3([0.0, 0.0, 0.0]))

##(x,y,z) coordinates
##dh_pos=pyrr.matrix44.create_from_translation(pyrr.Vector3([0,-26,-12]))
##dh_pos=pyrr.matrix44.create_from_translation(pyrr.Vector3([0,0,0]))


view_loc = glGetUniformLocation(shader, "view")
proj_loc = glGetUniformLocation(shader, "projection")
model_loc = glGetUniformLocation(shader, "model")
transform_loc = glGetUniformLocation(shader, "transform")
light_loc = glGetUniformLocation(shader, "light")

glUniformMatrix4fv(view_loc, 1, GL_FALSE, view)
##glUniformMatrix4fv(light_loc, 1, GL_FALSE, light)
glUniformMatrix4fv(proj_loc, 1, GL_FALSE, projection)
glUniformMatrix4fv(model_loc, 1, GL_FALSE, model)


DEG=0.01      ##FOR TOGGLING OF ROTATION

##the main applcn loop
while not glfw.window_should_close(window):
    
    glfw.poll_events()
    do_movement()

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT)
    
    ##changing the view point
    view=cam.get_view_matrix()
    glUniformMatrix4fv(view_loc,1 ,GL_FALSE,view)
    
    if Rotate:
        DEG+=.005

    rot_y=pyrr.Matrix44.from_y_rotation(0.8*DEG)
    ##rot_y=pyrr.Matrix44.from_y_rotation(0.8*glfw.get_time())
    ##rot_y=pyrr.Matrix44.from_y_rotation(0.8)

##    if quake:
##        PTS=Vector3([15.0,15.0,15.0])
##        rot_y=pyrr.matrix44.create_from_translation(PTS)
##        
    
    ##model = pyrr.matrix44.multiply(rot_y, dh_pos)

    ##draw the Dharara
    ##glBindVertexArray(VAO)
    ##glBindTexture(GL_TEXTURE_2D,textures)
    glUniformMatrix4fv(transform_loc, 1, GL_FALSE, rot_y)
    glUniformMatrix4fv(light_loc,1,GL_FALSE,rot_y)
    glDrawArrays(GL_TRIANGLES,0, len(dh_indices))

    glfw.swap_buffers(window)

##terminate glfw,free up allocated resources
glfw.terminate()
        
    






























    
    
    

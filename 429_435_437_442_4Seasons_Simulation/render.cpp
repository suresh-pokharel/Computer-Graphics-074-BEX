// Std. Includes
#include <string>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GL includes
#include "shader.h"
#include "camera.h"
#include "model.h"

// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other Libs
#include "../SOIL.h"

// Properties
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Function prototypes
void KeyCallback( GLFWwindow *window, int key, int scancode, int action, int mode );
void MouseCallback( GLFWwindow *window, double xPos, double yPos );
void DoMovement( );

// Camera
Camera camera( glm::vec3( 0.0f, 0.0f, 3.0f ) );
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;
//camera practise
glm::vec3 cameraPos = glm::vec3(50.0f, 3.0f, -15.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

//light position
// glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

//lighting on & off
bool night_mode=false;
//season chang
char which_season;
int main( )
{
    // Init GLFW
    glfwInit( );
    // Set all the required options for GLFW
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
    glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
    glfwWindowHint( GLFW_RESIZABLE, GL_FALSE );
    
    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow *window = glfwCreateWindow( WIDTH, HEIGHT, "4 Seasons", nullptr, nullptr );
    
    if ( nullptr == window )
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate( );
        
        return EXIT_FAILURE;
    }
    
    glfwMakeContextCurrent( window );
    
    glfwGetFramebufferSize( window, &SCREEN_WIDTH, &SCREEN_HEIGHT );
    
    // Set the required callback functions
    glfwSetKeyCallback( window, KeyCallback );
    glfwSetCursorPosCallback( window, MouseCallback );
    
    // GLFW Options
    glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );
    
    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    if ( GLEW_OK != glewInit( ) )
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return EXIT_FAILURE;
    }
    
    // Define the viewport dimensions
    glViewport( 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT );
    
    // OpenGL options
    glEnable( GL_DEPTH_TEST );
    
    // Setup and compile our shaders
    Shader shader( "shader.vs", "book.frag" );
    Shader season1_shader( "shader.vs", "common.frag" );
    Shader season2_shader( "shader.vs", "common.frag" );
    Shader season3_shader( "shader.vs", "common.frag" );
    Shader season4_shader( "shader.vs", "common.frag" );


    Shader lampShader("lamp.vs", "lamp.frag");
    // Load models
    Model ourModel( "des/trees (3).obj" );
    Model season2Model("final_final/SPRING.obj");
    Model sun("final/sun.obj");
    Model season3Model("final_final/WINTER.obj");
    // Model season4Model("go_on/trees.obj");
    
    // Draw in wireframe
    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    

    // Point light positions
    glm::vec3 pointLightPositions[] = {
//         glm::vec3(-34.4845,8.13808,-13.2533
// )
        glm::vec3(-34.4173,7.5735,-13.3274),
        glm::vec3(-15.9184,7.58819,-13.2185),
        glm::vec3(4.94882,7.68453,-13.1896),
        glm::vec3(24.2309,7.46559,-13.0907),
        glm::vec3(4.06948,7.54208,8.35854),

    };


    glm::mat4 projection = glm::perspective( camera.GetZoom( ), ( float )SCREEN_WIDTH/( float )SCREEN_HEIGHT, 0.1f, 100.0f );
    
    // Game loop
    while( !glfwWindowShouldClose( window ) )
    {
        // Set frame time
        GLfloat currentFrame = glfwGetTime( );
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // std::cout<< camera.position[0]<<" "<<camera.position[1]<<" "<<camera[2]<<std::endl;
        // glm::vec3 s =camera.GetPosition();
        // std::cout<<s.x<<" "<<s.y<<" "<<s.z<<std::endl;
        
        // Check and call events
        glfwPollEvents( );
        DoMovement( );
        
        if(night_mode){
            // Clear the colorbuffer
            glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
            
            shader.Use( );
            
            glm::mat4 view = camera.GetViewMatrix( );
            glUniformMatrix4fv( glGetUniformLocation( shader.Program, "projection" ), 1, GL_FALSE, glm::value_ptr( projection ) );
            glUniformMatrix4fv( glGetUniformLocation( shader.Program, "view" ), 1, GL_FALSE, glm::value_ptr( view ) );
        
            // Point light 1
            glUniform3f(glGetUniformLocation(shader.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);     
            glUniform3f(glGetUniformLocation(shader.Program, "pointLights[0].ambient"), 0.0f, 0.0f, 0.0f);       
            glUniform3f(glGetUniformLocation(shader.Program, "pointLights[0].diffuse"), 0.5f, 0.5f, 0.0f); 
            glUniform3f(glGetUniformLocation(shader.Program, "pointLights[0].specular"), 0.0f, 0.6f, 0.6f);
            glUniform1f(glGetUniformLocation(shader.Program, "pointLights[0].constant"), 1.0f);
            glUniform1f(glGetUniformLocation(shader.Program, "pointLights[0].linear"), 0.009);
            glUniform1f(glGetUniformLocation(shader.Program, "pointLights[0].quadratic"), 0.0032);      
            // Point light 2
            glUniform3f(glGetUniformLocation(shader.Program, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);     
            glUniform3f(glGetUniformLocation(shader.Program, "pointLights[1].ambient"), 0.0f, 0.0f, 0.0f);       
            glUniform3f(glGetUniformLocation(shader.Program, "pointLights[1].diffuse"), 0.5f, 0.5f, 0.0f); 
            glUniform3f(glGetUniformLocation(shader.Program, "pointLights[1].specular"), 0.0f, 0.6f, 0.6f);
            glUniform1f(glGetUniformLocation(shader.Program, "pointLights[1].constant"), 1.0f);
            glUniform1f(glGetUniformLocation(shader.Program, "pointLights[1].linear"), 0.009);
            glUniform1f(glGetUniformLocation(shader.Program, "pointLights[1].quadratic"), 0.0032);  
            // Point light 1
            glUniform3f(glGetUniformLocation(shader.Program, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);     
            glUniform3f(glGetUniformLocation(shader.Program, "pointLights[2].ambient"), 0.0f, 0.0f, 0.0f);       
            glUniform3f(glGetUniformLocation(shader.Program, "pointLights[2].diffuse"), 0.5f, 0.5f, 0.0f); 
            glUniform3f(glGetUniformLocation(shader.Program, "pointLights[2].specular"), 0.0f, 0.6f, 0.6f);
            glUniform1f(glGetUniformLocation(shader.Program, "pointLights[2].constant"), 1.0f);
            glUniform1f(glGetUniformLocation(shader.Program, "pointLights[2].linear"), 0.009);
            glUniform1f(glGetUniformLocation(shader.Program, "pointLights[2].quadratic"), 0.0032);      
            // Point light 2
            glUniform3f(glGetUniformLocation(shader.Program, "pointLights[3].position"), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);     
            glUniform3f(glGetUniformLocation(shader.Program, "pointLights[3].ambient"), 0.0f, 0.0f, 0.0f);       
            glUniform3f(glGetUniformLocation(shader.Program, "pointLights[3].diffuse"), 0.5f, 0.5f, 0.0f); 
            glUniform3f(glGetUniformLocation(shader.Program, "pointLights[3].specular"), 0.0f, 0.6f, 0.6f);
            glUniform1f(glGetUniformLocation(shader.Program, "pointLights[3].constant"), 1.0f);
            glUniform1f(glGetUniformLocation(shader.Program, "pointLights[3].linear"), 0.009);
            glUniform1f(glGetUniformLocation(shader.Program, "pointLights[3].quadratic"), 0.0032);  
            //point light 5
            glUniform3f(glGetUniformLocation(shader.Program, "pointLights[4].position"), pointLightPositions[4].x, pointLightPositions[4].y, pointLightPositions[4].z);     
            glUniform3f(glGetUniformLocation(shader.Program, "pointLights[4].ambient"), 0.5f, 0.5f, 0.5f);       
            glUniform3f(glGetUniformLocation(shader.Program, "pointLights[4].diffuse"), 1.0f, 1.0f, 1.0f); 
            glUniform3f(glGetUniformLocation(shader.Program, "pointLights[4].specular"), 1.0f, 1.0f, 1.0f);
            glUniform1f(glGetUniformLocation(shader.Program, "pointLights[4].constant"), 1.0f);
            glUniform1f(glGetUniformLocation(shader.Program, "pointLights[4].linear"), 0.009);
            glUniform1f(glGetUniformLocation(shader.Program, "pointLights[4].quadratic"), 0.0032);  
            
            // Draw the loaded model
            glm::mat4 model=glm::mat4(1.0f);
            model = glm::translate( model, glm::vec3( 0.0f, -1.75f, 0.0f ) ); // Translate it down a bit so it's at the center of the scene
            model = glm::scale( model, glm::vec3( 0.2f, 0.2f, 0.2f ) ); // It's a bit too big for our scene, so scale it down
            glUniformMatrix4fv( glGetUniformLocation( shader.Program, "model" ), 1, GL_FALSE, glm::value_ptr( model ) );
            ourModel.Draw( shader );

            //draw the lamps
            lampShader.Use();
            glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
            glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
            for(GLuint i = 0; i < 5; i++)
            {
                model = glm::mat4(1.0f);
                model = glm::translate(model, pointLightPositions[i]);
                model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f)); // Downscale lamp object (a bit too large)
                glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
                sun.Draw(lampShader);
            }
        }
        
        else{
            glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
            
            if(which_season=='h'){
                season1_shader.Use( );
                    glm::mat4 view = camera.GetViewMatrix( );
                    glUniformMatrix4fv( glGetUniformLocation( season1_shader.Program, "projection" ), 1, GL_FALSE, glm::value_ptr( projection ) );
                    glUniformMatrix4fv( glGetUniformLocation( season1_shader.Program, "view" ), 1, GL_FALSE, glm::value_ptr( view ) );
                    // Draw the loaded model
                    glm::mat4 model=glm::mat4(1.0f);
                    model = glm::translate( model, glm::vec3( 0.0f, -1.75f, 0.0f ) ); // Translate it down a bit so it's at the center of the scene
                    model = glm::scale( model, glm::vec3( 0.2f, 0.2f, 0.2f ) ); // It's a bit too big for our scene, so scale it down
                    glUniformMatrix4fv( glGetUniformLocation( season1_shader.Program, "model" ), 1, GL_FALSE, glm::value_ptr( model ) );
                    ourModel.Draw( season1_shader );
            }

            // else if(which_season=='j'){
            //         season4_shader.Use( );
            //         glm::mat4 view = camera.GetViewMatrix( );
            //         glUniformMatrix4fv( glGetUniformLocation( season4_shader.Program, "projection" ), 1, GL_FALSE, glm::value_ptr( projection ) );
            //         glUniformMatrix4fv( glGetUniformLocation( season4_shader.Program, "view" ), 1, GL_FALSE, glm::value_ptr( view ) );
            //         // Draw the loaded model
            //         glm::mat4 model=glm::mat4(1.0f);
            //         model = glm::translate( model, glm::vec3( 0.0f, -1.75f, 0.0f ) ); // Translate it down a bit so it's at the center of the scene
            //         model = glm::scale( model, glm::vec3( 0.2f, 0.2f, 0.2f ) ); // It's a bit too big for our scene, so scale it down
            //         glUniformMatrix4fv( glGetUniformLocation( season4_shader.Program, "model" ), 1, GL_FALSE, glm::value_ptr( model ) );
            //         season4Model.Draw( season4_shader );
            // }

            else if(which_season=='k'){
                    season2_shader.Use( );
                    glm::mat4 view = camera.GetViewMatrix( );
                    glUniformMatrix4fv( glGetUniformLocation( season3_shader.Program, "projection" ), 1, GL_FALSE, glm::value_ptr( projection ) );
                    glUniformMatrix4fv( glGetUniformLocation( season3_shader.Program, "view" ), 1, GL_FALSE, glm::value_ptr( view ) );
                    // Draw the loaded model
                    glm::mat4 model=glm::mat4(1.0f);
                    model = glm::translate( model, glm::vec3( 0.0f, -1.75f, 0.0f ) ); // Translate it down a bit so it's at the center of the scene
                    model = glm::scale( model, glm::vec3( 0.2f, 0.2f, 0.2f ) ); // It's a bit too big for our scene, so scale it down
                    glUniformMatrix4fv( glGetUniformLocation( season3_shader.Program, "model" ), 1, GL_FALSE, glm::value_ptr( model ) );
                    season3Model.Draw( season3_shader );
            }
            
            else 
            // (which_season=='j')
            {
                    season2_shader.Use( );
                    glm::mat4 view = camera.GetViewMatrix( );
                    glUniformMatrix4fv( glGetUniformLocation( season2_shader.Program, "projection" ), 1, GL_FALSE, glm::value_ptr( projection ) );
                    glUniformMatrix4fv( glGetUniformLocation( season2_shader.Program, "view" ), 1, GL_FALSE, glm::value_ptr( view ) );
                    // Draw the loaded model
                    glm::mat4 model=glm::mat4(1.0f);
                    model = glm::translate( model, glm::vec3( 0.0f, -1.75f, 0.0f ) ); // Translate it down a bit so it's at the center of the scene
                    model = glm::scale( model, glm::vec3( 0.2f, 0.2f, 0.2f ) ); // It's a bit too big for our scene, so scale it down
                    glUniformMatrix4fv( glGetUniformLocation( season2_shader.Program, "model" ), 1, GL_FALSE, glm::value_ptr( model ) );
                    season2Model.Draw( season2_shader );
            }                    
                    
            
            
            
            
        }
        
        
        // Swap the buffers
        glfwSwapBuffers( window );

    }

    
    glfwTerminate( );
    return 0;
}

// Moves/alters the camera positions based on user input
void DoMovement( )
{
    // Camera controls
    if ( keys[GLFW_KEY_W] || keys[GLFW_KEY_UP] )
    {
        camera.ProcessKeyboard( FORWARD, deltaTime );
    }
    
    if ( keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN] )
    {
        camera.ProcessKeyboard( BACKWARD, deltaTime );
    }
    
    if ( keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT] )
    {
        camera.ProcessKeyboard( LEFT, deltaTime );
    }
    
    if ( keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT] )
    {
        camera.ProcessKeyboard( RIGHT, deltaTime );
    }

    if ( keys[GLFW_KEY_O] )
    {
        night_mode=true;
    }

    if ( keys[GLFW_KEY_P]){
        night_mode=false;
    }

    if ( keys[GLFW_KEY_H]){
        which_season='h';
    }
    if ( keys[GLFW_KEY_J]){
        which_season='j';
    }
    if ( keys[GLFW_KEY_K]){
        which_season='k';
    }
    if ( keys[GLFW_KEY_L]){
        which_season='l';
    }
}

// Is called whenever a key is pressed/released via GLFW
void KeyCallback( GLFWwindow *window, int key, int scancode, int action, int mode )
{
    if ( GLFW_KEY_ESCAPE == key && GLFW_PRESS == action )
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    // if(GLFW_KEY_O == key && GLFW_PRESS == action){
    //     night_mode=true;
    // }

    // if(GLFW_KEY_P == key && GLFW_PRESS == action){
    //     night_mode=true;
    // }
    
    if ( key >= 0 && key < 1024 )
    {
        if ( action == GLFW_PRESS )
        {
            keys[key] = true;
        }
        else if ( action == GLFW_RELEASE )
        {
            keys[key] = false;
        }
    }
}

void MouseCallback( GLFWwindow *window, double xPos, double yPos )
{
    if ( firstMouse )
    {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }
    
    GLfloat xOffset = xPos - lastX;
    GLfloat yOffset = lastY - yPos;  // Reversed since y-coordinates go from bottom to left
    
    lastX = xPos;
    lastY = yPos;
    
    camera.ProcessMouseMovement( xOffset, yOffset );
}

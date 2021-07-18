// Std. Includes
#include <string>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GL includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"

// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other Libs
#include "SOIL2/SOIL2.h"

// Properties
const GLuint WIDTH = 1920, HEIGHT = 1080;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Function prototypes
void KeyCallback( GLFWwindow *window, int key, int scancode, int action, int mode );
void MouseCallback( GLFWwindow *window, double xPos, double yPos );
void DoMovement( );

// Camera
Camera camera( glm::vec3( 3.0f, 3.0f, 6.0f ) );
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

int fish = 0;

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
    GLFWwindow *window = glfwCreateWindow( WIDTH, HEIGHT, "SilvaNunezAlejandroBryan_ProyectoFinal", nullptr, nullptr );
    
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
    //glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );
    
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
    Shader shader( "Shaders/modelLoading.vs", "Shaders/modelLoading.frag" );
    
    // Load models
    Model Fachada((char*)"Models/Fachada/fachada.obj");
    Model Pasto((char*)"Models/Fachada/grass.obj");
    Model Interior((char*)"Models/Fachada/interior.obj");
    /*Model fish1((char *)"Models/Fishes/TropicalFish01.obj");
    Model fish2((char*)"Models/Fishes/TropicalFish02.obj");
    Model fish3((char*)"Models/Fishes/TropicalFish03.obj");
    Model fish4((char*)"Models/Fishes/TropicalFish04.obj");
    Model fish5((char*)"Models/Fishes/TropicalFish05.obj");
    Model fish6((char*)"Models/Fishes/TropicalFish06.obj");
    Model fish7((char*)"Models/Fishes/TropicalFish07.obj");
    Model fish8((char*)"Models/Fishes/TropicalFish08.obj");
    Model fish9((char*)"Models/Fishes/TropicalFish09.obj");
    Model fish10((char*)"Models/Fishes/TropicalFish10.obj");
    Model fish11((char*)"Models/Fishes/TropicalFish11.obj");
    Model fish12((char*)"Models/Fishes/TropicalFish12.obj");
    Model fish13((char*)"Models/Fishes/TropicalFish13.obj");
    Model fish14((char*)"Models/Fishes/TropicalFish14.obj");
    Model fish15((char*)"Models/Fishes/TropicalFish15.obj");*/
   

    // Draw in wireframe
    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    
    glm::mat4 projection = glm::perspective( camera.GetZoom( ), ( float )SCREEN_WIDTH/( float )SCREEN_HEIGHT, 0.1f, 100.0f );
    
    // Game loop
    while( !glfwWindowShouldClose( window ) )
    {
        // Set frame time
        GLfloat currentFrame = glfwGetTime( );
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // Check and call events
        glfwPollEvents( );
        DoMovement( );
        
        // Clear the colorbuffer
        glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        
        shader.Use( );
        
        glm::mat4 view = camera.GetViewMatrix();
        glUniformMatrix4fv( glGetUniformLocation( shader.Program, "projection" ), 1, GL_FALSE, glm::value_ptr( projection ) );
        glUniformMatrix4fv( glGetUniformLocation( shader.Program, "view" ), 1, GL_FALSE, glm::value_ptr( view ) );
        
        // Draw the loaded model
        glm::mat4 model(1);
        //model = glm::translate( model, glm::vec3( 0.0f, -1.75f, 0.0f ) ); // Translate it down a bit so it's at the center of the scene
        //model = glm::scale( model, glm::vec3( 0.01f, 0.01f, 0.01f ) );	// It's a bit too big for our scene, so scale it down
		//model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv( glGetUniformLocation( shader.Program, "model" ), 1, GL_FALSE, glm::value_ptr( model ) );
	
		//Dibujando peces
        Fachada.Draw(shader);
        Pasto.Draw(shader);
        //Interior.Draw(shader);
        /*if (fish == 0) { fish1.Draw(shader); }
        if (fish == 1) { fish2.Draw(shader); }
        if (fish == 2) { fish3.Draw(shader); }
        if (fish == 3) { fish4.Draw(shader); }
        if (fish == 4) { fish5.Draw(shader); }
        if (fish == 5) { fish6.Draw(shader); }
        if (fish == 6) { fish7.Draw(shader); }
        if (fish == 7) { fish8.Draw(shader); }
        if (fish == 8) { fish9.Draw(shader); }
        if (fish == 9) { fish10.Draw(shader); }
        if (fish == 10) { fish11.Draw(shader); }
        if (fish == 11) { fish12.Draw(shader); }
        if (fish == 12) { fish13.Draw(shader); }
        if (fish == 13) { fish14.Draw(shader); }
        if (fish == 14) { fish15.Draw(shader); }*/
        
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
}

// Is called whenever a key is pressed/released via GLFW
void KeyCallback( GLFWwindow *window, int key, int scancode, int action, int mode )
{
    if ( GLFW_KEY_ESCAPE == key && GLFW_PRESS == action )
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    
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
    /*if (keys[GLFW_KEY_Y]) {
        fish++;
        if (fish > 14) {
            fish = 0;
        }
        printf("%d", fish);

    }
    if (keys[GLFW_KEY_T]) {
        fish--;
        if (fish < 0) {
            fish = 14;
        }
        printf("%d", fish);
    }*/
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


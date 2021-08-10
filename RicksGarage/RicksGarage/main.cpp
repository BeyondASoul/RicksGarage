#include <iostream>
#include <cmath>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include "stb_image.h"

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Load Models
#include "SOIL2/SOIL2.h"


// Other includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Texture.h"
#include "modelAnim.h"

// Musica de fondo
#include <irrKlang.h>
using namespace irrklang;

// Function prototypes
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow *window, double xPos, double yPos);
void DoMovement();
void animacion();
void animacionCarro();
void animacionGarage();
void animacionSilla();

// Window dimensions
const GLuint WIDTH = 1280, HEIGHT = 720;
int SCREEN_WIDTH, SCREEN_HEIGHT;
// Camera
Camera camera(glm::vec3(3.0f, 3.0f, 6.0f));
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
bool firstMouse = true;
float rotGarage = 0.0f, rotGarageDoor = 0.0f, rotHouseDoor = -100.0;
// Light attributes
bool active=true;
//Animation Variables
glm::vec3 PosIniCar(16.0f, 1.5f, 3.7f);
glm::vec3 PosIniChair(-0.5f, 1.1f, 1.0f);
bool circuitoCar = false, garageAnim = false, sillaAnim = false;
bool rec1 = true, rec2 = false, rec3 = false, rec4 = false;
bool recG1 = true, recG2 = false;
bool recS1 = true, recS2 = false, recS3 = false;
float movCarX = 0.0f, movCarZ = 0.0f, rotCar = 0.0f;
// Deltatime
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame
// Keyframes
float movChairX = PosIniChair.x, movChairZ = PosIniChair.z, rotCh = 0.0f, rotIncCh = 0.0f, rotPortal = 0.0f, incPortal = 0.0;

#define MAX_FRAMES 9
int i_max_steps = 190;
int i_curr_steps = 0;
typedef struct _frame
{
	//Variables para GUARDAR Key Frames
	float movChairX;		//Variable para PosicionX
	float movChairZ;		//Variable para PosicionZ
	float incChairX;		//Variable para IncrementoX
	float incChairZ;		//Variable para IncrementoZ
	float rotCh;			//Rotaci�n silla
	float rotIncCh;			//Incremento rotaci�n silla
	float rotPortal;		//Rotaci�n portal
	float incPortal;		//Incremento rotaci�n portal

}FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 0;//introducir datos
bool play = false;
int playIndex = 0;

// Positions of the point lights
glm::vec3 pointLightPositions[] = {
	glm::vec3(-3.45f,3.6f,-0.21f),
	glm::vec3(-1.3f,3.7f,3.4f),
	glm::vec3(1.75f,3.7f,3.4f),
	glm::vec3(100.0f,0.0f,0.0f)
};

glm::vec3 LightP1, LightP2, LightP3;

void saveFrame(void)
{
	printf("KeyFrame 0 guardado\n");
	//Keyframes Posici�n
	KeyFrame[0].movChairX = PosIniChair.x;
	KeyFrame[0].movChairZ = PosIniChair.z;
	//KeyFrames Rotaci�n
	KeyFrame[0].rotCh = rotCh;
	KeyFrame[0].rotPortal = -90;
	FrameIndex++;
	printf("KeyFrame 1 guardado\n");
	//Keyframes Posici�n
	KeyFrame[1].movChairX = 1.0f;
	KeyFrame[1].movChairZ = 0.0f;
	//KeyFrames Rotaci�n
	KeyFrame[1].rotCh = 180;
	KeyFrame[1].rotPortal = 180;
	FrameIndex++;
	printf("KeyFrame 2 guardado\n");
	//Keyframes Posici�n
	KeyFrame[2].movChairX = -1.0f;
	KeyFrame[2].movChairZ = 1.5f;
	//KeyFrames Rotaci�n
	KeyFrame[2].rotCh = -180;
	KeyFrame[2].rotPortal = -180;
	FrameIndex++;
	printf("KeyFrame 3 guardado\n");
	//Keyframes Posici�n
	KeyFrame[3].movChairX = PosIniChair.x;
	KeyFrame[3].movChairZ = PosIniChair.z;
	//KeyFrames Rotaci�n
	KeyFrame[3].rotCh = rotCh;
	KeyFrame[3].rotPortal = rotPortal;
	FrameIndex++;
	printf("KeyFrame 4 guardado\n");
	//Keyframes Posici�n
	KeyFrame[4].movChairX = 1.0f;
	KeyFrame[4].movChairZ = 0.0f;
	//KeyFrames Rotaci�n
	KeyFrame[4].rotCh = 180;
	KeyFrame[4].rotPortal = 180;
	FrameIndex++;
	printf("KeyFrame 5 guardado\n");
	//Keyframes Posici�n
	KeyFrame[5].movChairX = -1.0f;
	KeyFrame[5].movChairZ = 1.5f;
	//KeyFrames Rotaci�n
	KeyFrame[5].rotCh = -180;
	KeyFrame[5].rotPortal = -180;
	//FrameIndex++;
}

void resetElements(void)
{
	movChairX= KeyFrame[0].movChairX;
	movChairZ = KeyFrame[0].movChairZ;

	rotCh = KeyFrame[0].rotCh;
	rotPortal = KeyFrame[0].rotPortal;
}

void interpolation(void)
{
	KeyFrame[playIndex].incChairX = (KeyFrame[playIndex + 1].movChairX - KeyFrame[playIndex].movChairX) / i_max_steps;
	KeyFrame[playIndex].incChairZ = (KeyFrame[playIndex + 1].movChairZ - KeyFrame[playIndex].movChairZ) / i_max_steps;
	
	KeyFrame[playIndex].rotIncCh = (KeyFrame[playIndex + 1].rotCh - KeyFrame[playIndex].rotCh) / i_max_steps;
	KeyFrame[playIndex].incPortal = (KeyFrame[playIndex + 1].rotPortal - KeyFrame[playIndex].rotPortal) / i_max_steps;
}

int main()
{
	// Init GLFW
	glfwInit();

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "SilvaNunezAlejandroBryan_ProyectoFinal", nullptr, nullptr);

	if (nullptr == window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();

		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);

	glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

	// Set the required callback functions
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, MouseCallback);

	// GLFW Options
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	// Define the viewport dimensions
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	// OpenGL options
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	// Musica
	ISoundEngine* SoundEngine = createIrrKlangDevice();

	if (!SoundEngine)
		return 0; //error al iniciar el audio
	// Reproduce la audio
	SoundEngine->play2D("Audio/hombres-lunares.mp3", true);


	//Shaders
	Shader lightingShader("Shaders/lighting.vs", "Shaders/lighting.frag");
	Shader lampShader("Shaders/lamp.vs", "Shaders/lamp.frag");
	Shader SkyBoxshader("Shaders/SkyBox.vs", "Shaders/SkyBox.frag");
	Shader animShader("Shaders/anim.vs", "Shaders/anim.frag");

	//Cargando los modelos
	//FACHADA
	Model Fachada((char*)"Models/Fachada/fachada.obj");
	Model Zaguan((char*)"Models/Fachada/garagedoor.obj");
	Model PuertaGarage((char*)"Models/Fachada/doortogarage.obj");
	Model PuertaCasa((char*)"Models/Fachada/housedoor.obj");
	//INTERIOR
	Model Escritorio((char*)"Models/Interior/desktop.obj");
	Model Lavadora((char*)"Models/Interior/washer.obj");
	Model Pizarron((char*)"Models/Interior/board.obj");
	Model Pizarron2((char*)"Models/Interior/board2.obj");
	Model Reloj((char*)"Models/Interior/clock.obj");
	Model Extinguidor((char*)"Models/Interior/extinguisher.obj");
	Model Repisa((char*)"Models/Interior/shelf.obj");
	Model Tuboensayo((char*)"Models/Interior/testtube.obj");
	Model Extras((char*)"Models/Interior/extras.obj");
	//PERSONAJES
	Model Morty((char*)"Models/Personaje/morty.obj");
	//EXTRAS
	Model Carro((char*)"Models/Extra/car.obj");
	Model Label((char*)"Models/Extra/label.obj");
	Model Silla((char*)"Models/Extra/chair.obj");
	Model Portal((char*)"Models/Extra/portal.obj");

	//MODELO ANIMADO
	ModelAnim animacionPersonaje("Animaciones/RickAnimado/macarena.dae");
	animacionPersonaje.initShaders(animShader.Program);

	// Build and compile our shader program

	//Inicializaci�n de KeyFrames
	for(int i=0; i<MAX_FRAMES; i++)
	{
		KeyFrame[i].movChairX = 0;
		KeyFrame[i].movChairZ = 0;
		KeyFrame[i].incChairX = 0;
		KeyFrame[i].incChairZ = 0;
		KeyFrame[i].rotCh = 0;
		KeyFrame[i].rotIncCh = 0;
		KeyFrame[i].rotPortal= 0;
		KeyFrame[i].incPortal = 0;

	}

	// Set up vertex data (and buffer(s)) and attribute pointers
	GLfloat vertices[] =
	{
		// Positions            // Normals              // Texture Coords
		-0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  1.0f,
		0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,  	1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  0.0f,

		0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  1.0f,
		0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  1.0f,
		0.5f, -0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  1.0f,
		0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  1.0f,
		0.5f,  0.5f, -0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  1.0f
	};


	GLfloat skyboxVertices[] = {
		// Positions
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};


	GLuint indices[] =
	{  // Note that we start from 0!
		0,1,2,3,
		4,5,6,7,
		8,9,10,11,
		12,13,14,15,
		16,17,18,19,
		20,21,22,23,
		24,25,26,27,
		28,29,30,31,
		32,33,34,35
	};

	// Positions all containers
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};


	// First, set the container's VAO (and VBO)
	GLuint VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);
	// Normals attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// Texture Coordinate attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);

	// Then, we set the light's VAO (VBO stays the same. After all, the vertices are the same for the light object (also a 3D cube))
	GLuint lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	// We only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Set the vertex attributes (only position data for the lamp))
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)0); // Note that we skip over the other data in our buffer object (we don't need the normals/textures, only positions).
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);


	//SkyBox
	GLuint skyboxVBO, skyboxVAO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1,&skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices),&skyboxVertices,GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT,GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);

	// Load textures
	vector<const GLchar*> faces;
	faces.push_back("SkyBox/right.tga");
	faces.push_back("SkyBox/left.tga");
	faces.push_back("SkyBox/top.tga");
	faces.push_back("SkyBox/bottom.tga");
	faces.push_back("SkyBox/back.tga");
	faces.push_back("SkyBox/front.tga");
	
	GLuint cubemapTexture = TextureLoading::LoadCubemap(faces);

	glm::mat4 projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 1000.0f);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{

		// Calculate deltatime of current frame
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		DoMovement();
		animacion();
		animacionCarro();
		animacionGarage();
		animacionSilla();


		// Clear the colorbuffer
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// Use cooresponding shader when setting uniforms/drawing objects
		lightingShader.Use();
		GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
		glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
		// Set material properties
		glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 32.0f);
		// Setting lights
		// Directional light
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), 0.3f, 0.3f, 0.3f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), 0.3f, 0.3f, 0.3f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), 0.1f, 0.1f, 0.1f);

		// Point light 1
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].diffuse"), LightP1.x, LightP1.y, LightP1.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].specular"), LightP1.x, LightP1.y, LightP1.z);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].quadratic"), 0.032f);

		// Point light 2
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].diffuse"), LightP2.x, LightP2.y, LightP2.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].specular"), 0.1f, 0.1f, 0.1f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].quadratic"), 0.032f);

		// Point light 3
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].diffuse"), LightP3.x, LightP3.y, LightP3.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].specular"), 0.1f, 0.1f, 0.1f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].quadratic"), 0.032f);

		// Point light 4
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].position"), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].diffuse"), 1.0f, 0.0f, 1.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].specular"), 1.0f, 0.0f, 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].quadratic"), 0.032f);

		// SpotLight
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.position"), camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.direction"), camera.GetFront().x, camera.GetFront().y, camera.GetFront().z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.ambient"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.diffuse"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.specular"), 0.0f, 0.0f, 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.quadratic"), 0.032f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.cutOff"), glm::cos(glm::radians(12.5f)));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.outerCutOff"), glm::cos(glm::radians(15.0f)));

		// Set material properties
		glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 32.0f);

		// Create camera transformations
		glm::mat4 view;
		view = camera.GetViewMatrix();

		// Get the uniform locations
		GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(lightingShader.Program, "view");
		GLint projLoc = glGetUniformLocation(lightingShader.Program, "projection");

		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glBindVertexArray(VAO);
		glm::mat4 tmp = glm::mat4(1.0f); //Temp

		//Carga de modelos
		//FACHADA
		view = camera.GetViewMatrix();
		glm::mat4 model(1);
		tmp = model = glm::translate(model, glm::vec3(0, 1, 0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Fachada.Draw(lightingShader);

		//INTERIOR
		//Escritorio 
		view = camera.GetViewMatrix();
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Escritorio.Draw(lightingShader);

		//Lavadora
		view = camera.GetViewMatrix();
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Lavadora.Draw(lightingShader);

		//Pizarron
		view = camera.GetViewMatrix();
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Pizarron.Draw(lightingShader);

		//Pizarron2
		view = camera.GetViewMatrix();
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Pizarron2.Draw(lightingShader);

		//Reloj
		view = camera.GetViewMatrix();
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Reloj.Draw(lightingShader);

		//Extinguidor
		view = camera.GetViewMatrix();
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Extinguidor.Draw(lightingShader);

		//Repisa
		view = camera.GetViewMatrix();
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Repisa.Draw(lightingShader);

		//Tuboensayo
		view = camera.GetViewMatrix();
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Tuboensayo.Draw(lightingShader);

		//Extras
		view = camera.GetViewMatrix();
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Extras.Draw(lightingShader);

		//PERSONAJES
		//Morty
		view = camera.GetViewMatrix();
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Morty.Draw(lightingShader);

		//EXTRAS

		//Label
		view = camera.GetViewMatrix();
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Label.Draw(lightingShader);

		//ANIMADOS
		//Zaguan
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(4.7f, 4.2f, 3.3781f));
		model = glm::rotate(model, glm::radians(rotGarage), glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Zaguan.Draw(lightingShader);

		//Puerta Garage
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-2.8f, 2.23f, -0.6996f));
		model = glm::rotate(model, glm::radians(rotGarageDoor), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		PuertaGarage.Draw(lightingShader);

		//Puerta Casa
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0.96f, 2.23f, -10.47f));
		model = glm::rotate(model, glm::radians(rotHouseDoor), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		PuertaCasa.Draw(lightingShader);

		//Carro
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, PosIniCar + glm::vec3(movCarX, 0.0f, movCarZ));
		model = glm::rotate(model, glm::radians(rotCar), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Carro.Draw(lightingShader);

		//SIlla
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, PosIniChair + glm::vec3(movChairX, 0.0f, movChairZ));
		model = glm::rotate(model, glm::radians(rotCh), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Silla.Draw(lightingShader);

		//Portal
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(12.0f, 3.0f, 3.15));
		model = glm::rotate(model, glm::radians(rotPortal), glm::vec3(1.0f, 0.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Portal.Draw(lightingShader);
		glBindVertexArray(0);

		//Rick Animado
		animShader.Use();
		modelLoc = glGetUniformLocation(animShader.Program, "model");
		viewLoc = glGetUniformLocation(animShader.Program, "view");
		projLoc = glGetUniformLocation(animShader.Program, "projection");

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glUniform3f(glGetUniformLocation(animShader.Program, "material.specular"), 0.2f, 0.2f, 0.2f);
		glUniform1f(glGetUniformLocation(animShader.Program, "material.shininess"), 32.0f);
		glUniform3f(glGetUniformLocation(animShader.Program, "light.ambient"), 0.5f, 0.5f, 0.5f);
		glUniform3f(glGetUniformLocation(animShader.Program, "light.diffuse"), 0.5f, 0.5f, 0.5f);
		glUniform3f(glGetUniformLocation(animShader.Program, "light.specular"), 0.2f, 0.2f, 0.2f);
		glUniform3f(glGetUniformLocation(animShader.Program, "light.direction"), 0.0f, -1.0f, -1.0f);
		view = camera.GetViewMatrix();

		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(8.0f,1.0f,3.0));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.01f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		animacionPersonaje.Draw(animShader);
		glBindVertexArray(0);

		// Draw skybox as last
		glDepthFunc(GL_LEQUAL);  // Change depth function so depth test passes when values are equal to depth buffer's content
		SkyBoxshader.Use();
		view = glm::mat4(glm::mat3(camera.GetViewMatrix()));	// Remove any translation component of the view matrix
		glUniformMatrix4fv(glGetUniformLocation(SkyBoxshader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(SkyBoxshader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // Set depth function back to default

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteBuffers(1, &skyboxVBO);
	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();

	return 0;
}

void animacionCarro() {
	if (circuitoCar) {
		if (rec1) {
			rotCar = 0.0f;
			movCarZ -= 0.1f;
			if (movCarZ < -40.0f) {
				rec1 = false;
				rec2 = true;
			}
		}
		if (rec2) {
			rotCar = -90.0f;
			movCarX += 0.1;
			if (movCarX > 7.0f) {
				rec2 = false;
				rec3 = true;
			}
		}
		if (rec3) {
			rotCar = -180.0f;
			movCarZ += 0.1f;
			if (movCarZ > 0.0f) {
				rec3 = false;
				rec4 = true;
			}
		}
		if (rec4) {
			rotCar = 90.0f;
			movCarX -= 0.1f;
			if (movCarX < 0.0f) {
				rec4 = false;
				rec1 = true;
			}
		}
	}
}

void animacionGarage() {
	if (garageAnim) {
		if (recG1) {
			rotGarage -= 0.5f;
			if (rotGarage < -90.0f) {
				recG1 = false;
				recG2 = true;
			}
		}
		if (recG2) {
			rotGarage += 0.5f;
			if (rotGarage > 0.0f) {
				recG2 = false;
				recG1 = true;
			}
		}
	}
}

void animacionSilla() {
	if (sillaAnim) {
		if (recS1) {
			rotCh = 90.0f;
			movChairX += 0.01f;
			if (movChairX > 1.0f) {
				recS1 = false;
				recS2 = true;
			}
		}
		if (recS2) {
			rotCh = -45.0f;
			movChairX -= 0.01f;
			movChairZ += 0.01f;
			if (movChairZ > 1.0f) {
				recS2 = false;
				recS3 = true;
			}
		}
		if (recS3) {
			rotCh = -180.0f;
			movChairZ -= 0.01f;
			if (movChairZ < 0.0f) {
				recS3 = false;
				recS1 = true;
			}
		}
	}
}

void animacion()
{
	//Movimiento por keyframes
	if (play)
	{
		if (i_curr_steps >= i_max_steps) //end of animation between frames?
		{
		playIndex++;
		if (playIndex>FrameIndex - 2)	//end of total animation?
			{
				printf("termina anim\n");
				playIndex = 0;
				play = false;
			}
		else //Next frame interpolations
			{
				i_curr_steps = 0; //Reset counter
				//Interpolation
				interpolation();
			}
		}
	else
		{
			//Draw animation
			movChairX += KeyFrame[playIndex].incChairX;
			movChairZ += KeyFrame[playIndex].incChairZ;
			rotCh += KeyFrame[playIndex].rotIncCh;
			rotPortal += KeyFrame[playIndex].incPortal;
			i_curr_steps++;
		}
	}
}

// Is called whenever a key is pressed/released via GLFW
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	// Saves all the keyframes 
	if (keys[GLFW_KEY_L])
	{
		if (play == false && (FrameIndex > 1))
		{
			resetElements();
			//First Interpolation				
			interpolation();
			play = true;
			playIndex = 0;
			i_curr_steps = 0;
		}
		else
		{
			play = false;
		}
	}
	// Plays keyframe animations
	if (keys[GLFW_KEY_K])
	{
		if (FrameIndex<MAX_FRAMES)
		{
			saveFrame();
		}
	}
	// Closes principal window
	if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			keys[key] = false;
		}
	}
	// Alternates between lights
	if (keys[GLFW_KEY_SPACE])
	{
		active = !active;
		if (active) {
			LightP1 = glm::vec3(0.5f, 0.0f, 0.0f);
			LightP2 = glm::vec3(0.0f, 0.0f, 0.0f);
			LightP3 = glm::vec3(0.0f, 0.0f, 0.0f);
		}
		else {
			LightP1 = glm::vec3(0.0f, 0.0f, 0.0f);
			LightP2 = glm::vec3(0.7f, 0.7f, 0.7f);
			LightP3 = glm::vec3(0.7f, 0.7f, 0.7f);
		}
	}
	//Turn all the lights off
	if (keys[GLFW_KEY_F]) {
		LightP1 = glm::vec3(0.0f, 0.0f, 0.0f);
		LightP2 = glm::vec3(0.0f, 0.0f, 0.0f);
		LightP3 = glm::vec3(0.0f, 0.0f, 0.0f);
	}
}

// Is called whenever the mouse is moved
void MouseCallback(GLFWwindow *window, double xPos, double yPos)
{

	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	GLfloat xOffset = xPos - lastX;
	GLfloat yOffset = lastY - yPos;  // Reversed since y-coordinates go from bottom to left

	lastX = xPos;
	lastY = yPos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}

// Moves/alters the camera positions based on user input
void DoMovement()
{
	// Opens garage
	if (keys[GLFW_KEY_1]){
		if (rotGarage > -90.0f) {
			rotGarage -= 1.0f;
		}
	}
	// Closes garage
	if (keys[GLFW_KEY_2]){
		if (rotGarage < 0.0f) {
			rotGarage += 1.0f;
		}
	}
	// Opens garage door
	if (keys[GLFW_KEY_3]){
		if (rotGarageDoor < 90.0f) {
			rotGarageDoor += 1.0f;
		}
	}
	// Closes garage door
	if (keys[GLFW_KEY_4]){
		if (rotGarageDoor > 0.0f) {
			rotGarageDoor -= 1.0f;
		}
	}
	// Opens house door
	if (keys[GLFW_KEY_5]){
		if (rotHouseDoor > -100.0f) {
			rotHouseDoor -= 1.0f;
		}
	}
	// Closes house door
	if (keys[GLFW_KEY_6]){
		if (rotHouseDoor < -5.0f) {
			rotHouseDoor += 1.0f;
		}
	}
	// Starts car animation
	if (keys[GLFW_KEY_7]){
		circuitoCar = true;
	}
	// Stops car animation
	if (keys[GLFW_KEY_8]){
		circuitoCar = false;
	}
	// Starts garage animation
	if (keys[GLFW_KEY_9]){
		garageAnim = true;
	}
	// Pauses garage animation
	if (keys[GLFW_KEY_0]){
		garageAnim = false;
	}
	// Starts chair animation
	if (keys[GLFW_KEY_O]) {
		sillaAnim = true;
	}
	// Pauses chair animations
	if (keys[GLFW_KEY_P]) {
		sillaAnim = false;
	}
	// Camera controls
	if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP]){
		camera.ProcessKeyboard(FORWARD, deltaTime);

	}
	if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN]){
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	}
	if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT]){
		camera.ProcessKeyboard(LEFT, deltaTime);
	}
	if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT]){
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}
}
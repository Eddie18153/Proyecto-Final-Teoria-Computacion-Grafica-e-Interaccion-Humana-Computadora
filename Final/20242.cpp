/*---------------------------------------------------------*/
/* ---   Proyecto Final Laboratorio Computacion Grafica   --------------------------*/
/*-----------------------------    2024-2   ----------------------------------------*/
/*------ Alumnos: Garcia Leon Cesar Andre - Gomez Moctezuma Eddie Jovany ---------------*/
/*------ Numeros Cuenta: 315193082                 417009993 ---------------*/

#include <Windows.h>

#include <glad/glad.h>
#include <glfw3.h>						//main
#include <stdlib.h>		
#include <glm/glm.hpp>					//camera y model
#include <glm/gtc/matrix_transform.hpp>	//camera y model
#include <glm/gtc/type_ptr.hpp>
#include <time.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>					//Texture
//Biblioteca de musica
//#include <irrKlang.h>
//using namespace irrklang;

#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>

#include <shader_m.h>
#include <camera.h>
#include <modelAnim.h>
#include <model.h>
#include <Skybox.h>
#include <iostream>
#include <mmsystem.h>



void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void my_input(GLFWwindow* window, int key, int scancode, int action, int mods);
void animate(void);


// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
GLFWmonitor* monitors;

GLuint VBO[3], VAO[3], EBO[3];

//Camera
Camera camera(glm::vec3(0.0f, 20.0f, 40.0f));//posicion inicial que tendra la camara 
float MovementSpeed = 0.1f;
GLfloat lastX = SCR_WIDTH / 2.0f,
		lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;



//Timing
const int FPS = 60;
const int LOOP_TIME = 1000 / FPS; // = 16 milisec // 1000 millisec == 1 sec
double	deltaTime = 0.0f,
lastFrame = 0.0f;

void getResolution(void);
void myData(void);							// De la practica 4
void LoadTextures(void);					// De la práctica 6
unsigned int generateTextures(char*, bool, bool);	// De la práctica 6

//For Keyboard
float	movX = 0.0f,
movY = 0.0f,
movZ = -5.0f,
rotX = 0.0f,
rotbrazoizq = 3.0f;

//Texture
unsigned int	t_onepice,
				t_smile,
				t_toalla,
				t_unam,
				t_white,
				t_ladrillos,
				t_cubo;

//Lighting
glm::vec3 posMiLuz(0.0f, 0.0f, 0.0f);
float myVariable = 0.0f;
glm::vec3 lightPosition(0.0f, 4.0f, -10.0f);
glm::vec3 lightDirection(0.0f, -1.0f, -1.0f);

//cambio de luz
glm::vec3 MiLuz(0.0f, 0.0f, 0.0f);
float contador = 0.0f;

//// Light
glm::vec3 lightColor = glm::vec3(0.7f);
glm::vec3 diffuseColor = lightColor * glm::vec3(0.0f);
glm::vec3 ambientColor = diffuseColor * glm::vec3(0.0f);



// posiciones
float	movAuto_x = 0.0f,
movAuto_z = 0.0f,
orienta = 90.0f;
bool	animacion = false,
recorrido1 = true,
recorrido2 = false,
recorrido3 = false,
recorrido4 = false;


//Keyframes (Manipulación y dibujo)
float	posX = 0.0f,
		posY = 0.0f,
		posZ = 0.0f,
		rotRodIzq = 0.0f,
		giroMonito = 0.0f;
float	incX = 0.0f,
		incY = 0.0f,
		incZ = 0.0f,
		rotRodIzqInc = 0.0f,
		giroMonitoInc = 0.0f;
		

#define MAX_FRAMES 9
int i_max_steps = 60;
int i_curr_steps = 0;
typedef struct _frame
{
	//Variables para GUARDAR Key Frames
	float posX;		//Variable para PosicionX
	float posY;		//Variable para PosicionY
	float posZ;		//Variable para PosicionZ
	float rotRodIzq;
	float giroMonito;

}FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 0;			//introducir número en caso de tener Key guardados
bool play = false;
int playIndex = 0;

void saveFrame(void)
{
	//printf("frameindex %d\n", FrameIndex);
	std::cout << "Frame Index = " << FrameIndex << std::endl;

	KeyFrame[FrameIndex].posX = posX;
	KeyFrame[FrameIndex].posY = posY;
	KeyFrame[FrameIndex].posZ = posZ;

	KeyFrame[FrameIndex].rotRodIzq = rotRodIzq;
	KeyFrame[FrameIndex].giroMonito = giroMonito;

	FrameIndex++;
}

void resetElements(void)
{
	posX = KeyFrame[0].posX;
	posY = KeyFrame[0].posY;
	posZ = KeyFrame[0].posZ;

	rotRodIzq = KeyFrame[0].rotRodIzq;
	giroMonito = KeyFrame[0].giroMonito;
}

void interpolation(void)
{
	incX = (KeyFrame[playIndex + 1].posX - KeyFrame[playIndex].posX) / i_max_steps;
	incY = (KeyFrame[playIndex + 1].posY - KeyFrame[playIndex].posY) / i_max_steps;
	incZ = (KeyFrame[playIndex + 1].posZ - KeyFrame[playIndex].posZ) / i_max_steps;

	rotRodIzqInc = (KeyFrame[playIndex + 1].rotRodIzq - KeyFrame[playIndex].rotRodIzq) / i_max_steps;
	giroMonitoInc = (KeyFrame[playIndex + 1].giroMonito - KeyFrame[playIndex].giroMonito) / i_max_steps;

}

unsigned int generateTextures(const char* filename, bool alfa, bool isPrimitive)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	
	if(isPrimitive)
		stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	else
		stbi_set_flip_vertically_on_load(false); // tell stb_image.h to flip loaded texture's on the y-axis.


	unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
	if (data)
	{
		if (alfa)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		return textureID;
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
		return 100;
	}

	stbi_image_free(data);
}

void LoadTextures()
{
	t_onepice = generateTextures("Texturas/onepice.jpg", 0, true);   /// canal alfa transparencia 
	t_smile = generateTextures("Texturas/awesomeface.png", 1, true);
	t_toalla = generateTextures("Texturas/toalla.tga", 0, true);
	t_unam = generateTextures("Texturas/escudo_unam.jpg", 0, true);
	t_cubo = generateTextures("Texturas/cubo.jpg", 0, true);
	t_ladrillos = generateTextures("Texturas/bricks.jpg", 0, true);
	//This must be the last
	t_white = generateTextures("Texturas/white.jpg", 0, false);
}



void animate(void) 
{
	posMiLuz.x = 150.0f * cos(myVariable);
	posMiLuz.z = 150.0f * sin(myVariable);

	myVariable += 0.10f;

	if (contador >= 0.0f && contador < 10.0f)
		MiLuz = glm::vec3(1.0f, 0.0f, 0.0f);
	if (contador >= 10.0f && contador < 20.0f)
		MiLuz = glm::vec3(1.0f, 1.0f, 1.0f);
	if (contador >= 20.0f && contador < 30.0f)
		MiLuz = glm::vec3(0.0f, 1.0f, 0.0f);
	if (contador >= 30.0f && contador < 40.0f)
		MiLuz = glm::vec3(0.0f, 0.0f, 1.0f);
	if (contador >= 40.0f && contador < 50.0f)
		MiLuz = glm::vec3(1.0f, 1.0f, 0.0f);
	if (contador >= 50.0f)
		contador = 0.0f;
	
	contador += 0.05f;

	if (play)
	{
		if (i_curr_steps >= i_max_steps) //end of animation between frames?
		{
			playIndex++;
			if (playIndex > FrameIndex - 2)	//end of total animation?
			{
				std::cout << "Animation ended" << std::endl;
				//printf("termina anim\n");
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
			
			posX += incX;
			posY += incY;
			posZ += incZ;

			rotRodIzq += rotRodIzqInc;
			giroMonito += giroMonitoInc;

			i_curr_steps++;
		}
	}

	//Vehículo
	if (animacion)
	{
		movAuto_x += 3.0f;
	}
}

void getResolution() {
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	SCR_WIDTH = mode->width;
	SCR_HEIGHT = (mode->height) - 80;
}

void myData() {
	float vertices[] = {
		// positions          // texture coords
		 0.5f,  0.5f, 0.0f,   2.0f, 4.0f, // top right
		 0.5f, -0.5f, 0.0f,   2.0f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, // bottom left
		-0.5f,  0.5f, 0.0f,   0.0f, 4.0f  // top left 
	};
	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	float verticesPiso[] = {
		// positions          // texture coords
		 10.5f,  10.5f, 0.0f,   4.0f, 4.0f, // top right
		 10.5f, -10.5f, 0.0f,   4.0f, 0.0f, // bottom right
		-10.5f, -10.5f, 0.0f,   0.0f, 0.0f, // bottom left
		-10.5f,  10.5f, 0.0f,   0.0f, 4.0f  // top left 
	};
	unsigned int indicesPiso[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	GLfloat verticesCubo[] = {
		//Position				//texture coords
		-0.5f, -0.5f, 0.5f,		0.5f, 0.3333f,	//V0 - Frontal
		0.5f, -0.5f, 0.5f,		0.75f, 0.3333f,	//V1
		0.5f, 0.5f, 0.5f,		0.75f, 0.6666f,	//V5
		-0.5f, -0.5f, 0.5f,		0.5f, 0.3333f,	//V0
		-0.5f, 0.5f, 0.5f,		0.5f, 0.6666f,	//V4
		0.5f, 0.5f, 0.5f,		0.75f, 0.6666f,	//V5

		0.5f, -0.5f, -0.5f,		0.0f, 0.3333f,	//V2 - Trasera
		-0.5f, -0.5f, -0.5f,	0.25f, 0.3333f,	//V3
		-0.5f, 0.5f, -0.5f,		0.25f, 0.666f,	//V7
		0.5f, -0.5f, -0.5f,		0.0f, 0.3333f,	//V2
		0.5f, 0.5f, -0.5f,		0.0f, 0.6666f,	//V6
		-0.5f, 0.5f, -0.5f,		0.25f, 0.666f,	//V7

		-0.5f, 0.5f, 0.5f,		0.5f, 0.6666f,	//V4 - Izq
		-0.5f, 0.5f, -0.5f,		0.25f, 0.666f,	//V7
		-0.5f, -0.5f, -0.5f,	0.25f, 0.3333f,	//V3
		-0.5f, -0.5f, -0.5f,	0.25f, 0.3333f,	//V3
		-0.5f, 0.5f, 0.5f,		0.5f, 0.6666f,	//V4
		-0.5f, -0.5f, 0.5f,		0.5f, 0.3333f,	//V0

		0.5f, 0.5f, 0.5f,		0.75f, 0.6666f,	//V5 - Der
		0.5f, -0.5f, 0.5f,		0.75f, 0.3333f,	//V1
		0.5f, -0.5f, -0.5f,		1.0f, 0.3333f,	//V2
		0.5f, 0.5f, 0.5f,		0.75f, 0.6666f,	//V5
		0.5f, 0.5f, -0.5f,		1.0f, 0.6666f,	//V6
		0.5f, -0.5f, -0.5f,		1.0f, 0.3333f,	//V2

		-0.5f, 0.5f, 0.5f,		0.5f, 0.6666f,	//V4 - Sup
		0.5f, 0.5f, 0.5f,		0.5f, 1.0f,	//V5
		0.5f, 0.5f, -0.5f,		0.25f, 1.0f,	//V6
		-0.5f, 0.5f, 0.5f,		0.5f, 0.6666f,	//V4
		-0.5f, 0.5f, -0.5f,		0.25f, 0.666f,	//V7
		0.5f, 0.5f, -0.5f,		0.25f, 1.0f,	//V6

		-0.5f, -0.5f, 0.5f,		0.5f, 0.3333f,	//V0 - Inf
		-0.5f, -0.5f, -0.5f,	0.25f, 0.3333f,	//V3
		0.5f, -0.5f, -0.5f,		0.25f, 0.0f,	//V2
		-0.5f, -0.5f, 0.5f,		0.5f, 0.3333f,	//V0
		0.5f, -0.5f, -0.5f,		0.25f, 0.0f,	//V2
		0.5f, -0.5f, 0.5f,		0.5f, 0.0f,	//V1
	};

	glGenVertexArrays(3, VAO);
	glGenBuffers(3, VBO);
	glGenBuffers(3, EBO);

	glBindVertexArray(VAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//Para Piso
	glBindVertexArray(VAO[2]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesPiso), verticesPiso, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesPiso), indicesPiso, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


	//PARA CUBO
	glBindVertexArray(VAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesCubo), verticesCubo, GL_STATIC_DRAW);

	/*glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);*/

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

int main() {
	// glfw: initialize and configure
	glfwInit();

	// glfw window creation
	monitors = glfwGetPrimaryMonitor();
	getResolution();

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Proyecto Final Lab CG 2024-2", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwSetWindowPos(window, 0, 30);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, my_input);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	//Mis funciones
	//Datos a utilizar
	LoadTextures();
	myData();
	glEnable(GL_DEPTH_TEST);

	

	// build and compile shaders
	// -------------------------
	Shader myShader("shaders/shader_texture_color.vs", "shaders/shader_texture_color.fs"); //To use with primitives
	Shader staticShader("Shaders/shader_Lights.vs", "Shaders/shader_Lights_mod.fs");	//To use with static models
	Shader skyboxShader("Shaders/skybox.vs", "Shaders/skybox.fs");	//To use with skybox
	Shader animShader("Shaders/anim.vs", "Shaders/anim.fs");	//To use with animated models 
	
	vector<std::string> faces{
		"resources/skybox/right.png",
		"resources/skybox/left.png",
		"resources/skybox/top.png",
		"resources/skybox/bottom.png",
		"resources/skybox/front.png",
		"resources/skybox/back.png"
	};

	Skybox skybox = Skybox(faces);

	// Shader configuration
	// --------------------
	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

	// load models
	// -----------
	//Model piso("resources/objects/piso/piso.obj");
	//Model carro("resources/objects/lambo/carroceria.obj");
	//Model llanta("resources/objects/lambo/Wheel.obj");
	//Model casaVieja("resources/objects/casa/OldHouse.obj");
	//Model cubo("resources/objects/cubo/cube02.obj");
	//Model casaDoll("resources/objects/casa/DollHouse.obj");
	//Model CasaBrujas("resources/objects/Casita/CasaBrujas.obj");
	//Model Parca("resources/objects/parca/parca.obj");
	//Model Cubito("resources/objects/Cubito/cubito.obj");
	//Model Arbol("resources/objects/arbol/arbol.obj");

	// Modelos Centro Comercial

	Model centroComercial1("resources/objects/Centro Comercial/paredes.obj");
	Model centroComercial2("resources/objects/Centro Comercial/pasto.obj");
	Model centroComercial3("resources/objects/Centro Comercial/piso1.obj");
	Model centroComercial4("resources/objects/Centro Comercial/piso2.obj");
	Model centroComercial5("resources/objects/Centro Comercial/barandales.obj");
	Model centroComercial6("resources/objects/Centro Comercial/escaleras.obj");
	Model centroComercial7("resources/objects/Centro Comercial/puertas_tapetes.obj");
	Model centroComercial8("resources/objects/Centro Comercial/fachadas_tiendas.obj");
	Model centroComercial9("resources/objects/Centro Comercial/bote_basura1.obj");
	Model centroComercial10("resources/objects/Tienda de mascotas/animales.obj");
	Model centroComercial11("resources/objects/Centro Comercial Lab/gas.obj");
	Model centroComercial12("resources/objects/Tienda de mascotas/tiendamascotas.obj");

	Model centroComercial13("resources/objects/Tienda Ropa/armario.obj");
	Model centroComercial14("resources/objects/Tienda Ropa/carrito1.obj");
	Model centroComercial15("resources/objects/Tienda Ropa/carrito2.obj");
	Model centroComercial16("resources/objects/Tienda Ropa/carrito3.obj");
	Model centroComercial17("resources/objects/Tienda Ropa/cuadro1.obj");
	Model centroComercial18("resources/objects/Tienda Ropa/cuadro2.obj");
	Model centroComercial19("resources/objects/Tienda Ropa/cuadro3.obj");
	Model centroComercial20("resources/objects/Tienda Ropa/escritorio.obj");
	Model centroComercial21("resources/objects/Tienda Ropa/espejos.obj");
	Model centroComercial22("resources/objects/Tienda Ropa/maniqui1.obj");
	Model centroComercial23("resources/objects/Tienda Ropa/maniqui2.obj");
	Model centroComercial24("resources/objects/Tienda Ropa/maniqui3.obj");
	Model centroComercial25("resources/objects/Tienda Ropa/maniqui4.obj");
	Model centroComercial26("resources/objects/Tienda Ropa/paredes.obj");
	Model centroComercial27("resources/objects/Tienda Ropa/piso.obj");
	Model centroComercial28("resources/objects/Tienda Ropa/puerta.obj");
	Model centroComercial29("resources/objects/Tienda Ropa/sala.obj");
	Model centroComercial30("resources/objects/Tienda Ropa/sillones.obj");
	Model centroComercial31("resources/objects/Tienda Ropa/ventanas.obj");
	Model centroComercial32("resources/objects/Tienda Ropa/vestidor.obj");
	Model centroComercial33("resources/objects/Centro Comercial/fachadas1.obj");
	Model centroComercial34("resources/objects/Centro Comercial/fachadas2_2.obj");
	Model centroComercial35("resources/objects/Centro Comercial/bote_basura2.obj");
	Model centroComercial36("resources/objects/Centro Comercial/fachadas4.obj");
	Model centroComercial37("resources/objects/Centro Comercial/columna.obj");
	Model centroComercial38("resources/objects/Centro Comercial/fachada7.obj");
	Model centroComercial39("resources/objects/Centro Comercial/fachada8.obj");
	Model centroComercial40("resources/objects/Centro Comercial/banca1.obj");
	Model centroComercial41("resources/objects/Centro Comercial/banca2.obj");
	Model centroComercial42("resources/objects/Centro Comercial/banca3.obj");
	Model centroComercial43("resources/objects/Centro Comercial/banca4.obj");
	Model centroComercial44("resources/objects/Centro Comercial/banca5.obj");
	Model centroComercial45("resources/objects/Centro Comercial/banca6.obj");
	Model centroComercial46("resources/objects/Centro Comercial/banca7.obj");
	Model centroComercial47("resources/objects/Centro Comercial/banos2.obj");
	Model centroComercial48("resources/objects/Tienda de juguetes/anaquelesjuguetes.obj");
	Model centroComercial49("resources/objects/Tienda de juguetes/juguetes.obj");
	Model centroComercial50("resources/objects/Tienda de juguetes/puerta.obj");
	Model centroComercial51("resources/objects/Tienda de juguetes/tiendajuguetes.obj");
	Model centroComercial52("resources/objects/locales2/locales2dopiso.obj");
	Model centroComercial53("resources/objects/fastfood/sillasfast.obj");
	Model centroComercial54("resources/objects/Banco/atm.obj");
	Model centroComercial55("resources/objects/carrosplaza/carrosplaza.obj");
	Model centroComercial56("resources/objects/carrosplaza/carrosplaza.obj");
	Model centroComercial57("resources/objects/calle/calle/calle.obj");
	Model centroComercial58("resources/objects/calle/edificio1/edificio1.obj");
	Model centroComercial59("resources/objects/fastfood/fastfood2.obj");
	Model centroComercial60("resources/objects/calle/edificio3/edificio3.obj");
	Model centroComercial61("resources/objects/calle/edificio4/edificio4.obj");
	Model centroComercial62("resources/objects/calle/edificio5/edificio5.obj");
	Model centroComercial63("resources/objects/calle/edificio6/edificio6.obj");
	Model centroComercial64("resources/objects/calle/elementoscalle/elementoscalle1.obj");
	Model centroComercial65("resources/objects/calle/elementoscalle/elementoscalle2.obj");
	Model centroComercial66("resources/objects/calle/elementoscalle/elementoscalle3.obj");
	Model centroComercial67("resources/objects/calle/elementoscalle/elementoscalle4.obj");
	Model centroComercial68("resources/objects/Tienda de mascotas/estantemas.obj");
	Model centroComercial69("resources/objects/calle/edificio7/edificio7.obj");
	Model centroComercial70("resources/objects/calle/edificio8/edificio8.obj");
	Model centroComercial71("resources/objects/carros2/carros2.obj");
	Model centroComercial72("resources/objects/calle/calle/peatones.obj");
	Model centroComercial73("resources/objects/people/people1.obj");
	Model centroComercial74("resources/objects/people/people2.obj");






	//ModelAnim animacionPersonaje("resources/objects/Personaje1/Arm.dae");
	//animacionPersonaje.initShaders(animShader.ID);

	//ModelAnim elvis("resources/objects/Elvis/elvis.dae");
	//elvis.initShaders(animShader.ID);

	//configuracion del sonido
	
	//ISoundEngine* engine = createIrrKlangDevice();
	//ISoundEngine* engine2 = createIrrKlangDevice();
	//engine->play2D("media/centrocomercial.mp3", true);



	//Inicialización de KeyFrames
	for (int i = 0; i < MAX_FRAMES; i++)
	{
		KeyFrame[i].posX = 0;
		KeyFrame[i].posY = 0;
		KeyFrame[i].posZ = 0;
		KeyFrame[i].rotRodIzq = 0;
		KeyFrame[i].giroMonito = 0;
	}


	// create transformations and Projection
	glm::mat4 modelOp = glm::mat4(1.0f);		// initialize Matrix, Use this matrix for individual models
	glm::mat4 viewOp = glm::mat4(1.0f);		//Use this matrix for ALL models
	glm::mat4 projectionOp = glm::mat4(1.0f);	//This matrix is for Projection

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		skyboxShader.setInt("skybox", 0);

		// per-frame time logic
		// --------------------
		lastFrame = SDL_GetTicks();

		// input
		// -----
		//my_input(window);
		animate();

		// render
		// ------
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



		// don't forget to enable shader before setting uniforms
		//Setup shader for static models
		staticShader.use();
		//Setup Advanced Lights
		staticShader.setVec3("viewPos", camera.Position);
		staticShader.setVec3("dirLight.direction", lightDirection); /// para representar luz como el sol
		staticShader.setVec3("dirLight.ambient", glm::vec3(1.0f));
		staticShader.setVec3("dirLight.diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("dirLight.specular", glm::vec3(0.0f, 0.0f, 0.0f));
		/// fuente de luz posicional
		staticShader.setVec3("pointLight[0].position", lightPosition);
		staticShader.setVec3("pointLight[0].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[0].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[0].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		// valores de atenuacion
		staticShader.setFloat("pointLight[0].constant", 0.08f);// amplifica la luz entre menor sea mas potente
		staticShader.setFloat("pointLight[0].linear", 0.009f);
		staticShader.setFloat("pointLight[0].quadratic", 0.032f);

		staticShader.setVec3("pointLight[1].position", glm::vec3(-80.0, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[1].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[1].constant", 0.08f);
		staticShader.setFloat("pointLight[1].linear", 0.009f);
		staticShader.setFloat("pointLight[1].quadratic", 0.032f);

		staticShader.setVec3("pointLight[2].position", posMiLuz);
		//staticShader.setVec3("pointLight[2].ambient", glm::vec3(0.8f, 0.0f, 0.0f));
		//staticShader.setVec3("pointLight[2].diffuse", glm::vec3(1.0f, 0.0f, 0.0f));
		//staticShader.setVec3("pointLight[2].ambient", MiLuz);
		//staticShader.setVec3("pointLight[2].diffuse", MiLuz);
		staticShader.setVec3("pointLight[2].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[2].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("pointLight[2].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("pointLight[2].constant", 0.001f);
		staticShader.setFloat("pointLight[2].linear", 0.009f);
		staticShader.setFloat("pointLight[2].quadratic", 0.00032f);

		staticShader.setVec3("spotLight[0].position", glm::vec3(camera.Position.x, camera.Position.y, camera.Position.z));
		staticShader.setVec3("spotLight[0].direction", camera.Front);
		staticShader.setVec3("spotLight[0].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("spotLight[0].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setVec3("spotLight[0].specular", glm::vec3(0.0f, 0.0f, 0.0f));
		staticShader.setFloat("spotLight[0].cutOff", glm::cos(glm::radians(10.0f)));
		staticShader.setFloat("spotLight[0].outerCutOff", glm::cos(glm::radians(12.0f)));
		staticShader.setFloat("spotLight[0].constant", 0.01f);
		staticShader.setFloat("spotLight[0].linear", 0.0009f);
		staticShader.setFloat("spotLight[0].quadratic", 0.0005f);

		staticShader.setFloat("material_shininess", 32.0f);

		//glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 tmp = glm::mat4(1.0f);
		// view/projection transformations
		//glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
		projectionOp = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
		viewOp = camera.GetViewMatrix();
		staticShader.setMat4("projection", projectionOp);
		staticShader.setMat4("view", viewOp);

		


		//Setup shader for primitives
		myShader.use();
		// view/projection transformations
		//projectionOp = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 400.0f);
		viewOp = camera.GetViewMatrix();
		// pass them to the shaders
		//myShader.setMat4("model", modelOp);
		myShader.setMat4("view", viewOp);
		// note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
		myShader.setMat4("projection", projectionOp);
		/**********/


		// -------------------------------------------------------------------------------------------------------------------------
		// Personaje Animacion
		// -------------------------------------------------------------------------------------------------------------------------
		//Remember to activate the shader with the animation
		animShader.use();
		animShader.setMat4("projection", projectionOp);
		animShader.setMat4("view", viewOp);

		animShader.setVec3("material.specular", glm::vec3(0.5f));
		animShader.setFloat("material.shininess", 32.0f);
		animShader.setVec3("light.ambient", glm::vec3(0.8f));
		animShader.setVec3("light.diffuse", diffuseColor);
		animShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		animShader.setVec3("light.direction", lightDirection);
		animShader.setVec3("viewPos", camera.Position);
/*
		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-40.3f, 1.75f, 0.3f)); // translate it down so it's at the center of the scene
		modelOp = glm::scale(modelOp, glm::vec3(0.05f));	// it's a bit too big for our scene, so scale it down
		modelOp = glm::rotate(modelOp, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		animShader.setMat4("model", modelOp);
		animacionPersonaje.Draw(animShader);*/

		// -------------------------------------------------------------------------------------------------------------------------
		// Segundo Personaje Animacion
		// -------------------------------------------------------------------------------------------------------------------------
/*
		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, 0.0f, 15.0f)); // translate it down so it's at the center of the scene
		modelOp = glm::scale(modelOp, glm::vec3(0.05f));	// it's a bit too big for our scene, so scale it down
		animShader.setMat4("model", modelOp);
		elvis.Draw(animShader);*/


		// -------------------------------------------------------------------------------------------------------------------------
		// Escenario Primitivas
		// -------------------------------------------------------------------------------------------------------------------------
		myShader.use();

		//Tener Piso como referencia
		glBindVertexArray(VAO[2]);
		//Colocar código aquí
		/*modelOp = glm::scale(glm::mat4(1.0f), glm::vec3(40.0f, 2.0f, 40.0f));
		modelOp = glm::translate(modelOp, glm::vec3(0.0f, -1.0f, 0.0f));
		modelOp = glm::rotate(modelOp, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		myShader.setMat4("model", modelOp);
		myShader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
		glBindTexture(GL_TEXTURE_2D, t_ladrillos);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);*/

		glBindVertexArray(VAO[0]);
		//Colocar código aquí
		/*modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 10.0f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(5.0f, 5.0f, 1.0f));
		myShader.setMat4("model", modelOp);
		myShader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
		glBindTexture(GL_TEXTURE_2D, t_onepice);
		//glDrawArrays(GL_TRIANGLES, 0, 36); //A lonely cube :(
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);*/

		/***   Segundo objeto  **/
		
		/*glBindVertexArray(VAO[1]);
		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 10.0f, 10.0f));
		myShader.setMat4("model", modelOp);
		myShader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
		glBindTexture(GL_TEXTURE_2D, t_cubo);
		//glDrawArrays(GL_TRIANGLES, 0, 36); //A lonely cube :(
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);*/
		
		glBindVertexArray(0);
		// ------------------------------------------------------------------------------------------------------------------------
		// Termina Escenario Primitivas
		// -------------------------------------------------------------------------------------------------------------------------

		// -------------------------------------------------------------------------------------------------------------------------
		// Escenario
		// -------------------------------------------------------------------------------------------------------------------------
		staticShader.use();
		staticShader.setMat4("projection", projectionOp);
		staticShader.setMat4("view", viewOp);

/*
		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-40.0f, 0.0f, 50.0f));
		modelOp = glm::scale(modelOp, glm::vec3(3.5f));
		staticShader.setMat4("model", modelOp);
		//CasaBrujas.Draw(staticShader);



		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(250.0f, 0.0f, -10.0f));
		modelOp = glm::rotate(modelOp, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		//casaDoll.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.75f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.2f));
		staticShader.setMat4("model", modelOp);
		piso.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -70.0f));
		modelOp = glm::scale(modelOp, glm::vec3(5.0f));
		staticShader.setMat4("model", modelOp);
		staticShader.setVec3("dirLight.specular", glm::vec3(0.0f, 0.0f, 0.0f));
		//casaVieja.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(40.0f, 0.0f, 70.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.5f));
		staticShader.setMat4("model", modelOp);
		staticShader.setVec3("dirLight.specular", glm::vec3(0.0f, 0.0f, 0.0f));
		//Parca.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(100.0f, 0.0f, 70.0f));
		modelOp = glm::scale(modelOp, glm::vec3(5.0f));
		staticShader.setMat4("model", modelOp);
		staticShader.setVec3("dirLight.specular", glm::vec3(0.0f, 0.0f, 0.0f));
		//Arbol.Draw(staticShader);
		

		*/

		// -------------------------------------------------------------------------------------------------------------------------
		// Carro
		// -------------------------------------------------------------------------------------------------------------------------
		//modelOp = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	/*	modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(movAuto_x, -1.0f, movAuto_z - 15.0f));
		tmp = modelOp = glm::rotate(modelOp, glm::radians(orienta), glm::vec3(0.0f, 1.0f, 0.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.1f, 0.1f, 0.1f));
		staticShader.setVec3("dirLight.specular", glm::vec3(0.6f, 0.6f, 0.6f));
		staticShader.setMat4("model", modelOp);
		carro.Draw(staticShader);

		modelOp = glm::translate(tmp, glm::vec3(8.5f, 2.5f, 12.9f));
		modelOp = glm::scale(modelOp, glm::vec3(0.1f, 0.1f, 0.1f));
		staticShader.setMat4("model", modelOp);
		llanta.Draw(staticShader);	//Izq delantera

		modelOp = glm::translate(tmp, glm::vec3(-8.5f, 2.5f, 12.9f));
		modelOp = glm::scale(modelOp, glm::vec3(0.1f, 0.1f, 0.1f));
		modelOp = glm::rotate(modelOp, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		llanta.Draw(staticShader);	//Der delantera

		modelOp = glm::translate(tmp, glm::vec3(-8.5f, 2.5f, -14.5f));
		modelOp = glm::scale(modelOp, glm::vec3(0.1f, 0.1f, 0.1f));
		modelOp = glm::rotate(modelOp, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		llanta.Draw(staticShader);	//Der trasera

		modelOp = glm::translate(tmp, glm::vec3(8.5f, 2.5f, -14.5f));
		modelOp = glm::scale(modelOp, glm::vec3(0.1f, 0.1f, 0.1f));
		staticShader.setMat4("model", modelOp);
		llanta.Draw(staticShader);	//Izq trase*/
		// -------------------------------------------------------------------------------------------------------------------------
		// Personaje
		// -------------------------------------------------------------------------------------------------------------------------


		
		// -------------------------------------------------------------------------------------------------------------------------
		// Just in case
		// -------------------------------------------------------------------------------------------------------------------------
		/*modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(posX, posY, posZ));
		tmp = modelOp = glm::rotate(modelOp, glm::radians(giroMonito), glm::vec3(0.0f, 1.0f, 0.0));
		staticShader.setMat4("model", modelOp);
		torso.Draw(staticShader);

		//Pierna Der
		modelOp = glm::translate(tmp, glm::vec3(-0.5f, 0.0f, -0.1f));
		modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0));
		modelOp = glm::rotate(modelOp, glm::radians(-rotRodIzq), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		piernaDer.Draw(staticShader);

		//Pie Der
		modelOp = glm::translate(modelOp, glm::vec3(0, -0.9f, -0.2f));
		staticShader.setMat4("model", modelOp);
		botaDer.Draw(staticShader);

		//Pierna Izq
		modelOp = glm::translate(tmp, glm::vec3(0.5f, 0.0f, -0.1f));
		modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		piernaIzq.Draw(staticShader);

		//Pie Iz
		modelOp = glm::translate(modelOp, glm::vec3(0, -0.9f, -0.2f));
		staticShader.setMat4("model", modelOp);
		botaDer.Draw(staticShader);	//Izq trase

		//Brazo derecho
		modelOp = glm::translate(tmp, glm::vec3(0.0f, -1.0f, 0.0f));
		modelOp = glm::translate(modelOp, glm::vec3(-0.75f, 2.5f, 0));
		modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		brazoDer.Draw(staticShader);

		//Brazo izquierdo
		modelOp = glm::translate(tmp, glm::vec3(0.0f, -1.0f, 0.0f));
		modelOp = glm::translate(modelOp, glm::vec3(0.75f, 2.5f, 0));
		modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		staticShader.setMat4("model", modelOp);
		brazoIzq.Draw(staticShader);

		//Cabeza
		modelOp = glm::translate(tmp, glm::vec3(0.0f, -1.0f, 0.0f));
		modelOp = glm::rotate(modelOp, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0));
		modelOp = glm::translate(modelOp, glm::vec3(0.0f, 2.5f, 0));
		staticShader.setMat4("model", modelOp);
		cabeza.Draw(staticShader);*/
		

		/*********************************************** Modelos Centro Comercial *********************************************/
		
		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial1.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial2.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial3.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial4.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial5.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial6.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial7.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial8.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial9.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial10.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial11.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial12.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial13.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial14.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial15.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial16.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial17.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial18.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial19.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial20.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial21.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial22.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial23.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial24.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial25.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial26.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial27.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial28.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial29.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial30.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial31.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial32.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial33.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial34.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial35.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial36.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial37.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial38.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial39.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial40.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial41.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial42.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial43.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial44.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial45.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial46.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial47.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial48.Draw(staticShader);


		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial49.Draw(staticShader);


		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial50.Draw(staticShader);
		

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial51.Draw(staticShader);


		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial52.Draw(staticShader);


		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial53.Draw(staticShader);


		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial54.Draw(staticShader);


		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial55.Draw(staticShader);


		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial56.Draw(staticShader);


		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial57.Draw(staticShader);


		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial58.Draw(staticShader);
		
	
		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial59.Draw(staticShader);


		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial60.Draw(staticShader);


		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial61.Draw(staticShader);


		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial62.Draw(staticShader);


		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial63.Draw(staticShader);


		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial64.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial65.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial66.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial67.Draw(staticShader); 

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial68.Draw(staticShader);
	
		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial69.Draw(staticShader);	

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial70.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial71.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial72.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial73.Draw(staticShader);

		modelOp = glm::translate(glm::mat4(1.0f), glm::vec3(-150.0f, 0.0f, 100.0f));
		modelOp = glm::scale(modelOp, glm::vec3(0.3f));
		staticShader.setMat4("model", modelOp);
		centroComercial74.Draw(staticShader);

		

		



		//-------------------------------------------------------------------------------------
		// draw skybox as last
		// -------------------
		skyboxShader.use();
		skybox.Draw(skyboxShader, viewOp, projectionOp, camera);

		// Limitar el framerate a 60
		deltaTime = SDL_GetTicks() - lastFrame; // time for full 1 loop
		if (deltaTime < LOOP_TIME)
		{
			SDL_Delay((int)(LOOP_TIME - deltaTime));
		}

		
		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glDeleteVertexArrays(2, VAO);
	glDeleteBuffers(2, VBO);
	//skybox.Terminate();
	glfwTerminate();
	//engine->drop();


	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void my_input(GLFWwindow* window, int key, int scancode, int action, int mode) 
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, (float)deltaTime);

	//To Configure Model
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
		posZ++;
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
		posZ--;
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
		posX--;
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		posX++;
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
		rotRodIzq--;
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
		rotRodIzq++;
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
		giroMonito--;
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
		giroMonito++;
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
		lightPosition.x++;
	//if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
		lightPosition.x--;


	//if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
		rotbrazoizq += +3.0;
	//if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
		rotbrazoizq += -3.0;

	//Car animation
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		animacion ^= true;

	//To play KeyFrame animation 
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
	{
		if (play == false && (FrameIndex > 1))
		{
			std::cout << "Play animation" << std::endl;
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
			std::cout << "Not enough Key Frames" << std::endl;
		}
	}

	//To Save a KeyFrame
	if (key == GLFW_KEY_L && action == GLFW_PRESS)
	{
		if (FrameIndex < MAX_FRAMES)
		{
			saveFrame();
		}
	}

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xpos, double ypos) 
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	double xoffset = xpos - lastX;
	double yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(yoffset);
}
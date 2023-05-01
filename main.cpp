//libraries/header files including
#include <iostream>
#include <fstream>
#include <istream>
#include <string>
#include "imgui.h"
#include "imgui_internal.h"
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include "Account.h"
#define STB_IMPLAMENTATION
#include <stb\stb_image.h>
#include <glad\glad.h>
#include <GLFW\glfw3.h>

//constants
GLFWwindow* window;
const int window_Width = 800;
const int window_Height = 600;
const char* GAME_NAME = "Mirage Source";
const int TILE_SIZE = 32;
const int MAP_WIDTH = 20;
const int MAP_HEIGHT = 15;

GLuint backgroundTextureID;
const char* backgroundTexturePath = "background.png";
GLuint tileTextureID;
const char* tileTexturePath = "tiles.png";

std::vector<std::vector<int>> map(MAP_HEIGHT, std::vector<int>(MAP_WIDTH, 0));

/*bool mainMenu = false;
bool Login = false;
bool cAccount = false;
bool dAccount = false;
bool cChar = false;
bool dChar = false;
*/
//function calls
void initGLFW(GLFWwindow*& window);
void initUI(GLFWwindow*& window);
void cleanupUI();
void cleanupGLFW();

enum class menuState
{
	main,
	cAcc,
	cChar,
	dAcc,
	dChar,
	login,
	credits,
	exit
};
static char username[32] = "";
static char password[32] = "";
static char email[64] = "";
static char confirm_password[32] = "";
static bool terms_accepted = false;

void initGLFW(GLFWwindow*& window)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(window_Width, window_Height, GAME_NAME, NULL, NULL);
	glfwSetFramebufferSizeCallback(window, [](GLFWwindow*, int width, int height) {
		glViewport(0, 0, width, height);
		});
	glfwMakeContextCurrent(window);
}

void initUI(GLFWwindow*& window)
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
	//	ImGui::StyleColorsDark();  <- use normal ImGui Colors for menu's
		//change color of styles
	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.2f, 0.2f, 0.2f, 0.8f);
	//	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.1f, 0.1f, 0.1f, 0.8f);
	//	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.3f, 0.3f, 0.3f, 0.8f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.3f, 0.3f, 0.3f, 0.8f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.4f, 0.4f, 0.4f, 0.8f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.5f, 0.5f, 0.5f, 0.8f);

	//change size of the style
//	style.WindowBorderSize = 1.0f;
//	style.WindowRounding = 0.0f;
//	style.FramePadding = ImVec2(4.0f, 4.0f);

	// adding image background instead
}

void cleanupUI()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void cleanupGLFW()
{
	glfwDestroyWindow(window);
	glfwTerminate();

}
GLuint loadTexture(const char* path)
{
	int width, height, nrChannels;
	unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
	if (!data)
	{
		std::cerr << "Failed to load texture: " << path << std::endl;
		exit(EXIT_FAILURE);
	}
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);
	return textureID;
}

void initTextures()
{
	backgroundTextureID = loadTexture(backgroundTexturePath);
	tileTextureID = loadTexture(tileTexturePath);
}

void drawTexture(GLuint textureID, float x, float y, float width, float height)
{
	glBindTexture(GL_TEXTURE_2D, textureID);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex2f(x, y);
	glTexCoord2f(1.0f, 0.0f); glVertex2f(x + width, y);
	glTexCoord2f(1.0f, 0.0f); glVertex2f(x + width, y + height);
	glTexCoord2f(1.0f, 0.0f); glVertex2f(x, y + height);
	glEnd();
}

void drawBackground()
{
	drawTexture(backgroundTextureID, 0, 0, window_Width, window_Height);
}

void drawTile(int tileIndex, float x, float y)
{
	int row = tileIndex / (tileTextureID / TILE_SIZE);
	int col = tileIndex % (tileTextureID / TILE_SIZE);
	float texCoordX = col * TILE_SIZE / (float)tileTextureID;
	float texCoordY = row * TILE_SIZE / (float)tileTextureID;
	drawTexture(tileTextureID, x, y, TILE_SIZE, TILE_SIZE);
}

void drawMap()
{
	float x;
	float y;;
	for (int row = 0; row < MAP_HEIGHT; row++)
	{
		for (int col = 0; col < MAP_WIDTH; col++)
		{
			int tileIndex = map[row][col];
			x = col * TILE_SIZE;
			y = row * TILE_SIZE;
			drawTile(tileIndex, x, y);
		}
	}
}
void saveMap()
{
	std::ofstream file("map.dat");
	for (int row = 0; row < MAP_HEIGHT; row++)
	{
		for (int col = 0; col < MAP_WIDTH; col++)
		{
			file << map[row][col] << " ";
		}
		file << std::endl;
	}
}

void loadMap()
{
	std::ofstream file("map.dat");
	if (!file)
	{
		std::cerr << "Failed to load map from file" << std::endl;
		return;
	}
	for (int row = 0; row < MAP_HEIGHT; row++)
	{
		for (int col = 0; col < MAP_WIDTH; col++)
		{
			file << map[row][col] << " ";
		}
		file << std::endl;
	}
}

void drawMapGrid()
{
	ImVec2 windowPos = ImGui::GetCursorScreenPos();
	ImVec2 cellSize(TILE_SIZE, TILE_SIZE);
	for (int row = 0; row < MAP_HEIGHT; row++)
	{
		for (int col = 0; col < MAP_WIDTH; col++)
		{
			ImVec2 cellPos = ImVec2(windowPos.x + col * cellSize.x, windowPos.y + row * cellSize.y);
			ImGui::GetWindowDrawList()->AddRect(cellPos, ImVec2(cellPos.x + cellSize.x, cellPos.y + cellSize.y), IM_COL32_WHITE);
		}
	}
	ImGui::End();
}

int main()
{
	GLFWwindow* window;

	//init glfw
	initGLFW(window);
	//init opengl loader/viewport
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	//init GUI
	initUI(window);
	initTextures();
	loadMap();
	menuState currentMenuState = menuState::main;
	while (!glfwWindowShouldClose(window))
	{

		//poll for events
		glfwPollEvents();

		//start the GUI frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		//draw UI here
		ImGui::SetNextWindowSize(ImVec2(700, 500));
		ImGui::SetNextWindowPos(ImVec2(50, 50));
		ImGui::BeginChild("##LeftSide", ImVec2(200, 475), true);
		//imgui image
//		ImTextureID myTextureID = (ImTextureID)(intptr_t)myTexture;
//		ImGui::Image(myTextureID, ImVec2(imageWidth, imageHeight));
		drawBackground();
		drawMap();
		drawMapGrid();

		ImGui::EndChild();
		ImGui::SameLine();
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
		ImGui::SameLine();
		ImGui::BeginChild("##RightSide", ImVec2(465, 475), true);
		//add gui here
		if (ImGui::Button("Save"))
		{
			saveMap();
		}
		if (ImGui::Button("Load"))
		{
			loadMap();
		}
/*		switch (currentMenuState)
		{
		case menuState::main:
		{
			ImGui::SetNextWindowSize(ImVec2(400, 400));
			ImGui::BeginChild("Main Menu");
			if (ImGui::Button("Login"))
			{
				currentMenuState = menuState::login;
			}
			if (ImGui::Button("Create Account"))
			{
				currentMenuState = menuState::cAcc;
			}
			if (ImGui::Button("Exit"))
			{
				currentMenuState = menuState::exit;
			}
			ImGui::EndChild();
			break;
		}
		case menuState::login:
		{	ImGui::SetNextWindowSize(ImVec2(400, 400));
			ImGui::BeginChild("Login");
			ImGui::InputText("Username", username, 32);
			ImGui::InputText("Password", password, 32);
			ImGui::Button("Login");
			if (ImGui::Button("Cnacel"))
			{
				currentMenuState = menuState::main;
			}
			ImGui::EndChild();

		}

		}
		ImGui::EndChild();*/
		//imgui image
		// ImGui::GetWindowDrawList() -> AddImage(texture_id, window_pos, ImVec2(window_pos.x + window_size.x, window_pos.y + window_size.y));
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
	}

	cleanupUI();
	cleanupGLFW();
	return 0;

}
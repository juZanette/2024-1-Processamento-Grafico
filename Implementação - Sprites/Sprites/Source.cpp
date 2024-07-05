/* Hello Triangle - c�digo adaptado de https://learnopengl.com/#!Getting-started/Hello-Triangle
 *
 * Adaptado por Rossana Baptista Queiroz
 * para a disciplina de Processamento Gr�fico - Unisinos
 * Vers�o inicial: 7/4/2017
 * �ltima atualiza��o em 14/08/2023
 *
 */

#include <iostream>
#include <string>
#include <assert.h>

 // STB IMAGE
#include <stb_image.h>

using namespace std;

// Classe para manipula��o dos shaders
#include "Shader.h"

// Classe para manipula��o das sprites
#include "Sprite.h"

// Prot�tipos das fun��es
GLuint loadTexture(string texturePath);

// Dimens�es da janela (pode ser alterado em tempo de execu��o)
const GLuint WIDTH = 800, HEIGHT = 600;

// Vari�veis globais
Sprite spr1, spr2, background, spr3;

// Fun��o MAIN
int main()
{
    // Inicializa��o da GLFW
    glfwInit();

    // Inicializa��o da semente para gera��o de n�meros aleat�rios
    srand((int)glfwGetTime());

    // Cria��o da janela GLFW
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "M4 - Mapeamento de Texturas", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    // GLAD: carrega todos os ponteiros de fun��es da OpenGL
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "Failed to initialize GLAD" << endl;
    }

    // Obtendo as informa��es de vers�o
    const GLubyte* renderer = glGetString(GL_RENDERER); /* get renderer string */
    const GLubyte* version = glGetString(GL_VERSION); /* version as a string */
    cout << "Renderer: " << renderer << endl;
    cout << "OpenGL version supported " << version << endl;

    // Habilitar teste de profundidade
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_ALWAYS); // a cada ciclo

    // Habilitar o modo de transpar�ncia (blend - mistura de cores)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Compilando e buildando o programa de shader
    Shader shader("../shaders/tex_vert.glsl", "../shaders/tex_frag.glsl");

    GLuint texID1 = loadTexture("../../Textures/imagem2.png");
    GLuint texID2 = loadTexture("../../Textures/imagem1.png");
    GLuint texID3 = loadTexture("../../Textures/fundo1.jpg");
    GLuint texID4 = loadTexture("../../Textures/imagem3.png");

    // Cria��o das sprites
    spr1.setShader(&shader);
    spr1.inicializar(texID2, glm::vec3(600.0, 150.0, 0.0), glm::vec3(128, 128, 1.0));

    spr2.setShader(&shader);
    spr2.inicializar(texID1, glm::vec3(400.0, 250.0, 0.0), glm::vec3(128, 128, 1.0));

    background.setShader(&shader);
    background.inicializar(texID3, glm::vec3(400.0, 300.0, 0.0), glm::vec3(1920.0 / 2.0, 1080.0 / 2.0, 1.0));

    spr3.setShader(&shader);
    spr3.inicializar(texID4, glm::vec3(500.0, 450.0, 0.0), glm::vec3(128, 128, 1.0));

    // Ativando o buffer de textura 0 da OpenGL
    glActiveTexture(GL_TEXTURE0);

    shader.Use();

    // Matriz de proje��o paralela ortogr�fica
    glm::mat4 projection = glm::ortho(0.0, 800.0, 0.0, 600.0, -1.0, 1.0);
    // Enviando para o shader a matriz como uma var uniform
    shader.setMat4("projection", glm::value_ptr(projection));

    // Matriz de transforma��o do objeto (matriz de modelo)
    shader.setInt("texBuffer", 0);

    // Loop da aplica��o - "game loop"
    while (!glfwWindowShouldClose(window))
    {
        // Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as fun��es de callback correspondentes
        glfwPollEvents();

        // Definindo as dimens�es da viewport com as mesmas dimens�es da janela da aplica��o
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height); // unidades de tela: pixel

        // Limpa o buffer de cor
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f); // cor de fundo
        glClear(GL_COLOR_BUFFER_BIT);

        background.desenhar();

        spr1.desenhar();
        spr2.desenhar();
        spr3.desenhar();
 
        // Troca os buffers da tela
        glfwSwapBuffers(window);
    }

    // Finaliza a execu��o da GLFW, limpando os recursos alocados por ela
    glfwTerminate();
    return 0;
}

GLuint loadTexture(string texturePath)
{
    GLuint texID;

    // Gera o identificador da textura na mem�ria
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    // Configura��o do par�metro WRAPPING nas coords s e t
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Configura��o do par�metro FILTERING na minifica��o e magnifica��o da textura
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);

    if (data)
    {
        if (nrChannels == 3) // jpg, bmp
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        else // png
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);

    return texID;
}

/* Hello Triangle - código adaptado de https://learnopengl.com/#!Getting-started/Hello-Triangle
 *
 * Adaptado por Rossana Baptista Queiroz
 * para a disciplina de Processamento Gráfico - Unisinos
 * Versão inicial: 7/4/2017
 * Última atualização em 14/08/2023
 *
 */

#include <iostream>
#include <string>
#include <assert.h>

 // STB IMAGE
#include <stb_image.h>

using namespace std;

// Classe para manipulação dos shaders
#include "Shader.h"

// Classe para manipulação das sprites
#include "Sprite.h"

// Protótipos das funções
GLuint loadTexture(string texturePath);

// Dimensões da janela (pode ser alterado em tempo de execução)
const GLuint WIDTH = 800, HEIGHT = 600;

// Variáveis globais
Sprite spr1, spr2, background, spr3;

// Função MAIN
int main()
{
    // Inicialização da GLFW
    glfwInit();

    // Inicialização da semente para geração de números aleatórios
    srand((int)glfwGetTime());

    // Criação da janela GLFW
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "M4 - Mapeamento de Texturas", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    // GLAD: carrega todos os ponteiros de funções da OpenGL
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "Failed to initialize GLAD" << endl;
    }

    // Obtendo as informações de versão
    const GLubyte* renderer = glGetString(GL_RENDERER); /* get renderer string */
    const GLubyte* version = glGetString(GL_VERSION); /* version as a string */
    cout << "Renderer: " << renderer << endl;
    cout << "OpenGL version supported " << version << endl;

    // Habilitar teste de profundidade
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_ALWAYS); // a cada ciclo

    // Habilitar o modo de transparência (blend - mistura de cores)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Compilando e buildando o programa de shader
    Shader shader("../shaders/tex_vert.glsl", "../shaders/tex_frag.glsl");

    GLuint texID1 = loadTexture("../../Textures/imagem2.png");
    GLuint texID2 = loadTexture("../../Textures/imagem1.png");
    GLuint texID3 = loadTexture("../../Textures/fundo1.jpg");
    GLuint texID4 = loadTexture("../../Textures/imagem3.png");

    // Criação das sprites
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

    // Matriz de projeção paralela ortográfica
    glm::mat4 projection = glm::ortho(0.0, 800.0, 0.0, 600.0, -1.0, 1.0);
    // Enviando para o shader a matriz como uma var uniform
    shader.setMat4("projection", glm::value_ptr(projection));

    // Matriz de transformação do objeto (matriz de modelo)
    shader.setInt("texBuffer", 0);

    // Loop da aplicação - "game loop"
    while (!glfwWindowShouldClose(window))
    {
        // Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funções de callback correspondentes
        glfwPollEvents();

        // Definindo as dimensões da viewport com as mesmas dimensões da janela da aplicação
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

    // Finaliza a execução da GLFW, limpando os recursos alocados por ela
    glfwTerminate();
    return 0;
}

GLuint loadTexture(string texturePath)
{
    GLuint texID;

    // Gera o identificador da textura na memória
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    // Configuração do parâmetro WRAPPING nas coords s e t
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Configuração do parâmetro FILTERING na minificação e magnificação da textura
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

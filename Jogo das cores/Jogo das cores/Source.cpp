/* Hello Triangle - código adaptado de https://learnopengl.com/#!Getting-started/Hello-Triangle
 *
 * Adaptado por Rossana Baptista Queiroz
 * para a disciplina de Processamento Gráfico - Unisinos
 * Versão inicial: 7/4/2017
 * Última atualização em 14/08/2023
 *
 */

 // Codigo de Júlia Faccio Zanette e Samuel de Oliveira Pasquali

#include <iostream>
#include <string>
#include <cmath>
#include <assert.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
//Classe para manipulação dos shaders
#include "Shader.h"

// Protótipo da função de callback da GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

// Protótipos das funções
int setup();
void pickColor(GLdouble xpos, GLdouble ypos);
void initRandomColors();
void renderScene(Shader& shader, GLuint VAO);
glm::vec3 randomColor();
bool colorsMatch(const glm::vec3& color1, const glm::vec3& color2, float tolerance);

// Dimensões da janela (pode ser alterado em tempo de execução)
const GLuint WIDTH = 1024, HEIGHT = 768;
const int COLUMNS = 5, LINES = 11;

glm::vec3 colorMatrix[COLUMNS][LINES];
int score = 0;

// Função MAIN
int main()
{
    // Inicialização da GLFW
    if (!glfwInit()) {
        cerr << "Falha ao inicializar GLFW" << endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Criação da janela GLFW
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Jogo das cores", nullptr, nullptr);
    if (!window) {
        cerr << "Falha ao criar a janela GLFW" << endl;
        glfwTerminate();
        return -1;
    }

    glfwSetWindowAttrib(window, GLFW_RESIZABLE, GLFW_FALSE);
    glfwMakeContextCurrent(window);

    // Fazendo o registro das funções de callback para a janela GLFW
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // GLAD: carrega todos os ponteiros de funções da OpenGL
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cerr << "Failed to initialize GLAD" << endl;
        return -1;
    }

    // Obtendo as informações de versão
    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version = glGetString(GL_VERSION);
    cout << "Renderer: " << renderer << endl;
    cout << "OpenGL version supported " << version << endl;

    // Definindo as dimensões da viewport com as mesmas dimensões da janela da aplicação
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    // Compilando e buildando o shader
    Shader shader("../shaders/retangulo.vs", "../shaders/retangulo.fs");

    // Gerando um buffer simples
    GLuint VAO = setup();
    initRandomColors();

    // Loop da aplicação - "game loop"
    while (!glfwWindowShouldClose(window))
    {
        // Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funções de callback correspondentes
        glfwPollEvents();

        // Limpa o buffer de cor
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // cor de fundo
        glClear(GL_COLOR_BUFFER_BIT);

        renderScene(shader, VAO);

        // Troca os buffers da tela
        glfwSwapBuffers(window);
    }

    // Exibe a pontuação final no console
    cout << "Pontuacao final: " << score << endl;

    // Pede pra OpenGL desalocar os buffers
    glDeleteVertexArrays(1, &VAO);

    // Finaliza a execução da GLFW, limpando os recursos alocados por ela
    glfwTerminate();
    return 0;
}

void renderScene(Shader& shader, GLuint VAO) {
    glBindVertexArray(VAO); // Conectando ao buffer de geometria

    shader.Use();
    GLfloat xc = -0.77f, xl = 0.90f;
    for (int c = 0; c < COLUMNS; c++) {
        for (int l = 0; l < LINES; l++) {
            if (colorMatrix[c][l] == glm::vec3(0.0f, 0.0f, 0.0f)) // Se a cor for preto, pula a renderização
                continue;

            shader.setVec3("cor", colorMatrix[c][l].r, colorMatrix[c][l].g, colorMatrix[c][l].b);

            glm::mat4 model = glm::mat4(1);
            model = glm::translate(model, glm::vec3(xc + c * 0.385f, xl - l * 0.125f, 0));
            model = glm::scale(model, glm::vec3(0.38f, 0.38f, 1.0f));

            shader.setMat4("model", glm::value_ptr(model));

            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }
    }

    glBindVertexArray(0); // Desconectando o buffer de geometria
}

// Função de callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        // Reinicia o jogo
        initRandomColors();
        score = 0;
    }
}

// Função de callback de clique do botão
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xpos, ypos;
        int w, h;
        glfwGetCursorPos(window, &xpos, &ypos);
        glfwGetWindowSize(window, &w, &h);
        cout << "Mouse X: " << xpos << " Y: " << ypos << endl; // posição do pixel
        pickColor(xpos, h - ypos); // cor do pixel
    }
}

int setup()
{
    GLfloat vertices[] = {
        // Posições         // Cores
        -0.5f, -0.15f, 0.0f, 0.0f, 0.0f, 1.0f,  // Vértice retângulo
        -0.5f, 0.15f, 0.0f, 0.0f, 0.0f, 1.0f,   // Vértice retângulo
        0.5f, -0.15f, 0.0f, 0.0f, 0.0f, 1.0f,   // Vértice retângulo
        0.5f, 0.15f, 0.0f, 0.0f, 0.0f, 1.0f    // Vértice retângulo 
    };

    GLuint VBO, VAO;

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Atributo 0 - posição
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // Atributo 1 - cor
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Desvincula o VAO (é uma boa prática desvincular qualquer buffer ou array para evitar bugs medonhos)
    glBindVertexArray(0);

    return VAO;
}

// Função para capturar a cor do pixel clicado e verificar se corresponde a alguma cor no jogo
void pickColor(GLdouble xpos, GLdouble ypos) {
    unsigned char pixel[3];
    // Captura a cor do pixel na posição (xpos, ypos) da janela
    glReadPixels(xpos, ypos, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);
    cout << "Color picked: R: " << (int)pixel[0] << " G: " << (int)pixel[1] << " B: " << (int)pixel[2] << endl;

    // Converte a cor capturada para um vetor glm::vec3
    glm::vec3 pickedColor(pixel[0] / 255.0f, pixel[1] / 255.0f, pixel[2] / 255.0f);

    bool colorMatched = false;
    // Itera através da matriz de cores para encontrar uma correspondência
    for (int c = 0; c < COLUMNS; c++) {
        for (int l = 0; l < LINES; l++) {
            // Verifica se a cor capturada corresponde a alguma cor da matriz com uma tolerância de 0.16
            if (colorsMatch(colorMatrix[c][l], pickedColor, 0.16f)) {
                // Remove o retângulo alterando sua cor para preto
                colorMatrix[c][l] = glm::vec3(0.0f, 0.0f, 0.0f);
                score += 10; // Incrementa a pontuação
                colorMatched = true;
            }
        }
    }
    if (colorMatched) {
        // Exibe a pontuação atual se houver uma correspondência
        cout << "Pontuacao: " << score << endl;
    }
    else {
        // Informa se não houver correspondência para a cor selecionada
        cout << "Nenhuma correspondência encontrada para a cor selecionada." << endl;
    }
}

// Função para inicializar a matriz de cores com cores aleatórias
void initRandomColors() {
    for (int c = 0; c < COLUMNS; c++) {
        for (int l = 0; l < LINES; l++) {
            // Atribui uma cor aleatória a cada posição da matriz
            colorMatrix[c][l] = randomColor();
        }
    }
}

// Função para gerar uma cor aleatória
glm::vec3 randomColor() {
    return glm::vec3(static_cast<float>(rand()) / RAND_MAX, static_cast<float>(rand()) / RAND_MAX, static_cast<float>(rand()) / RAND_MAX);
}

// Função para verificar se duas cores correspondem dentro de uma tolerância específica
bool colorsMatch(const glm::vec3& color1, const glm::vec3& color2, float tolerance) {
    return fabs(color1.r - color2.r) < tolerance &&
        fabs(color1.g - color2.g) < tolerance &&
        fabs(color1.b - color2.b) < tolerance;
}
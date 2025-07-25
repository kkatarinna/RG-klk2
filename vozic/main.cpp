// Autor: Nedeljko Tesanovic
// Opis: Testiranje dubine, Uklanjanje lica, Transformacije, Prostori i Projekcije
// Edit: Katarina Krstin - vozic

#define _CRT_SECURE_NO_WARNINGS
 
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>


#include <GL/glew.h> 
#include <GLFW/glfw3.h>


//GLM biblioteke
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Text.h"


unsigned int compileShader(GLenum type, const char* source);
unsigned int createShader(const char* vsSource, const char* fsSource);

const double TARGET_FPS = 144.0;
const double FRAME_DURATION = 1.0 / TARGET_FPS;

static float cameraDistance = 10.0f;
const float minCameraDistance = 2.0f;
const float maxCameraDistance = 30.0f;
const float zoomSpeed = 0.1f;


std::vector<glm::vec3> train = {
    { glm::vec3(2,0,-0.5)},
    { glm::vec3(0,0,0)},
    { glm::vec3(-2,0,0)},
    { glm::vec3(-4,0,-0.5)} 
};

int main(void)
{

   
    if (!glfwInit())
    {
        std::cout<<"GLFW Biblioteka se nije ucitala! :(\n";
        return 1;
    }


    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window;
    unsigned int wWidth = 800;
    unsigned int wHeight = 800;
    const char wTitle[] = "[Generic Title]";
    window = glfwCreateWindow(wWidth, wHeight, wTitle, NULL, NULL);
    
    if (window == NULL)
    {
        std::cout << "Prozor nije napravljen! :(\n";
        glfwTerminate();
        return 2;
    }
    
    glfwMakeContextCurrent(window);

    
    if (glewInit() != GLEW_OK)
    {
        std::cout << "GLEW nije mogao da se ucita! :'(\n";
        return 3;
    }

    if (!InitFreeType("C:/Windows/Fonts/arial.ttf")) {
        std::cerr << "Neuspešna inicijalizacija FreeType!" << std::endl;
        return -1;
    }

    // Omogući blending za tekst (potrebno za alpha kanale)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ PROMJENLJIVE I BAFERI +++++++++++++++++++++++++++++++++++++++++++++++++

    unsigned int unifiedShader = createShader("basic.vert", "basic.frag");
    unsigned int textShader = createShader("text.vert", "text.frag");

    float vertices[] =
    {
        // Strana 1 (DNO) RED — A-B-F-E (nije menjano)
        0.0f, -0.5f, 0.0f,    1,0,0,1,   // centroid (dno)
        0.5f, -0.5f, -0.5f,   1,0,0,1,   // E
        0.5f, -0.5f,  0.5f,   1,0,0,1,   // F
        -0.5f, -0.5f,  0.5f,  1,0,0,1,   // B
        -0.5f, -0.5f, -0.5f,  1,0,0,1,   // A
        0.5f, -0.5f, -0.5f,   1,0,0,1,   // E (zatvaranje)

        // Strana 2 (GORNJA) GREEN — C-G-H-D (promenjeno)
            0.0f, 0.5f, 0.0f,     0,1,0,1,   // centroid (gore)
        -0.5f, 0.5f, -0.5f,   0,1,0,1,   // C
        -0.5f, 0.5f,  0.5f,   0,1,0,1,   // D
         0.5f, 0.5f,  0.5f,   0,1,0,1,   // H
         0.5f, 0.5f, -0.5f,   0,1,0,1,   // G
        -0.5f, 0.5f, -0.5f,   0,1,0,1,   // C (zatvaranje)

        // Strana 3 (LEVA) BLUE — A-B-D-C (nije menjano)
        -0.5f, 0.0f, 0.0f,    0,0,1,1,   // centroid (leva)
        -0.5f, -0.5f, -0.5f,  0,0,1,1,   // A
        -0.5f, -0.5f,  0.5f,  0,0,1,1,   // B
        -0.5f,  0.5f,  0.5f,  0,0,1,1,   // D
        -0.5f,  0.5f, -0.5f,  0,0,1,1,   // C
        -0.5f, -0.5f, -0.5f,  0,0,1,1,   // A (zatvaranje)

        // Strana 4 (DESNA) YELLOW — E-G-H-F (promenjeno)
        0.5f, 0.0f, 0.0f,     1,1,0,1,   // centroid (desna)
        0.5f, -0.5f, -0.5f,   1,1,0,1,   // E
        0.5f,  0.5f, -0.5f,   1,1,0,1,   // G
        0.5f,  0.5f,  0.5f,   1,1,0,1,   // H
        0.5f, -0.5f,  0.5f,   1,1,0,1,   // F
        0.5f, -0.5f, -0.5f,   1,1,0,1,   // E (zatvaranje)

        // Strana 5 (PREDNJA) ROZE — B-F-H-D (promenjeno)
        0.0f, 0.0f, 0.5f,     1,0,1,1,   // centroid (prednja)
        -0.5f, -0.5f, 0.5f,   1,0,1,1,   // B
        0.5f, -0.5f, 0.5f,    1,0,1,1,   // F
        0.5f,  0.5f, 0.5f,    1,0,1,1,   // H
        -0.5f,  0.5f, 0.5f,   1,0,1,1,   // D
        -0.5f, -0.5f, 0.5f,   1,0,1,1,   // B (zatvaranje)

        // Strana 6 (ZADNJA) — A-E-G-C (nije menjano)
        0.0f, 0.0f, -0.5f,    0,1,1,1,   // centroid (zadnja)
        -0.5f, -0.5f, -0.5f,  0,1,1,1,   // A
        -0.5f,  0.5f, -0.5f,  0,1,1,1,   // C
        0.5f,  0.5f, -0.5f,   0,1,1,1,   // G
        0.5f, -0.5f, -0.5f,   0,1,1,1,   // E
        -0.5f, -0.5f, -0.5f,  0,1,1,1    // A (zatvaranje)
    };


    unsigned int stride = (3 + 4) * sizeof(float); 
    
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // ---------------- KRUŽNICA KOJA PREDSTAVLJA PUTANJU ----------------
    const int circleSegments = 100;
    const float circleRadius = 15.0f;

    float circleVertices[(circleSegments + 1) * 3]; // X, Y, Z

    for (int i = 0; i <= circleSegments; i++) {
        float angle = (float)i / (float)circleSegments * 2.0f * 3.14159265f;
        circleVertices[i * 3 + 0] = cos(angle) * circleRadius;  // X
        circleVertices[i * 3 + 1] = -0.5;                       // Y (u ravni)
        circleVertices[i * 3 + 2] = sin(angle) * circleRadius - 1.0f - 10.0f; // Z = sin(angle) * R - 1.0f
    }
    unsigned int circleVAO, circleVBO;
    glGenVertexArrays(1, &circleVAO);
    glGenBuffers(1, &circleVBO);

    glBindVertexArray(circleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, circleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(circleVertices), circleVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++            UNIFORME            +++++++++++++++++++++++++++++++++++++++++++++++++
    glm::mat4 model = glm::mat4(1.0f); //Matrica transformacija - mat4(1.0f) generise jedinicnu matricu
    unsigned int modelLoc = glGetUniformLocation(unifiedShader, "uM");
    
    unsigned int viewLoc = glGetUniformLocation(unifiedShader, "uV");
    
    glm::mat4 model2 = glm::mat4(1.0f);

    glm::mat4 projectionP = glm::perspective(glm::radians(90.0f), (float)wWidth / (float)wHeight, 0.1f, 100.0f); //Matrica perspektivne projekcije (FOV, Aspect Ratio, prednja ravan, zadnja ravan)
    glm::mat4 projectionO = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, 0.1f, 100.0f); //Matrica ortogonalne projekcije (Lijeva, desna, donja, gornja, prednja i zadnja ravan)
    unsigned int projectionLoc = glGetUniformLocation(unifiedShader, "uP");
    bool orthoBirdsEyeView = false;

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ RENDER LOOP - PETLJA ZA CRTANJE +++++++++++++++++++++++++++++++++++++++++++++++++
    glUseProgram(unifiedShader); //Slanje default vrijednosti uniformi
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model)); //(Adresa matrice, broj matrica koje saljemo, da li treba da se transponuju, pokazivac do matrica)
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projectionP));

    glClearColor(0.5, 0.5, 0.5, 1.0);
    glCullFace(GL_BACK);//Biranje lica koje ce se eliminisati (tek nakon sto ukljucimo Face Culling)
    double previousTime = glfwGetTime();

    float radius = 0.2f; // možeš da ga menjaš da bi povećao ili smanjio krug

    float baseAngularSpeed = glm::radians(0.14f); // bazni ugaoni pomeraj
    float speedFactor = 5.0f; // ili 1.0 za normalnu brzinu, <1 za sporije, >1 za brže

    bool prevWPressed = false;
    bool prevSPressed = false;
    bool prevSpacePressed = false;
    bool isPaused = false;
    float previousSpeed = 0.0f;

    

    while (!glfwWindowShouldClose(window))
    {
        float dTheta = baseAngularSpeed * speedFactor; // ugao za ovu iteraciju
        float translation = radius * dTheta; // dužina translacije
            
        double currentTime = glfwGetTime();
        double elapsedTime = currentTime - previousTime;

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }

        float time = (float)glfwGetTime();

        //Testiranje dubine
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        {
            glEnable(GL_DEPTH_TEST); //Ukljucivanje testiranja Z bafera
        }
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        {
            glDisable(GL_DEPTH_TEST);
        }

        //Odstranjivanje lica (Prethodno smo podesili koje lice uklanjamo sa glCullFace)
        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
        {
            glEnable(GL_CULL_FACE);
        }
        if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
        {
            glDisable(GL_CULL_FACE);
        }

        //Mijenjanje projekcija
        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        {
            orthoBirdsEyeView = false;
            glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projectionP));
        }
        if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
        {
            orthoBirdsEyeView = true;
            glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projectionO));
        }
        //Transformisanje trouglova
        model = glm::translate(model, glm::vec3(-translation, 0.0, 0.0)); //Pomjeranje (Matrica transformacije, pomjeraj po XYZ)
        model = glm::rotate(model, glm::radians(-dTheta), glm::vec3(0.0f, 1.0f, 0.0f)); //Rotiranje (Matrica transformacije, ugao rotacije u radijanima, osa rotacije)
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        
        
        bool currWPressed = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
        bool currSPressed = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
        bool currSpacePressed = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;

        if (currWPressed && !prevWPressed) {
            speedFactor += 3.0f;  // samo kad se pritisne, ne kad se drži
        }

        if (currSPressed && !prevSPressed) {
            speedFactor -= 3.0f;
        }

        if (currSpacePressed && !prevSpacePressed) {
            if (!isPaused) {
                // Pauziraj: sačuvaj staru brzinu i postavi na 0
                previousSpeed = speedFactor;
                speedFactor = 0.0f;
                isPaused = true;
            }
            else {
                // Vrati brzinu iz pre pauze
                speedFactor = previousSpeed;
                isPaused = false;
            }
        }

        // Ažuriraj prethodno stanje za sledeći frejm
        prevWPressed = currWPressed;
        prevSPressed = currSPressed;
        prevSpacePressed = currSpacePressed;

        if (elapsedTime >= FRAME_DURATION) {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Osvjezavamo i Z bafer i bafer boje

            previousTime = currentTime;
            glBindVertexArray(VAO);

            glm::mat4 view; //Matrica pogleda (kamere)

            // ========== KAMERA PRATI POSLEDNJU KOCKU =========
            
            if (orthoBirdsEyeView)
            {
                view = glm::lookAt(glm::vec3(0.0f, 20.0f, 0.0f), // Pozicija kamere (gore)
                glm::vec3(0.0f, 0.0f, 0.0f),  // Gleda u centar scene
                glm::vec3(0.0f, 0.0f, -1.0f));  // "Gore" za kameru (Y usmerenje));

            }
            else {

                


                // Pozicija poslednje kocke (world space)
                glm::vec4 localPos = glm::vec4(train.back(), 1.0f);
                glm::vec4 worldPos = model * localPos;
                glm::vec3 cubePosition = glm::vec3(worldPos);

                // Prethodna pozicija kocke
                glm::vec3 prevCubePos = glm::vec3(model * glm::vec4(train[train.size() - 2], 1.0f));
                glm::vec3 direction = glm::normalize(cubePosition - prevCubePos);

                if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
                    cameraDistance -= zoomSpeed;
                    if (cameraDistance < minCameraDistance)
                        cameraDistance = minCameraDistance;
                }

                if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {  // recimo X za odzumiranje
                    cameraDistance += zoomSpeed;
                    if (cameraDistance > maxCameraDistance)
                        cameraDistance = maxCameraDistance;
                }

                // Offset kamere (udaljena malo od kocke i iznad)
                glm::vec3 baseCameraOffset = -direction * cameraDistance + glm::vec3(0.0f, 1.0f, 0.0f);
                glm::vec3 cameraPos = cubePosition + baseCameraOffset;

                // Rotacija direction vektora (gledanja) oko Y ose po pritisku A
                static float headRotationAngle = 0.0f;
                const float rotationSpeed = glm::radians(1.5f);

                const float maxYaw = glm::radians(90.0f);  // Maksimalna rotacija +/- 90 stepeni

                if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
                    if (headRotationAngle + rotationSpeed <= maxYaw) {
                        headRotationAngle += rotationSpeed;
                    }
                }
                if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
                    headRotationAngle -= rotationSpeed;
                }

                // Kreiraj rotacionu matricu koja rotira pravac gledanja oko Y ose za headRotationAngle
                glm::mat4 rotMat = glm::rotate(glm::mat4(1.0f), headRotationAngle, glm::vec3(0, 1, 0));

                // Pravac gledanja (gledanje iz kocke prema napred) rotiran za headRotationAngle
                glm::vec4 rotatedDirection4 = rotMat * glm::vec4(direction, 0.0f);
                glm::vec3 rotatedDirection = glm::normalize(glm::vec3(rotatedDirection4));

                // Cilj na koji kamera gleda — sada je slobodno rotiran u odnosu na pravac kocke
                glm::vec3 cameraTarget = cameraPos + rotatedDirection;

                // Kreiraj view matricu
                view = glm::lookAt(
                    cameraPos,
                    cameraTarget,
                    glm::vec3(0, 1, 0)
                );
            }

            // Pošalji novu view matricu shaderu
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

            for (int i = 0; i < 4; ++i) {

                glm::mat4 localModel = model; // Lokalni model za svaku kocku
                localModel = glm::translate(localModel,train[i]); // Translacija kocke na poziciju

                //localModel = glm::translate(localModel, glm::vec3(train[i][0]*x*translation, train[i][1], train[i][2] * z) ); // Translacija kocke na poziciju
                // Ako želiš da rotiraš, skaliraš ili nešto drugo — dodaj ovde!
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(localModel));
                
                glDrawArrays(GL_TRIANGLE_FAN, 0, 6);
                glDrawArrays(GL_TRIANGLE_FAN, 6, 6);
                glDrawArrays(GL_TRIANGLE_FAN, 12, 6);
                glDrawArrays(GL_TRIANGLE_FAN, 18, 6);
                glDrawArrays(GL_TRIANGLE_FAN, 24, 6);
                glDrawArrays(GL_TRIANGLE_FAN, 30, 6);
            }

            glBindVertexArray(circleVAO);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model2));
            glDrawArrays(GL_LINE_LOOP, 0, circleSegments + 1);

            // --- Crtanje teksta ---
            glUseProgram(textShader);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glDisable(GL_DEPTH_TEST);
            RenderText(textShader, "Katarina Krstin SV57/2021", 25.0f, 550.0f, 1.0f, glm::vec3(1.0, 1.0, 1.0), 800, 600);

            // Vrati stanje (nije obavezno, ali je dobra praksa)
            glEnable(GL_DEPTH_TEST);
            glDisable(GL_BLEND);
            glUseProgram(unifiedShader);
        }

        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Osvjezavamo i Z bafer i bafer boje
        //glDrawArrays(GL_TRIANGLES, 0, 6);
        // Iscrtaj VAO
        

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ POSPREMANJE +++++++++++++++++++++++++++++++++++++++++++++++++


    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(unifiedShader);

    glfwTerminate();
    return 0;
}

unsigned int compileShader(GLenum type, const char* source)
{
    std::string content = "";
    std::ifstream file(source);
    std::stringstream ss;
    if (file.is_open())
    {
        ss << file.rdbuf();
        file.close();
        std::cout << "Uspjesno procitao fajl sa putanje \"" << source << "\"!" << std::endl;
    }
    else {
        ss << "";
        std::cout << "Greska pri citanju fajla sa putanje \"" << source << "\"!" << std::endl;
    }
     std::string temp = ss.str();
     const char* sourceCode = temp.c_str();

    int shader = glCreateShader(type);
    
    int success;
    char infoLog[512];
    glShaderSource(shader, 1, &sourceCode, NULL);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        if (type == GL_VERTEX_SHADER)
            printf("VERTEX");
        else if (type == GL_FRAGMENT_SHADER)
            printf("FRAGMENT");
        printf(" sejder ima gresku! Greska: \n");
        printf(infoLog);
    }
    return shader;
}
unsigned int createShader(const char* vsSource, const char* fsSource)
{
    unsigned int program;
    unsigned int vertexShader;
    unsigned int fragmentShader;

    program = glCreateProgram();

    vertexShader = compileShader(GL_VERTEX_SHADER, vsSource);
    fragmentShader = compileShader(GL_FRAGMENT_SHADER, fsSource);

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);
    glValidateProgram(program);

    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_VALIDATE_STATUS, &success);
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(program, 512, NULL, infoLog);
        std::cout << "Objedinjeni sejder ima gresku! Greska: \n";
        std::cout << infoLog << std::endl;
    }

    glDetachShader(program, vertexShader);
    glDeleteShader(vertexShader);
    glDetachShader(program, fragmentShader);
    glDeleteShader(fragmentShader);

    return program;
}

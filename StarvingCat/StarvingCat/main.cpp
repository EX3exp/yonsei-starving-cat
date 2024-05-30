// windows가 아닐 경우 파일경로 구분 문자열을 "\\"에서 '/'로 바꿔야 돌아갑니다 -- learnopengl, utils 내부 관련 헤더도 전부 수정해야 함 
// (main.cpp 이외엔 딱히 수정하진 않으셔도 되고 교수님께서 올려 주신 헤더파일들 그대로 사용하시면 됩니다)
// 
// <디버그 폴더 내에 리소스 파일을 붙여넣어야 할 경우>:
// (수동으로 복붙하지 마시고)
// 프로젝트 속성 >> 빌드 후 이벤트
// 명령줄에 `copy "$(ProjectDir)<추가할 파일>" "$(OutDir)" ` 을 추가해 주세요

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>
#include <learnopengl/animator.h>
#include <learnopengl/model_animation.h>
#include <iostream>

#include <learnopengl/render_text.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <unordered_map>

glm::vec3 darkblue = glm::vec3(0.06, 0.08, 0.71);
// Source and Data directories
string sourceDirStr = "C:\\Users\\inthe\\Downloads\\yonsei-starving-cat\\StarvingCat\\StarvingCat";
string dataDirStr = "C:\\Users\\inthe\\Downloads\\yonsei-starving-cat\\data";
string fontPath = dataDirStr + "\\fonts\\Galmuri14.ttf";


enum Scene
{ // 현재 렌더되는 화면을 구분하는 enum -- 씬 전환 시  
    cat_init, // 게임 기동 시 맨 처음 화면
    cat_eat_right, // 고양이가 오른쪽 밥그릇에서 먹음
    cat_eat_left // 고양이가 왼쪽 밥그릇에서 먹음 
};

// FUNCTION PROTOTYPES
GLFWwindow *glAllInit();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// GLOBAL VARIABLES
const double MAX_FRAMERATE_LIMIT = 1.0 / 60.0; // 현재 프레임레이트 -- 기본값은 60프레임

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

const int MAX_STAGE = 9;
const int MIN_STAGE = 0;

// share variables
static int stage; // stage, 0~9
static float catSize; // stage가 늘 때마다 증가
static bool catMoveFlag = false; // true면 고양이의 위치를 catMoveAmt만큼 움직임 
GLfloat catMoveAmt; // 왼쪽 밥그릇 먹을 때엔 감소, 오른쪽 밥그릇 먹을 때엔 증가 -- 양쪽 100px씩 이동함, 3초간 애니메이션
double catMovedTime = 0.0; // 고양이가 움직이기 시작한 시간, 3초에 도달하면 0으로 초기화
string foodRight = "none"; // 오른쪽 밥그릇에 있는 food, 아무것도 없을 때엔 "none"
string foodLeft = "none"; // 왼쪽 밥그릇에 있는 food, 아무것도 없을 때엔 "none"
Scene currentScene = cat_init; // 씬 전환 시 이 변수를 변경
float catMoveStartTime = 0.0f; // 고양이가 움직이기 시작한 시간
float catShouldStopMoveAmt = 2.0f; // 고양이가 움직임을 멈추는 시간
bool catReturnFlag = false; // true일 경우 고양이의 위치를 가운데로 되돌림
bool catMoving = false; // true일 경우 고양이가 움직이고 있음
bool catMovingLeft = false; // true일 경우 고양이가 오른쪽으로 움직이는 것으로 가정, 아닐 경우 고양이가 왼쪽으로 움직이는 것으로 가정 
Text* messageText;

GLFWwindow *mainWindow = NULL;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
GLdouble deltaTime = 0.0f;
GLdouble lastUpdateTime = 0.0f;
GLdouble lastFrameTime = 0.0f;

// 정수 => 유니코드 문자열로 바꾸는 함수
std::u32string intToChar32(const int i) 
{
    auto s = std::to_string(i);
    return { s.begin(), s.end() };
}

// 3d 모델 오브젝트들은 모두 이 클래스를 상속받아서 사용 - 애니메이팅 적용되는 오브젝트의 경우 AnimatedObj3D 상속해야 함
class Obj3D
{ 
public:
    Obj3D(string name, string modelPath, string vertexShaderPath, string fragShaderPath) 
        : modelPath(modelPath), model(modelPath), shader(vertexShaderPath.c_str(), fragShaderPath.c_str())
    {
        cout << "[Obj3D] '" + name + "' object created" << endl;
    }

    ~Obj3D() {
        cout << "[Obj3D] '" + name + "' object deleted" << endl;
    }

    void draw()
    {
        shader.use();
        updateAnimation();
        // view/projection transformations
        projectionMatrix = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        viewMatrix = camera.GetViewMatrix();
        shader.setMat4("projection", projectionMatrix);
        shader.setMat4("view", viewMatrix);

        sendAnimationToShader();

        shader.setMat4("model", modelMatrix);
        model.Draw(shader);

    }
    virtual void updateAnimation() 
    {
    }

    virtual void sendAnimationToShader() 
    {
    }
    
    void translate(float x = 0.f, float y = -0.4f, float z = 0.f) 
    {
        modelMatrix = glm::translate(modelMatrix, glm::vec3(x, y, z));
    }

    void scale(float x = .5f, float y = .5f, float z = .5f) 
    {
        modelMatrix = glm::scale(modelMatrix, glm::vec3(x, y, z));
    }
    
    void rotate(float rotationAngle, glm::vec3 rotationAxis) {
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rotationAngle), rotationAxis);
    }
protected:
    string name;
    Model model;
    string modelPath;
    Shader shader;

    glm::mat4 projectionMatrix = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    glm::mat4 viewMatrix = camera.GetViewMatrix();
    glm::mat4 modelMatrix = glm::mat4(1.0f);

    Animation anim{ modelPath, &model };
    Animator animator{ &anim };

    std::vector<glm::mat4> transformsMatrixes = animator.GetFinalBoneMatrices();
};

// 애니메이팅되는 3d 모델 오브젝트들은 모두 이 클래스를 상속받아서 사용
class AnimatedObj3D : public Obj3D
{ 
public:
    AnimatedObj3D(string name, string modelPath, string vertexShaderPath, string fragShaderPath) : Obj3D(name, modelPath, vertexShaderPath, fragShaderPath)
    {
        
    }


    void updateAnimation() override
    {
        animator.UpdateAnimation(deltaTime); // update animation
    }

    void sendAnimationToShader() override 
    {
        transformsMatrixes = animator.GetFinalBoneMatrices();
        for (int i = 0; i < transformsMatrixes.size(); ++i) {
            shader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transformsMatrixes[i]);
        }
    }
};

// 고양이
class Cat : public AnimatedObj3D
{ 
public:
    Cat(string modelPath, string vertexShaderPath, string fragShaderPath) : AnimatedObj3D("cat", modelPath, vertexShaderPath, fragShaderPath) 
    {

    }

    void eat(string &food) 
    {
        if (!checkCanEat(food)) { // 먹을 수 없는 걸 먹음
            
        }
        else { // 먹을 수 있는 걸 먹음

        }
    }

private:
    bool checkCanEat(string &food) 
    {
        return (foods.find(food) != foods.end() && foods[food] != 0 ? true : false);
    }
    unordered_map<string, int> foods; // <음식이름, 먹을 수 있음> - 예: <"fish", 1> -- 0 이상은 전부 먹을 수 있음 
};
Cat* cat;

int main()
{
    mainWindow = glAllInit();

	

	// load models
	// -----------
    //string modelPath = modelDirStr + "/vampire/dae/dancing_vampire.dae";
    string catModelPath = dataDirStr + "\\vampire\\dae\\dancing_vampire.dae"; // 고양이 모델의 경로
    string catModelWalkPath = dataDirStr + "\\vampire\\dae2\\dancing_vampire.dae"; // 걷는 고양이 모델의 경로
    //string modelPath = modelDirStr + "/chapa/dae/Chapa-Giratoria.dae";
    
    // build and compile shaders
    // -------------------------
    string vs = sourceDirStr + "\\skel_anim.vs"; // vertex shader
    string fs = sourceDirStr + "\\skel_anim.fs"; // fragment shader

    string vsText = sourceDirStr + "\\text_render.vs"; // text용 vertex shader
    string fsText = sourceDirStr + "\\text_render.fs"; // text용 fragment shader


    cat = new Cat(catModelPath, vs, fs);
    cat->translate();

    Text* mainText;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    
    glm::mat4 textProjection = glm::ortho(0.0f, static_cast<GLfloat>(SCR_WIDTH), 0.0f, static_cast<GLfloat>(SCR_HEIGHT));

    mainText = new Text(vsText, fsText, fontPath, textProjection, U"Stage" + intToChar32(stage + 1), darkblue);
    mainText->setPos(SCR_WIDTH * 0.43f, SCR_HEIGHT * 0.9f, 0.8f);
    
    messageText = new Text(vsText, fsText, fontPath, textProjection, U"", darkblue);
    messageText->setPos(SCR_WIDTH * 0.3f, SCR_HEIGHT * 0.5f, 1.f);
    glEnable(GL_CULL_FACE); // cull face to reduce memory usage

	// render loop
	// -----------
    float factor;
    const float PI = 3.141592;
	while (!glfwWindowShouldClose(mainWindow))
    {
        factor = (catMovingLeft ? 1.f : -1.f);
        GLdouble now = glfwGetTime();
        if (catMoveFlag && !catMoving) {
            catMoving = true;
            catMoveStartTime = now;
        }

       
        glfwPollEvents();

        if ((now - lastFrameTime) >= MAX_FRAMERATE_LIMIT)
        {
           
            deltaTime = now - lastUpdateTime;

            processInput(mainWindow);
            // draw your frame here
            glClearColor(1.f,1.f, 1.f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            if (catMoveFlag && catMoving && now - catMoveStartTime <= catShouldStopMoveAmt) {
               
                catMoveAmt = factor * 0.5 * sin(PI * now);
                cat->translate(catMoveAmt, 0.f, 0.f);
                catMoving = false;
                
            }
            else if (catMoveFlag && !catMoving) {
                catMoveFlag = false;
            }

            
            cat->draw(); // Draw & animate cat
            
            mainText->draw();
            messageText->draw();
            glfwSwapBuffers(mainWindow);

            lastFrameTime = now;
        }

        lastUpdateTime = now;
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

GLFWwindow *glAllInit()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    
    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Starving Cat", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(-1);
    }
    
    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);
    
    
    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    
    return window;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        catMoveFlag = true;
        catMoving = false;
        catMovingLeft = true;
    }
    else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        catMoveFlag = true;
        catMoving = false;
        catMovingLeft = false;
    }

    /*
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
     */
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
    
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

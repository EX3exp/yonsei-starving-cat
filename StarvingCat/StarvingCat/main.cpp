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


#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <unordered_map>

// Source and Data directories
string sourceDirStr = "C:\\Users\\inthe\\Downloads\\yonsei-starving-cat\\StarvingCat\\StarvingCat";
string modelDirStr = "C:\\Users\\inthe\\Downloads\\yonsei-starving-cat\\data";

enum Scene
{ // 현재 렌더되는 화면을 구분하는 enum -- 씬 전환 시  
    cat_init, // 게임 기동 시 맨 처음 화면
    cat_eat_right,
    cat_eat_left
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

const int MAX_STAGE = 10;
const int MIN_STAGE = 1;

// share variables
static int stage; // stage, 1~10
static float catSize; // stage가 늘 때마다 증가
string foodRight = "none"; // 오른쪽 밥그릇에 있는 food, 아무것도 없을 때엔 "none"
string foodLeft = "none"; // 왼쪽 밥그릇에 있는 food, 아무것도 없을 때엔 "none"
Scene currentScene = cat_init; // 씬 전환 시 이 변수를 변경해 주세요

GLFWwindow *mainWindow = NULL;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
double deltaTime = 0.0;
double lastUpdateTime = 0.0;
double lastFrameTime = 0.0;

class Obj3D
{ // 3d 모델 오브젝트들은 모두 이 클래스를 상속받아서 사용 - 애니메이팅 적용되는 오브젝트의 경우 AnimatedObj3D 상속해야 함
public:
    Obj3D(string name, string modelPath, string vertexShaderPath, string fragShaderPath) : modelPath(modelPath), model(modelPath), shader(vertexShaderPath.c_str(), fragShaderPath.c_str())
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
    
    void translate(float x = 0.f, float y = -0.4f, float z = 0.f) {
        modelMatrix = glm::translate(modelMatrix, glm::vec3(x, y, z));
    }

    void scale(float x = .5f, float y = .5f, float z = .5f) {
        modelMatrix = glm::scale(modelMatrix, glm::vec3(x, y, z));
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

class AnimatedObj3D : public Obj3D
{ // 애니메이팅되는 3d 모델 오브젝트들은 모두 이 클래스를 상속받아서 사용
public:
    AnimatedObj3D(string name, string modelPath, string vertexShaderPath, string fragShaderPath) : Obj3D(name, modelPath, vertexShaderPath, fragShaderPath)
    {
        
    }


    void updateAnimation() override
    {
        animator.UpdateAnimation(deltaTime); // update animation
    }

    void sendAnimationToShader() override {
        transformsMatrixes = animator.GetFinalBoneMatrices();
        for (int i = 0; i < transformsMatrixes.size(); ++i)
            shader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transformsMatrixes[i]);
    }
};



class Cat : public AnimatedObj3D
{ // 고양이
public:
    Cat(string modelPath, string vertexShaderPath, string fragShaderPath) : AnimatedObj3D("cat", modelPath, vertexShaderPath, fragShaderPath) 
    {

    }

    void eat(string &food) 
    {
        if (!checkCanEat(food)) 
        { // 먹을 수 없는 걸 먹음
            
        }
        else 
        { // 먹을 수 있는 걸 먹음

        }
    }

private:
    bool checkCanEat(string &food) 
    {
        return (foods.find(food) != foods.end() && foods[food] != 0 ? true : false);
    }
    unordered_map<string, int> foods; // <음식이름, 먹을 수 있음> - 예: <"fish", 1> -- 0 이상은 전부 먹을 수 있음 
};

int main()
{
    mainWindow = glAllInit();

	// build and compile shaders
	// -------------------------
    string vs = sourceDirStr + "\\skel_anim.vs"; // vertex shader
    string fs = sourceDirStr + "\\skel_anim.fs"; // fragment shader

	// load models
	// -----------
    //string modelPath = modelDirStr + "/vampire/dae/dancing_vampire.dae";
    string catModelPath = modelDirStr + "\\boxing\\dae\\boxing.dae"; // 고양이 모델의 경로
    //string modelPath = modelDirStr + "/chapa/dae/Chapa-Giratoria.dae";
    
    Cat *cat = new Cat(catModelPath, vs, fs);
    cat->translate();
    cat->scale();
	// render loop
	// -----------
	while (!glfwWindowShouldClose(mainWindow))
    {
        double now = glfwGetTime();
        deltaTime = now - lastFrameTime;

        glfwPollEvents();

        if ((now - lastFrameTime) >= MAX_FRAMERATE_LIMIT)
        {
            processInput(mainWindow);
            // draw your frame here
            glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            cat->draw(); // Draw & animate cat

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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "StarvingCat", NULL, NULL);
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
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

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

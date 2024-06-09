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
#include <foods.h>
#include <cube.h>
#include <learnopengl/render_text.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <unordered_map>

glm::vec3 darkblue = glm::vec3(0.06, 0.08, 0.71);
// Source and Data directories
string sourceDirStr = "C:/Users/inthe/Downloads/yonsei-starving-cat/StarvingCat/StarvingCat";
string dataDirStr = "C:/Users/inthe/Downloads/yonsei-starving-cat/data";
string fontPath = dataDirStr + "/fonts/Galmuri14.ttf";


// FUNCTION PROTOTYPES
GLFWwindow* glAllInit();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void goToFirstStage();
void goToNextStage();

// GLOBAL VARIABLES
const double MAX_FRAMERATE_LIMIT = 1.0 / 60.0; // 현재 프레임레이트 -- 기본값은 60프레임

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

const int MAX_STAGE = 9;
const int MIN_STAGE = 0;


// share variables
static int stage; // stage, 0~9
static float catSize; // stage가 늘 때마다 증가 TODO cat scale 적용

// start flags
static bool timerInitNeeded = true; // 타이머 초기화가 필요할 시 true
static bool catMoveFlag = false; // true면 고양이의 위치를 catMoveAmt만큼 움직임 
static bool catStopAndEatFlag = false; // true일 경우 고양이가 멈춰서 먹고 음식 종류에 따라 반응
static bool catShowResultFlag = false; // true면 고양이의 반응 보여줌
static bool catStageTransitionFlag = false; // true면 다음 스테이지로 이동
static bool catMoveNext = false; // true면 다음 스테이지로 이동하거나 게임 종료, false면 첫 스테이지로 이동
static bool gameEndingFlag = false; // true일 경우 게임 종료 연출 -- 게임 진행 안함
static bool catStageEndFlag = false; // true일 경우 스테이지 종료 연출 

static bool isTimeOver = false; // true일 경우 시간이 다 됨 -- catEating이 False일 때 true로 바뀌면 게임 강제 종료
// stop flags
static bool catMoveStopFlag = false; // true면 고양이 위치가 다 이동했다고 간주
static bool catStopAndEatStopFlag = false;
static bool catShowResultStopFlag = false;
static bool catStageTransitionStopFlag = false;
static bool catStageEndStopFlag = false;

// processing
static bool catEating = false; // true while whole cat motion -- blocks left+right input when false

static bool catMoving = false; // true일 경우 고양이가 움직이고 있음
static bool catStopAndEating = false; // true일 경우 고양이가 먹고 있음
static bool catShowingResult = false; // true일 경우 고양이가 반응 표시
static bool catStageTransitioning = false; // true일 경우 스테이지 전환 중
static bool catStageEnding = false; // true일 경우 스테이지종료 연출 중

// time temp
static double catMovedTime = 0.0; // [초기 이동] 고양이가 마지막으로 움직인 시간
static double catStopAndEatTime = 0.0; // [초기 이동 후 음식 먹음] 고양이 음식먹는 모션 시간
static double catShowResultTime = 0.0; // [초기 이동 후 음식 먹고 반응] 고양이 반응 모션 시간
static double catStageTransitionTime = 0.0;
static double catStageEndingTime = 0.0;

// start time
static double catMoveStartTime = 0.0; // 고양이가 움직이기 시작한 시간
static double catStopAndEatStartTime = 0.0; // 고양이가 마지막으로 음식먹는 움직임을 한 시간
static double catShowResultStartTime = 0.0; // 고양이가 마지막으로 반응 움직임을 보인 시간
static double catStageTransitionStartTime = 0.0;

static double catStageEndingStartTime = 0.0;

static bool catMovingLeft = false; // true일 경우 고양이가 오른쪽으로 움직이는 것으로 가정, 아닐 경우 고양이가 왼쪽으로 움직이는 것으로 가정 


int maxRecord = 0; // 0~9, 최고기록을 기록하는 변수
int tryNum = 1; // 시도 횟수를 기록하는 변수
GLFWwindow* mainWindow = NULL;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
GLdouble deltaTime = 0.0;
GLdouble lastUpdateTime = 0.0;
GLdouble lastFrameTime = 0.0;

glm::mat4 textProjection = glm::ortho(0.0f, static_cast<GLfloat>(SCR_WIDTH), 0.0f, static_cast<GLfloat>(SCR_HEIGHT));
// 정수 => 유니코드 문자열로 바꾸는 함수
std::u32string intToChar32(const int i)
{
    auto s = std::to_string(i);
    return { s.begin(), s.end() };
}

FoodManager foodManager;

class SkyCube
{
public:
    SkyCube() = default;
    SkyCube(string vs, string fs, string texturePath) :
        shader(vs.c_str(), fs.c_str()), dataPath(texturePath)
    {
        std::cout << "[FoodCube] object created" << std::endl;

        cube.addTexture(U"sky", dataPath);
        cube.scale(15.f);
        cube.translate(4.f, 5.f, 0.f);
        
        cube.initBuffers();
    }
    void setLightPos(glm::vec3 l) {
        lightPos = l;
    }

    void setLightStrength(float s) {
        lightStrength = s;
    }

    void draw() {
        shader.use();
        cube.switchTexture(U"sky");
        projectionMatrix = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        viewMatrix = camera.GetViewMatrix();

        // view/projection transformations
        shader.setMat4("projection", projectionMatrix);
        shader.setMat4("view", viewMatrix);

        shader.setMat4("model", modelMatrix);
        shader.setVec3("viewPos", camera.Position);
        shader.setVec3("lightPosition", lightPos);
        shader.setFloat("lightStrength", lightStrength);
        cube.draw(&shader);

    }
private:
    Cube cube;
    Shader shader;
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    glm::mat4 viewMatrix = camera.GetViewMatrix();
    glm::mat4 modelMatrix = glm::mat4(1.0f);

    string dataPath;
    glm::vec3 lightPos = glm::vec3(0.0f, 0.0f, 1.0f); // light position
    float lightStrength = 1.f; // light strength
};
class FoodCube
{
public:
    FoodCube() = default;
    FoodCube(string vs, string fs, string dataPath, bool isLeft) :
        shader(vs.c_str(), fs.c_str()), dataPath(dataPath)
    {
        glEnable(GL_POLYGON_SMOOTH);
        std::cout << "[FoodCube] object created" << std::endl;
        for (int i = 0; i < foodManager.foods.size(); i++) {
            cube.addTexture(foodManager.foods[i].getName(), dataPath + foodManager.foods[i].getTexturefileName());
        }
        if (isLeft) {
            cube.translate(-2.5f, -1.f, -0.5f);
            cube.scale(0.9f);
        }
        else {
            cube.translate(2.5f, -1.f, -0.5f);
            cube.scale(0.9f);
        }

        cube.initBuffers();
    }
    void setLightPos(glm::vec3 l) {
        lightPos = l;
    }

    void setLightStrength(float s) {
        lightStrength = s;
    }

    void setFood(Food& food) {
        cube.switchTexture(food.getName());
    }

    void draw() {
        shader.use();
		projectionMatrix = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		viewMatrix = camera.GetViewMatrix();

		// view/projection transformations
		shader.setMat4("projection", projectionMatrix);
		shader.setMat4("view", viewMatrix);

		shader.setMat4("model", modelMatrix);
        shader.setVec3("viewPos", camera.Position);
        shader.setVec3("lightPosition", lightPos);
        shader.setFloat("lightStrength", lightStrength);
		cube.draw(&shader);
	
    }
private:
    Cube cube;
	Shader shader;
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(90.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    glm::mat4 viewMatrix = camera.GetViewMatrix();
    glm::mat4 modelMatrix = glm::mat4(1.0f);

    string dataPath;
    glm::vec3 lightPos = glm::vec3(0.0f, 0.0f, 1.0f); // light position
    float lightStrength = 1.f; // light strength
};
// 3d 모델 오브젝트들은 모두 이 클래스를 상속받아서 사용 - 애니메이팅 적용되는 오브젝트의 경우 AnimatedObj3D 상속해야 함
class Obj3D
{
public:
    Obj3D() = default;
    Obj3D(string name, string modelPath, string vertexShaderPath, string fragShaderPath,
        float defaultXScale = 0.5f, float defaultYScale = 0.5f, float defaultZScale = 0.5f,
        float defaultRotationAngle = 0.f, glm::vec3 defaultRotationAxis = glm::vec3(0.f, 1.f, 0.f),
        float defaultXtranslation = 0.f, float defaultYtranslation = 0.4f, float defaultZtranslation = 0.f)
        : modelPath(modelPath), model(modelPath), shader(vertexShaderPath.c_str(), fragShaderPath.c_str())
    {
        cout << "[Obj3D] '" + name + "' object created" << endl;

        // modify default transform matrix
        changeDefaultTransformMatrix(defaultXScale, defaultYScale, defaultZScale,
            defaultRotationAngle, defaultRotationAxis,
            defaultXtranslation, defaultYtranslation, defaultZtranslation);
        modelMatrix = defaultTransformMatrix * modelMatrix;
    }

    ~Obj3D() {
        cout << "[Obj3D] '" + name + "' object deleted" << endl;
    }

    void changeDefaultTransformMatrix(float defaultXScale, float defaultYScale, float defaultZScale,
        float defaultRotationAngle, glm::vec3(defaultRotationAxis),
        float defaultXtranslation, float defaultYtranslation, float defaultZtranslation) {
        // make default transform matrix
        glm::mat4 mat;
        // 1. scale
        defaultTransformMatrix = glm::mat4(1.0f);
        mat = glm::scale(glm::mat4(1.0f), glm::vec3(defaultXScale, defaultYScale, defaultZScale));
        defaultTransformMatrix = mat * defaultTransformMatrix;

        // 2. rotate
        mat = glm::rotate(glm::mat4(1.0f), glm::radians(defaultRotationAngle), defaultRotationAxis);
        defaultTransformMatrix = mat * defaultTransformMatrix;

        // 3. translate
        mat = glm::translate(glm::mat4(1.0f), glm::vec3(defaultXtranslation, defaultYtranslation, defaultZtranslation));
        defaultTransformMatrix = mat * defaultTransformMatrix;
    }

    void draw()
    {
        shader.use();
        if (isPlaying) {
            updateAnimation();
        }

        projectionMatrix = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        viewMatrix = camera.GetViewMatrix();

        // view/projection transformations
        shader.setMat4("projection", projectionMatrix);
        shader.setMat4("view", viewMatrix);

        sendTransformsToShader();
        shader.setMat4("model", modelMatrix);
        shader.setVec3("viewPos", camera.Position);

        shader.setVec3("lightPosition", lightPos);
        shader.setFloat("lightStrength", lightStrength);
        model.Draw(shader);
    }

    void setLightPos(glm::vec3 l) {
        lightPos = l;
    }

    void setLightStrength(float s) {
		lightStrength = s;
	} 


    virtual void updateAnimation()
    {
    }

    virtual void sendTransformsToShader()
    {
    }


    void translate(float x = 0.f, float y = -0.4f, float z = 0.f)
    {
        glm::mat4 mat = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
        lastTranslateMatrix = glm::inverse(mat) * lastTranslateMatrix;
        modelMatrix = mat * modelMatrix;
    }

    void scale(float x = .5f, float y = .5f, float z = .5f)
    {
        glm::mat4 mat = glm::scale(glm::mat4(1.0f), glm::vec3(x, y, z));
        lastScaleMatrix = glm::inverse(mat) * lastScaleMatrix;
        modelMatrix = mat * modelMatrix;
    }

    void rotate(float rotationAngle, glm::vec3 rotationAxis) {
        glm::mat4 mat = glm::rotate(glm::mat4(1.0f), glm::radians(rotationAngle), rotationAxis);
        lastRotateMatrix = glm::inverse(mat) * lastRotateMatrix;
        modelMatrix = mat * modelMatrix;
    }

    void undoTranslate() {
        modelMatrix = modelMatrix * lastTranslateMatrix;
        lastTranslateMatrix = glm::mat4(1.0f);
    }

    void undoScale() {
        modelMatrix = modelMatrix * lastScaleMatrix;
        lastScaleMatrix = glm::mat4(1.0f);
    }

    void undoRotate() {
        modelMatrix = modelMatrix * lastRotateMatrix;
        lastRotateMatrix = glm::mat4(1.0f);
    }

    void resetTransform() {
        modelMatrix = defaultTransformMatrix;
    }

protected:
    string name;
    Model model;
    string modelPath;
    Shader shader;

    glm::mat4 projectionMatrix = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    glm::mat4 viewMatrix = camera.GetViewMatrix();
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    glm::mat4 defaultTransformMatrix = glm::mat4(1.0f);
    bool isPlaying = true; // 애니메이션 재생하고 있을 경우에만 true


    glm::mat4 lastTranslateMatrix = glm::mat4(1.0f); // translate
    glm::mat4 lastScaleMatrix = glm::mat4(1.0f); // scale
    glm::mat4 lastRotateMatrix = glm::mat4(1.0f); // rotate

    glm::vec3 lightPos = glm::vec3(0.0f, 0.0f, 1.0f); // light position
    float lightStrength = 1.f; // light strength
};

// 애니메이팅되는 3d 모델 오브젝트들은 모두 이 클래스를 상속받아서 사용
class AnimatedObj3D : public Obj3D
{
public:
    AnimatedObj3D() = default;
    AnimatedObj3D(string name, string modelPath, string vertexShaderPath, string fragShaderPath,
        float defaultXScale = 0.5f, float defaultYScale = 0.5f, float defaultZScale = 0.5f,
        float defaultRotationAngle = 0.f, glm::vec3 defaultRotationAxis = glm::vec3(0.f, 1.f, 0.f),
        float defaultXtranslation = 0.f, float defaultYtranslation = 0.4f, float defaultZtranslation = 0.f)
        : Obj3D(name, modelPath, vertexShaderPath, fragShaderPath,
            defaultXScale, defaultYScale, defaultZScale,
            defaultRotationAngle, defaultRotationAxis,
            defaultXtranslation, defaultYtranslation, defaultZtranslation)
    {
    }


    void updateAnimation() override
    {
        animator.UpdateAnimation(deltaTime); // update animation
        transformsMatrixes = animator.GetFinalBoneMatrices();
    }

    void sendTransformsToShader() override
    {
        for (int i = 0; i < transformsMatrixes.size(); ++i) {
            shader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transformsMatrixes[i]);
        }
    }

    // 움직임 리셋하고 초기 포즈로 돌아감
    void reset()
    {
        transformsMatrixes = originalTransformsMatrixes;
    }

    // 모션 재생
    void play() {
        isPlaying = true;
    }

    // 모션 일시정지
    void pause() {
        isPlaying = false;
    }

    // 모션 정지 -- 초기 포즈로 돌아감
    void stop() {
        isPlaying = false;
        reset();
    }

    void changeMotion(int motionNo)
    {
        cout << "changed motion -- " << motionNo << endl;
        anim = Animation(modelPath, &model, motionNo);
        animator = Animator(&anim);
        transformsMatrixes = animator.GetFinalBoneMatrices();
        originalTransformsMatrixes = animator.GetFinalBoneMatrices();
        reset();

    }

protected:
    Animation anim{ modelPath, &model };
    Animator animator{ &anim };

    std::vector<glm::mat4> originalTransformsMatrixes = animator.GetFinalBoneMatrices(); // 모션 reset 용도 -- 최초의 transforms를 저장함
    std::vector<glm::mat4> transformsMatrixes = animator.GetFinalBoneMatrices(); // 애니메이션 수행 용도
};

// 고양이
class Cat : public AnimatedObj3D
{
public:
    Cat() = default;
    Cat(string modelPath,
        string vertexShaderPath, string fragShaderPath,
        float defaultXScale = 0.5f, float defaultYScale = 0.5f, float defaultZScale = 0.5f,
        float defaultRotationAngle = 0.f, glm::vec3 defaultRotationAxis = glm::vec3(0.f, 1.f, 0.f),
        float defaultXtranslation = 0.f, float defaultYtranslation = 0.4f, float defaultZtranslation = 0.f) :
        defaultScale(defaultYScale), defaultRotationAngle(defaultRotationAngle), defaultRotationAxis(defaultRotationAxis),
        defaultXtranslation(defaultXtranslation), defaultYtranslation(defaultYtranslation), defaultZtranslation(defaultZtranslation),
        AnimatedObj3D("cat", modelPath, vertexShaderPath, fragShaderPath,
            defaultXScale, defaultYScale, defaultZScale,
            defaultRotationAngle, defaultRotationAxis,
            defaultXtranslation, defaultYtranslation, defaultZtranslation)
    {
        toDefaultMotion();
        initialTransformMatrix = defaultTransformMatrix;
        
    }

    void toDefaultMotion() {
        changeMotion(1);
    }
    void walk() {
        cout << " ** changed motion - walk";
        changeMotion(5);
    }

    void eat() {
        cout << "** changed motion - eat";
        changeMotion(3);
    }

    bool result(Food& food)
    {
        cout << "** changed motion - show result";
        if (!checkCanEat(food)) { // 먹을 수 없는 걸 먹음
            cout << "eated: " << food.PrintName() << " -- cat will die" << endl;
            changeMotion(4);

            return false;
        }
        else { // 먹을 수 있는 걸 먹음
            cout << "eated: " << food.PrintName() << " -- happy cat" << endl;
            changeMotion(2);

            return true;
        }
    }

    void grow(float scaleIncrease = 1.1f) { // grow up, and reset Pos
        defaultScale *= scaleIncrease;
        changeDefaultTransformMatrix(defaultScale, defaultScale, defaultScale, defaultRotationAngle, defaultRotationAxis, defaultXtranslation, defaultYtranslation, defaultZtranslation);
        resetTransform();
    }

    void resetToRetry() { // stage 1로 이동했을 때 호출 
        defaultTransformMatrix = initialTransformMatrix;
        resetTransform();
    }
private:
    bool checkCanEat(Food& food)
    {
        cout << "checking";
        return food.isCanEat();
    }

    float defaultScale;

    float defaultXtranslation;
    float defaultYtranslation;
    float defaultZtranslation;

    float defaultRotationAngle;
    glm::vec3 defaultRotationAxis;

    glm::mat4 initialTransformMatrix;

};
Cat* cat;

FoodCube* foodCubeRight;
FoodCube* foodCubeLeft;
SkyCube* skyCube;

Text* mainText;
Text* messageText;
Text* leftText;
Text* rightText;
Text* helperText;
Text* titleText;
Text* timerText;

Food foodRight; // 오른쪽 밥그릇에 있는 food
Food foodLeft; // 왼쪽 밥그릇에 있는 food
// TODO 밥그릇과 초원

int main()
{
    mainWindow = glAllInit();

    glEnable(GL_CULL_FACE);
    
    
    GLfloat catMoveAmt; // 왼쪽 밥그릇 먹을 때엔 감소, 오른쪽 밥그릇 먹을 때엔 증가
    // load models
    // -----------
    //string modelPath = modelDirStr + "/vampire/dae/dancing_vampire.dae";
    string catModelPath = dataDirStr + "/ycat/gltf/TuxCat.gltf"; // 고양이 모델 경로

    string grassPath = dataDirStr + "/grass/gltf/untitled.gltf"; // 초원 모델 경로
    //string modelPath = modelDirStr + "/chapa/dae/Chapa-Giratoria.dae";

    // build and compile shaders
    // -------------------------
    string vs = sourceDirStr + "/skel_anim.vs"; // vertex shader
    string fs = sourceDirStr + "/skel_anim.fs"; // fragment shader

    string vsCube = sourceDirStr + "/cube.vs"; // vertex shader
    string fsCube = sourceDirStr + "/cube.fs"; // fragment shader

    string vsText = sourceDirStr + "/text_render.vs"; // text용 vertex shader
    string fsText = sourceDirStr + "/text_render.fs"; // text용 fragment shader
    string fsSkyCube = sourceDirStr + "/skycube.fs"; // sky cube fragment shader
    cat = new Cat(catModelPath, // model path
        vs, fs, // shaders
        1.f, 1.f, 1.f, // default scale
        0.f, glm::vec3(0.f, 0.f, 1.f), // default rotation
        0.f, -1.5f, 0.f // default translation
    );

    
    mainText = new Text(vsText, fsText, fontPath, textProjection, U"Stage" + intToChar32(stage + 1), darkblue);
    mainText->setPos(SCR_WIDTH * 0.5f, SCR_HEIGHT * 0.9f, 1.0f);

    messageText = new Text(vsText, fsText, fontPath, textProjection, U"", darkblue);
    messageText->setPos(SCR_WIDTH * 0.5f, SCR_HEIGHT * 0.6f, 0.7f);

    leftText = new Text(vsText, fsText, fontPath, textProjection, U"", darkblue);
    leftText->setPos(SCR_WIDTH * 0.2f, SCR_HEIGHT * 0.1f, 0.8f);

    rightText = new Text(vsText, fsText, fontPath, textProjection, U"", darkblue);
    rightText->setPos(SCR_WIDTH * 0.8f, SCR_HEIGHT * 0.1f, 0.8f);


    helperText = new Text(vsText, fsText, fontPath, textProjection, U"방향키를 눌러 음식을 먹으세요!", darkblue);
    helperText->setPos(SCR_WIDTH * 0.5f, SCR_HEIGHT * 0.1f, 0.4f);

    titleText = new Text(vsText, fsText, fontPath, textProjection, U"", darkblue);
    titleText->setPos(SCR_WIDTH * 0.5f, SCR_HEIGHT * 0.7f, 0.85f);


    timerText = new Text(vsText, fsText, fontPath, textProjection, U"0", darkblue);
    timerText->setPos(SCR_WIDTH * 0.5f, SCR_HEIGHT * 0.6f, 0.85f);
    foodCubeRight = new FoodCube(vsCube, fsCube, dataDirStr + "/food_img/", false);
    foodCubeLeft = new FoodCube(vsCube, fsCube, dataDirStr + "/food_img/", true);
    
    skyCube = new SkyCube(vsCube, fsSkyCube, dataDirStr + "/bg_full.jpg");
    // render loop
    // -----------

    const float PI = 3.141592;

    cat->resetToRetry();

    foodManager.selectRandom(stage, foodRight);
    foodManager.selectRandom(stage, foodLeft);
    leftText->setText(foodLeft.getName());
    rightText->setText(foodRight.getName());
    foodCubeLeft->setFood(foodLeft);
    foodCubeRight->setFood(foodRight);
    double timerOffset = 0.0;

    while (!glfwWindowShouldClose(mainWindow))
    {
        GLdouble now = glfwGetTime();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        
        // 프레임에 맞춰서 인풋 받고 게임 진행 
        if ((now - lastFrameTime) >= MAX_FRAMERATE_LIMIT)
        {
            
            float factor = (catMovingLeft ? -1.f : 1.f);

            if (gameEndingFlag) {
                // 게임 종료 연출
                glClearColor(0.894f, 0.882f, 0.875f, 1.f);
            }
            else if (timerInitNeeded) {
                glClearColor(0.894f, 0.882f, 0.875f, 1.f);
                timerOffset = now;
				timerInitNeeded = false;
            }
            else if (!catEating && !isTimeOver) {
                cout << "-";
                double secRemain = (10.0 + timerOffset - now);
                double angle = 2.0 * PI * (now - timerOffset) / 10.0; // 0에서 2pi 사이의 각도
                double x = -cos(angle); // 코사인 값으로 x좌표 설정
                double y = -sin(angle);
                double z = 0.0;
                float lightStrength = std::min(1.0, secRemain / 10.0 + 0.3);
                cat->setLightPos(glm::vec3(x, y, z));
                cat->setLightStrength(lightStrength);
                foodCubeLeft->setLightPos(glm::vec3(x, y, z));
                foodCubeLeft->setLightStrength(lightStrength);
                foodCubeRight->setLightPos(glm::vec3(x, y, z));
                foodCubeRight->setLightStrength(lightStrength);
                skyCube->setLightPos(glm::vec3(x, y, z));
                skyCube->setLightStrength(lightStrength);
                glClearColor(0.894f , 0.882f , 0.875f * (lightStrength), 1.f);
                if (secRemain <= 0) {
                    timerText->clearText();
                    isTimeOver = true;
					
                }
                if (secRemain > 5) {
                    timerText->setText(U"남은 시간: " + intToChar32((int)secRemain));
                }
                else {
                    timerText->setText(U"남은 시간: " + intToChar32((int)secRemain), true);
                }
			}
            else if (!catEating && isTimeOver) {
				// 시간이 다 됨
                catStageTransitionFlag = true;
                catMoveNext = false;
                catEating = true;
			}
            else {
                // ----------
                // 고양이 모션 - 1. 밥그릇 위치까지 움직임
                if (catMoveFlag) { // 고양이 움직임 준비
                    cout << "   cat start Move" << endl;
                    cat->walk();
                    // starts move at next frame
                    catMoveStartTime = now;

                    catMoveFlag = false;
                    catMoving = true;
                    catMoveStopFlag = false;

                    catStopAndEatFlag = false;
                    catStopAndEating = false;
                    catStopAndEatStopFlag = false;

                    catShowResultFlag = false;
                    catShowingResult = false;
                    catShowResultStopFlag = false;

                    catStageTransitionFlag = false;
                    catStageTransitioning = false;
                    catStageTransitionStopFlag = false;

                    catStageEndFlag = false;
                    catStageEnding = false;
                    catStageEndStopFlag = false;

                    cat->rotate(factor * 90.f, glm::vec3(0.f, 1.f, 0.f));
                }
                if (catMoving) { // 고양이 움직임
                    cout << " - ";
                    catMovedTime = now - catMoveStartTime;
                    catMoveAmt = 0.03 * factor * sin(catMovedTime / 2.f * PI);
                    cat->translate(catMoveAmt, 0.f, 0.f);
                    if (now - catMoveStartTime >= 2) {
                        cout << "   cat will Stop" << endl;
                        // stops move at next frame
                        catMoveStopFlag = true;
                        catMoving = false;
                    }
                }
                if (catMoveStopFlag) { // 고양이 멈춤 준비
                    cout << endl << "   cat Stopped" << endl;
                    catMoveStopFlag = false;
                    catStopAndEatFlag = true;
                    cat->eat();
                }

                // ---------------- 
                // 고양이 모션 - 2. 밥 먹음 
                if (catStopAndEatFlag) { // 고양이 먹을 준비
                    cout << "   cat starts Eating" << endl;
                    // starts eat at next frame
                    catStopAndEatStartTime = now;
                    catStopAndEatFlag = false;
                    catStopAndEating = true;
                    catStopAndEatStopFlag = false;
                }
                if (catStopAndEating) { // 고양이 먹음
                    cout << " - ";

                    if (now - catStopAndEatStartTime >= 0.8) {
                        cout << "   cat will Stop Eating" << endl;
                        // stops move at next frame
                        catStopAndEatStopFlag = true;
                        catStopAndEating = false;
                    }
                }
                if (catStopAndEatStopFlag) { // 고양이 먹는거 멈춤 준비
                    cout << endl << "   cat Stopped Eating" << endl;
                    catStopAndEatStopFlag = false;
                    catShowResultFlag = true;
                    catMoveNext = cat->result((catMovingLeft ? foodLeft : foodRight));
                }

                // ---------------- 
                // 고양이 모션 - 3. 반응 표시 
                if (catShowResultFlag) { // 고양이 반응 준비
                    cout << "   cat starts Eating" << endl;
                    // starts eat at next frame
                    catShowResultStartTime = now;
                    catShowResultFlag = false;
                    catShowingResult = true;
                    catShowResultFlag = false;
                    cat->rotate(factor * -1.f * 90.f, glm::vec3(0.f, 1.f, 0.f));
                }
                if (catShowingResult) { // 고양이 반응
                    cout << " - ";

                    if (now - catMoveStartTime >= 3) {
                        cout << "   stage will change or return to 1" << endl;
                        // stops move at next frame
                        catShowResultStopFlag = true;
                        catShowingResult = false;
                    }
                }
                if (catShowResultStopFlag) { // 고양이 반응 멈춤 준비
                    cout << endl << "   cat stopped" << endl;
                    catShowResultStopFlag = false;
                    catStageTransitionFlag = true;
                }

                // ---------------- 
                // 4. 전환  
                if (catStageTransitionFlag) { // 스테이지 전환 준비 
                    cout << "   going to next stage" << endl;
                    // starts eat at next frame
                    timerText->clearText();
                    catStageTransitionStartTime = now;
                    catStageTransitionFlag = false;
                    catStageTransitioning = true;
                    catStageTransitionStopFlag = false;
                }
                if (catStageTransitioning) { // 스테이지 전환
                    cout << " - ";
                    if (isTimeOver) {
                        helperText->setText(U"해가 지고 말았습니다.");
                        rightText->clearText();
                        leftText->clearText();
                        if (now - catStageTransitionStartTime >= 2.f) {
                            cout << "   cat will go next" << endl;
                            // stops move at next frame
                            catStageTransitionStopFlag = true;
                            catStageTransitioning = false;
                        }
                    }
                    else if (catMoveNext) {
                        helperText->setText(U"맛있었다.");
                        rightText->clearText();
                        leftText->clearText();
                        if (catMovingLeft) {
                            titleText->setText(foodLeft.getName());
                            messageText->setText(foodLeft.getMessage());
                        }
                        else {
                            titleText->setText(foodRight.getName());
                            messageText->setText(foodRight.getMessage());
                        }
                        if (now - catStageTransitionStartTime >= messageText->getTextLength() * 0.04) {
                            cout << "   cat will go next" << endl;
                            // stops move at next frame
                            catStageTransitionStopFlag = true;
                            catStageTransitioning = false;
                        }
                    }
                    else {
                        rightText->clearText();
                        leftText->clearText();

                        helperText->setText(U"으윽, 이건!", true);
                        if (catMovingLeft) {
                            titleText->setText(foodLeft.getName(), true);
                            messageText->setText(foodLeft.getMessage(), true);
                        }
                        else {
                            titleText->setText(foodRight.getName(), true);
                            messageText->setText(foodRight.getMessage(), true);
                        }
                        if (now - catStageTransitionStartTime >= messageText->getTextLength() * 0.04) {
                            cout << "   cat will go next" << endl;
                            // stops move at next frame
                            catStageTransitionStopFlag = true;
                            catStageTransitioning = false;
                        }
                    }

                }
                if (catStageTransitionStopFlag) { // 멈춤 준비
                    cout << endl << "   will finish the stage" << endl;
                    catStageTransitionStopFlag = false;
                    messageText->clearText();

                    if (catMoveNext) {
                        titleText->setText(U"클리어!");
                        if (stage == MAX_STAGE) {
                            messageText->setText(U"몸이 조금 자라났다!");
                        }
                        else {
                            messageText->setText(U"몸이 조금 자라났다! \n다음 스테이지로 넘어갑니다.");
                        }
                        
                        helperText->clearText();
                        cat->grow();
                        catStageTransitionStopFlag = false;
                        catStageEndFlag = true;
                    }
                    else {
                        titleText->setText(U"최고기록: " + intToChar32(maxRecord + 1) + U" 스테이지", true);

                        helperText->clearText();
                        if (isTimeOver) {
                            timerText->clearText();
                            messageText->setText(U"너무 음식을 늦게 골랐어요! 굶어 죽고 말았습니다... \n1스테이지로 다시 돌아갑니다.", true);
                        }
                        else {
                            messageText->setText(U"이런, 죽어버렸다... \n1스테이지로 다시 돌아갑니다.", true);
                        }
                        cat->resetToRetry();
                        catStageTransitionStopFlag = false;
                        catStageEndFlag = true;
                    }

                }

                // ---------------- 
                // 5. 종료 연출 
                if (catStageEndFlag) { // 스테이지 종료 준비 
                    cout << "   going to next stage" << endl;
                    // starts eat at next frame
                    cat->changeMotion(0);
                    catStageEndingStartTime = now;
                    catStageEndFlag = false;
                    catStageEnding = true;
                    catStageEndStopFlag = false;
                }
                if (catStageEnding) { // 스테이지 종료
                    cout << " - ";
                    if (now - catStageEndingStartTime >= 2) {
                        cout << "   finishing stage" << endl;
                        // stops move at next frame
                        catStageEndStopFlag = true;
                        catStageEnding = false;
                    }
                }
                if (catStageEndStopFlag) { // 멈춤 준비
                    cout << endl << "   proceed" << endl;

                    titleText->clearText();

                    if (catMoveNext) {
                        if (stage == MAX_STAGE) {
                            helperText->clearText();
                            titleText->setText(U"게임 클리어!");
                            gameEndingFlag = true;
                            messageText->setText(U"축하합니다! 게임을 클리어했습니다.\n총 시도한 횟수: " + intToChar32(tryNum) + U"번");

                        }
                        else {
                            messageText->clearText();
                            helperText->clearText();
                            titleText->clearText();
                            goToNextStage();
                        }
                    }
                    else {
                        messageText->clearText();
                        cat->resetToRetry();
                        goToFirstStage();
                    }
                    
                    catStageEndStopFlag = false;
                    catEating = false;
                }
            }
            

            
            // End of the Motions

            processInput(mainWindow);
            deltaTime = now - lastUpdateTime;


            lastFrameTime = now;
        }

        // Draw
        
        glCullFace(GL_FRONT);
        cat->draw();
        skyCube->draw();

        glCullFace(GL_BACK);
        foodCubeLeft->draw();
        foodCubeRight->draw();
        
        mainText->draw();
        messageText->draw();
        leftText->draw();
        rightText->draw();
        helperText->draw();
        titleText->draw();
        timerText->draw();
        

        

        // TODO 초원, 밥그릇, 밥 그리기, Lighting

        lastUpdateTime = now;
        glfwSwapBuffers(mainWindow);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    delete cat;
    delete mainText;
    delete messageText;
    delete leftText;
    delete rightText;
    delete helperText;
    delete titleText;
    delete timerText;

    delete foodCubeRight;
    delete foodCubeLeft;
    delete skyCube;
    glfwTerminate();
    return 0;
}

GLFWwindow* glAllInit()
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
        if (!catEating && !gameEndingFlag) {
            cout << "cat eat left" << endl;
            catMoveFlag = true;
            catMovingLeft = true;
            catEating = true;
            helperText->setText(U"[System] 왼쪽 음식을 먹었다!");
        }

    }
    else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        if (!catEating && !gameEndingFlag) {
            cout << "cat eat right" << endl;
            catMoveFlag = true;
            catMovingLeft = false;
            catEating = true;
            helperText->setText(U"[System] 오른쪽 음식을 먹었다!");
        }
    }
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

void goToFirstStage()
{
    stage = 0;
    tryNum += 1;
    cout << "Go to 1st Stage -- " << stage << endl;
    titleText->clearText();
    mainText->setText(U"Stage" + intToChar32(stage + 1)); // update stage label

    // 사용된 음식 초기화
    foodManager.reset();

    // 음식 배정
    cat->toDefaultMotion();
    helperText->setText(U"방향키를 눌러 음식을 먹으세요!");
    foodManager.selectRandom(stage, foodRight);
    foodManager.selectRandom(stage, foodLeft);
    leftText->setText(foodLeft.getName());
    rightText->setText(foodRight.getName());
    foodCubeLeft->setFood(foodLeft);
    foodCubeRight->setFood(foodRight);

    timerText->clearText();
    isTimeOver = false;
    timerInitNeeded = true;
}

void goToNextStage()
{	
    stage += 1;

    if (maxRecord < stage) {
        maxRecord = stage; // 최고기록 갱신
    }
    cout << "Go to next Stage -- " << stage << endl;

    mainText->setText(U"Stage" + intToChar32(stage + 1)); // update stage label

    // 음식 배정
    cat->toDefaultMotion();
    helperText->setText(U"방향키를 눌러 음식을 먹으세요!");
    foodManager.selectRandom(stage, foodRight);
    foodManager.selectRandom(stage, foodLeft);
    leftText->setText(foodLeft.getName());
    rightText->setText(foodRight.getName());
    foodCubeLeft->setFood(foodLeft);
    foodCubeRight->setFood(foodRight);
    
    timerText->clearText();
    isTimeOver = false;
    timerInitNeeded = true;
}

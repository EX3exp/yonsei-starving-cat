// windows�� �ƴ� ��� ���ϰ�� ���� ���ڿ��� "\\"���� '/'�� �ٲ�� ���ư��ϴ� -- learnopengl, utils ���� ���� ����� ���� �����ؾ� �� 
// (main.cpp �̿ܿ� ���� �������� �����ŵ� �ǰ� �����Բ��� �÷� �ֽ� ������ϵ� �״�� ����Ͻø� �˴ϴ�)
// 
// <����� ���� ���� ���ҽ� ������ �ٿ��־�� �� ���>:
// (�������� �������� ���ð�)
// ������Ʈ �Ӽ� >> ���� �� �̺�Ʈ
// ����ٿ� `copy "$(ProjectDir)<�߰��� ����>" "$(OutDir)" ` �� �߰��� �ּ���

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


// FUNCTION PROTOTYPES
GLFWwindow *glAllInit();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void goToNextStage();
void goToFirstStage();

// GLOBAL VARIABLES
const double MAX_FRAMERATE_LIMIT = 1.0 / 60.0; // ���� �����ӷ���Ʈ -- �⺻���� 60������

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

const int MAX_STAGE = 9;
const int MIN_STAGE = 0;


// share variables
static int stage; // stage, 0~9
static float catSize; // stage�� �� ������ ���� TODO cat scale ����

// starg flags
static bool catMoveFlag = false; // true�� ������� ��ġ�� catMoveAmt��ŭ ������ 
static bool catStopAndEatFlag = false; // true�� ��� ����̰� ���缭 �԰� ���� ������ ���� ����
static bool catShowResultFlag = false; // true�� ������� ���� ������
static bool catStageTransitionFlag = false; // true�� ���� ���������� �̵�
    static bool catMoveNext = false; // true�� ���� ���������� �̵��ϰų� ���� ����, false�� ù ���������� �̵�
static bool gameEndingFlag = false; // true�� ��� ���� ���� ����


// stop flags
static bool catMoveStopFlag = false; // true�� ����� ��ġ�� �� �̵��ߴٰ� ����
static bool catStopAndEatStopFlag = false;
static bool catShowResultStopFlag = false;
static bool catStageTransitionStopFlag = false;


// processing
static bool catEating = false; // true while whole cat motion -- blocks left+right input when false

static bool catMoving = false; // true�� ��� ����̰� �����̰� ����
static bool catStopAndEating = false; // true�� ��� ����̰� �԰� ����
static bool catShowingResult = false; // true�� ��� ����̰� ���� ǥ��
static bool catStageTransitioning = false; // true�� ��� �������� ��ȯ ��

// time temp
static double catMovedTime = 0.0; // [�ʱ� �̵�] ����̰� ���������� ������ �ð�
static double catStopAndEatTime = 0.0; // [�ʱ� �̵� �� ���� ����] ����� ���ĸԴ� ��� �ð�
static double catShowResultTime = 0.0; // [�ʱ� �̵� �� ���� �԰� ����] ����� ���� ��� �ð�
static double catStageTransitionTime = 0.0;

// start time
static double catMoveStartTime = 0.0; // ����̰� �����̱� ������ �ð�
static double catStopAndEatStartTime = 0.0; // ����̰� ���������� ���ĸԴ� �������� �� �ð�
static double catShowResultStartTime = 0.0; // ����̰� ���������� ���� �������� ���� �ð�
static double catStageTransitionStartTime = 0.0;

string foodRight = "chru"; // ������ ��׸��� �ִ� food, �ƹ��͵� ���� ���� "none"
string foodLeft = "none"; // ���� ��׸��� �ִ� food, �ƹ��͵� ���� ���� "none"

static bool catMovingLeft = false; // true�� ��� ����̰� ���������� �����̴� ������ ����, �ƴ� ��� ����̰� �������� �����̴� ������ ���� 


GLFWwindow *mainWindow = NULL;

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
// ���� => �����ڵ� ���ڿ��� �ٲٴ� �Լ�
std::u32string intToChar32(const int i) 
{
    auto s = std::to_string(i);
    return { s.begin(), s.end() };
}

// 3d �� ������Ʈ���� ��� �� Ŭ������ ��ӹ޾Ƽ� ��� - �ִϸ����� ����Ǵ� ������Ʈ�� ��� AnimatedObj3D ����ؾ� ��
class Obj3D
{ 
public:
    Obj3D() = default;
    Obj3D(string name, string modelPath, string vertexShaderPath, string fragShaderPath, 
        float defaultXScale=0.5f, float defaultYScale=0.5f, float defaultZScale=0.5f,
        float defaultRotationAngle=0.f, glm::vec3 defaultRotationAxis= glm::vec3(0.f, 1.f, 0.f),
        float defaultXtranslation=0.f, float defaultYtranslation=0.4f, float defaultZtranslation=0.f)
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

        model.Draw(shader);
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
    bool isPlaying = true; // �ִϸ��̼� ����ϰ� ���� ��쿡�� true


    glm::mat4 lastTranslateMatrix = glm::mat4(1.0f); // translate
    glm::mat4 lastScaleMatrix = glm::mat4(1.0f); // scale
    glm::mat4 lastRotateMatrix = glm::mat4(1.0f); // rotate
    
};

// �ִϸ����õǴ� 3d �� ������Ʈ���� ��� �� Ŭ������ ��ӹ޾Ƽ� ���
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

    // ������ �����ϰ� �ʱ� ����� ���ư�
    void reset() 
    {
        transformsMatrixes = originalTransformsMatrixes;
    }

    // ��� ���
    void play() {
        isPlaying = true;
    }

    // ��� �Ͻ�����
    void pause() {
        isPlaying = false;
    }

    // ��� ���� -- �ʱ� ����� ���ư�
    void stop() {
        isPlaying = false;
        reset();
    }

    void changeMotion(string newMotionPath)
    {
        cout << "changed motion -- " << newMotionPath << endl;
        anim = Animation(newMotionPath, &model);
        animator = Animator(&anim);
        transformsMatrixes = animator.GetFinalBoneMatrices();
        originalTransformsMatrixes = animator.GetFinalBoneMatrices();
        reset();

    }

    void changeMotion(Animation anim)
    {
        cout << "changed motion -- ";
        animator = Animator(&anim);
        transformsMatrixes = animator.GetFinalBoneMatrices();
        originalTransformsMatrixes = animator.GetFinalBoneMatrices();
        reset();
    }
protected:
    Animation anim{ modelPath, &model };
    Animator animator{ &anim };

    std::vector<glm::mat4> originalTransformsMatrixes = animator.GetFinalBoneMatrices(); // ��� reset �뵵 -- ������ transforms�� ������
    std::vector<glm::mat4> transformsMatrixes = animator.GetFinalBoneMatrices(); // �ִϸ��̼� ���� �뵵
};

// �����
class Cat : public AnimatedObj3D
{ 
public:
    Cat() = default;
    Cat(string modelPath, string walkMotionPath, string eatMotionPath, string joyMotionPath, string dieMotionPath,
        string vertexShaderPath, string fragShaderPath,
        float defaultXScale = 0.5f, float defaultYScale = 0.5f, float defaultZScale = 0.5f,
        float defaultRotationAngle = 0.f, glm::vec3 defaultRotationAxis = glm::vec3(0.f, 1.f, 0.f),
        float defaultXtranslation = 0.f, float defaultYtranslation = 0.4f, float defaultZtranslation = 0.f) :
        AnimatedObj3D("cat", modelPath, vertexShaderPath, fragShaderPath, 
            defaultXScale, defaultYScale, defaultZScale, 
            defaultRotationAngle, defaultRotationAxis, 
            defaultXtranslation, defaultYtranslation, defaultZtranslation),
        walkMotionPath(walkMotionPath), eatMotionPath(eatMotionPath), dieMotionPath(dieMotionPath), joyMotionPath(joyMotionPath), 
        defaultScale(defaultYScale), defaultRotationAngle(defaultRotationAngle), defaultRotationAxis(defaultRotationAxis),
        defaultXtranslation(defaultXtranslation), defaultYtranslation(defaultYtranslation), defaultZtranslation(defaultZtranslation)
    {
        initialTransformMatrix = defaultTransformMatrix;
        foods.insert({ "none", 0 });
        foods.insert({"chru", 1});
    }

    void toDefaultMotion() {
        changeMotion(modelPath);
    }
    void walk() {
        cout << " ** changed motion - walk";
        changeMotion(walkMotionPath);
    }

    void eat() {
        cout << "** changed motion - eat";
        changeMotion(eatMotionPath);
    }
    // ������ ������ ���� ��� isMovingLeft = true, ������ ������ ���� ��� isMovingLeft = false
    bool result(const string food) 
    {
        cout << "** changed motion - show result";
        if (!checkCanEat(food)) { // ���� �� ���� �� ����
            cout << "eated: " << food << " -- cat will die" << endl;
            changeMotion(dieMotionPath);
            // TODO
            return false;
        }
        else { // ���� �� �ִ� �� ����
            cout << "eated: " << food << " -- happy cat" << endl;
            changeMotion(joyMotionPath);
            // TODO
            return true;
        }
    }

    void grow(float scaleIncrease = 1.1f) { // grow up, and reset Pos
        defaultScale *= scaleIncrease;
        changeDefaultTransformMatrix(defaultScale, defaultScale, defaultScale, defaultRotationAngle, defaultRotationAxis, defaultXtranslation, defaultYtranslation, defaultZtranslation);
        resetTransform();
    }

    void resetToRetry() { // stage 1�� �̵����� �� ȣ�� 
        defaultTransformMatrix = initialTransformMatrix;
        resetTransform();
    }
private:
    bool checkCanEat(const string food) 
    {
        cout << "checking";
        return (foods.find(food) != foods.end() && foods[food] != 0 ? true : false);
    }

    // TODO foods �ۼ� 
    unordered_map<string, int> foods; // <�����̸�, ���� �� ����> - ��: <"fish", 1> -- 0 �̻��� ���� ���� �� ���� 
    float defaultScale;
    
    float defaultXtranslation;
    float defaultYtranslation;
    float defaultZtranslation;

    float defaultRotationAngle;
    glm::vec3 defaultRotationAxis;

    glm::mat4 initialTransformMatrix;

    string walkMotionPath;
    string eatMotionPath;
    string joyMotionPath;
    string dieMotionPath;
    Animation defaultMotion{ modelPath, &model };
    Animation walkMotion{ walkMotionPath, &model };
    Animation eatMotion{ eatMotionPath, &model };
    Animation joyMotion{ joyMotionPath, &model };
    Animation dieMotion{ dieMotionPath, &model };

};
Cat* cat;
Text* mainText;
Text* messageText;
// TODO ��׸��� �ʿ�

int main()
{
    mainWindow = glAllInit();


    GLfloat catMoveAmt; // ���� ��׸� ���� ���� ����, ������ ��׸� ���� ���� ����
    // load models
    // -----------
    //string modelPath = modelDirStr + "/vampire/dae/dancing_vampire.dae";
    string catModelPath = dataDirStr + "\\vampire\\dae\\dancing_vampire.dae"; // ����� �� ��� -- ��, �⺻ ��ǿ�
    string catWalkPath = dataDirStr + "\\vampire\\dae2\\dancing_vampire.dae"; // �ȴ� ����� �� ��� -- ��ǿ�
    string catEatPath = dataDirStr + "\\vampire\\dae2\\dancing_vampire.dae"; // �Դ� ����� �� ��� -- ��ǿ�
    string catJoyPath = dataDirStr + "\\vampire\\dae\\dancing_vampire.dae"; // ��ſ� ����� �� ��� -- ��ǿ�
    string catDiePath = dataDirStr + "\\vampire\\dae2\\dancing_vampire.dae"; // ���� �߸����� ����� �� ��� -- ��ǿ�

    //string modelPath = modelDirStr + "/chapa/dae/Chapa-Giratoria.dae";

    // build and compile shaders
    // -------------------------
    string vs = sourceDirStr + "\\skel_anim.vs"; // vertex shader
    string fs = sourceDirStr + "\\skel_anim.fs"; // fragment shader

    string vsText = sourceDirStr + "\\text_render.vs"; // text�� vertex shader
    string fsText = sourceDirStr + "\\text_render.fs"; // text�� fragment shader

    cat = new Cat(catModelPath, catWalkPath, catEatPath, catJoyPath, catDiePath, // models path
        vs, fs, // shaders
        1.f, 1.f, 1.f, // default scale
        0.f, glm::vec3(0.f, 1.f, 0.f), // default rotation
        0.f, -1.f, 0.f // default translation
    );


    mainText = new Text(vsText, fsText, fontPath, textProjection, U"Stage" + intToChar32(stage + 1), darkblue);
    mainText->setPos(SCR_WIDTH * 0.43f, SCR_HEIGHT * 0.9f, 0.8f);

    messageText = new Text(vsText, fsText, fontPath, textProjection, U"", darkblue);
    messageText->setPos(SCR_WIDTH * 0.3f, SCR_HEIGHT * 0.5f, 1.f);

    glEnable(GL_CULL_FACE); // cull face to reduce memory usage
    glClearColor(1.f, 1.f, 1.f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // render loop
    // -----------

    const float PI = 3.141592;

    cat->resetToRetry();
    while (!glfwWindowShouldClose(mainWindow))
    {
        GLdouble now = glfwGetTime();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // �����ӿ� ���缭 ��ǲ �ް� ���� ���� 
        if ((now - lastFrameTime) >= MAX_FRAMERATE_LIMIT)
        {
            float factor = (catMovingLeft ? -1.f : 1.f);
            // ----------
            // ����� ��� - 1. ��׸� ��ġ���� ������
            if (catMoveFlag) { // ����� ������ �غ�
                cout << "   cat start Move" << endl;
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

                cat->rotate(factor * 90.f, glm::vec3(0.f, 1.f, 0.f));
            }
            if (catMoving) { // ����� ������
                cout << " - ";
                catMovedTime = now - catMoveStartTime;
                catMoveAmt = 0.015 * factor * sin(catMovedTime / 2.f * PI);
                cat->translate(catMoveAmt, 0.f, 0.f);
                if (now - catMoveStartTime >= 2) {
                    cout << "   cat will Stop" << endl;
                    // stops move at next frame
                    catMoveStopFlag = true;
                    catMoving = false;
                }
            }
            if (catMoveStopFlag) { // ����� ���� �غ�
                cout << endl << "   cat Stopped" << endl;
                catMoveStopFlag = false;
                catStopAndEatFlag = true;
                cat->eat();
            }

            // ---------------- 
            // ����� ��� - 2. �� ���� 
            if (catStopAndEatFlag) { // ����� ���� �غ�
                cout << "   cat starts Eating" << endl;
                // starts eat at next frame
                catStopAndEatStartTime = now;
                catStopAndEatFlag = false;
                catStopAndEating = true;
                catStopAndEatStopFlag = false;
            }
            if (catStopAndEating) { // ����� ����
                cout << " - ";

                if (now - catStopAndEatStartTime >= 2) {
                    cout << "   cat will Stop Eating" << endl;
                    // stops move at next frame
                    catStopAndEatStopFlag = true;
                    catStopAndEating = false;
                }
            }
            if (catStopAndEatStopFlag) { // ����� �Դ°� ���� �غ�
                cout << endl << "   cat Stopped Eating" << endl;
                catStopAndEatStopFlag = false;
                catShowResultFlag = true;
                catMoveNext = cat->result((catMovingLeft ? foodLeft : foodRight));
            }

            // ---------------- 
            // ����� ��� - 3. ���� ǥ�� 
            if (catShowResultFlag) { // ����� ���� �غ�
                cout << "   cat starts Eating" << endl;
                // starts eat at next frame
                catShowResultStartTime = now;
                catShowResultFlag = false;
                catShowingResult = true;
                catShowResultFlag = false;
                cat->rotate(factor * -1.f * 90.f, glm::vec3(0.f, 1.f, 0.f));
            }
            if (catShowingResult) { // ����� ����
                cout << " - ";

                if (now - catMoveStartTime >= 3) {
                    cout << "   stage will change or return to 1" << endl;
                    // stops move at next frame
                    catShowResultStopFlag = true;
                    catShowingResult = false;
                }
            }
            if (catShowResultStopFlag) { // ����� ���� ���� �غ�
                cout << endl << "   cat stopped" << endl;
                catShowResultStopFlag = false;
                catStageTransitionFlag = true;
            }

            // ---------------- 
            // 4. ��ȯ  
            if (catStageTransitionFlag) { // �������� ��ȯ �غ� 
                cout << "   going to next stage" << endl;
                // starts eat at next frame
                catStageTransitionStartTime = now;
                catStageTransitionFlag = false;
                catStageTransitioning = true;
                catStageTransitionStopFlag = false;
            }
            if (catStageTransitioning) { // �������� ��ȯ
                cout << " - ";
                if (catMoveNext) {
                    messageText->setText(U"�Ǹ��ؿ�!"); // TODO
                    if (now - catStageTransitionStartTime >= 5) {
                        cout << "   cat will go next" << endl;
                        // stops move at next frame
                        catStageTransitionStopFlag = true;
                        catStageTransitioning = false;
                    }
                }
                else {
                    messageText->setText(U"���ݸ��� ���̸� �� �˴ϴ� ��¼��"); // TODO
                    if (now - catStageTransitionStartTime >= 5) {
                        cout << "   cat will go next" << endl;
                        // stops move at next frame
                        catStageTransitionStopFlag = true;
                        catStageTransitioning = false;
                    }
                }

            }
            if (catStageTransitionStopFlag) { // ���� �غ�
                cout << endl << "   go to next" << endl;
                catStageTransitionStopFlag = false;
                messageText->clearText();

                if (catMoveNext) {
                    cat->grow();
                    goToNextStage();
                }
                else {
                    cat->resetToRetry();
                    goToFirstStage();
                }

                catEating = false; // �Է� ���� Ǯ��

            }
                // End of the Motions

                processInput(mainWindow);
                deltaTime = now - lastUpdateTime;


                lastFrameTime = now;
            }

            // Draw
            cat->draw();
            mainText->draw();
            messageText->draw();
            // TODO �ʿ�, ��׸�, �� �׸���, Lighting

            lastUpdateTime = now;
            glfwSwapBuffers(mainWindow);
            glfwPollEvents();
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
        if (!catEating) {
            cout << "cat eat left" << endl;
            catMoveFlag = true;
            catMovingLeft = true;
            catEating = true;
        }
        
    }
    else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        if (!catEating) {
            cout << "cat eat right" << endl;
            catMoveFlag = true;
            catMovingLeft = false;
            catEating = true;
        }
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

// 
void goToNextStage() 
{
    stage += 1;
    cout << "Go to next Stage -- " << stage << endl;

    mainText->setText(U"Stage" + intToChar32(stage + 1)); // update stage label
    // TODO -- ���� ����
    // TODO -- �������� ���� �����ϸ� ���� ����
}
void goToFirstStage()
{
    stage = 0;
    cout << "Go to 1st Stage -- " << stage << endl;

    mainText->setText(U"Stage" + intToChar32(stage + 1)); // update stage label
    // TODO -- ���� ����
}
#include <WindowManager.hpp>

WindowManager::WindowManager(string title, int width, int height)
{
    this->width = width;
    this->height = height;
    this->leftButtonIsPressing = false;
    this->clipping = { 0.0f, 0.0f, 0.0f, 0.0f };

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

    this->window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
    if (window == NULL)
    {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "Failed to initialize GLAD" << endl;
        glfwTerminate();
    }

    glViewport(0, 0, width, height);
}

void WindowManager::initCallbacks()
{
    glfwSetWindowUserPointer(this->window, this);

    auto resizeCb = [](GLFWwindow* window, int width, int height){
        static_cast<WindowManager*>(glfwGetWindowUserPointer(window))->resizeCallback(window, width, height);
    };

    auto keyCb = [](GLFWwindow * window, int key, int scancode, int action, int mods){
        static_cast<WindowManager*>(glfwGetWindowUserPointer(window))->keyCallback(window, key, scancode, action, mods);
    };

    auto mouseMoveCb = [](GLFWwindow* window, double xpos, double ypos){
        static_cast<WindowManager*>(glfwGetWindowUserPointer(window))->mouseMoveCallback(window, xpos, ypos);
    };

    auto mousePressCb = [](GLFWwindow* window, int button, int action, int mods){
        static_cast<WindowManager*>(glfwGetWindowUserPointer(window))->mousePressCallback(window, button, action, mods);
    };

    auto scrollCb = [](GLFWwindow* window, double xoffset, double yoffset){
        static_cast<WindowManager*>(glfwGetWindowUserPointer(window))->scrollCallback(window, xoffset, yoffset);
    };

    glfwSetFramebufferSizeCallback(this->window, resizeCb);
    glfwSetKeyCallback(this->window, keyCb);
    glfwSetMouseButtonCallback(this->window, mousePressCb);
    glfwSetCursorPosCallback(this->window, mouseMoveCb);
    glfwSetScrollCallback(this->window, scrollCb);
}

void WindowManager::initObjects(string filename, float isovalue)
{

    camera = new Camera(glm::vec3(0.0f, 0.0f, -10.0f), 90.0f, 0.0f);
    light = new Light(camera->getPosition(), camera->getDirection());
    cubeModel = new CubeModel();

    fr = new FileReader("./Data/VolumeData/");
    fr->initNameList();
    fr->readRawData(filename);

    iso.push_back(new Isosurface(fr->getData(), fr->getInfo()->getVoxelSize(), isovalue));
}

void WindowManager::resizeCallback(GLFWwindow* window, int width, int height)
{
    this->width = width;
    this->height = height;

    glViewport(0, 0, width, height);
}

void WindowManager::keyCallback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
    string filename;
    float isovalue;
    static char option = 's';

    if(action == GLFW_PRESS)
    {
        switch(key)
        {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(this->window, GLFW_TRUE);
                break;

            case GLFW_KEY_S:
                for(auto i: iso)
                    i->setShader("src/Shaders/vertex.vert", "src/Shaders/fragment.frag");
                break;

            case GLFW_KEY_F:
                cout << "data file name(exclude file extension): ";
                cin >> filename;
                cout << "isovalue: ";
                cin >> isovalue;

                do
                {
                    cout << "multiple or single (m/s): ";
                    cin >> option;
                }while(option != 's' && option != 'm');

                fr->readRawData(filename);

                if( option == 's')
                    iso.clear();
                
                iso.push_back(new Isosurface(fr->getData(), fr->getInfo()->getVoxelSize(), isovalue));

                for(auto i: iso)
                    i->marchingCube();
                break;

            case GLFW_KEY_I:
                if(option != 's')
                    break;

                cout << "isovalue: ";
                cin >> isovalue;

                iso[0]->setIsovalue(isovalue);
                iso[0]->marchingCube();
                break;

            case GLFW_KEY_C:
                if(makeCrossSection)
                    makeCrossSection = false;
                else
                    makeCrossSection = true;
            default:
                break;
        }
    }
}

void WindowManager::checkKeyPress()
{
    if (glfwGetKey(window, GLFW_KEY_KP_1) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
    {
        setClippingNormal(0, false);
    }
    if (glfwGetKey(window, GLFW_KEY_KP_2) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
    {
        setClippingNormal(1, false);
    }
    if (glfwGetKey(window, GLFW_KEY_KP_3) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
    {
        setClippingNormal(2, false);
    }
    if (glfwGetKey(window, GLFW_KEY_KP_4) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
    {
        setClippingNormal(0, true);
    }
    if (glfwGetKey(window, GLFW_KEY_KP_5) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
    {
        setClippingNormal(1, true);
    }
    if (glfwGetKey(window, GLFW_KEY_KP_6) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
    {
        setClippingNormal(2, true);
    }
    if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS)
    {
        setClippingValue(true);
    }
    if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS)
    {
        setClippingValue(false);
    }
}

void  WindowManager::mousePressCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        this->leftButtonIsPressing = true;
    else if( button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
        this->leftButtonIsPressing = false;
}

void WindowManager::mouseMoveCallback(GLFWwindow* window, double xpos, double ypos)
{
    static bool firstPress = true;
    static float lastX, lastY;
    float yoffset, xoffset;

    if( leftButtonIsPressing )
    {
        if( firstPress )
        {
            lastX = xpos;
            lastY = ypos;
            firstPress = false;
            return;
        }

        xoffset = xpos - lastX;
        yoffset = lastY - ypos;

        lastX = xpos;
        lastY = ypos;

        const float sensitivity = 0.5f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        this->camera->move(xoffset, yoffset);
        this->light->getSpotlight(0)->setPosition(camera->getPosition());
        this->light->getSpotlight(0)->setDirection(camera->getDirection());
    }

    firstPress = true;
}

void WindowManager::scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera->zoom(yoffset); 
}

void WindowManager::setClippingNormal(int order, bool increase)
{
    if( increase ) clipping[order] += 0.02f;
    else clipping[order] -= 0.02f;

    glm::vec3 temp = glm::vec3(clipping[0], clipping[1], clipping[2]);
    temp = glm::normalize(temp);

    clipping[0] = temp.x; clipping[1] = temp.y; clipping[2] = temp.z;

    for(auto t: clipping) cout << t << ", ";
    cout << endl;
}

void WindowManager::setClippingValue(bool increase)
{
    if( increase ) clipping[3]++;
    else clipping[3]--;

    for(auto t: clipping) cout << t << ", ";
    cout << endl;
}

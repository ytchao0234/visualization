#include <WindowManager.hpp>

WindowManager::WindowManager(string title, int width, int height, string glslVersion)
{
    this->title = title;
    this->width = width;
    this->height = height;
    this->leftButtonIsPressing = false;
    this->clipping = { 1.0f, 1.0f, 1.0f, 500.0f };
    this->makeCrossSection = false;
    this->glslVersion = glslVersion;

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

WindowManager::~WindowManager()
{
    glfwDestroyWindow(window);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void WindowManager::initGUI()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glslVersion.c_str());
    
}

void WindowManager::renderGUI(vector<string> infFileList, vector<string> rawFileList)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // ImPlot::ShowDemoWindow();

    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_Once);

    ImGui::Begin("Main Menu");
    {
        ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::NewLine();

        ImGui::Text("File: ");

        static const char* selectedInf = "engine";
        static const char* selectedRaw = selectedInf;
        static int isovalue = 80;

        if (ImGui::BeginCombo(".inf", selectedInf))
        {
            for (int i = 0; i < infFileList.size(); i++)
            {
                if(ImGui::Selectable(infFileList[i].c_str()))
                {
                    selectedInf = infFileList[i].c_str();
                    selectedRaw = selectedInf;
                    fr->readFile(selectedInf, selectedRaw);
                }
                if(selectedInf == infFileList[i])
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        if (ImGui::BeginCombo(".raw", selectedRaw))
        {
            for (int i = 0; i < rawFileList.size(); i++)
            {
                if(strstr(rawFileList[i].c_str(), selectedInf))
                {
                    if(ImGui::Selectable(rawFileList[i].c_str()))
                    {
                        selectedRaw = rawFileList[i].c_str();
                        fr->readFile(selectedInf, selectedRaw);
                    }
                    if(selectedRaw == rawFileList[i])
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
            }
            ImGui::EndCombo();
        }

        ImGui::Text("Isovalue:");
        ImGui::Text("%s", to_string((int)(fr->getMinValue())).c_str());
        ImGui::SameLine();
        ImGui::SliderInt(to_string((int)(fr->getMaxValue())).c_str(), &isovalue, fr->getMinValue(), fr->getMaxValue());

        if(ImGui::Button("Load Single"))
        {
            iso.clear();
            iso.push_back(new Isosurface(fr->getData(), fr->getInfo()->getVoxelSize(), isovalue));
            iso.back()->marchingCube();
        }

        ImGui::SameLine();

        if(ImGui::Button("Load Multiple"))
        {
            iso.push_back(new Isosurface(fr->getData(), fr->getInfo()->getVoxelSize(), isovalue));
            iso.back()->marchingCube();
        }

        if(ImGui::Button("Clear"))
        {
            iso.clear();
        }

        ImGui::NewLine();
        ImGui::Text("Clipping Plane: ");

        if(ImGui::SliderFloat("x", &clipping[0], -1.0f, 1.0f)|
           ImGui::SliderFloat("y", &clipping[1], -1.0f, 1.0f)|
           ImGui::SliderFloat("z", &clipping[2], -1.0f, 1.0f))
        {
            glm::vec3 temp = glm::vec3(clipping[0], clipping[1], clipping[2]);
            temp = glm::normalize(temp);

            clipping[0] = temp.x; clipping[1] = temp.y; clipping[2] = temp.z;
        }

        ImGui::SliderFloat("offset", &clipping[3], -500.0f, 500.0f);
        ImGui::Checkbox("make cross-section", &makeCrossSection);
    } ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(width - 300 - 10, 10), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_Once);

    ImGui::Begin("Histogram");
    {
        ImGui::PlotHistogram("", fr->getHistogram().data(), fr->getHistogram().size(), 0, "", 0.0f, (float)(fr->getMaxNum()), ImVec2(285, 250));
    }ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(width - 300 - 10, 350), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_Once);

    ImGui::Begin("Heap Map");
    {
        static float values1[7][7] = {{0.8f, 2.4f, 2.5f, 3.9f, 0.0f, 4.0f, 0.0f},
                                    {2.4f, 0.0f, 4.0f, 1.0f, 2.7f, 0.0f, 0.0f},
                                    {1.1f, 2.4f, 0.8f, 4.3f, 1.9f, 4.4f, 0.0f},
                                    {0.6f, 0.0f, 0.3f, 0.0f, 3.1f, 0.0f, 0.0f},
                                    {0.7f, 1.7f, 0.6f, 2.6f, 2.2f, 6.2f, 0.0f},
                                    {1.3f, 1.2f, 0.0f, 0.0f, 0.0f, 3.2f, 5.1f},
                                    {0.1f, 2.0f, 0.0f, 1.4f, 0.0f, 1.9f, 6.3f}};
        static float scale_min = 0;
        static float scale_max = 6.3f;
        static ImPlotAxisFlags axes_flags = ImPlotAxisFlags_LockMin | ImPlotAxisFlags_LockMax | ImPlotAxisFlags_TickLabels;
        static const char* xlabels[] = {"C1","C2","C3","C4","C5","C6","C7"};
        static const char* ylabels[] = {"R1","R2","R3","R4","R5","R6","R7"};

        ImPlot::SetNextPlotTicksX(0 + 1.0/14.0, 1 - 1.0/14.0, 7, xlabels);
        ImPlot::SetNextPlotTicksY(1- 1.0/14.0, 0 + 1.0/14.0, 7,  ylabels);

        if (ImPlot::BeginPlot("##Heatmap1",NULL,NULL,ImVec2(225,225),0,axes_flags,axes_flags)) {
            ImPlot::PlotHeatmap("heat",values1[0],7,7,scale_min,scale_max);
            ImPlot::EndPlot();
        }
        ImGui::SameLine();
        ImPlot::ShowColormapScale(scale_min, scale_max, 225);
        static ImVec4 gray[2] = {ImVec4(0,0,0,1), ImVec4(1,1,1,1)};
        ImPlot::SetColormap(&gray[0], 2);

    }ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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

void WindowManager::initObjects()
{
    camera = new Camera(glm::vec3(0.0f, 0.0f, -10.0f), 90.0f, 0.0f);
    light = new Light(camera->getPosition(), camera->getDirection());
    cubeModel = new CubeModel();

    fr = new FileReader("./Data/VolumeData/");
    fr->initNameList();
    fr->readFile("engine", "engine");
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
    if(ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow))
        return;
    
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

    ImGuiIO& io = ImGui::GetIO();
	io.MouseWheelH += (float)xoffset;
	io.MouseWheel += (float)yoffset;
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

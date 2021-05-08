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
    this->methods = {"Marching Cube", "Ray Casting", "Slicing"};

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
    ImGui::SetNextWindowSize(ImVec2(300, 420), ImGuiCond_Once);

    static bool toChangeHeatMapRange = true;

    ImGui::Begin("Main Menu");
    {
        ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::NewLine();

        ImGui::Text("Method: ");
        static string selectedMethod = methods[0];

        if (ImGui::BeginCombo("method", selectedMethod.c_str()))
        {
            for (int i = 0; i < methods.size(); i++)
            {
                if(ImGui::Selectable(methods[i].c_str()))
                {
                    cout << methods[i] << endl;
                    selectedMethod = methods[i];
                    
                    for(auto &i : iso)
                        i->setMethod(selectedMethod);
                }
                if(selectedMethod == methods[i])
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
        ImGui::NewLine();

        ImGui::Text("File: ");

        static string selectedInf = "engine";
        static string selectedRaw = selectedInf;
        static int isovalue = 80;

        if (ImGui::BeginCombo(".inf", selectedInf.c_str()))
        {
            for (int i = 0; i < infFileList.size(); i++)
            {
                if(ImGui::Selectable(infFileList[i].c_str()))
                {
                    selectedInf = infFileList[i];
                    selectedRaw = selectedInf;
                    fr->setGMaxLimit(0);
                    fr->readFile(selectedInf, selectedRaw);
                    fr->calcuGradient();
                    fr->calcuGraph();
                    toChangeHeatMapRange = true;
                }
                if(selectedInf == infFileList[i])
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        if (ImGui::BeginCombo(".raw", selectedRaw.c_str()))
        {
            for (int i = 0; i < (int)rawFileList.size(); i++)
            {
                if(strstr(rawFileList[i].c_str(), selectedInf.c_str()))
                {
                    if(ImGui::Selectable(rawFileList[i].c_str()))
                    {
                        selectedRaw = rawFileList[i];
                        fr->setGMaxLimit(0);
                        fr->readFile(selectedInf, selectedRaw);
                        fr->calcuGradient();
                        fr->calcuGraph();
                        toChangeHeatMapRange = true;
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
        ImGui::Text("%s", to_string((int)(fr->getIMin())).c_str());
        ImGui::SameLine();
        ImGui::SliderInt(to_string((int)(fr->getIMax())).c_str(), &isovalue, fr->getIMin(), fr->getIMax());

        if(ImGui::Button("Load Single"))
        {
            iso.clear();
            iso.push_back(new Isosurface(fr->getData(), fr->getIMin(), fr->getIMax(), fr->getDataGradient(), fr->getInfo()->getVoxelSize(), selectedMethod, isovalue));
            iso.back()->makeVertices();
        }

        ImGui::SameLine();

        if(ImGui::Button("Load Multiple"))
        {
            iso.push_back(new Isosurface(fr->getData(), fr->getIMin(), fr->getIMax(), fr->getDataGradient(), fr->getInfo()->getVoxelSize(), selectedMethod, isovalue));
            iso.back()->makeVertices();
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

    ImGui::SetNextWindowPos(ImVec2(10, height - 460 - 10), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(460, 460), ImGuiCond_Once);

    ImGui::Begin("Graph");
    {
        if(ImGui::CollapsingHeader("Histogram"))
        {
            static bool toLog = true;
            ImGui::Checkbox("Log", &toLog);

            static bool changeHistogram = false;
            ImGui::Checkbox("ImGui Histogram", &changeHistogram);

            if(changeHistogram)
            {
                if(toLog)
                    ImGui::PlotHistogram("", fr->getLogIHistogram().data(), fr->getLogIHistogram().size(),
                                         0, "Frequency of Intensity", 0.0f,
                                         (float)(fr->getLogIMaxNum()), ImVec2(430, 300));
                else
                    ImGui::PlotHistogram("", fr->getIHistogram().data(), fr->getIHistogram().size(),
                                         0, "Frequency of Intensity", 0.0f,
                                         (float)(fr->getIMaxNum()), ImVec2(430, 300));
            }
            else
            {
                if(toLog)
                {
                    ImPlot::SetNextPlotLimits(fr->getIMin(), fr->getIMax(), 0.0f, fr->getLogIMaxNum(), ImGuiCond_Always);

                    if (ImPlot::BeginPlot("Frequency of Intensity", "Intensity", "Frequency")) {
                        
                        ImPlot::PlotBars("frequency", fr->getLogIHistogram().data(), fr->getLogIHistogram().size(), 0.67f, 0.0f, -(int)(fr->getIMin()));

                        ImPlot::EndPlot();
                    }
                }
                else
                {
                    ImPlot::SetNextPlotLimits(fr->getIMin(), fr->getIMax(), 0.0f, fr->getIMaxNum(), ImGuiCond_Always);

                    if (ImPlot::BeginPlot("Frequency of Intensity", "Intensity", "Number")) {
                        
                        ImPlot::PlotBars("frequency", fr->getIHistogram().data(), fr->getIHistogram().size(), 0.67f, 0.0f, -(int)(fr->getIMin()));

                        ImPlot::EndPlot();
                    }
                }
            }
        }

        if(ImGui::CollapsingHeader("Heat Map"))
        {
            static float x_min = fr->getIMin(), x_max = fr->getIMax(), y_min = fr->getDecibelMin(), y_max = fr->getDecibelMax();

            ImGui::Text("gMax: ");
            ImGui::Text(to_string(fr->getGMin()).c_str());
            ImGui::SameLine();
            ImGui::SliderFloat(to_string(fr->getGMax()).c_str(), &(fr->gMaxLimit), fr->getGMin(), fr->getGMax());

            if(ImGui::Button("Reload Heat Map"))
            {
                fr->calcuGraph();

                toChangeHeatMapRange = true;
            }

            if(toChangeHeatMapRange)
            {
                x_min = fr->getIMin(), x_max = fr->getIMax(), y_min = fr->getDecibelMin(), y_max = fr->getDecibelMax();
                toChangeHeatMapRange = false;
            }

            static ImVec4 gray[2] = {ImVec4(0,0,0,1), ImVec4(1,1,1,1)};
            ImPlot::SetColormap(gray, 2);

            ImPlot::SetNextPlotLimits(x_min, x_max, y_min, y_max, ImGuiCond_Always);

            static ImPlotAxisFlags axes_flags = ImPlotAxisFlags_LockMin | ImPlotAxisFlags_LockMax | ImPlotAxisFlags_TickLabels;

            if (ImPlot::BeginPlot("","Intensity","Magnitude of Gradient",ImVec2(370,310),0,axes_flags,axes_flags))
            {
                ImPlot::PlotHeatmap("frequency",fr->getHeatMap(),fr->getHeatMapRows(),fr->getHeatMapCols(),0, fr->getHMaxNum(),
                                    NULL, ImPlotPoint(x_min,y_min), ImPlotPoint(x_max,y_max));
                ImPlot::EndPlot();
            }
            ImGui::SameLine();
            ImPlot::ShowColormapScale(0, fr->getHMaxNum(), 310);
            ImPlot::SetColormap(ImPlotColormap_Default);
        }
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
    fr->calcuGradient();
    fr->calcuGraph();
}

void WindowManager::resizeCallback(GLFWwindow* window, int width, int height)
{
    this->width = width;
    this->height = height;

    glViewport(0, 0, width, height);
}

void WindowManager::keyCallback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
    if(action == GLFW_PRESS)
    {
        switch(key)
        {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(this->window, GLFW_TRUE);
                break;

            case GLFW_KEY_S:
                for(auto i: iso)
                    i->setShader();
                break;

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

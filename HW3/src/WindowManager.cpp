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
    this->methods = {"Isosurface", "Ray Casting", "Slicing"};

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

    static bool toRenderGraph = false;
    static float gMaxLimit = fr->getVolumeData()->gradMax;

    ImGui::Begin("Main Menu");
    {
        ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::NewLine();

        ImGui::Text("Method: ");
        static string selectedMethod = methods[0];

        if (ImGui::BeginCombo("method", selectedMethod.c_str()))
        {
            for (int i = 0; i < (int)methods.size(); i++)
            {
                if(ImGui::Selectable(methods[i].c_str()))
                {
                    selectedMethod = methods[i];
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
        static bool toLoad = false;

        if (ImGui::BeginCombo(".inf", selectedInf.c_str()))
        {
            for (int i = 0; i < (int)infFileList.size(); i++)
            {
                if(ImGui::Selectable(infFileList[i].c_str()))
                {
                    selectedInf = infFileList[i];
                    selectedRaw = selectedInf;
                    toLoad = false;
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
                        toLoad = false;
                    }
                    if(selectedRaw == rawFileList[i])
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
            }
            ImGui::EndCombo();
        }
        
        if(selectedMethod == "Isosurface")
            if(ImGui::SliderInt(to_string((int)(fr->getVolumeData()->dataMax)).c_str(), &isovalue,
                            fr->getVolumeData()->dataMin, fr->getVolumeData()->dataMax))
                toLoad = true;


        if(ImGui::Button("Read File"))
        {
            fr->readFile(selectedInf, selectedRaw);
            gMaxLimit = fr->getVolumeData()->gradMax;

            if(histogram != NULL) delete histogram;
            histogram = new Histogram(fr->getVolumeData());

            if(heatmap != NULL) delete heatmap;
            heatmap = new Heatmap(fr->getVolumeData(), gMaxLimit);

            toLoad = true;
        }

        ImGui::SameLine();

        if(ImGui::Button("Clear"))
        {
            volumeList.clear();
        }

        if(toLoad)
        {
            if(ImGui::Button("Load Single"))
            {
                volumeList.clear();

                if(selectedMethod == "Isosurface")
                    volumeList.push_back(new Isosurface(fr->getVolumeData(), isovalue));
                else if(selectedMethod == "Ray Casting")
                    volumeList.push_back(new RayCasting(fr->getVolumeData()));

                volumeList.back()->makeVertices();
                toLoad = false;
            }

            ImGui::SameLine();

            if(ImGui::Button("Load Multiple"))
            {
                if(selectedMethod == "Isosurface")
                    volumeList.push_back(new Isosurface(fr->getVolumeData(), isovalue));
                else if(selectedMethod == "Ray Casting")
                    volumeList.push_back(new RayCasting(fr->getVolumeData()));

                volumeList.back()->makeVertices();
                toLoad = false;
            }
        }

        ImGui::Checkbox("Render Graph", &toRenderGraph);

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

    if(toRenderGraph)
    {
        ImGui::Begin("Graph");
        {
            if(ImGui::CollapsingHeader("Histogram") && histogram != NULL)
            {
                static bool toLog = false;
                static bool toEqual = false;
                if(ImGui::Checkbox("Log", &toLog) && toLog) toEqual = false;
                if(ImGui::Checkbox("Equalize", &toEqual) && toEqual) toLog = false;

                static bool changeHistogram = false;
                ImGui::Checkbox("ImGui Histogram", &changeHistogram);

                if(changeHistogram)
                {
                    if(toLog)
                        ImGui::PlotHistogram("", histogram->getData("LOG").data(), histogram->getRangeMax("LOG") - histogram->getRangeMin("LOG") + 1,
                                            0, "Frequency of Intensity", (float)histogram->getValueMin("LOG"),
                                            (float)histogram->getValueMax("LOG"), ImVec2(430, 300));
                    if(toEqual)
                        ImGui::PlotHistogram("", histogram->getData("EQUAL").data(), histogram->getRangeMax("EQUAL") - histogram->getRangeMin("EQUAL") + 1,
                                            0, "Frequency of Intensity", (float)histogram->getValueMin("EQUAL"),
                                            (float)histogram->getValueMax("EQUAL"), ImVec2(430, 300));
                    else
                        ImGui::PlotHistogram("", histogram->getData("ORIGIN").data(), histogram->getRangeMax("ORIGIN") - histogram->getRangeMin("ORIGIN") + 1,
                                            0, "Frequency of Intensity", (float)histogram->getValueMin("ORIGIN"),
                                            (float)histogram->getValueMax("ORIGIN"), ImVec2(430, 300));
                }
                else
                {
                    if(toLog)
                    {
                        ImPlot::SetNextPlotLimits(histogram->getRangeMin("LOG"), histogram->getRangeMax("LOG"),
                                                  histogram->getValueMin("LOG"), histogram->getValueMax("LOG"), ImGuiCond_Always);

                        if (ImPlot::BeginPlot("Frequency of Intensity", "Intensity", "Frequency")) {
                            
                            ImPlot::PlotBars("frequency", histogram->getData("LOG").data(), histogram->getRangeMax("LOG") - histogram->getRangeMin("LOG") + 1,
                                             0.67f, 0.0f, histogram->getRangeOffset("LOG"));
                            ImPlot::EndPlot();
                        }
                    }
                    if(toEqual)
                    {
                        ImPlot::SetNextPlotLimits(histogram->getRangeMin("EQUAL"), histogram->getRangeMax("EQUAL"),
                                                  histogram->getValueMin("EQUAL"), histogram->getValueMax("EQUAL"), ImGuiCond_Always);

                        if (ImPlot::BeginPlot("Frequency of Intensity", "Intensity", "Frequency")) {
                            
                            ImPlot::PlotBars("frequency", histogram->getData("EQUAL").data(), histogram->getRangeMax("EQUAL") - histogram->getRangeMin("EQUAL") + 1,
                                             0.67f, 0.0f, histogram->getRangeOffset("EQUAL"));
                            ImPlot::EndPlot();
                        }
                    }
                    else
                    {
                        ImPlot::SetNextPlotLimits(histogram->getRangeMin("ORIGIN"), histogram->getRangeMax("ORIGIN"),
                                                  histogram->getValueMin("ORIGIN"), histogram->getValueMax("ORIGIN"), ImGuiCond_Always);

                        if (ImPlot::BeginPlot("Frequency of Intensity", "Intensity", "Frequency")) {
                            
                            ImPlot::PlotBars("frequency", histogram->getData("ORIGIN").data(), histogram->getRangeMax("ORIGIN") - histogram->getRangeMin("ORIGIN") + 1,
                                             0.67f, 0.0f, histogram->getRangeOffset("ORIGIN"));
                            ImPlot::EndPlot();
                        }
                    }
                }
            }

            if(ImGui::CollapsingHeader("Heat Map") && heatmap != NULL)
            {
                ImGui::Text("gMax: ");
                ImGui::Text(to_string(fr->getVolumeData()->gradMin).c_str());
                ImGui::SameLine();
                ImGui::SliderFloat(to_string(fr->getVolumeData()->gradMax).c_str(), &gMaxLimit, fr->getVolumeData()->gradMin, fr->getVolumeData()->gradMax);

                if(ImGui::Button("Reload Heat Map"))
                {
                    delete heatmap;
                    heatmap = new Heatmap(fr->getVolumeData(), gMaxLimit);
                }

                static ImVec4 gray[2] = {ImVec4(0,0,0,1), ImVec4(1,1,1,1)};
                ImPlot::SetColormap(gray, 2);

                ImPlot::SetNextPlotLimits(heatmap->getRangeMin("INTENSITY"), heatmap->getRangeMax("INTENSITY"),
                                          heatmap->getRangeMin("GRADMAG"), heatmap->getRangeMax("GRADMAG"), ImGuiCond_Always);

                static ImPlotAxisFlags axes_flags = ImPlotAxisFlags_LockMin | ImPlotAxisFlags_LockMax | ImPlotAxisFlags_TickLabels;

                if (ImPlot::BeginPlot("","Intensity","Magnitude of Gradient",ImVec2(370,310),0,axes_flags,axes_flags))
                {
                    ImPlot::PlotHeatmap("frequency",heatmap->getData().data(), heatmap->getRangeMax("GRADMAG") - heatmap->getRangeMin("GRADMAG") + 1,
                                        heatmap->getRangeMax("INTENSITY") - heatmap->getRangeMin("INTENSITY") + 1, 0, heatmap->getValueMax(), NULL,
                                        ImPlotPoint(heatmap->getRangeMin("INTENSITY"), heatmap->getRangeMin("GRADMAG")),
                                        ImPlotPoint(heatmap->getRangeMax("INTENSITY"), heatmap->getRangeMax("GRADMAG")));
                    ImPlot::EndPlot();
                }
                ImGui::SameLine();
                ImPlot::ShowColormapScale(0, heatmap->getValueMax(), 310);
                ImPlot::SetColormap(ImPlotColormap_Default);
            }
        }ImGui::End();
    }

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

    histogram = NULL;
    heatmap = NULL;
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
                for(auto volume: volumeList)
                    volume->setShader();
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

#include <WindowManager.hpp>

WindowManager::WindowManager(string title, int width, int height, string glslVersion)
{
    this->title = title;
    this->width = width;
    this->height = height;
    this->leftButtonIsPressing = false;
    this->clipping = { 1.0f, 1.0f, 1.0f, 500.0f };
    this->makeCrossSection = false;
    this->gap = 2.0f;
    this->adjust = 1.0f;
    this->threshold = 0.8f;
    this->alpha = 0.005;
    this->glslVersion = glslVersion;
    this->methods = {"Isosurface", "Ray Casting", "StreamLine"};
    this->importList.clear();

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

void WindowManager::renderGUI()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // ImPlot::ShowDemoWindow();
    // ImGui::ShowDemoWindow();

    static string selectedMethod = methods[0];
    static bool toRenderGraph = false;
    static bool toRenderCanvas = false;
    static float gMaxLimit = fr->getVolumeData()->gradMax;

    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(320, 420), ImGuiCond_Once);
    makeMainMenu(toRenderGraph, toRenderCanvas, selectedMethod, gMaxLimit);

    if(toRenderGraph)
    {
        ImGui::SetNextWindowPos(ImVec2(10, height - 460 - 10), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(450, 460), ImGuiCond_Once);
        makeGraph(gMaxLimit);
    }

    if(toRenderCanvas)
    {
        ImGui::SetNextWindowPos(ImVec2(width - 360 - 10, height - 490 - 10), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(360, 490), ImGuiCond_Once);
        makeCanvas(selectedMethod);
    }
    
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void WindowManager::makeMainMenu(bool& toRenderGraph, bool& toRenderCanvas, string& selectedMethod, float& gMaxLimit)
{
    ImGui::Begin("Main Menu");
    {
        ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::Checkbox("Graph", &toRenderGraph); ImGui::SameLine();
        ImGui::Checkbox("Canvas", &toRenderCanvas);

        ImGui::NewLine();

        ImGui::Text("Method: ");

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

        static bool drawing_s = false;

        if(selectedMethod == "StreamLine")
        {
            static string selectedVec = "1";
            static double h_step = 0.1;
            static int iteration = 500;
            static int minLength = 50;
            static double gridSize = 1;
            static double distanceLimit = 0.1;
            static bool toLoad_s = true;
            static bool generating = false;

            if (ImGui::BeginCombo(".vec", selectedVec.c_str()))
            {
                toLoad_s = false;
                for (auto vec: fr->getVecNameList())
                {
                    if(ImGui::Selectable(vec.c_str()))
                    {
                        selectedVec = vec;
                        fr->readFile(selectedVec);
                        toLoad_s = true;
                        drawing_s = false;
                    }
                    if(selectedVec == vec)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            if(toLoad_s)
            {
                if(ImGui::Button("Load"))
                {
                    toLoad_s = false;
                    drawing_s = true;
                    generating = false;
                    volumeList.clear();
                    volumeList.push_back(new StreamLine(fr->getVectorData(), h_step, iteration, minLength, gridSize, distanceLimit));
                    volumeList.back()->makeVertices();
                }

                ImGui::SameLine();
            }

            if(ImGui::Button("Clear"))
            {
                volumeList.clear();
                drawing_s = false;
            }

            if(ImGui::Button("Generate Test Data"))
            {
                toLoad_s = false;
                drawing_s = true;
                generating = true;
                volumeList.clear();
                volumeList.push_back(new StreamLine(fr->getVectorData(), h_step, iteration, minLength, gridSize, distanceLimit, generating));
                volumeList.back()->makeVertices();
            }

            ImGui::NewLine();
            ImGui::Text("Parameters: ");

            if(ImGui::InputDouble("h", &h_step, 0.1, 1))
            {
                if(h_step < 0.01) h_step = 0.01;
                else if(h_step > 1) h_step = 1;

                volumeList.clear();
                volumeList.push_back(new StreamLine(fr->getVectorData(), h_step, iteration, minLength, gridSize, distanceLimit, generating));
                
                if(drawing_s)
                    volumeList.back()->makeVertices();
            }

            if(ImGui::InputInt("iteration", &iteration, 100, 100))
            {
                if(iteration < minLength) iteration = minLength;
                else if(iteration > 5000) iteration = 5000;

                volumeList.clear();
                volumeList.push_back(new StreamLine(fr->getVectorData(), h_step, iteration, minLength, gridSize, distanceLimit, generating));
                
                if(drawing_s)
                    volumeList.back()->makeVertices();
            }

            if(ImGui::InputInt("Min Length", &minLength, 10, 100))
            {
                if(minLength < 0) minLength = 0;
                else if(minLength > 5000) minLength = 5000;

                volumeList.clear();
                volumeList.push_back(new StreamLine(fr->getVectorData(), h_step, iteration, minLength, gridSize, distanceLimit, generating));
                
                if(drawing_s)
                    volumeList.back()->makeVertices();
            }

            if(ImGui::InputDouble("grid size", &gridSize, 0.2, 10))
            {
                if(gridSize < 1) gridSize = 1;
                else if(gridSize > 20) gridSize = 20;

                volumeList.clear();
                volumeList.push_back(new StreamLine(fr->getVectorData(), h_step, iteration, minLength, gridSize, distanceLimit, generating));
                
                if(drawing_s)
                    volumeList.back()->makeVertices();
            }

            if(ImGui::InputDouble("distance limit", &distanceLimit, 0.1, 1))
            {
                if(distanceLimit < 0.1) distanceLimit = 0.1;
                else if(distanceLimit > 5) distanceLimit = 5;

                volumeList.clear();
                volumeList.push_back(new StreamLine(fr->getVectorData(), h_step, iteration, minLength, gridSize, distanceLimit, generating));
                
                if(drawing_s)
                    volumeList.back()->makeVertices();
            }

            if(drawing_s)
            {
                if(ImGui::InputDouble("Line Width", &alpha, 0.001, 1))
                {
                    if(alpha < 0.001) alpha = 0.001;
                    else if(alpha > 0.05) alpha = 0.05;
                    
                    volumeList.back()->setShader();
                }
            }
        }
        else
        {
            drawing_s = false;

            static string selectedInf = "engine";
            static string selectedRaw = selectedInf;
            static int isovalue = 80;
            static bool toLoad = false;

            if (ImGui::BeginCombo(".inf", selectedInf.c_str()))
            {
                for (auto inf: fr->getInfNameList())
                {
                    if(ImGui::Selectable(inf.c_str()))
                    {
                        selectedInf = inf;
                        selectedRaw = selectedInf;
                        toLoad = false;
                    }
                    if(selectedInf == inf)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            if (ImGui::BeginCombo(".raw", selectedRaw.c_str()))
            {
                for (auto raw: fr->getRawNameList())
                {
                    if(strstr(raw.c_str(), selectedInf.c_str()))
                    {
                        if(ImGui::Selectable(raw.c_str()))
                        {
                            selectedRaw = raw;
                            toLoad = false;
                        }
                        if(selectedRaw == raw)
                        {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                }
                ImGui::EndCombo();
            }
            
            if(selectedMethod == "Isosurface" && fr->getVolumeData()->valueMax > 0)
                if(ImGui::SliderInt(to_string((int)(fr->getVolumeData()->valueMax)).c_str(), &isovalue,
                                fr->getVolumeData()->valueMin, fr->getVolumeData()->valueMax))
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

            ImGui::NewLine();

            if(selectedMethod == "Isosurface")
            {
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
            }
            else if(selectedMethod == "Ray Casting")
            {
                ImGui::Text("Parameters: ");

                if(ImGui::SliderFloat("step", &gap, 0.1f, 10.0f));
                if(ImGui::SliderFloat("adjust", &adjust, 0.01f, 1.0f));
                if(ImGui::SliderFloat("threshold", &threshold, 0.1f, 1.0f));
            }
        }
    } ImGui::End();
}

void WindowManager::makeGraph(float& gMaxLimit)
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

bool comparePoint(glm::vec2 a, glm::vec2 b)
{
    return a.x < b.x;
}

bool comparePoint_all(pair<glm::vec2, int> a, pair<glm::vec2, int> b)
{
    return a.first.x < b.first.x;
}

void WindowManager::makeCanvas(string selectedMethod)
{
    ImGui::Begin("Transfer Function");
    {
        static bool startUpdating = false;
        static bool isUpdating = false;

        static vector<pair<glm::vec2, int>> points;
        static vector<glm::vec2> points_red, points_green, points_blue, points_alpha;
        static ImU32 RED  = IM_COL32(255, 0, 0, 255), GREEN = IM_COL32(0, 255, 0, 255),
                     BLUE = IM_COL32(0, 0, 255, 255), ALPHA = IM_COL32(180, 180, 180, 255);
        static bool toAddLine = false;

        static string importTrans;
        static char exportTrans[128];
        
        if(ImGui::BeginCombo(" ##1", importTrans.c_str()))
        {
            for (int i = 0; i < (int)importList.size(); i++)
            {
                if(ImGui::Selectable(importList[i].c_str()))
                {
                    importTrans = importList[i];
                }
                if(importTrans == importList[i])
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
        ImGui::SameLine();

        if(ImGui::Button("Import") && importTrans.length() > 0)
        {
            string filename = "./transferFunc/";
            filename += importTrans + ".trans";

            ifstream fs;
            fs.open(filename.c_str(), ios::in | ios::binary );

            if (!fs.is_open())
            {
                cout << "ERROR::FILEREADER::OPENFILE::" << filename.c_str() << "_FAILED"<< endl;
                exit(1);
            }

            string line;
            stringstream ss;
            int x, y, color;

            getline(fs, line);
            ss.clear();
            ss << line;

            points.clear();
            while(true)
            {
                ss >> x; ss >> y; ss >> color;
                if(ss.fail()) break;
                points.push_back({glm::vec2(x,y), color});
            }

            getline(fs, line);
            ss.clear();
            ss << line;

            points_red.clear();
            while(true)
            {
                ss >> x; ss >> y;
                if(ss.fail()) break;
                points_red.push_back(glm::vec2(x,y));
            }

            getline(fs, line);
            ss.clear();
            ss << line;

            points_green.clear();
            while(true)
            {
                ss >> x; ss >> y;
                if(ss.fail()) break;
                points_green.push_back(glm::vec2(x,y));
            }

            getline(fs, line);
            ss.clear();
            ss << line;

            points_blue.clear();
            while(true)
            {
                ss >> x; ss >> y;
                if(ss.fail()) break;
                points_blue.push_back(glm::vec2(x,y));
            }

            getline(fs, line);
            ss.clear();
            ss << line;

            points_alpha.clear();
            while(true)
            {
                ss >> x; ss >> y;
                if(ss.fail()) break;
                points_alpha.push_back(glm::vec2(x,y));
            }

            fs.close();
        }

        ImGui::InputText(" ##2", exportTrans, IM_ARRAYSIZE(exportTrans)); ImGui::SameLine();

        istringstream ss(exportTrans);

        if(ImGui::Button("Export") && ss.str().length() > 0)
        {
            string filename = "./transferFunc/";
            filename += ss.str() + ".trans";

            ofstream fs;
            fs.open(filename);

            if (!fs.is_open())
            {
                cout << "ERROR::FILEREADER::OPENFILE::" << filename.c_str() << "_FAILED"<< endl;
                exit(1);
            }

            for(auto point: points)
                fs << " " << point.first.x << " " << point.first.y << " " << point.second;
            fs << "\n";

            for(auto point: points_red)
                fs << " " << point.x << " " << point.y;
            fs << "\n";

            for(auto point: points_green)
                fs << " " << point.x << " " << point.y;
            fs << "\n";

            for(auto point: points_blue)
                fs << " " << point.x << " " << point.y;
            fs << "\n";

            for(auto point: points_alpha)
                fs << " " << point.x << " " << point.y;
            fs << "\n";

            fs.close();

            importList.push_back(ss.str());
            exportTrans[0] = '\0';
        }

        static int color = 0;
        ImGui::RadioButton("Red"  , &color, 0); ImGui::SameLine();
        ImGui::RadioButton("Green", &color, 1); ImGui::SameLine();
        ImGui::RadioButton("Blue" , &color, 2); ImGui::SameLine();
        ImGui::RadioButton("Alpha", &color, 3); ImGui::SameLine();

        if(ImGui::Button("Clear Canvas"))
        {
            points.clear();
            points_red.clear();
            points_green.clear();
            points_blue.clear();
            points_alpha.clear();
        }

        if(!startUpdating)
        {
            if(ImGui::Button("Start Updating"))
            {
                isUpdating = true;
                startUpdating = true;
            }
        }
        else
        {
            if(ImGui::Button(" Stop Updating"))
            {
                isUpdating = false;
                startUpdating = false;
            }
        }

        ImGui::NewLine();

        ImVec2 canvas_size = ImVec2(340.0f, 340.0f);
        ImVec2 canvas_topleft = ImGui::GetCursorScreenPos();
        ImVec2 canvas_buttomright = ImVec2(canvas_topleft.x + canvas_size.x, canvas_topleft.y + canvas_size.y);

        ImDrawList* draw_api = ImGui::GetWindowDrawList();
        draw_api->AddRectFilled(canvas_topleft, canvas_buttomright, IM_COL32(50, 50, 50, 255));
        draw_api->AddRect(canvas_topleft, canvas_buttomright, IM_COL32(255, 255, 255, 255));

        const float GRID_STEP = 34.0f;
        for (float x = 0.0f; x < canvas_size.x; x += GRID_STEP)
            draw_api->AddLine(ImVec2(canvas_topleft.x + x, canvas_topleft.y), ImVec2(canvas_topleft.x + x, canvas_buttomright.y), IM_COL32(200, 200, 200, 40));
        for (float y = 0.0f; y < canvas_size.y; y += GRID_STEP)
            draw_api->AddLine(ImVec2(canvas_topleft.x, canvas_topleft.y + y), ImVec2(canvas_buttomright.x, canvas_topleft.y + y), IM_COL32(200, 200, 200, 40));

        ImGuiIO& io = ImGui::GetIO();
        ImGui::InvisibleButton("canvas", canvas_size, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight | ImGuiButtonFlags_MouseButtonMiddle);
        const bool is_hovered = ImGui::IsItemHovered();
        const bool is_active = ImGui::IsItemActive();
        const glm::vec2 mouse_pos(io.MousePos.x - canvas_topleft.x, io.MousePos.y - canvas_topleft.y);

        if(is_hovered && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
        {
            points.push_back({ mouse_pos, color });
            sort(points.begin(), points.end(), comparePoint_all);

            switch(color)
            {
                case 0: points_red.push_back(mouse_pos);
                        sort(points_red.begin(), points_red.end(), comparePoint);
                        break;
                case 1: points_green.push_back(mouse_pos);
                        sort(points_green.begin(), points_green.end(), comparePoint);
                        break;
                case 2: points_blue.push_back(mouse_pos);
                        sort(points_blue.begin(), points_blue.end(), comparePoint);
                        break;
                case 3: points_alpha.push_back(mouse_pos);
                        sort(points_alpha.begin(), points_alpha.end(), comparePoint);
                        break;
            }
        }

        if(is_hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Middle))
        {
            auto point_iter = getIntersectedPoint(points, mouse_pos);

            if(point_iter != points.end())
            {
                vector<glm::vec2>::iterator iter;
                color = point_iter->second;

                switch(color)
                {
                    case 0: iter = getIntersectedPoint(points_red, mouse_pos);
                            points_red.erase(iter);
                            break;
                    case 1: iter = getIntersectedPoint(points_green, mouse_pos);
                            points_green.erase(iter);
                            break;
                    case 2: iter = getIntersectedPoint(points_blue, mouse_pos);
                            points_blue.erase(iter);
                            break;
                    case 3: iter = getIntersectedPoint(points_alpha, mouse_pos);
                            points_alpha.erase(iter);
                            break;
                }

                points.erase(point_iter);
            }
        }

        static vector<pair<glm::vec2, int>>::iterator draggingPoint_all;
        static vector<glm::vec2>::iterator draggingPoint, begin, end;
        static bool isDragging = false;

        if(!isDragging && ImGui::IsMouseDragging(ImGuiMouseButton_Left, 0.0f))
        {
            auto point_iter = getIntersectedPoint(points, mouse_pos);

            if(point_iter != points.end())
            {
                draggingPoint_all = point_iter;
                isDragging = true;
                color = point_iter->second;

                switch(color)
                {
                    case 0: draggingPoint = getIntersectedPoint(points_red, mouse_pos);
                            begin = points_red.begin();
                            end = points_red.end();
                            break;
                    case 1: draggingPoint = getIntersectedPoint(points_green, mouse_pos);
                            begin = points_green.begin();
                            end = points_green.end();
                            break;
                    case 2: draggingPoint = getIntersectedPoint(points_blue, mouse_pos);
                            begin = points_blue.begin();
                            end = points_blue.end();
                            break;
                    case 3: draggingPoint = getIntersectedPoint(points_alpha, mouse_pos);
                            begin = points_alpha.begin();
                            end = points_alpha.end();
                            break;
                }
            }
        }

        if(isDragging)
        {
            float x = mouse_pos.x, y = mouse_pos.y;

            setLt(x, 0.0f); setGt(x, canvas_size.x);
            setLt(y, 0.0f); setGt(y, canvas_size.y);

            if(draggingPoint != begin && mouse_pos.x - prev(draggingPoint)->x < 0.1f)
            {
                draggingPoint_all->first.x = draggingPoint->x = prev(draggingPoint)->x + 1;
                draggingPoint_all->first.y = draggingPoint->y = y;
            }
            else if(next(draggingPoint) != end && next(draggingPoint)->x - mouse_pos.x < 0.1f)
            {
                draggingPoint_all->first.x = draggingPoint->x = next(draggingPoint)->x - 1;
                draggingPoint_all->first.y = draggingPoint->y = y;
            }
            else
            {
                draggingPoint_all->first.x = draggingPoint->x = x;
                draggingPoint_all->first.y = draggingPoint->y = y;
            }
        }

        if(ImGui::IsMouseReleased(ImGuiMouseButton_Left)) isDragging = false;

        drawLine(draw_api, points_red, glm::vec2(canvas_topleft.x, canvas_topleft.y), RED);
        drawLine(draw_api, points_green, glm::vec2(canvas_topleft.x, canvas_topleft.y), GREEN);
        drawLine(draw_api, points_blue, glm::vec2(canvas_topleft.x, canvas_topleft.y), BLUE);
        drawLine(draw_api, points_alpha, glm::vec2(canvas_topleft.x, canvas_topleft.y), ALPHA);

        drawPoints(draw_api, points_red, glm::vec2(canvas_topleft.x, canvas_topleft.y), RED, mouse_pos);
        drawPoints(draw_api, points_green, glm::vec2(canvas_topleft.x, canvas_topleft.y), GREEN, mouse_pos);
        drawPoints(draw_api, points_blue, glm::vec2(canvas_topleft.x, canvas_topleft.y), BLUE, mouse_pos);
        drawPoints(draw_api, points_alpha, glm::vec2(canvas_topleft.x, canvas_topleft.y), ALPHA, mouse_pos);

        auto point_iter = getIntersectedPoint(points, mouse_pos);

        if(point_iter != points.end())
            draw_api->AddCircle(ImVec2(canvas_topleft.x + point_iter->first.x, canvas_topleft.y + point_iter->first.y), 5.0f, IM_COL32(255, 255, 255, 255), 12, 2.0f);

        if(isUpdating && volumeList.size() > 0 && selectedMethod == "Ray Casting")
        {
            dynamic_cast<RayCasting*>(volumeList.back())->make1DTexture(glm::vec2(canvas_size.x, canvas_size.y), points_red, points_green, points_blue, points_alpha);
        }

    }ImGui::End();
}

void WindowManager::setLt(float& theElement, float thatElement)
{
    if(theElement < thatElement) theElement = thatElement;
}

void WindowManager::setGt(float& theElement, float thatElement)
{
    if(theElement > thatElement) theElement = thatElement;
}

vector<pair<glm::vec2, int>>::iterator WindowManager::getIntersectedPoint(vector<pair<glm::vec2, int>>& points, glm::vec2 mouse_pos)
{
    auto iter = points.end();

    while(iter != points.begin())
    {
        iter--;

        if(glm::distance(mouse_pos, iter->first) < 5.0f)
            return iter;
    }

    return points.end();
}

vector<glm::vec2>::iterator WindowManager::getIntersectedPoint(vector<glm::vec2>& points, glm::vec2 mouse_pos)
{
    auto iter = points.end();

    while(iter != points.begin())
    {
        iter--;

        if(glm::distance(mouse_pos, *iter) < 5.0f)
            return iter;
    }

    return points.end();
}

void WindowManager::drawPoints(ImDrawList* draw_api, vector<glm::vec2> points, glm::vec2 origin, ImU32 color, glm::vec2 mouse_pos)
{
    for(auto point: points)
    {
        draw_api->AddCircleFilled(ImVec2(origin.x + point.x, origin.y + point.y), 5.0f, color, 12);
        draw_api->AddCircle(ImVec2(origin.x + point.x, origin.y + point.y), 5.0f, IM_COL32(255, 255, 255, 255), 12, 0.2f);
    }
}

void WindowManager::drawLine(ImDrawList* draw_api, vector<glm::vec2> points, glm::vec2 origin, ImU32 color)
{
    for(int i = 1; i < (int)points.size(); i++)
    {
        draw_api->AddLine(ImVec2(origin.x + points[i-1].x, origin.y + points[i-1].y), 
                          ImVec2(origin.x + points[i].x, origin.y + points[i].y), color, 2.5f);
    }
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

    fr = new FileReader("./Data/");
    fr->initNameList();
    fr->readFile("1");

    histogram = NULL;
    heatmap = NULL;

    initImportList();
}

void WindowManager::initImportList()
{
    DIR * dirp;
    struct dirent * entry;

    dirp = opendir("./transferFunc/");

    if (dirp == NULL) 
    {
        cout << "Error(" << errno << ") opening " << "./transferFunc/" << endl;
        perror("opendir");

        return;
    }

    importList.clear();
    string fullname = "";

    while ((entry = readdir(dirp)) != NULL) {
        if(strstr(entry->d_name, ".trans"))
        {
            fullname = entry->d_name;
            importList.push_back(fullname.substr(0, fullname.length() - 6));
        }
    }

    closedir(dirp);
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
}

void WindowManager::setClippingValue(bool increase)
{
    if( increase ) clipping[3]++;
    else clipping[3]--;
}

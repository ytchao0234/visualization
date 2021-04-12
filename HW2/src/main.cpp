#include <WindowManager.hpp>

int main()
{
    string filename;
    float isovalue;

    cout << "data file name(exclude file extension): ";
    cin >> filename;
    cout << "isovalue: ";
    cin >> isovalue;

    WindowManager wm("Isosurface", 800, 600);
    wm.initCallbacks();

    wm.initObjects(filename, isovalue);

    (wm.getIsosurface())[0]->marchingCube();

    while(!glfwWindowShouldClose(wm.getWindow()))
    {
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        for(auto iso: wm.getIsosurface())
        {
            wm.getLight()->lighting(iso->getShader(), wm.getCamera()->getPosition());
            iso->draw(wm.getCamera()->getProjection(wm.getWidth(), wm.getHeight()), wm.getCamera()->getView(), wm.getClipping(), wm.getMakeCrossSection());
        }

        wm.checkKeyPress();
        glfwPollEvents();
        glfwSwapBuffers(wm.getWindow());
    }

    glfwTerminate();
    return 0;
}
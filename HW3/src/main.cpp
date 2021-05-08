#include <WindowManager.hpp>

int main()
{
    WindowManager wm("Isosurface", 1200, 900, "#version 460");
    wm.initGUI();
    wm.initObjects();
    wm.initCallbacks();

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

        wm.renderGUI(wm.getFileReader()->getInfNameList(), wm.getFileReader()->getRawNameList());

        wm.checkKeyPress();
        glfwPollEvents();
        glfwSwapBuffers(wm.getWindow());
    }

    glfwTerminate();
    return 0;
}
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
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        for(auto volume: wm.getVolumeList())
        {
            wm.getLight()->lighting(volume->getShader(), wm.getCamera()->getPosition());
            volume->draw(wm.getCamera()->getProjection(wm.getWidth(), wm.getHeight()), wm.getCamera()->getView(), wm.getClipping(), wm.getMakeCrossSection());
        }

        wm.renderGUI(wm.getFileReader()->getInfNameList(), wm.getFileReader()->getRawNameList());

        wm.checkKeyPress();
        glfwPollEvents();
        glfwSwapBuffers(wm.getWindow());
    }

    glfwTerminate();
    return 0;
}
#include <WindowManager.hpp>

int main()
{
    WindowManager wm("Isosurface", 800, 600);
    wm.initCallbacks();
    wm.initObjects();

    wm.getIsosurface()->marchingCube();

    while(!glfwWindowShouldClose(wm.getWindow()))
    {
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // wm.getCube()->draw(wm.getCamera()->getProjection(), wm.getCamera()->getView());
        wm.getIsosurface()->draw(wm.getCamera()->getProjection(), wm.getCamera()->getView());

        glfwPollEvents();
        glfwSwapBuffers(wm.getWindow());
    }

    glfwTerminate();
    return 0;
}
#include <CubeModel.hpp>

CubeModel::CubeModel()
{
    vertices = {
        // position
        0.5f, -0.5f, -0.5f, // 0,1,2
        -0.5f, -0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f, // 2,3,0
        0.5f,  0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,

        0.5f, -0.5f,  0.5f, // 4,5,6
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f, // 6,7,4
        0.5f,  0.5f,  0.5f,
        0.5f, -0.5f,  0.5f,

        0.5f, -0.5f, -0.5f, // 0,1,5
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f, // 5,4,0
        0.5f, -0.5f,  0.5f,
        0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f, -0.5f, // 1,5,6
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f, // 6,2,1
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f,  0.5f, -0.5f, // 2,6,7
        -0.5f,  0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,
        0.5f,  0.5f,  0.5f, // 7,3,2
        0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,

        0.5f, -0.5f,  0.5f, // 4,0,3
        0.5f, -0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,
        0.5f,  0.5f, -0.5f, // 3,7,4
        0.5f,  0.5f,  0.5f,
        0.5f, -0.5f,  0.5f,
    };

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void CubeModel::draw(glm::mat4 projection, glm::mat4 view)
{
    Shader shader("src/Shaders/vertex.vert", "src/Shaders/fragment.frag");
    shader.use();

    shader.setMatrix4("projection", glm::value_ptr(projection));
    shader.setMatrix4("view", glm::value_ptr(view));
    
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(40.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    shader.setMatrix4("model", glm::value_ptr(model));

    glBindVertexArray(this->VAO);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}
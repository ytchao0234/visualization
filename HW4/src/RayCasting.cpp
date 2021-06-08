#include <RayCasting.hpp>

RayCasting::RayCasting(const VolumeData* data)
{
    this->data = new VolumeData(data);
    
    this->shader = new Shader("src/Shaders/rayCasting.vert", "src/Shaders/rayCasting.frag");
    this->VAO = new unsigned int[1];
    this->VBO = new unsigned int[1];
    glGenVertexArrays(1, VAO);
    glGenBuffers(1, VBO);
    this->vertices.clear();
    
    this->texture = new Texture(2);
    this->texture3D = new unsigned char[data->resolution.x * data->resolution.y * data->resolution.z][4];
    this->texture1D = new unsigned char[256][4];
}

RayCasting::~RayCasting()
{
    delete this->data;
    delete this->shader;
    delete this->VAO;
    delete this->VBO;
    delete this->texture;
    delete[] this->texture3D;
    delete[] this->texture1D;
}

void RayCasting::makeVertices()
{
    float x = data->resolution.x;
    float y = data->resolution.y;
    float z = data->resolution.z;

    // 0: 0.0f, 0.0f, 0.0f,     0.0f, 0.0f, 0.0f,
    // 1:    x, 0.0f, 0.0f,     1.0f, 0.0f, 0.0f,
    // 2:    x,    y, 0.0f,     1.0f, 1.0f, 0.0f,
    // 3: 0.0f,    y, 0.0f,     0.0f, 1.0f, 0.0f,

    // 4: 0.0f, 0.0f,    z,     0.0f, 0.0f, 1.0f,
    // 5:    x, 0.0f,    z,     1.0f, 0.0f, 1.0f,
    // 6:    x,    y,    z,     1.0f, 1.0f, 1.0f,
    // 7: 0.0f,    y,    z,     0.0f, 1.0f, 1.0f,

    // 012, 230, 476, 654, 037, 740, 045, 510, 156, 621, 267, 732

    vertices = 
    {
        0.0f - x/2,     0.0f - y/2,     0.0f - z/2,     0.0f, 0.0f, 0.0f,
           x - x/2,     0.0f - y/2,     0.0f - z/2,     1.0f, 0.0f, 0.0f,
           x - x/2,        y - y/2,     0.0f - z/2,     1.0f, 1.0f, 0.0f,
           x - x/2,        y - y/2,     0.0f - z/2,     1.0f, 1.0f, 0.0f,
        0.0f - x/2,        y - y/2,     0.0f - z/2,     0.0f, 1.0f, 0.0f,
        0.0f - x/2,     0.0f - y/2,     0.0f - z/2,     0.0f, 0.0f, 0.0f,

        0.0f - x/2,     0.0f - y/2,        z - z/2,     0.0f, 0.0f, 1.0f,
        0.0f - x/2,        y - y/2,        z - z/2,     0.0f, 1.0f, 1.0f,
           x - x/2,        y - y/2,        z - z/2,     1.0f, 1.0f, 1.0f,
           x - x/2,        y - y/2,        z - z/2,     1.0f, 1.0f, 1.0f,
           x - x/2,     0.0f - y/2,        z - z/2,     1.0f, 0.0f, 1.0f,
        0.0f - x/2,     0.0f - y/2,        z - z/2,     0.0f, 0.0f, 1.0f,

        0.0f - x/2,     0.0f - y/2,     0.0f - z/2,     0.0f, 0.0f, 0.0f,
        0.0f - x/2,        y - y/2,     0.0f - z/2,     0.0f, 1.0f, 0.0f,
        0.0f - x/2,        y - y/2,        z - z/2,     0.0f, 1.0f, 1.0f,
        0.0f - x/2,        y - y/2,        z - z/2,     0.0f, 1.0f, 1.0f,
        0.0f - x/2,     0.0f - y/2,        z - z/2,     0.0f, 0.0f, 1.0f,
        0.0f - x/2,     0.0f - y/2,     0.0f - z/2,     0.0f, 0.0f, 0.0f,
        
        0.0f - x/2,     0.0f - y/2,     0.0f - z/2,     0.0f, 0.0f, 0.0f,
        0.0f - x/2,     0.0f - y/2,        z - z/2,     0.0f, 0.0f, 1.0f,
           x - x/2,     0.0f - y/2,        z - z/2,     1.0f, 0.0f, 1.0f,
           x - x/2,     0.0f - y/2,        z - z/2,     1.0f, 0.0f, 1.0f,
           x - x/2,     0.0f - y/2,     0.0f - z/2,     1.0f, 0.0f, 0.0f,
        0.0f - x/2,     0.0f - y/2,     0.0f - z/2,     0.0f, 0.0f, 0.0f,

           x - x/2,     0.0f - y/2,     0.0f - z/2,     1.0f, 0.0f, 0.0f,
           x - x/2,     0.0f - y/2,        z - z/2,     1.0f, 0.0f, 1.0f,
           x - x/2,        y - y/2,        z - z/2,     1.0f, 1.0f, 1.0f,
           x - x/2,        y - y/2,        z - z/2,     1.0f, 1.0f, 1.0f,
           x - x/2,        y - y/2,     0.0f - z/2,     1.0f, 1.0f, 0.0f,
           x - x/2,     0.0f - y/2,     0.0f - z/2,     1.0f, 0.0f, 0.0f,

           x - x/2,        y - y/2,     0.0f - z/2,     1.0f, 1.0f, 0.0f,
           x - x/2,        y - y/2,        z - z/2,     1.0f, 1.0f, 1.0f,
        0.0f - x/2,        y - y/2,        z - z/2,     0.0f, 1.0f, 1.0f,
        0.0f - x/2,        y - y/2,        z - z/2,     0.0f, 1.0f, 1.0f,
        0.0f - x/2,        y - y/2,     0.0f - z/2,     0.0f, 1.0f, 0.0f,
           x - x/2,        y - y/2,     0.0f - z/2,     1.0f, 1.0f, 0.0f,
    };

    bindVertices();
    make3DTexture();
    make1DTexture();
}

void RayCasting::bindVertices()
{
    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)( 3 * sizeof(float) ));
    glEnableVertexAttribArray(1);
}

void RayCasting::draw(glm::mat4 projection, glm::mat4 view, const vector<float> clipping, bool makeCrossSection, float gap, float adjust, float threshold)
{
    shader->use();

    shader->setMatrix4("projection", glm::value_ptr(projection));
    shader->setMatrix4("view", glm::value_ptr(view));

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(data->voxelSize.x, data->voxelSize.y, data->voxelSize.z));
    shader->setMatrix4("model", glm::value_ptr(model));

    shader->setFloatVec("resolution", {(float)data->resolution.x, (float)data->resolution.y, (float)data->resolution.z}, 3);
    shader->setFloatVec("voxelSize", {data->voxelSize.x, data->voxelSize.y, data->voxelSize.z}, 3);

    shader->setFloat("gap", gap);
    shader->setFloat("adjust", adjust);
    shader->setFloat("threshold", threshold);

    shader->setInt("tex3D", 0);
    shader->setInt("tex1D", 1);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);
    
    texture->active3D(0);
    texture->active1D(1);

    glBindVertexArray(VAO[0]);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 6);
}

void RayCasting::setShader()
{
    delete this->shader;
    this->shader = new Shader("src/Shaders/rayCasting.vert", "src/Shaders/rayCasting.frag");
}

Shader* RayCasting::getShader() const
{
    return this->shader;
}

void RayCasting::make3DTexture()
{
    for(int x = 0; x < data->resolution.x; x++)
    for(int y = 0; y < data->resolution.y; y++)
    for(int z = 0; z < data->resolution.z; z++)
    {
        texture3D[z*data->resolution.y*data->resolution.x + y*data->resolution.x + x][0] = (data->gradient[x][y][z][0] + 1) / 2 * 255;
        texture3D[z*data->resolution.y*data->resolution.x + y*data->resolution.x + x][1] = (data->gradient[x][y][z][1] + 1) / 2 * 255;
        texture3D[z*data->resolution.y*data->resolution.x + y*data->resolution.x + x][2] = (data->gradient[x][y][z][2] + 1) / 2 * 255;
        texture3D[z*data->resolution.y*data->resolution.x + y*data->resolution.x + x][3] = (data->data[x][y][z] - data->dataMin) / (data->dataMax - data->dataMin) * 255;
    }
    
    texture->make3DTexture(0, texture3D, data->resolution.x, data->resolution.y, data->resolution.z);
}

void RayCasting::make1DTexture()
{
    for(int i = 0; i < 256; i++)
    {
        texture1D[i][0] = i;
        texture1D[i][1] = 0;
        texture1D[i][2] = 0;
        texture1D[i][3] = 0.1f * 255;
    }

    texture->make1DTexture(1, texture1D, 256);
}

void RayCasting::make1DTexture(glm::vec2 canvas_size, vector<glm::vec2> red, vector<glm::vec2> green, vector<glm::vec2> blue, vector<glm::vec2> alpha)
{
    glm::vec2 ratio = glm::vec2(1.0f / canvas_size.x * 255.0f, 1.0f / canvas_size.y * 255.0f);

    for(int i = 0; i < 256; i++)
    {
        texture1D[i][0] = 0;
        texture1D[i][1] = 0;
        texture1D[i][2] = 0;
        texture1D[i][3] = 0;
    }

    setColorValue(ratio, red  , 0);
    setColorValue(ratio, green, 1);
    setColorValue(ratio, blue , 2);
    setColorValue(ratio, alpha, 3);
    
    texture->make1DTexture(1, texture1D, 256);
}

void RayCasting::setColorValue(glm::vec2 ratio, vector<glm::vec2> points, int color)
{
    if(points.size() == 0) return;

    vector<glm::vec2>::iterator iter = points.begin(), next_iter = next(iter);
    vector<glm::vec2>::reverse_iterator end_iter = points.rbegin();

    int start_x = iter->x, end_x = end_iter->x;
    float alpha_x, base_y, y;
    int i;

    texture1D[start_x][color] = 255 - iter->y * ratio.y;

    for(i = start_x + 1; i <= end_x; i++)
    {
        if(i == (int)(next_iter->x))
        {
            iter++;
            next_iter++;
            texture1D[i][color] = 255 - iter->y * ratio.y;
        }
        else
        {
            base_y = iter->y;
            y = (next_iter->y - iter->y);
            alpha_x = (i - iter->x) / (next_iter->x - iter->x);

            texture1D[i][color] = 255 - (base_y + y * alpha_x) * ratio.y;
        }
    }
}
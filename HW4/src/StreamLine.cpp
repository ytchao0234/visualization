#include <StreamLine.hpp>

StreamLine::StreamLine(const VectorData* data, double h, int gridSize, double distanceLimit)
{
    this->data2 = new VectorData(data);
    this->h = h;
    this->gridSize = gridSize;
    this->distanceLimit = 1.0 / distanceLimit;
    this->collisionTable.assign(data->size.first * this->distanceLimit, vector<bool>(data->size.second * this->distanceLimit, false));

    this->shader = new Shader("src/Shaders/streamLine.vert", "src/Shaders/streamLine.frag");
    this->VAO = new unsigned int[1];
    this->VBO = new unsigned int[1];
    glGenVertexArrays(1, VAO);
    glGenBuffers(1, VBO);
    this->vertices.clear();
    
    this->texture = new Texture(2);
    this->texture3D = NULL;
    this->texture2D = new unsigned char[data2->size.first * data2->size.second][4];
    this->texture1D = new unsigned char[256][4];
}

StreamLine::~StreamLine()
{
    delete this->data2;
    delete this->shader;
    delete this->VAO;
    delete this->VBO;
    delete this->texture;
    delete[] this->texture2D;
    delete[] this->texture1D;
}

void StreamLine::makeVertices()
{
    this->vertices.clear();

    for(int x = 0; x < this->data2->size.first; x += gridSize)
    for(int y = 0; y < this->data2->size.second; y += gridSize)
    {
        // cout << "---------------------------------------------------Line " << x << y << endl;
        makeSingleLine(x, y);
    }

    bindVertices();
}

void StreamLine::bindVertices()
{
    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);

    glBufferData(GL_ARRAY_BUFFER, sizeof(double) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 3 * sizeof(double), (void*)0);
    glEnableVertexAttribArray(0);
    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)( 3 * sizeof(float) ));
    // glEnableVertexAttribArray(1);
}

void StreamLine::draw(glm::mat4 projection, glm::mat4 view, const vector<float> clipping, bool makeCrossSection, float gap, float adjust, float threshold)
{
    shader->use();

    shader->setMatrix4("projection", glm::value_ptr(projection));
    shader->setMatrix4("view", glm::value_ptr(view));

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(data2->size.first / 2.0, -data2->size.second / 2.0, 0.0));
    shader->setMatrix4("model", glm::value_ptr(model));

    // shader->setFloatVec("resolution", {(float)data1->resolution.x, (float)data1->resolution.y, (float)data1->resolution.z}, 3);
    // shader->setFloatVec("voxelSize", {data1->voxelSize.x, data1->voxelSize.y, data1->voxelSize.z}, 3);

    // shader->setFloat("gap", gap);
    // shader->setFloat("adjust", adjust);
    // shader->setFloat("threshold", threshold);

    // shader->setInt("tex3D", 0);
    // shader->setInt("tex1D", 1);

    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK);
    // glFrontFace(GL_CW);
    
    // texture->active3D(0);
    // texture->active1D(1);

    glBindVertexArray(VAO[0]);
    glLineWidth(0.1f);
    glDrawArrays(GL_LINES, 0, vertices.size() / 3);
}

void StreamLine::setShader()
{
    delete this->shader;
    this->shader = new Shader("src/Shaders/streamLine.vert", "src/Shaders/streamLine.frag");
}

Shader* StreamLine::getShader() const
{
    return this->shader;
}

void StreamLine::makeSingleLine(double x, double y)
{
    glm::dvec2 point = glm::dvec2(x, y);
    glm::dvec2 lb, rb, lt, rt;
    glm::dvec2 K1, P, K2;
    double dx, dy;

    set<pair<int, int>> cross;

    int limit = 100, count = 0;
    
    while(!hasCollision(point.x, point.y) &&
          point.x >= 0.0 && point.y >= 0.0 &&
          point.x < this->data2->size.first - this->gridSize && point.y < this->data2->size.second - this->gridSize)
    {
        // printf("%12lf, %12lf\n", point.x, point.y);
        cross.insert({(int)(point.x * this->distanceLimit), (int)(point.y * this->distanceLimit)});

        if(count++ == limit) break;

        this->vertices.push_back(-point.x);
        this->vertices.push_back(point.y);
        this->vertices.push_back(0.0);

        lb = glm::dvec2(data2->value[(int)point.x    ][(int)point.y    ].first,
                        data2->value[(int)point.x    ][(int)point.y    ].second);
        rb = glm::dvec2(data2->value[(int)point.x + 1][(int)point.y    ].first,
                        data2->value[(int)point.x + 1][(int)point.y    ].second);
        lt = glm::dvec2(data2->value[(int)point.x    ][(int)point.y + 1].first,
                        data2->value[(int)point.x    ][(int)point.y + 1].second);
        rt = glm::dvec2(data2->value[(int)point.x + 1][(int)point.y + 1].first,
                        data2->value[(int)point.x + 1][(int)point.y + 1].second);

        dx = point.x - (int)point.x;
        dy = point.y - (int)point.y;

        K1 = lb * (1-dx)*(1-dy) + rb * dx*(1-dy) +
             lt * (1-dx)*dy     + rt * dx*dy;

        P = point + this->h * glm::normalize(K1);

        if(P.x < 0.0 || P.y < 0.0 ||
           P.x >= this->data2->size.first - this->gridSize || P.y >= this->data2->size.second - this->gridSize)
        {
            vertices.pop_back();
            vertices.pop_back();
            vertices.pop_back();
            break;
        }

        lb = glm::dvec2(data2->value[(int)P.x    ][(int)P.y    ].first,
                        data2->value[(int)P.x    ][(int)P.y    ].second);
        rb = glm::dvec2(data2->value[(int)P.x + 1][(int)P.y    ].first,
                        data2->value[(int)P.x + 1][(int)P.y    ].second);
        lt = glm::dvec2(data2->value[(int)P.x    ][(int)P.y + 1].first,
                        data2->value[(int)P.x    ][(int)P.y + 1].second);
        rt = glm::dvec2(data2->value[(int)P.x + 1][(int)P.y + 1].first,
                        data2->value[(int)P.x + 1][(int)P.y + 1].second);

        dx = P.x - (int)P.x;
        dy = P.y - (int)P.y;

        K2 = lb * (1-dx)*(1-dy) + rb * dx*(1-dy) +
             lt * (1-dx)*dy     + rt * dx*dy;

        point = point + this->h * 0.5 * (glm::normalize(K1) + glm::normalize(K2));

        this->vertices.push_back(-point.x);
        this->vertices.push_back(point.y);
        this->vertices.push_back(0.0);
    }

    if(cross.size() > 0)
        cross.insert({(int)(point.x * this->distanceLimit), (int)(point.y * this->distanceLimit)});

    for(auto cell: cross)
    {
        this->collisionTable[cell.first][cell.second] = true;
    }
}

bool StreamLine::hasCollision(double x, double y)
{
    return this->collisionTable[(int)(x * this->distanceLimit)][(int)(y * this->distanceLimit)];
}

void make1DTexture()
{
    return;
}

void make1DTexture(glm::vec2 canvas_size, vector<glm::vec2> red, vector<glm::vec2> green, vector<glm::vec2> blue, vector<glm::vec2> alpha)
{
    return;
}

void setColorValue(glm::vec2 ratio, vector<glm::vec2> points, int color)
{
    return;
}

double StreamLine::default_U(double, double)
{
    return 0.0;
}

void StreamLine::generateTestData(int, int, double (*)(double, double))
{
    return;
}
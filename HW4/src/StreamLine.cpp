#include <StreamLine.hpp>

StreamLine::StreamLine(const VectorData* data, double h, int iteration, double gridSize, double distanceLimit, bool useDefault_U)
{
    this->data2 = new VectorData(data);
    this->h = h;
    this->gridSize = gridSize;
    this->iteration = iteration;
    this->distanceLimit = 1 / distanceLimit;
    this->valueMax = 0.0;
    this->useDefault_U = useDefault_U;

    if(!useDefault_U)
        this->collisionTable.assign(data->size.first * this->distanceLimit + 5,
                                    vector<bool>(data->size.second * this->distanceLimit + 5, false));
    else
        this->collisionTable.assign(50 * this->distanceLimit + 5, vector<bool>(50 * this->distanceLimit + 5, false));

    this->shader = new Shader("src/Shaders/streamLine.vert", "src/Shaders/streamLine.frag", "src/Shaders/streamLine.gs");
    this->VAO = new unsigned int[1];
    this->VBO = new unsigned int[1];
    glGenVertexArrays(1, VAO);
    glGenBuffers(1, VBO);
    this->vertices.clear();
    
    this->texture = new Texture(1);
    this->texture3D = NULL;
    this->texture2D = NULL;
    this->texture1D = new unsigned char[256][4];
}

StreamLine::~StreamLine()
{
    delete this->data2;
    delete this->shader;
    delete this->VAO;
    delete this->VBO;
    delete this->texture;
    delete[] this->texture1D;
}

void StreamLine::makeVertices()
{
    this->vertices.clear();

    if(!this->useDefault_U)
    {
        for(double x = 0; x < this->data2->size.first; x += this->gridSize)
        for(double y = 0; y < this->data2->size.second; y += this->gridSize)
        {
            makeSingleLine(x, y);
        }
    }
    else
    {
        for(double x = -5; x < 5; x += 0.2 * this->gridSize)
        for(double y = -5; y < 5; y += 0.2 * this->gridSize)
        {
            makeSingleLine_U(x, y);
        }
    }
    
    bindVertices();
    make1DTexture();
}

void StreamLine::bindVertices()
{
    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);

    glBufferData(GL_ARRAY_BUFFER, sizeof(double) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 5 * sizeof(double), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 1, GL_DOUBLE, GL_FALSE, 5 * sizeof(double), (void*)( 3 * sizeof(double) ));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 1, GL_DOUBLE, GL_FALSE, 5 * sizeof(double), (void*)( 4 * sizeof(double) ));
    glEnableVertexAttribArray(2);
}

void StreamLine::draw(glm::mat4 projection, glm::mat4 view, const vector<float> clipping, bool makeCrossSection, float gap, float adjust, float threshold, float alpha)
{
    shader->use();

    shader->setMatrix4("projection", glm::value_ptr(projection));
    shader->setMatrix4("view", glm::value_ptr(view));

    glm::mat4 model = glm::mat4(1.0f);

    if(!useDefault_U)
        model = glm::translate(model, glm::vec3(data2->size.first / 2.0, -data2->size.second / 2.0, 0.0));

    shader->setMatrix4("model", glm::value_ptr(model));

    shader->setFloat("valueMax", valueMax);
    shader->setFloat("alpha", alpha);
    shader->setInt("tex1D", 0);

    texture->active1D(0);

    glBindVertexArray(VAO[0]);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawArrays(GL_LINES, 0, vertices.size() / 5);
}

void StreamLine::setShader()
{
    delete this->shader;
    this->shader = new Shader("src/Shaders/streamLine.vert", "src/Shaders/streamLine.frag", "src/Shaders/streamLine.gs");
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
    double dx, dy, lenK1;

    set<pair<int, int>> cross;
    cross.insert({point.x * this->distanceLimit + 0.5, point.y * this->distanceLimit + 0.5});

    int count = 0, length = 0;
    
    while(point.x >= 0.0 && point.y >= 0.0 &&
          point.x < this->data2->size.first - this->gridSize && point.y < this->data2->size.second - this->gridSize)
    {
        if(this->collisionTable[point.x * this->distanceLimit + 0.5][point.y * this->distanceLimit + 0.5])
            break;
        if(count++ == this->iteration)
            break;

        lb = glm::dvec2(data2->value[point.x    ][point.y    ].first,
                        data2->value[point.x    ][point.y    ].second);
        rb = glm::dvec2(data2->value[point.x + 1][point.y    ].first,
                        data2->value[point.x + 1][point.y    ].second);
        lt = glm::dvec2(data2->value[point.x    ][point.y + 1].first,
                        data2->value[point.x    ][point.y + 1].second);
        rt = glm::dvec2(data2->value[point.x + 1][point.y + 1].first,
                        data2->value[point.x + 1][point.y + 1].second);

        dx = point.x - (int)point.x;
        dy = point.y - (int)point.y;

        K1 = lb * (1-dx)*(1-dy) + rb * dx*(1-dy) + lt * (1-dx)*dy + rt * dx*dy;  
        lenK1 = glm::length(K1);

        this->vertices.push_back(-point.x);
        this->vertices.push_back(point.y);
        this->vertices.push_back(0.0);
        this->vertices.push_back(count);
        this->vertices.push_back(lenK1);

        P = point + this->h * glm::normalize(K1);

        if(P.x < 0.0 || P.y < 0.0 ||
           P.x >= this->data2->size.first - this->gridSize || P.y >= this->data2->size.second - this->gridSize)
        {
            this->vertices.pop_back();
            this->vertices.pop_back();
            this->vertices.pop_back();
            this->vertices.pop_back();
            this->vertices.pop_back();
            break;
        }

        lb = glm::dvec2(data2->value[P.x    ][P.y    ].first,
                        data2->value[P.x    ][P.y    ].second);
        rb = glm::dvec2(data2->value[P.x + 1][P.y    ].first,
                        data2->value[P.x + 1][P.y    ].second);
        lt = glm::dvec2(data2->value[P.x    ][P.y + 1].first,
                        data2->value[P.x    ][P.y + 1].second);
        rt = glm::dvec2(data2->value[P.x + 1][P.y + 1].first,
                        data2->value[P.x + 1][P.y + 1].second);

        dx = P.x - (int)P.x;
        dy = P.y - (int)P.y;

        K2 = lb * (1-dx)*(1-dy) + rb * dx*(1-dy) + lt * (1-dx)*dy + rt * dx*dy;

        point = point + this->h * 0.5 * (glm::normalize(K1) + glm::normalize(K2));

        this->vertices.push_back(-point.x);
        this->vertices.push_back(point.y);
        this->vertices.push_back(0.0);
        this->vertices.push_back(count);
        this->vertices.push_back(lenK1);
        
        this->valueMax = max(this->valueMax, lenK1);

        cross.insert({point.x * this->distanceLimit + 0.5, point.y * this->distanceLimit + 0.5});

        length++;
    }

    for(auto cell: cross)
    {
        this->collisionTable[cell.first][cell.second] = true;
    }

    for(int i = 1; i <= length * 2; i++)
    {
        vertices[vertices.size() - i * 5 + 3]  = vertices[vertices.size() - i * 5 + 3] / length + 0.5;
    }
}

void StreamLine::makeSingleLine_U(double x, double y)
{
    glm::dvec2 point = glm::dvec2(x, y);
    glm::dvec2 K1, P, K2;
    double lenK1;
    pair<double, double> U_vec;

    set<pair<int, int>> cross;
    cross.insert({(int)((point.x + 5) * 5 * this->distanceLimit + 0.5), (int)((point.y + 5) * 5 * this->distanceLimit + 0.5)});

    int count = 0, length = 0;
    
    while(point.x >= -5 && point.y >= -5 &&
          point.x < 5 - 0.2 * this->gridSize && point.y < 5 - 0.2 * this->gridSize)
    {
        if(this->collisionTable[(point.x + 5) * 5 * this->distanceLimit + 0.5][(point.y + 5) * 5 * this->distanceLimit + 0.5])
            break;
        if(count++ == this->iteration)
            break;

        U_vec = default_U(point.x, point.y);

        K1 = glm::dvec2(U_vec.first, U_vec.second);  
        lenK1 = glm::length(K1);

        this->vertices.push_back(point.x);
        this->vertices.push_back(point.y);
        this->vertices.push_back(0.0);
        this->vertices.push_back(count);
        this->vertices.push_back(lenK1);

        P = point + this->h * glm::normalize(K1);

        if(P.x < -5 || P.y < -5 ||
           P.x >= 5 - 0.2 * this->gridSize || P.y >= 5 - 0.2 * this->gridSize)
        {
            this->vertices.pop_back();
            this->vertices.pop_back();
            this->vertices.pop_back();
            this->vertices.pop_back();
            this->vertices.pop_back();
            break;
        }

        U_vec = default_U(P.x, P.y);

        K2 = glm::dvec2(U_vec.first, U_vec.second);  

        point = point + this->h * 0.5 * (glm::normalize(K1) + glm::normalize(K2));

        this->vertices.push_back(point.x);
        this->vertices.push_back(point.y);
        this->vertices.push_back(0.0);
        this->vertices.push_back(count);
        this->vertices.push_back(lenK1);
        
        this->valueMax = max(this->valueMax, lenK1);

        cross.insert({(int)((point.x + 5) * 5 * this->distanceLimit + 0.5), (int)((point.y + 5) * 5 * this->distanceLimit + 0.5)});

        length++;
    }

    for(auto cell: cross)
    {
        this->collisionTable[cell.first][cell.second] = true;
    }

    for(int i = 1; i <= length * 2; i++)
    {
        vertices[vertices.size() - i * 5 + 3]  = vertices[vertices.size() - i * 5 + 3] / length;
    }
}

void StreamLine::make1DTexture()
{
    for(int i = 0; i < 256; i++)
    {
        if(i < 128)
        {
            texture1D[i][0] = i * 2;
            texture1D[i][1] = 50;
            texture1D[i][2] = 255 - i * 1.5;
            texture1D[i][3] = 255;
        }
        else
        {
            texture1D[i][0] = 255;
            texture1D[i][1] = ((i - 127) * 4 + 50 > 255) ? 255 : (i - 127) * 4 + 50;
            texture1D[i][2] = 63;
            texture1D[i][3] = 255;
        }
    }

    texture->make1DTexture(0, texture1D, 256);
}

pair<double, double> StreamLine::default_U(double x, double y)
{
    return {x*y*y, x*x*y};
}

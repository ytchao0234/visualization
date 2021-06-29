#include <MultiDimention.hpp>

MultiDimention::MultiDimention(const MultiDimData* data, double lambda, double alpha, string method)
{
    this->data3 = new MultiDimData(data);
    this->x_offset = 0.0;
    this->y_offset = 0.0;
    this->lambda = lambda;
    this->alpha = alpha;
    this->method = method;
    this->cluster.assign(this->data3->size.first, 0);

    for(int i = 0; i < (int)this->cluster.size(); i++)
        this->cluster[i] = i;

    this->shader = new Shader("src/Shaders/multiDimention.vert", "src/Shaders/multiDimention.frag");
    this->VAO = new unsigned int[2];
    this->VBO = new unsigned int[2];
    glGenVertexArrays(2, VAO);
    glGenBuffers(2, VBO);
    this->vertices.clear();
    
    this->texture = new Texture(1);
    this->texture3D = NULL;
    this->texture2D = NULL;
    this->texture1D = new unsigned char[256][4];
}

MultiDimention::~MultiDimention()
{
    delete data3;
    delete texture;
    delete texture1D;
}

void MultiDimention::makeVertices()
{
    this->vertices.clear();

    normalize();
    preprocessing();
    makeParallel();

    if(this->method == "SammonMapping")
        SammonMapping2D();
    else if(this->method == "PCA")
        PCA();

    make1DTexture();
    bindVertices();
}

void MultiDimention::bindVertices()
{
    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);

    glBufferData(GL_ARRAY_BUFFER, sizeof(double) * parallel.size(), parallel.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 4 * sizeof(double), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 1, GL_DOUBLE, GL_FALSE, 4 * sizeof(double), (void*)( 3 * sizeof(double) ));
    glEnableVertexAttribArray(1);

    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);

    glBufferData(GL_ARRAY_BUFFER, sizeof(double) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 4 * sizeof(double), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 1, GL_DOUBLE, GL_FALSE, 4 * sizeof(double), (void*)( 3 * sizeof(double) ));
    glEnableVertexAttribArray(1);
}

void MultiDimention::draw(glm::mat4 projection, glm::mat4 view, const vector<float> clipping, bool makeCrossSection, float gap, float adjust, float threshold, float alpha, bool drawParallel)
{
    if(drawParallel)
    {
        shader->use();

        shader->setMatrix4("projection", glm::value_ptr(projection));
        shader->setMatrix4("view", glm::value_ptr(view));

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(10.0, 40.0, 0.0));
        shader->setMatrix4("model", glm::value_ptr(model));

        shader->setInt("tex1D", 0);

        texture->active1D(0);

        glDisable(GL_BLEND);
        glBindVertexArray(VAO[0]);
        glLineWidth(2.0f);
        glPointSize(6.0f);
        glDrawArrays(GL_POINTS, data3->size.second * 2, parallel.size() / 4 - data3->size.second * 2);
        glDrawArrays(GL_LINES, data3->size.second * 2, parallel.size() / 4 - data3->size.second * 2);
        glDrawArrays(GL_LINES, 0, data3->size.second * 2);
        // glDrawArrays(GL_POINTS, 0, parallel.size() / 4);
    }
    else
    {
        shader->use();

        shader->setMatrix4("projection", glm::value_ptr(projection));
        shader->setMatrix4("view", glm::value_ptr(view));

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(20.0, 20.0, 0.0));
        model = glm::translate(model, glm::vec3(-x_offset, -y_offset, 0.0));
        shader->setMatrix4("model", glm::value_ptr(model));

        shader->setInt("tex1D", 0);

        texture->active1D(0);

        glBindVertexArray(VAO[1]);
        glPointSize(8.0f);
        glDrawArrays(GL_POINTS, 0, vertices.size() / 4);
    }
}

void MultiDimention::setShader()
{
    delete this->shader;
    this->shader = new Shader("src/Shaders/multiDimention.vert", "src/Shaders/multiDimention.frag");
}

Shader* MultiDimention::getShader() const
{
    return this->shader;
}

void MultiDimention::unionCluser(int i, int j)
{
    int k = min(this->cluster[i], this->cluster[j]);

    for(auto &c: this->cluster)
    {
        if(glm::abs(c - this->cluster[i]) < 0.001 || glm::abs(c - this->cluster[j]) < 0.001)
            c = k;
    }
}

bool MultiDimention::findCluser(int i, int j)
{
    return this->cluster[i] == this->cluster[j];
}

void MultiDimention::normalize()
{
    vector<double> maxVal(data3->size.second, 0);
    vector<double> minVal(data3->size.second, 0);
    int i, j;

    for(i = 0; i < data3->size.first; i++)
    {
        for(j = 0; j < data3->size.second; j++)
        {
            maxVal[j] = max(maxVal[j], data3->value[i][j]);
            minVal[j] = min(minVal[j], data3->value[i][j]);
        }
    }

    for(i = 0; i < data3->size.first; i++)
    {
        for(j = 0; j < data3->size.second; j++)
        {
            data3->value[i][j] = (data3->value[i][j] - minVal[j]) / (maxVal[j] - minVal[j]);
        }
    }
}

void MultiDimention::preprocessing()
{
    double pik_pjk;
    int i, j, k;

    for(i = 0; i < data3->size.first; i++)
    {
        for(j = 0; j < data3->size.first; j++)
        {
            for(k = 0; k < data3->size.second; k++)
            {
                pik_pjk = data3->value[i][k] - data3->value[j][k];
                data3->distance[i][j] += pik_pjk * pik_pjk;
            }
        }
    }

    data3->totalDist = 0;

    for(i = 0; i < data3->size.first; i++)
    {
        for(j = 0; j < data3->size.first; j++)
        {
            data3->distance[i][j] = sqrt(data3->distance[i][j]);
            data3->totalDist += data3->distance[i][j];

            if(data3->distance[i][j] < 0.5 && !findCluser(i, j))
                unionCluser(i, j);
        }
    }

    data3->totalDist /= 2;

    set<int> group;
    for(auto c: cluster)
    {
        group.insert(c);
    }

    i = 0;
    for(auto it = group.begin(); it != group.end(); it++, i++)
    {
        for(auto &c: cluster)
        {
            if((int)c == *it)
                c = (double)i / group.size();
        }
    }
}

void MultiDimention::makeParallel()
{
    parallel.clear();

    for(int j = 0; j < data3->size.second; j++)
    {
        parallel.push_back(j - data3->size.second / 2);
        parallel.push_back(-0.5);
        parallel.push_back(0.0);
        parallel.push_back(0.99);

        parallel.push_back(j - data3->size.second / 2);
        parallel.push_back(0.5);
        parallel.push_back(0.0);
        parallel.push_back(0.99);
    }

    for(int i = 0; i < data3->size.first; i++)
    {
        for(int j = 0; j < data3->size.second - 1; j++)
        {
            parallel.push_back(j - data3->size.second / 2);
            parallel.push_back(data3->value[i][j] - 0.5);
            parallel.push_back(0.0);
            parallel.push_back(cluster[i]);

            parallel.push_back(j + 1 - data3->size.second / 2);
            parallel.push_back(data3->value[i][j + 1] - 0.5);
            parallel.push_back(0.0);
            parallel.push_back(cluster[i]);
        }
    }

    // for(int j = 0; j < 256; j++)
    // {
    //     parallel.push_back((double)j / 255 - 0.5);
    //     parallel.push_back(0.0);
    //     parallel.push_back(0.0);
    //     parallel.push_back((double)j / 255);
    // }
}

void MultiDimention::SammonMapping2D()
{
    vector<glm::dvec2> Q(data3->size.first);
    double epsilon = 0.000'001, threshold = 0.000'001, error = numeric_limits<double>::max();
    double dij, _dij;
    glm::dvec2 deltaQi, deltaQj;
    double min_x = numeric_limits<double>::max(), min_y = numeric_limits<double>::max(),
           max_x = numeric_limits<double>::min(), max_y = numeric_limits<double>::min();
    int i, j;

    random_device rd;
    default_random_engine gen = std::default_random_engine(rd());
    uniform_real_distribution<double> dis(0.0, 1.0);

    for(i = 0; i < (int)Q.size(); i++)
    {
        Q[i] = glm::dvec2(dis(gen), dis(gen));
    }

    while(error > threshold)
    {
        error = 0.0;

        for(i = 0; i < (int)Q.size(); i++)
        {
            for(j = 0; j < (int)Q.size(); j++)
            {
                _dij = this->data3->distance[i][j];
                dij = glm::distance(Q[i], Q[j]);

                if(dij < epsilon) dij = epsilon;

                deltaQi = this->lambda * (_dij - dij) / dij * (Q[i] - Q[j]);
                deltaQj = -deltaQi;

                Q[i] += deltaQi;
                Q[j] += deltaQj;

                error += glm::length(deltaQi) + glm::length(deltaQj);
            }
        }
        
        this->lambda *= this->alpha;
    }

    for(i = 0; i < (int)Q.size(); i++)
    {
        this->vertices.push_back(Q[i].x);
        this->vertices.push_back(Q[i].y);
        this->vertices.push_back(0.0);
        this->vertices.push_back(cluster[i]);

        min_x = min(min_x, Q[i].x);
        min_y = min(min_y, Q[i].y);
        max_x = max(max_x, Q[i].x);
        max_y = max(max_y, Q[i].y);
    }

    this->x_offset = (min_x + max_x) / 2;
    this->y_offset = (min_y + max_y) / 2;
}

void MultiDimention::PCA()
{
    Eigen::MatrixXd X(data3->size.first, data3->size.second);
    int i, j;

    for(i = 0; i < data3->size.first; i++)
    {
        for(j = 0; j < data3->size.second; j++)
        {
            X(i, j) = data3->value[i][j];
        }
    }

    for(j = 0; j < data3->size.second; j++)
    {
        X.col(j) = X.col(j) - Eigen::MatrixXd::Constant(data3->size.first, 1, X.col(j).mean());
    }

    Eigen::MatrixXd A = X.transpose() * X;
    Eigen::EigenSolver<Eigen::MatrixXd> es(A);
    double prim = 0, seco = 0;
    int primOrder = 0, secoOrder = 0;

    for(j = 0; j < data3->size.second; j++)
    {
        if(prim < fabs(es.eigenvalues().real()[j]))
        {
            seco = prim;
            prim = es.eigenvalues().real()[j];
            secoOrder = primOrder;
            primOrder = j;
        }
        else if(seco < fabs(es.eigenvalues().real()[j]))
        {
            seco = es.eigenvalues().real()[j];
            secoOrder = j;
        }
    }

    for(i = 0; i < this->data3->size.first; i++)
    {
        this->vertices.push_back(X.row(i).dot(es.eigenvectors().col(primOrder).real()));
        this->vertices.push_back(X.row(i).dot(es.eigenvectors().col(secoOrder).real()));
        this->vertices.push_back(0.0);
        this->vertices.push_back(cluster[i]);
    }
}

void MultiDimention::make1DTexture()
{
    for(int i = 0; i < 256; i++)
    {
        if(i > 245)
        {
            texture1D[i][0] = 0;
            texture1D[i][1] = 0;
            texture1D[i][2] = 0;
            texture1D[i][3] = 255;
        }
        else if(i < 96)
        {
            texture1D[i][0] = 255;
            texture1D[i][1] = i * 2.6;
            texture1D[i][2] = 0;
            texture1D[i][3] = 255;
        }
        else if(i < 128)
        {
            texture1D[i][0] = 255 - (i-96) * 8;
            texture1D[i][1] = 255;
            texture1D[i][2] = 0;
            texture1D[i][3] = 255;
        }
        else if(i < 192)
        {
            texture1D[i][0] = 0;
            texture1D[i][1] = 255 - (i-128) * 4;
            texture1D[i][2] = (i-128) * 4;
            texture1D[i][3] = 255;
        }
        else
        {
            texture1D[i][0] = (i-192) * 4;
            texture1D[i][1] = 0;
            texture1D[i][2] = 255;
            texture1D[i][3] = 255;
        }
    }

    texture->make1DTexture(0, texture1D, 256);
}
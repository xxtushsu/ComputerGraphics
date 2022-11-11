#include "mainview.h"

#include <QDateTime>
#include <stddef.h>

struct Vertex{
    GLfloat x,y,z,r,g,b;
};

struct Triangle {
    Vertex a;
    Vertex b;
    Vertex c;
};

Vertex initVertex(GLfloat x, GLfloat y, GLfloat z, GLfloat r, GLfloat g, float b){
    Vertex vertex;
    vertex.x = x;
    vertex.y = y;
    vertex.z = z;
    vertex.r = r;
    vertex.g = g;
    vertex.b = b;
    return vertex;
}

Triangle initTriangle(Vertex v1, Vertex v2, Vertex v3) {
    Triangle triangle;
    triangle.a = v1;
    triangle.b = v2;
    triangle.c = v3;
    return triangle;
}

GLuint cubeVAO, pyramidVAO, cubeVBO, pyramidVBO;
Triangle cubeArray[12];
Triangle pyramidArray[16];
int location;

glm::mat4 translationSquare = glm::mat4(
    glm::vec4(1.0f, 0.0f, 0.0f, 2.0f),
    glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),
    glm::vec4(0.0f, 0.0f, 1.0f, -6.0f),
    glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
);

glm::mat4 translationPyramid = glm::mat4(
    glm::vec4(1.0f, 0.0f, 0.0f, -2.0f),
    glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),
    glm::vec4(0.0f, 0.0f, 1.0f, -6.0f),
    glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
);

GLfloat r = 6.0f, l = -6.0f, t = 6.0f, b = -6.0f, f = 10.3923, n = 4;

glm::mat4 perspective = glm::mat4(
    glm::vec4(2*n/(r-l), 0.0f, (r+l)/(r-l), 0.0f),
    glm::vec4(0.0f, 2*n/(t-b), (t+b)/(t-b), 0.0f),
    glm::vec4(0.0f, 0.0f, -(n+f)/(n-f), -2*n*f/(n-f)),
    glm::vec4(0.0f, 0.0f, -1.0f, 0.0f)
);

glm::mat4 scaling = glm::mat4(1.0f);
glm::mat4 rotation = glm::mat4(1.0f);

glm::mat4 MVP;

/**
 * @brief MainView::MainView
 *
 * Constructor of MainView
 *
 * @param parent
 */
MainView::MainView(QWidget *parent) : QOpenGLWidget(parent) {
    qDebug() << "MainView constructor";

    connect(&timer, SIGNAL(timeout()), this, SLOT(update()));
}

/**
 * @brief MainView::~MainView
 *
 * Destructor of MainView
 * This is the last function called, before exit of the program
 * Use this to clean up your variables, buffers etc.
 *
 */
MainView::~MainView() {
    qDebug() << "MainView destructor";

    makeCurrent();
}

// --- OpenGL initialization

/**
 * @brief MainView::initializeGL
 *
 * Called upon OpenGL initialization
 * Attaches a debugger and calls other init functions
 */
void MainView::initializeGL() {
    qDebug() << ":: Initializing OpenGL";
    initializeOpenGLFunctions();

    connect(&debugLogger, SIGNAL( messageLogged( QOpenGLDebugMessage)),
            this, SLOT(onMessageLogged(QOpenGLDebugMessage)), Qt::DirectConnection);

    if (debugLogger.initialize()) {
        qDebug() << ":: Logging initialized";
        debugLogger.startLogging(QOpenGLDebugLogger::SynchronousLogging);
    }

    QString glVersion;
    glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    qDebug() << ":: Using OpenGL" << qPrintable(glVersion);
    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // Enable backface culling
    glEnable(GL_CULL_FACE);

    // Default is GL_LESS
    glDepthFunc(GL_LEQUAL);

    // Initalize all the corners of the cube.
    Vertex lfd = initVertex(-1.0f,-1.0f, 1.0f,0.0f,0.0f,0.0f); // left front down
    Vertex rfd = initVertex( 1.0f,-1.0f, 1.0f,0.0f,.33f,0.0f); // right front down
    Vertex lbd = initVertex(-1.0f,-1.0f,-1.0f,0.0f,.33f,0.0f); // left back down
    Vertex rbd = initVertex( 1.0f,-1.0f,-1.0f,0.0f,.67f,0.0f); // right back down
    Vertex lfu = initVertex(-1.0f, 1.0f, 1.0f,0.0f,.33f,0.0f); // left front up
    Vertex rfu = initVertex( 1.0f, 1.0f, 1.0f,0.0f,.67f,0.0f); // right front up
    Vertex lbu = initVertex(-1.0f, 1.0f,-1.0f,0.0f,.67f,0.0f); // left back up
    Vertex rbu = initVertex( 1.0f, 1.0f,-1.0f,0.0f,1.0f,0.0f); // right back up

    // Initalize all the faces of the cube.
    cubeArray[0] = initTriangle(lfd, lfu, lbu); // left face
    cubeArray[1] = initTriangle(lfd, lbu, lbd);
    cubeArray[2] = initTriangle(rfu, rfd, rbd); // right face
    cubeArray[3] = initTriangle(rfu, rbd, rbu);
    cubeArray[4] = initTriangle(rbd, lbd,  lbu); // back face
    cubeArray[5] = initTriangle(rbd, lbu, rbu);
    cubeArray[6] = initTriangle(rfu, lfu, lfd); // front face
    cubeArray[7] = initTriangle(rfu, lfd, rfd);
    cubeArray[8] = initTriangle(rfd, lfd, lbd); // bottom face
    cubeArray[9] = initTriangle(rfd, lbd, rbd);
    cubeArray[10] = initTriangle(rbu, lbu, lfu); // top face
    cubeArray[11] = initTriangle(rbu, lfu, rfu);

    glGenBuffers(1, &cubeVBO);  // Make cube vbo
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glGenVertexArrays(1, &cubeVAO);  // Make cube vao
    glBindVertexArray(cubeVAO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeArray), cubeArray, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, x));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, r));

    // Initalize all the corners of the pyramid.
    Vertex lf = initVertex(-1.0f,-1.0f, 1.0f,0.0f,0.0f,0.0f); // left front
    Vertex rf = initVertex( 1.0f,-1.0f, 1.0f,.33f,0.0f,0.0f); // right front
    Vertex lb = initVertex(-1.0f,-1.0f,-1.0f,.33f,0.0f,0.0f); // left back
    Vertex rb = initVertex( 1.0f,-1.0f,-1.0f,.67f,0.0f,0.0f); // right back
    Vertex t = initVertex( 0.0f, 1.0f, 0.0f,1.0f,0.0f,0.0f); // top

    // Initalize all the faces of the pyramid
    pyramidArray[0] = initTriangle(lf, t, lb); // left face
    pyramidArray[1] = initTriangle(rf, rb, t); // right face
    pyramidArray[2] = initTriangle(lf, rf, t); // front face
    pyramidArray[3] = initTriangle(lb, t, rb); // back face
    pyramidArray[4] = initTriangle(lf, lb, rf); // base face
    pyramidArray[5] = initTriangle(rb, rf, lb);

    glGenBuffers(1, &pyramidVBO);  // Make pyramid vbo
    glBindBuffer(GL_ARRAY_BUFFER, pyramidVBO);
    glGenVertexArrays(1, &pyramidVAO); // Make pyramid vao
    glBindVertexArray(pyramidVAO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidArray), pyramidArray, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, x));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, r));

    // light blue background
    glClearColor(0.2f, 0.5f, 0.7f, 0.0f);

    createShaderProgram();
}

void MainView::createShaderProgram() {
    // Create shader program
    shaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,":/shaders/vertshader.glsl");
    shaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,":/shaders/fragshader.glsl");
    shaderProgram.link();
    location = shaderProgram.uniformLocation("MVP");
}

/**
 * @brief MainView::paintGL
 *
 * Actual function used for drawing to the screen
 *
 */
void MainView::paintGL() {
    // Clear the screen before rendering
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shaderProgram.bind();

    MVP = rotation * scaling * translationSquare * perspective;
    glUniformMatrix4fv(location, 1, GL_FALSE, &MVP[0][0]);

    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    MVP = rotation * scaling * translationPyramid * perspective;
    glUniformMatrix4fv(location, 1, GL_FALSE, &MVP[0][0]);

    glBindVertexArray(pyramidVAO);
    glDrawArrays(GL_TRIANGLES,0,18);

    shaderProgram.release();
}

/**
 * @brief MainView::resizeGL
 *
 * Called upon resizing of the screen
 *
 * @param newWidth
 * @param newHeight
 */
void MainView::resizeGL(int newWidth, int newHeight) {
    Q_UNUSED(newWidth)
    Q_UNUSED(newHeight)
    update();
}

// --- Public interface

void MainView::setRotation(int rotateX, int rotateY, int rotateZ) {
    glm::mat4 xRot = glm::mat4(
        glm::vec4(cosf(rotateX/180.0 * M_PI), -sinf(rotateX/180.0 * M_PI), 0.0f, 0.0f),
        glm::vec4(sinf(rotateX/180.0 * M_PI), cosf(rotateX/180.0 * M_PI), 0.0f, 0.0f),
        glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
        glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
    );
    glm::mat4 yRot = glm::mat4(
        glm::vec4(cosf(rotateY/180.0 * M_PI), 0.0f, -sinf(rotateY/180.0 * M_PI), 0.0f),
        glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),
        glm::vec4(sinf(rotateY/180.0 * M_PI), 0.0f, cosf(rotateY/180.0 * M_PI), 0.0f),
        glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
    );
    glm::mat4 zRot = glm::mat4(
        glm::vec4(1.0f, 0.0f, 0.0f, 0.0f),
        glm::vec4(0.0f, cosf(rotateZ/180.0 * M_PI), -sinf(rotateZ/180.0 * M_PI), 0.0f),
        glm::vec4(0.0f, sinf(rotateZ/180.0 * M_PI), cosf(rotateZ/180.0 * M_PI), 0.0f),
        glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
    );

    rotation = xRot * yRot * zRot;

    update();
}

void MainView::setScale(int scale) {
    qDebug() << "Scale changed to " << scale;
    scaling = glm::mat4(
        glm::vec4(scale/100.0, 0.0f, 0.0f, 0.0f),
        glm::vec4(0.0f, scale/100.0, 0.0f, 0.0f),
        glm::vec4(0.0f, 0.0f, scale/100.0, 0.0f),
        glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
    );
    update();
}

void MainView::setShadingMode(ShadingMode shading) {
    qDebug() << "Changed shading to" << shading;
    Q_UNIMPLEMENTED();
}

// --- Private helpers

/**
 * @brief MainView::onMessageLogged
 *
 * OpenGL logging function, do not change
 *
 * @param Message
 */
void MainView::onMessageLogged( QOpenGLDebugMessage Message ) {
    qDebug() << " → Log:" << Message;
}

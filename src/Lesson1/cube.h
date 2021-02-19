#ifndef CUBE_H
#define CUBE_H

#include "CubeObject.h"

class Cube final: public CubeObject
{
public:
    Cube() = delete;
    explicit Cube(GLfloat edgeLen) : CubeObject(edgeLen, 18){} /* 18 vertices for cube consists of 3 trianglestrip */

    void setColor(const QColor& color) override;
private:
    void setVertices() override;
    void drawArrays() override;
};

#endif // CUBE_H

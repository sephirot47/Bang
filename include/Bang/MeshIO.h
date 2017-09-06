#ifndef MESHIO_H
#define MESHIO_H

#include "Bang/Bang.h"

NAMESPACE_BANG_BEGIN

FORWARD class Scene;

class MeshIO
{
public:
    static int GetModelNumTriangles(const Path& filepath);
    static bool ReadModel(const Path& filepath,
                          Array<Vector3> *vertexPos,
                          Array<Vector3> *vertexNormals,
                          Array<Vector2> *vertexUvs);
private:
    MeshIO() {}
};

NAMESPACE_BANG_END

#endif // MESHIO_H

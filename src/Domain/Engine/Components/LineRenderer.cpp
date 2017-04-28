#include "Bang/LineRenderer.h"

#include "Bang/Mesh.h"
#include "Bang/Math.h"
#include "Bang/Material.h"
#include "Bang/Transform.h"
#include "Bang/ShaderProgram.h"
#include "Bang/AssetsManager.h"

LineRenderer::LineRenderer()
{
    m_meshRenderer = new MeshRenderer();
    m_meshRenderer->SetMesh( new Mesh() );
    SetRenderMode(GL::RenderMode::Lines);
}

LineRenderer::~LineRenderer()
{
    delete m_meshRenderer->GetMesh();
    delete m_meshRenderer;
}

void LineRenderer::CloneInto(ICloneable *clone) const
{
    Renderer::CloneInto(clone);
    LineRenderer *lr = Object::SCast<LineRenderer>(clone);
    lr->SetPoints( GetPoints() );
}

const Array<Vector3> &LineRenderer::GetPoints() const
{
    return m_points;
}

void LineRenderer::SetPoint(int i, const Vector3 &point)
{
    if (i >= m_points.Size())
    {
        if (i == m_points.Size()) { m_points.PushBack(point); }
        else { return; }
    }
    else
    {
        m_points[i] = point;
    }
    SetPoints(m_points);
}

void LineRenderer::SetPoints(const Array<Vector3> &points)
{
    m_points = points;
    m_meshRenderer->GetMesh()->LoadPositions(m_points);
}

void LineRenderer::RenderWithoutMaterial() const
{
    Mesh *mesh = m_meshRenderer->GetMesh();
    ENSURE(mesh && mesh->GetVAO() && mesh->GetVertexCount() > 0);
    GL::Render(mesh->GetVAO(), GetRenderMode(), mesh->GetVertexCount());
}

AABox LineRenderer::GetAABBox() const
{
    if (m_points.Empty()) { return AABox::Empty; }

    Vector3 minp = m_points.Front();
    Vector3 maxp = m_points.Front();
    for (const Vector3 &p : m_points)
    {
       minp.x = Math::Min(minp.x, p.x);
       minp.y = Math::Min(minp.y, p.y);
       minp.z = Math::Min(minp.z, p.z);
       maxp.x = Math::Max(maxp.x, p.x);
       maxp.y = Math::Max(maxp.y, p.y);
       maxp.z = Math::Max(maxp.z, p.z);
    }

    // Add a bit in every dimensions, to avoid flattened Rects
    const float strokeAdd = 0.01f;
    minp.x -= strokeAdd; maxp.x += strokeAdd;
    minp.y -= strokeAdd; maxp.y += strokeAdd;
    minp.z -= strokeAdd; maxp.z += strokeAdd;
    return AABox(minp, maxp);

}

void LineRenderer::Read(const XMLNode &xmlInfo)
{
    Renderer::Read(xmlInfo);
}

void LineRenderer::Write(XMLNode *xmlInfo) const
{
    Renderer::Write(xmlInfo);
}

#include <husky/mesh/Primitive.hpp>
#include <husky/math/Math.hpp>

namespace husky {

SimpleMesh Primitive::box(double sizeX, double sizeY, double sizeZ)
{
  const Vector3d h(sizeX * 0.5, sizeY * 0.5, sizeZ * 0.5); // Half size

  SimpleMesh m;

  m.addQuad( // X+
    m.addVertex({  h.x, -h.y, -h.z }, {  1,  0,  0 }, { 0, 0 }),
    m.addVertex({  h.x,  h.y, -h.z }, {  1,  0,  0 }, { 1, 0 }),
    m.addVertex({  h.x,  h.y,  h.z }, {  1,  0,  0 }, { 1, 1 }),
    m.addVertex({  h.x, -h.y,  h.z }, {  1,  0,  0 }, { 0, 1 }));

  m.addQuad( // X-
    m.addVertex({ -h.x,  h.y, -h.z }, { -1,  0,  0 }, { 0, 0 }),
    m.addVertex({ -h.x, -h.y, -h.z }, { -1,  0,  0 }, { 1, 0 }),
    m.addVertex({ -h.x, -h.y,  h.z }, { -1,  0,  0 }, { 1, 1 }),
    m.addVertex({ -h.x,  h.y,  h.z }, { -1,  0,  0 }, { 0, 1 }));

  m.addQuad( // Y+
    m.addVertex({  h.x,  h.y, -h.z }, {  0,  1,  0 }, { 0, 0 }),
    m.addVertex({ -h.x,  h.y, -h.z }, {  0,  1,  0 }, { 1, 0 }),
    m.addVertex({ -h.x,  h.y,  h.z }, {  0,  1,  0 }, { 1, 1 }),
    m.addVertex({  h.x,  h.y,  h.z }, {  0,  1,  0 }, { 0, 1 }));

  m.addQuad( // Y-
    m.addVertex({ -h.x, -h.y, -h.z }, {  0, -1,  0 }, { 0, 0 }),
    m.addVertex({  h.x, -h.y, -h.z }, {  0, -1,  0 }, { 1, 0 }),
    m.addVertex({  h.x, -h.y,  h.z }, {  0, -1,  0 }, { 1, 1 }),
    m.addVertex({ -h.x, -h.y,  h.z }, {  0, -1,  0 }, { 0, 1 }));

  m.addQuad( // Z+
    m.addVertex({ -h.x, -h.y,  h.z }, {  0,  0,  1 }, { 0, 0 }),
    m.addVertex({  h.x, -h.y,  h.z }, {  0,  0,  1 }, { 1, 0 }),
    m.addVertex({  h.x,  h.y,  h.z }, {  0,  0,  1 }, { 1, 1 }),
    m.addVertex({ -h.x,  h.y,  h.z }, {  0,  0,  1 }, { 0, 1 }));

  m.addQuad( // Z-
    m.addVertex({  h.x, -h.y, -h.z }, {  0,  0, -1 }, { 1, 0 }),
    m.addVertex({ -h.x, -h.y, -h.z }, {  0,  0, -1 }, { 0, 0 }),
    m.addVertex({ -h.x,  h.y, -h.z }, {  0,  0, -1 }, { 0, 1 }),
    m.addVertex({  h.x,  h.y, -h.z }, {  0,  0, -1 }, { 1, 1 }));

  return m;
}

SimpleMesh Primitive::cylinder(double radius, double height, bool cap, int segmentCount)
{
  SimpleMesh m;

  const double h0 = -height * 0.5;
  const double h1 =  height * 0.5;

  int v0Prev, v1Prev;
  Vector3d nPrev;
  Vector2d pPrev;
  for (int i = 0; i <= segmentCount; i++) {
    double u = i / double(segmentCount); // [0.0,1.0]
    Vector3d n(std::cos(u * Math::twoPi), std::sin(u * Math::twoPi), 0.0);
    Vector2d p = n.xy * radius;

    int v0 = m.addVertex({ p, h0 }, n, { u, 0.0 });
    int v1 = m.addVertex({ p, h1 }, n, { u, 1.0 });

    if (i > 0) {
      m.addQuad(v0, v1, v1Prev, v0Prev); // Wall

      if (cap) {
        m.addTriangle( // Floor
          m.addVertex(m.getVertex(v0Prev).pos, {  0,  0, -1 }, nPrev.xy * 0.5 + 0.5),
          m.addVertex({ 0, 0, h0 },            {  0,  0, -1 }, { 0.5, 0.5 }),
          m.addVertex(m.getVertex(v0).pos,     {  0,  0, -1 }, n.xy * 0.5 + 0.5));

        m.addTriangle( // Roof
          m.addVertex(m.getVertex(v1).pos,     {  0,  0,  1 }, n.xy * 0.5 + 0.5),
          m.addVertex({ 0, 0, h1 },            {  0,  0,  1 }, { 0.5, 0.5 }),
          m.addVertex(m.getVertex(v1Prev).pos, {  0,  0,  1 }, nPrev.xy * 0.5 + 0.5));
      }
    }

    nPrev = n;
    pPrev = p;
    v0Prev = v0;
    v1Prev = v1;
  }

  return m;
}

SimpleMesh Primitive::sphere(double radius, int uSegmentCount, int vSegmentCount)
{
  SimpleMesh m;

  for (int iu = 0; iu <= uSegmentCount; iu++) {
    const double u = iu / double(uSegmentCount); // [0:1]
    const double theta = u * 2.0 * Math::pi; // Azimuthal angle [0:2*pi]
    
    for (int iv = 0; iv <= vSegmentCount; iv++) {
      const double v = iv / double(vSegmentCount); // [0:1]
      const double phi = (1.0 - v) * Math::pi; // Polar angle [pi:0]

      Vector3d n;
      n.x = std::cos(theta) * std::sin(phi);
      n.y = std::sin(theta) * std::sin(phi);
      n.z = std::cos(phi);

      m.addVertex(n * radius, n, { u, v });

      if (iu > 0 && iv > 0) {
        assert(m.getVertexCount() >= 4);

        SimpleMesh::Quad q;
        q[0] = m.getVertexCount() - 1;
        q[1] = q[0] - (vSegmentCount + 1);
        q[2] = q[0] - (vSegmentCount + 2);
        q[3] = q[0] - 1;

        m.addQuad(q);
      }
    }
  }

  return m;
}

SimpleMesh Primitive::torus(double innerRadius, double outerRadius, int uSegmentCount, int vSegmentCount)
{
  SimpleMesh m;

  for (int iu = 0; iu <= uSegmentCount; iu++) {
    const double u = iu / double(uSegmentCount); // [0,1]
    const double theta = u * 2.0 * Math::pi; // [0,2*pi]

    const Matrix33d rotate = Matrix33d::rotate(theta, { 0, 0, 1 });
    const Vector3d radCenter = rotate * Vector3d(innerRadius, 0, 0);

    for (int iv = 0; iv <= vSegmentCount; iv++) {
      const double v = iv / double(vSegmentCount); // [0,1]
      const double phi = v * 2.0 * Math::pi; // [0,2*pi]

      const Vector3d vertNormal = rotate * Vector3d(std::cos(phi), 0.0, std::sin(-phi));
      const Vector3d vertPosition = (radCenter + vertNormal * outerRadius);
      m.addVertex(vertPosition, vertNormal, { u, v });

      if (iu > 0 && iv > 0) {
        assert(m.getVertexCount() >= 4);

        SimpleMesh::Quad q;
        q[0] = m.getVertexCount() - 1;
        q[1] = q[0] - 1; 
        q[2] = q[0] - (vSegmentCount + 2);
        q[3] = q[0] - (vSegmentCount + 1);

        m.addQuad(q);
      }
    }
  }

  return m;
}

}

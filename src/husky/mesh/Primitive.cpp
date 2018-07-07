#include <husky/mesh/Primitive.hpp>
#include <husky/math/MathUtil.hpp>

namespace husky {

SimpleMesh Primitive::uvSphere(double radius, int uSegmentCount, int vSegmentCount)
{
  SimpleMesh m;

  for (int iu = 0; iu <= uSegmentCount; iu++) {
    double u = iu / double(uSegmentCount); // [0.0,1.0]
    double c = std::cos(u * math::twoPi);

    for (int iv = 0; iv < vSegmentCount; iv++) {
      double v = iv / double(vSegmentCount); // [0.0,1.0]
      double s = std::sin(v * math::twoPi);

      // TODO
    }
  }

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
    Vector3d n(std::cos(u * math::twoPi), std::sin(u * math::twoPi), 0.0);
    Vector2d p = n.xy * radius;

    int v0 = m.addVertex({ p, h0 }, n, { u, 0.0 });
    int v1 = m.addVertex({ p, h1 }, n, { u, 1.0 });

    if (i > 0) {
      m.addQuad(v0, v1, v1Prev, v0Prev); // Wall

      if (cap) {
        m.addTriangle( // Floor
          m.addVertex(m.getVertex(v0Prev).pos, {  0,  0, -1 }, nPrev.xy),
          m.addVertex({ 0, 0, h0 },            {  0,  0, -1 }, { 0.5, 0.5 }),
          m.addVertex(m.getVertex(v0).pos,     {  0,  0, -1 }, n.xy));

        m.addTriangle( // Roof
          m.addVertex(m.getVertex(v1).pos,     {  0,  0,  1 }, n.xy),
          m.addVertex({ 0, 0, h1 },            {  0,  0,  1 }, { 0.5, 0.5 }),
          m.addVertex(m.getVertex(v1Prev).pos, {  0,  0,  1 }, nPrev.xy));
      }
    }

    nPrev = n;
    pPrev = p;
    v0Prev = v0;
    v1Prev = v1;
  }

  return m;
}

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
    m.addVertex({ -h.x,  h.y, -h.z }, { -1,  0,  0 }, { 1, 0 }),
    m.addVertex({ -h.x, -h.y, -h.z }, { -1,  0,  0 }, { 0, 0 }),
    m.addVertex({ -h.x, -h.y,  h.z }, { -1,  0,  0 }, { 0, 1 }),
    m.addVertex({ -h.x,  h.y,  h.z }, { -1,  0,  0 }, { 1, 1 }));

  m.addQuad( // Y+
    m.addVertex({  h.x,  h.y, -h.z }, {  0,  1,  0 }, { 0, 0 }),
    m.addVertex({ -h.x,  h.y, -h.z }, {  0,  1,  0 }, { 1, 0 }),
    m.addVertex({ -h.x,  h.y,  h.z }, {  0,  1,  0 }, { 1, 1 }),
    m.addVertex({  h.x,  h.y,  h.z }, {  0,  1,  0 }, { 0, 1 }));

  m.addQuad( // Y-
    m.addVertex({ -h.x, -h.y, -h.z }, {  0,  1,  0 }, { 1, 0 }),
    m.addVertex({  h.x, -h.y, -h.z }, {  0,  1,  0 }, { 0, 0 }),
    m.addVertex({  h.x, -h.y,  h.z }, {  0,  1,  0 }, { 0, 1 }),
    m.addVertex({ -h.x, -h.y,  h.z }, {  0,  1,  0 }, { 1, 1 }));

  m.addQuad( // Z+
    m.addVertex({ -h.x, -h.y,  h.z }, {  0,  0,  1 }, { 0, 0 }),
    m.addVertex({  h.x, -h.y,  h.z }, {  0,  0,  1 }, { 1, 0 }),
    m.addVertex({  h.x,  h.y,  h.z }, {  0,  0,  1 }, { 1, 1 }),
    m.addVertex({ -h.x,  h.y,  h.z }, {  0,  0,  1 }, { 0, 1 }));

  m.addQuad( // Z-
    m.addVertex({  h.x, -h.y, -h.z }, {  0,  0,  1 }, { 1, 0 }),
    m.addVertex({ -h.x, -h.y, -h.z }, {  0,  0,  1 }, { 0, 0 }),
    m.addVertex({ -h.x,  h.y, -h.z }, {  0,  0,  1 }, { 0, 1 }),
    m.addVertex({  h.x,  h.y, -h.z }, {  0,  0,  1 }, { 1, 1 }));

  return m;
}

}

#include <husky/math/Matrix44.hpp>
#include <husky/math/Math.hpp>
#include <cmath>

// TODO: Also return analytical inverse in ortho, perspective*, lookAt, etc.?

namespace husky {

template<typename T>
Matrix44<T> Matrix44<T>::identity()
{
  return {
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
  };
}

template<typename T>
Matrix44<T> Matrix44<T>::scale(const Vector3<T> &s)
{
  return {
    s.x,   0,   0, 0,
      0, s.y,   0, 0,
      0,   0, s.z, 0,
      0,   0,   0, 1
  };
}

template<typename T>
Matrix44<T> Matrix44<T>::translate(const Vector3<T> &pos)
{
  Matrix44<T> res = identity();
  res.col[3].xyz = pos;
  return res;
}

template<typename T>
Matrix44<T> Matrix44<T>::rotate(T rad, Vector3<T> axis)
{
  return { Matrix33<T>::rotate(rad,axis) };
}

template<typename T>
Matrix44<T> Matrix44<T>::ortho(T left, T right, T bottom, T top, T near, T far, Matrix44<T> *inv)
{
  T rpl = right + left;
  T rml = right - left;
  T tpb = top + bottom;
  T tmb = top - bottom;
  T fpn = far + near;
  T fmn = far - near;

  Matrix44<T> m = {
       2 / rml,          0,          0, 0,
             0,    2 / tmb,          0, 0,
             0,          0,   -2 / fmn, 0,
    -rpl / rml, -tpb / tmb, -fpn / fmn, 1
  };

  if (inv != nullptr) {
    *inv = {
      rml / 2,       0,        0, 0,
            0, tmb / 2,        0, 0,
            0,       0, fmn / -2, 0,
      rpl / 2, tpb / 2, fpn / -2, 1
    };
  }

  return m;
}

template<typename T>
Matrix44<T> Matrix44<T>::perspective(T vFovRad, T aspectRatio, T near, T far, Matrix44<T> *inv)
{
  T top = near * std::tan(vFovRad * T(0.5));
  T right = top * aspectRatio;
  return frustum(-right, right, -top, top, near, far, inv);
}

template<typename T>
Matrix44<T> Matrix44<T>::perspectiveInf(T vFovRad, T aspectRatio, T near, T epsilon, Matrix44<T> *inv)
{
  // Note: This differs slightly from http://www.terathon.com/gdc07_lengyel.pdf
  T e = T(1) / (std::tan(T(0.5) * vFovRad) * aspectRatio);
  T er = e * aspectRatio;
  T emo = epsilon - 1;
  T etn = (epsilon - 2) * near;

  Matrix44<T> m = {
    e,  0,   0,  0,
    0, er,   0,  0,
    0,  0, emo, -1,
    0,  0, etn,  0
  };

  if (inv != nullptr) {
    *inv = {
      1/e,    0,  0,       0,
        0, 1/er,  0,       0,
        0,    0,  0,   1/etn,
        0,    0, -1, emo/etn
    };
  }

  return m;
}

template<typename T>
Matrix44<T> Matrix44<T>::perspectiveInfRevZ(T vFovRad, T aspectRatio, T near, Matrix44<T> *inv)
{
  // https://nlguillemot.wordpress.com/2016/12/07/reversed-z-in-opengl/
  T e = T(1) / (std::tan(T(0.5) * vFovRad) * aspectRatio);
  T er = e * aspectRatio;

  Matrix44<T> m = {
    e,  0,    0,  0,
    0, er,    0,  0,
    0,  0,    0, -1,
    0,  0, near,  0
  };

  if (inv != nullptr) {
    *inv = {
      1/e,    0,  0,      0,
        0, 1/er,  0,      0,
        0,    0,  0, 1/near,
        0,    0, -1,      0
    };
  }

  return m;
}

template<typename T>
Matrix44<T> Matrix44<T>::frustum(T left, T right, T bottom, T top, T near, T far, Matrix44<T> *inv)
{
  T npn = 2 * near;
  T rpl = right + left;
  T rml = right - left;
  T tpb = top + bottom;
  T tmb = top - bottom;
  T fpn = far + near;
  T fmn = far - near;
  T mul = far * npn;

  Matrix44<T> m = {
    npn / rml,         0,          0,  0,
            0, npn / tmb,          0,  0,
    rpl / rml, tpb / tmb, -fpn / fmn, -1,
            0,         0, -mul / fmn,  0
  };

  if (inv != nullptr) {
    *inv = {
      rml/npn,       0,  0,        0,
            0, tmb/npn,  0,        0,
            0,       0,  0, -fmn/mul,
      rpl/npn, tpb/npn, -1,  fpn/mul
    };
  }

  return m;
}

template<typename T>
Matrix44<T> Matrix44<T>::lookAt(const Vector3<T> &camPos, const Vector3<T> &lookAtPos, const Vector3<T> &upDir, Matrix44<T> *inv)
{
  Vector3<T> u = upDir.normalized(); // Up
  Vector3<T> f = (lookAtPos - camPos).normalized(); // Forward
  Vector3<T> r = f.cross(u).normalized(); // Right
  u = r.cross(f);

  Matrix44<T> m = {
               r.x,            u.x,          -f.x, 0,
               r.y,            u.y,          -f.y, 0,
               r.z,            u.z,          -f.z, 0,
    -r.dot(camPos), -u.dot(camPos), f.dot(camPos), 1
  };

  if (inv != nullptr) {
    *inv = {
      {      r, 0 },
      {      u, 0 },
      {     -f, 0 },
      { camPos, 1 }
    };
  }

  return m;
}

template<typename T>
Matrix44<T> Matrix44<T>::compose(const Vector3<T> &scale, const Matrix33<T> &rot, const Vector3<T> &trans)
{
  Matrix44<T> m;
  m.col[0].xyz = rot.col[0] * scale[0];
  m.col[1].xyz = rot.col[1] * scale[1];
  m.col[2].xyz = rot.col[2] * scale[2];
  m.col[3] = { trans, T(1) };
  return m;
}

template<typename T>
void Matrix44<T>::decompose(Vector3<T> &scale, Matrix33<T> &rot, Vector3<T> &trans) const
{
  // Note: We assume scale >= 0
  scale[0] = col[0].xyz.length();
  scale[1] = col[1].xyz.length();
  scale[2] = col[2].xyz.length();

  // Note: This "rotation" matrix can also have shear
  rot = get3x3();
  if (scale[0] != T(0)) { rot.col[0] /= scale[0]; }
  if (scale[1] != T(0)) { rot.col[1] /= scale[1]; }
  if (scale[2] != T(0)) { rot.col[2] /= scale[2]; }

  trans = col[3].xyz;
}

template<typename T>
Matrix44<T>::Matrix44()
  : m{} // Zero
{
}

template<typename T>
Matrix44<T>::Matrix44(const T *m)
  : m{ m[0], m[1], m[2], m[3], m[4], m[5], m[6], m[7], m[8], m[9], m[10], m[11], m[12], m[13], m[14], m[15] }
{
}

template<typename T>
Matrix44<T>::Matrix44(T m00, T m01, T m02, T m03, T m10, T m11, T m12, T m13, T m20, T m21, T m22, T m23, T m30, T m31, T m32, T m33)
  : m{ m00, m01, m02, m03, m10, m11, m12, m13, m20, m21, m22, m23, m30, m31, m32, m33 }
{
}

template<typename T>
Matrix44<T>::Matrix44(const Vector4<T> &col0, const Vector4<T> &col1, const Vector4<T> &col2, const Vector4<T> &col3)
  : col{ col0, col1, col2, col3 }
{
}

template<typename T>
Matrix44<T>::Matrix44(const Matrix33<T> &other)
  : col{ { other.col[0], 0 }, { other.col[1], 0 }, { other.col[2], 0 }, { 0, 0, 0, 1 } }
{
}

template<typename T>
Vector4<T> Matrix44<T>::row(int i) const
{
  return Vector4<T>(col[0].val[i], col[1].val[i], col[2].val[i], col[3].val[i]);
}

template<typename T>
Matrix33<T> Matrix44<T>::get3x3() const
{
  return Matrix33<T>(col[0].xyz, col[1].xyz, col[2].xyz);
}

template<typename T>
void Matrix44<T>::transpose()
{
  *this = transposed();
}

template<typename T>
void Matrix44<T>::invert()
{
  Matrix44<T> tmp;
  tmp.m[0] = m[5] * m[10] * m[15] - m[5] * m[11] * m[14] - m[9] * m[6] * m[15] + m[9] * m[7] * m[14] + m[13] * m[6] * m[11] - m[13] * m[7] * m[10];
  tmp.m[4] = -m[4] * m[10] * m[15] + m[4] * m[11] * m[14] + m[8] * m[6] * m[15] - m[8] * m[7] * m[14] - m[12] * m[6] * m[11] + m[12] * m[7] * m[10];
  tmp.m[8] = m[4] * m[9] * m[15] - m[4] * m[11] * m[13] - m[8] * m[5] * m[15] + m[8] * m[7] * m[13] + m[12] * m[5] * m[11] - m[12] * m[7] * m[9];
  tmp.m[12] = -m[4] * m[9] * m[14] + m[4] * m[10] * m[13] + m[8] * m[5] * m[14] - m[8] * m[6] * m[13] - m[12] * m[5] * m[10] + m[12] * m[6] * m[9];
  tmp.m[1] = -m[1] * m[10] * m[15] + m[1] * m[11] * m[14] + m[9] * m[2] * m[15] - m[9] * m[3] * m[14] - m[13] * m[2] * m[11] + m[13] * m[3] * m[10];
  tmp.m[5] = m[0] * m[10] * m[15] - m[0] * m[11] * m[14] - m[8] * m[2] * m[15] + m[8] * m[3] * m[14] + m[12] * m[2] * m[11] - m[12] * m[3] * m[10];
  tmp.m[9] = -m[0] * m[9] * m[15] + m[0] * m[11] * m[13] + m[8] * m[1] * m[15] - m[8] * m[3] * m[13] - m[12] * m[1] * m[11] + m[12] * m[3] * m[9];
  tmp.m[13] = m[0] * m[9] * m[14] - m[0] * m[10] * m[13] - m[8] * m[1] * m[14] + m[8] * m[2] * m[13] + m[12] * m[1] * m[10] - m[12] * m[2] * m[9];
  tmp.m[2] = m[1] * m[6] * m[15] - m[1] * m[7] * m[14] - m[5] * m[2] * m[15] + m[5] * m[3] * m[14] + m[13] * m[2] * m[7] - m[13] * m[3] * m[6];
  tmp.m[6] = -m[0] * m[6] * m[15] + m[0] * m[7] * m[14] + m[4] * m[2] * m[15] - m[4] * m[3] * m[14] - m[12] * m[2] * m[7] + m[12] * m[3] * m[6];
  tmp.m[10] = m[0] * m[5] * m[15] - m[0] * m[7] * m[13] - m[4] * m[1] * m[15] + m[4] * m[3] * m[13] + m[12] * m[1] * m[7] - m[12] * m[3] * m[5];
  tmp.m[14] = -m[0] * m[5] * m[14] + m[0] * m[6] * m[13] + m[4] * m[1] * m[14] - m[4] * m[2] * m[13] - m[12] * m[1] * m[6] + m[12] * m[2] * m[5];
  tmp.m[3] = -m[1] * m[6] * m[11] + m[1] * m[7] * m[10] + m[5] * m[2] * m[11] - m[5] * m[3] * m[10] - m[9] * m[2] * m[7] + m[9] * m[3] * m[6];
  tmp.m[7] = m[0] * m[6] * m[11] - m[0] * m[7] * m[10] - m[4] * m[2] * m[11] + m[4] * m[3] * m[10] + m[8] * m[2] * m[7] - m[8] * m[3] * m[6];
  tmp.m[11] = -m[0] * m[5] * m[11] + m[0] * m[7] * m[9] + m[4] * m[1] * m[11] - m[4] * m[3] * m[9] - m[8] * m[1] * m[7] + m[8] * m[3] * m[5];
  tmp.m[15] = m[0] * m[5] * m[10] - m[0] * m[6] * m[9] - m[4] * m[1] * m[10] + m[4] * m[2] * m[9] + m[8] * m[1] * m[6] - m[8] * m[2] * m[5];

  T det = m[0] * tmp.m[0] + m[1] * tmp.m[4] + m[2] * tmp.m[8] + m[3] * tmp.m[12];

  if (det == 0) {
    *this = {}; // Set to 0
    return;
  }

  det = T(1) / det;

  for (int i = 0; i < 16; i++) {
    m[i] = tmp.m[i] * det;
  }
}

template<typename T>
Matrix44<T> Matrix44<T>::transposed() const
{
  return Matrix44<T>(row(0), row(1), row(2), row(3));
}

template<typename T>
Matrix44<T> Matrix44<T>::inverted() const
{
  Matrix44<T> res = *this;
  res.invert();
  return res;
}

template<typename T>
Matrix44<T>& Matrix44<T>::operator+=(const Matrix44<T> &other)
{
  *this = *this + other;
  return *this;
}

template<typename T>
Matrix44<T>& Matrix44<T>::operator-=(const Matrix44<T> &other)
{
  *this = *this - other;
  return *this;
}

template<typename T>
Matrix44<T>& Matrix44<T>::operator*=(const Matrix44<T> &other)
{
  *this = *this * other;
  return *this;
}

template<typename T>
Matrix44<T> Matrix44<T>::operator+(const Matrix44<T> &other) const
{
  return Matrix44<T>(col[0] + other.col[0], col[1] + other.col[1], col[2] + other.col[2], col[3] + other.col[3]);
}

template<typename T>
Matrix44<T> Matrix44<T>::operator-(const Matrix44<T> &other) const
{
  return Matrix44<T>(col[0] - other.col[0], col[1] - other.col[1], col[2] - other.col[2], col[3] - other.col[3]);
}

template<typename T>
Matrix44<T> Matrix44<T>::operator*(const Matrix44<T> &other) const
{
  Matrix44<T> m;
  m[0] = col[0] * other[0][0] + col[1] * other[0][1] + col[2] * other[0][2] + col[3] * other[0][3];
  m[1] = col[0] * other[1][0] + col[1] * other[1][1] + col[2] * other[1][2] + col[3] * other[1][3];
  m[2] = col[0] * other[2][0] + col[1] * other[2][1] + col[2] * other[2][2] + col[3] * other[2][3];
  m[3] = col[0] * other[3][0] + col[1] * other[3][1] + col[2] * other[3][2] + col[3] * other[3][3];
  return m;
}

template<typename T>
Matrix44<T> Matrix44<T>::operator-() const
{
  return Matrix44<T>(-col[0], -col[1], -col[2], -col[3]);
}

template<typename T>
Vector4<T> Matrix44<T>::operator*(const Vector4<T> &v) const
{
  Vector4<T> res;
  res[0] = row(0).dot(v);
  res[1] = row(1).dot(v);
  res[2] = row(2).dot(v);
  res[3] = row(3).dot(v);
  return res;
}

template class Matrix44<double>;
template class Matrix44<float>;

}

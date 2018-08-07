#include <husky/mesh/Transform.hpp>

namespace husky {

template<typename T>
Transform<T>::Transform()
  : Transform({ 0, 0, 0 }, { RotationOrder::ZXY, 0, 0, 0 }, { 1, 1, 1 })
{
}

template<typename T>
Transform<T>::Transform(const Vector3<T> &translation, const EulerAngles<T> &eulerAngles, const Vector3<T> &scale)
  : translation(translation)
  , eulerAngles(eulerAngles)
  , scale(scale)
{
  mtx = Matrix44<T>::compose(scale, eulerAngles.toMatrix(), translation);
  mtxInv = mtx.inverted(); // TODO: Calculate in compose?
}

template<typename T>
Transform<T>::Transform(const Matrix44<T> &mtx)
  : Transform(mtx, {})
{
  // TODO: Test!
  mtxInv = Matrix44<T>::translate(-translation) * Matrix44<T>((-eulerAngles).toMatrix()) * Matrix44<T>::scale({ 1 / scale.x, 1 / scale.y, 1 / scale.z });
}

template<typename T>
Transform<T>::Transform(const Matrix44<T> &mtx, const Matrix44<T> &mtxInv)
  : mtx(mtx)
  , mtxInv(mtxInv)
{
  Matrix33<T> mtxRot;
  mtx.decompose(scale, mtxRot, translation);
  eulerAngles = EulerAngles<T>(RotationOrder::ZYX, mtxRot);
}

template class Transform<double>;
template class Transform<float>;

}

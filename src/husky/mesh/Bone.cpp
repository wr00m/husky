#include <husky/mesh/Bone.hpp>

namespace husky {

Bone::Bone(const std::string &name)
  : Bone(name, -1)
{
}

Bone::Bone(const std::string &name, int parent)
  : Bone(name, parent, Matrix44f::identity())
{
}

Bone::Bone(const std::string &name, int parent, const Matrix44d &mtx)
  : name(name)
  , parent(parent)
  , mtx(mtx)
  , mtxCombined(mtx)
{
}

}

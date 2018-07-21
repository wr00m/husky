#include <husky/mesh/Bone.hpp>

namespace husky {

Bone::Bone(const std::string &name)
  : Bone(name, -1)
{
}

Bone::Bone(const std::string &name, int parent)
  : Bone(name, parent, husky::Matrix44f::identity())
{
}

Bone::Bone(const std::string &name, int parent, const husky::Matrix44f &mtx)
  : name(name)
  , parent(parent)
  , mtx(mtx)
  , mtxCombined(mtx)
{
}

}

#include <husky/mesh/Animation.hpp>
#include <husky/Log.hpp>

namespace husky {

AnimationChannel::AnimationChannel(const std::string &nodeName)
  : nodeName(nodeName)
{
}

Animation::Animation(const std::string &name, double durationTicks, double ticksPerSecond)
  : name(name), durationTicks(durationTicks), ticksPerSecond(ticksPerSecond)
{
}

Matrix44d Animation::getNodeTransform(const std::string &nodeName, double time, const Matrix44d &defaultTransform) const
{
  const auto &it = channels.find(nodeName);
  if (it == channels.end()) {
    return defaultTransform;
  }

  const double ticks = time * ticksPerSecond;
  const AnimationChannel &ch = it->second;
  Matrix44d transform = Matrix44d::identity();

  if (!ch.keyframePosition.empty()) { // Translation
    const auto keyframe1 = ch.keyframePosition.upper_bound(time);
    const auto keyframe0 = std::prev(keyframe1);

    if (keyframe1 == ch.keyframePosition.begin()) {
      transform = Matrix44d::translate(keyframe1->second);
    }
    else if (keyframe1 == ch.keyframePosition.end()) {
      transform = Matrix44d::translate(keyframe0->second);
    }
    else {
      const double t = (ticks - keyframe0->first) / (keyframe1->first - keyframe0->first);
      transform = Matrix44d::translate(keyframe0->second.lerp(keyframe1->second, t));
    }
  }

  if (!ch.keyframeRotation.empty()) { // Rotation
    const auto keyframe1 = ch.keyframeRotation.upper_bound(time);
    const auto keyframe0 = std::prev(keyframe1);

    if (keyframe1 == ch.keyframeRotation.begin()) {
      transform = transform * Matrix44d(keyframe1->second.toMatrix());
    }
    else if (keyframe1 == ch.keyframeRotation.end()) {
      transform = transform * Matrix44d(keyframe0->second.toMatrix());
    }
    else {
      const double t = (ticks - keyframe0->first) / (keyframe1->first - keyframe0->first);
      transform = transform * Matrix44d((keyframe0->second.slerp(keyframe1->second, t)).toMatrix());
    }
  }

  if (!ch.keyframeScale.empty()) { // Scale
    const auto keyframe1 = ch.keyframeScale.upper_bound(time);
    const auto keyframe0 = std::prev(keyframe1);

    if (keyframe1 == ch.keyframeScale.begin()) {
      transform = transform * Matrix44d::scale(keyframe1->second);
    }
    else if (keyframe1 == ch.keyframeScale.end()) {
      transform = transform * Matrix44d::scale(keyframe0->second);
    }
    else {
      const double t = (ticks - keyframe0->first) / (keyframe1->first - keyframe0->first);
      transform = transform * Matrix44d::scale(keyframe0->second.lerp(keyframe1->second, t));
    }
  }

  return transform;
}

Matrix44d Animation::getBoneTransform(const Bone &bone, double time) const
{
  return getNodeTransform(bone.name, time, bone.mtx);
}

}

#include <husky/mesh/Animation.hpp>
#include <husky/mesh/Model.hpp>
#include <husky/Log.hpp>

namespace husky {

AnimationChannel::AnimationChannel(const std::string &nodeName)
  : nodeName(nodeName)
{
}

AnimatedNode::AnimatedNode(const std::string &name)
  : name(name)
  , animated(false)
{
}

Animation::Animation(const std::string &name, double durationTicks, double ticksPerSecond)
  : name(name)
  , durationTicks(durationTicks)
  , ticksPerSecond(ticksPerSecond)
{
}

double Animation::getTicks(double seconds) const
{
  return std::fmod(seconds * ticksPerSecond, durationTicks); // TODO: Support different loop modes
}

void Animation::getAnimatedNodesRecursive(const ModelNode *node, double ticks, std::map<std::string, AnimatedNode> &animNodes, const AnimatedNode *parent) const
{
  AnimatedNode animNode(node->name);
  
  animNode.animated = getAnimatedNodeTransform(node->name, ticks, animNode.mtxRelToParent);
  animNode.mtxRelToModel = parent ? (parent->mtxRelToModel * animNode.mtxRelToParent) : animNode.mtxRelToParent;

  for (const ModelNode *child : node->children) {
    getAnimatedNodesRecursive(child, ticks, animNodes, &animNode);
  }

  animNodes.insert({ animNode.name, animNode });
}

bool Animation::getAnimatedNodeTransform(const std::string &nodeName, double ticks, Matrix44d &transform) const
{
  const auto &it = channels.find(nodeName);
  if (it == channels.end()) {
    transform = Matrix44d::identity();
    return false;
  }

  const AnimationChannel &ch = it->second;

  Vector3d trans(0.0);
  if (!ch.keyframePosition.empty()) { // Translation
    const auto keyframe1 = ch.keyframePosition.upper_bound(ticks);
    const auto keyframe0 = std::prev(keyframe1);

    if (keyframe1 == ch.keyframePosition.begin()) {
      trans = keyframe1->second;
    }
    else if (keyframe1 == ch.keyframePosition.end()) {
      trans = keyframe0->second;
    }
    else {
      const double t = (ticks - keyframe0->first) / (keyframe1->first - keyframe0->first);
      trans = keyframe0->second.lerp(keyframe1->second, t);
    }
  }

  Quaterniond rot;
  if (!ch.keyframeRotation.empty()) { // Rotation
    const auto keyframe1 = ch.keyframeRotation.upper_bound(ticks);
    const auto keyframe0 = std::prev(keyframe1);

    if (keyframe1 == ch.keyframeRotation.begin()) {
      rot = keyframe1->second;
    }
    else if (keyframe1 == ch.keyframeRotation.end()) {
      rot = keyframe0->second;
    }
    else {
      const double t = (ticks - keyframe0->first) / (keyframe1->first - keyframe0->first);
      rot = keyframe0->second.slerp(keyframe1->second, t);
    }
  }

  Vector3d scale(1.0);
  if (!ch.keyframeScale.empty()) { // Scale
    const auto keyframe1 = ch.keyframeScale.upper_bound(ticks);
    const auto keyframe0 = std::prev(keyframe1);

    if (keyframe1 == ch.keyframeScale.begin()) {
      scale = keyframe1->second;
    }
    else if (keyframe1 == ch.keyframeScale.end()) {
      scale = keyframe0->second;
    }
    else {
      const double t = (ticks - keyframe0->first) / (keyframe1->first - keyframe0->first);
      scale = keyframe0->second.lerp(keyframe1->second, t);
    }
  }

  transform = Matrix44d::compose(scale, rot.toMatrix(), trans);
  return true;
}

}

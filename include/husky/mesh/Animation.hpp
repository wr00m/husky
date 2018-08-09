#pragma once

#include <husky/math/Matrix44.hpp>
#include <husky/math/Quaternion.hpp>
#include <map>
#include <vector>

namespace husky {

class ModelNode;

class HUSKY_DLL AnimationChannel
{
public:
  AnimationChannel(const std::string &nodeName);

  std::string nodeName;
  std::map<double, Vector3d> keyframePosition;
  std::map<double, Quaterniond> keyframeRotation;
  std::map<double, Vector3d> keyframeScale;
};

class HUSKY_DLL AnimatedNode
{
public:
  std::string name;
  Matrix44d mtxRelToParent;
  Matrix44d mtxRelToModel;
};

class HUSKY_DLL Animation
{
public:
  Animation(const std::string &name, double durationTicks, double ticksPerSecond);

  double getTicks(double seconds) const;
  bool getAnimatedNodeTransform(const std::string &nodeName, double ticks, Matrix44d &transform) const;
  //Matrix44d getBoneTransform(const Bone &bone, double time) const;
  void getAnimatedNodesRecursive(const ModelNode *node, double ticks, std::vector<AnimatedNode> &animatedNodes, const AnimatedNode *parent) const;

  std::string name;
  double durationTicks;
  double ticksPerSecond;
  std::map<std::string, AnimationChannel> channels; // Node name is map key
};

}

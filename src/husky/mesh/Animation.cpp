#include <husky/mesh/Animation.hpp>

namespace husky {

AnimationChannel::AnimationChannel(const std::string &nodeName)
  : nodeName(nodeName)
{
}

Animation::Animation(const std::string &name, double durationTicks, double ticksPerSecond)
  : name(name), durationTicks(durationTicks), ticksPerSecond(ticksPerSecond)
{
}

}

#include <sstream>
#include <iostream>
#include <iomanip>
#include <husky/Log.hpp>
#include <husky/math/Math.hpp>
#include <husky/math/EulerAngles.hpp>
#include <husky/util/StringUtil.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/vector_angle.hpp>

static double matDiff(const husky::Matrix44d &a, const glm::dmat4x4 &g)
{
  std::ostringstream ossA, ossG;
  ossA.precision(2);
  ossG.precision(2);

  double diffSumSq = 0.0;
  for (int row = 0; row < 4; row++) {
    for (int col = 0; col < 4; col++) {
      ossA << std::fixed << std::setw(5) << a[col][row] << (col == 3 ? "\n" : " ");
      ossG << std::fixed << std::setw(5) << g[col][row] << (col == 3 ? "\n" : " ");
      double elementDiff = a[col][row] - g[col][row];
      diffSumSq += elementDiff * elementDiff;
    }
  }

  //std::cout << ossA.str() << std::endl << ossG.str() << std::endl << std::endl;

  return diffSumSq;
}

static double quatDiff(const husky::Quaterniond &a, const glm::dquat &g)
{
  std::ostringstream ossA, ossG;
  ossA.precision(2);
  ossG.precision(2);

  ossA << std::fixed << std::setw(5) << a.x << " " << a.y << " " << a.z << " " << a.w << " " << std::endl;
  ossG << std::fixed << std::setw(5) << g.x << " " << g.y << " " << g.z << " " << g.w << " " << std::endl;
  husky::Vector4d diff(a.x - g.x, a.y - g.y, a.z - g.z, a.w - g.w);
  double diffSumSq = diff.dot(diff);

  //std::cout << ossA.str() << std::endl << ossG.str() << std::endl << std::endl;

  return diffSumSq;
}

static void runUnitTests() // TODO: Remove GLM; use explicit expected matrices
{
  husky::Matrix44d lookAt      = husky::Matrix44d::lookAt({ 2.0, 8.0, 4.0 }, { -5.0, 6.0, 7.0 }, { 0.0, 1.0, 1.0 });
  glm::dmat4x4     lookAtGlm   = glm::lookAt(glm::dvec3   { 2.0, 8.0, 4.0 }, { -5.0, 6.0, 7.0 }, { 0.0, 1.0, 1.0 });
  double           lookAtDiff  = matDiff(lookAt, lookAtGlm);
  assert(lookAtDiff < 1e-9);

  husky::Matrix44d ortho       = husky::Matrix44d::ortho(0.0, 1.0, 2.0, 3.0, 4.0, 5.0);
  glm::dmat4x4     orthoGlm    =              glm::ortho(0.0, 1.0, 2.0, 3.0, 4.0, 5.0);
  double           orthoDiff   = matDiff(ortho, orthoGlm);
  assert(orthoDiff < 1e-9);

  husky::Matrix44d frustum     = husky::Matrix44d::frustum(0.0, 1.0, 2.0, 3.0, 4.0, 5.0);
  glm::dmat4x4     frustumGlm  =              glm::frustum(0.0, 1.0, 2.0, 3.0, 4.0, 5.0);
  double           frustumDiff = matDiff(frustum, frustumGlm);
  assert(frustumDiff < 1e-9);

  husky::Matrix44d persp       = husky::Matrix44d::perspective(husky::Math::deg2rad * 60.0, 1.5, 1.0, 100.0);
  glm::dmat4x4     perspGlm    = glm::perspective(husky::Math::deg2rad * 60.0, 1.5, 1.0, 100.0);
  double           perspDiff   = matDiff(persp, perspGlm);
  assert(perspDiff < 1e-9);

  husky::Matrix44d rot         = husky::Matrix44d::rotate(33.0, { 12.0, 13.0, 14.0 });
  glm::dmat4x4     rotGlm      = glm::rotate(33.0, glm::dvec3{ 12.0, 13.0, 14.0 });
  double           rotDiff     = matDiff(rot, rotGlm);
  assert(rotDiff < 1e-9);

  husky::Matrix44d mul = lookAt * persp;
  glm::dmat4x4 mulGlm = lookAtGlm * perspGlm;
  double mulDiff = matDiff(mul, mulGlm);
  assert(mulDiff < 1e-9);

  husky::Quaterniond quatAxisAngle = husky::Quaterniond::fromAxisAngle(33.0, husky::Vector3d(12.0, 13.0, 14.0).normalized());
  glm::dquat quatAxisAngleGlm = glm::angleAxis(33.0, glm::normalize(glm::dvec3{ 12.0, 13.0, 14.0 }));
  double quatAxisAngleDiff = quatDiff(quatAxisAngle, quatAxisAngleGlm);
  assert(quatAxisAngleDiff < 1e-9);

  husky::Matrix44d matFromQuat = quatAxisAngle.toMatrix();
  glm::dmat4x4 matFromQuatGlm(quatAxisAngleGlm);
  double matFromQuatDiff = matDiff(matFromQuat, matFromQuatGlm);
  assert(matFromQuatDiff < 1e-9);

  husky::Quaterniond quatDirs = husky::Quaterniond::fromDirections(husky::Vector3d(1, -2, 3).normalized(), husky::Vector3d(-4, 5, -6).normalized());
  glm::dquat quatDirsGlm = glm::rotation(glm::normalize(glm::dvec3(1, -2, 3)), glm::normalize(glm::dvec3(-4, 5, -6)));
  double quatDirsDiff = quatDiff(quatDirs, quatDirsGlm);
  assert(quatDirsDiff < 1e-9);

  husky::Matrix44d perspInf = husky::Matrix44d::perspectiveInf(husky::Math::pi / 3.0, 1.25, 0.1);
  glm::dmat4x4 perspInfGlm = glm::infinitePerspective(husky::Math::pi / 3.0, 1.25, 0.1);
  double perspInfDiff = matDiff(perspInf, perspInfGlm);
  assert(perspInfDiff < 1e-9);

  husky::Matrix44d perspInfTweak = husky::Matrix44d::perspectiveInf(husky::Math::pi / 3.0, 1.25, 0.1, 1e-4);
  glm::dmat4x4 perspInfTweakGlm = glm::tweakedInfinitePerspective(husky::Math::pi / 3.0, 1.25, 0.1, 1e-4);
  double perspInfTweakDiff = matDiff(perspInfTweak, perspInfTweakGlm);
  assert(perspInfTweakDiff < 1e-9);

  //husky::Matrix44d perspInfRevZ = husky::Matrix44d::perspectiveInfRevZ(husky::Math::pi / 3.0, 1.25, 0.1);

  double vec3Angle = husky::Vector3d(1, 0, 0).angleAbs({ 0, 1, 0 });
  double vec3AngleGlm = glm::angle(glm::dvec3(1, 0, 0), glm::dvec3(0, 1, 0));
  double vec3AngleDiff = std::abs(vec3Angle - vec3AngleGlm);
  assert(vec3AngleDiff < 1e-9);

  double vec2Angle = husky::Vector2d(1, 0).angleSigned({ 0, -1 });
  double vec2AngleGlm = glm::orientedAngle(glm::dvec2(1, 0), glm::dvec2(0, -1));
  double vec2AngleDiff = std::abs(vec2Angle - vec2AngleGlm);
  assert(vec2AngleDiff < 1e-9);

  // TODO: Test Quaternion::fromRotationMatrix()

  husky::Matrix44d eulerAnglesMtx = husky::Matrix44d::rotate(3.0, husky::Vector3d(3, 2, 1).normalized());
  glm::dmat4 eulerAnglesMtxGlm = glm::rotate(3.0, glm::normalize(glm::dvec3(3, 2, 1)));
  double eulerAnglesMtxDiff = matDiff(eulerAnglesMtx, eulerAnglesMtxGlm);
  assert(eulerAnglesMtxDiff < 1e-9);
  husky::Quaterniond eulerAnglesQuat = husky::Quaterniond::fromRotationMatrix(eulerAnglesMtx.get3x3());
  glm::dquat eulerAnglesQuatGlm(eulerAnglesMtxGlm);
  double eulerAnglesQuatDiff = quatDiff(eulerAnglesQuat, eulerAnglesQuatGlm);
  assert(eulerAnglesQuatDiff < 1e-9);
  husky::EulerAnglesd eulerAngles(husky::RotationOrder::YXZ, eulerAnglesMtx);
  double yawGlm, pitchGlm, rollGlm;
  glm::extractEulerAngleYXZ(eulerAnglesMtxGlm, yawGlm, pitchGlm, rollGlm);
  double eulerAngleDiff = std::abs(eulerAngles.yaw - yawGlm) + std::abs(eulerAngles.pitch - pitchGlm) + std::abs(eulerAngles.roll - rollGlm);
  assert(eulerAngleDiff < 1e-9);
  husky::Matrix44d eulerAnglesMtxRev = eulerAngles.toMatrix();
  glm::dmat4 eulerAnglesMtxRevGlm = glm::eulerAngleYXZ(yawGlm, pitchGlm, rollGlm);
  double eulerAnglesMtxRevDiff = matDiff(eulerAnglesMtxRev, eulerAnglesMtxRevGlm);
  assert(eulerAnglesMtxRevDiff < 1e-9);

  assert(husky::StringUtil::ltrim("abcd", "ad") == "bcd");
  assert(husky::StringUtil::rtrim("abcd", "ad") == "abc");
  assert(husky::StringUtil::trim("abcd", "ad") == "bc");
  assert(husky::StringUtil::trim("abcd", "efgh") == "abcd");
  assert(husky::StringUtil::toLower("abc") == "abc");
  assert(husky::StringUtil::toLower("ABC") == "abc");
  assert(husky::StringUtil::toLower("Abc") == "abc");
  assert(husky::StringUtil::toUpper("abc") == "ABC");
  assert(husky::StringUtil::toUpper("ABC") == "ABC");
  assert(husky::StringUtil::toUpper("Abc") == "ABC");
  assert(husky::StringUtil::startsWith("abcd", "abc"));
  assert(husky::StringUtil::startsWith("abc", "abc"));
  assert(husky::StringUtil::startsWith("abc", "abcd") == false);
  assert(husky::StringUtil::endsWith("abcd", "bcd"));
  assert(husky::StringUtil::endsWith("bcd", "bcd"));
  assert(husky::StringUtil::endsWith("bcd", "abcd") == false);
}

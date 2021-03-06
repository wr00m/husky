#include <sstream>
#include <iostream>
#include <iomanip>
#include <husky/Log.hpp>
#include <husky/geo/CoordSys.hpp>
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
  husky::Matrix44d lookAtInv;
  husky::Matrix44d lookAt      = husky::Matrix44d::lookAt({ 2.0, 8.0, 4.0 }, { -5.0, 6.0, 7.0 }, { 0.0, 1.0, 1.0 }, &lookAtInv);
  glm::dmat4x4     lookAtGlm   = glm::lookAt(glm::dvec3   { 2.0, 8.0, 4.0 }, { -5.0, 6.0, 7.0 }, { 0.0, 1.0, 1.0 });
  double           lookAtDiff  = matDiff(lookAt, lookAtGlm);
  assert(lookAtDiff < 1e-9);
  double           lookAtInvDiff = matDiff(lookAtInv, glm::make_mat4(lookAt.inverted().m));
  assert(lookAtInvDiff < 1e-14);

  husky::Matrix44d orthoInv;
  husky::Matrix44d ortho       = husky::Matrix44d::ortho(0.0, 1.0, 2.0, 3.0, 4.0, 5.0, &orthoInv);
  glm::dmat4x4     orthoGlm    =              glm::ortho(0.0, 1.0, 2.0, 3.0, 4.0, 5.0);
  double           orthoDiff   = matDiff(ortho, orthoGlm);
  assert(orthoDiff < 1e-9);
  double           orthoInvDiff = matDiff(orthoInv, glm::make_mat4(ortho.inverted().m));
  assert(orthoInvDiff < 1e-14);

  husky::Matrix44d frustumInv;
  husky::Matrix44d frustum     = husky::Matrix44d::frustum(0.0, 1.0, 2.0, 3.0, 4.0, 5.0, &frustumInv);
  glm::dmat4x4     frustumGlm  =              glm::frustum(0.0, 1.0, 2.0, 3.0, 4.0, 5.0);
  double           frustumDiff = matDiff(frustum, frustumGlm);
  assert(frustumDiff < 1e-9);
  double           frustumInvDiff = matDiff(frustumInv, glm::make_mat4(frustum.inverted().m));
  assert(frustumInvDiff < 1e-14);

  husky::Matrix44d perspInv;
  husky::Matrix44d persp       = husky::Matrix44d::perspective(husky::Math::deg2rad * 60.0, 1.5, 1.0, 100.0, &perspInv);
  glm::dmat4x4     perspGlm    = glm::perspective(husky::Math::deg2rad * 60.0, 1.5, 1.0, 100.0);
  double           perspDiff   = matDiff(persp, perspGlm);
  assert(perspDiff < 1e-9);
  double           perspInvDiff = matDiff(perspInv, glm::make_mat4(persp.inverted().m));
  assert(perspInvDiff < 1e-14);

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

  husky::Matrix44d perspInfInv;
  husky::Matrix44d perspInf = husky::Matrix44d::perspectiveInf(husky::Math::pi / 3.0, 1.25, 0.1, std::numeric_limits<double>::epsilon(), &perspInfInv);
  glm::dmat4x4 perspInfGlm = glm::tweakedInfinitePerspective(husky::Math::pi / 3.0, 1.25, 0.1);
  double perspInfDiff = matDiff(perspInf, perspInfGlm);
  assert(perspInfDiff < 1e-9);
  double perspInfInvDiff = matDiff(perspInfInv, glm::make_mat4(perspInf.inverted().m));
  assert(perspInfInvDiff < 1e-14);

  husky::Matrix44d perspInfTweak = husky::Matrix44d::perspectiveInf(husky::Math::pi / 3.0, 1.25, 0.1, 1e-4);
  glm::dmat4x4 perspInfTweakGlm = glm::tweakedInfinitePerspective(husky::Math::pi / 3.0, 1.25, 0.1, 1e-4);
  double perspInfTweakDiff = matDiff(perspInfTweak, perspInfTweakGlm);
  assert(perspInfTweakDiff < 1e-9);

  husky::Matrix44d perspInfRevZInv1;
  husky::Matrix44d perspInfRevZ = husky::Matrix44d::perspectiveInfRevZ(husky::Math::pi / 3.0, 1.25, 0.1, &perspInfRevZInv1); // TODO: Verify returned matrix
  husky::Matrix44d perspInfRevZInv2 = perspInfRevZ.inverted();
  double perspInfRevZInvDiff = matDiff(perspInfRevZInv1, glm::make_mat4(perspInfRevZInv2.m));
  assert(perspInfRevZInvDiff < 1e-14);

  double vec3AngleAbs = husky::Vector3d(1, 0, 0).angleAbs({ 0, 1, 0 });
  double vec3AngleAbsGlm = glm::angle(glm::dvec3(1, 0, 0), glm::dvec3(0, 1, 0));
  double vec3AngleAbsDiff = std::abs(vec3AngleAbs - vec3AngleAbsGlm);
  assert(vec3AngleAbsDiff < 1e-9);

  double vec3AngleSigned = husky::Vector3d(3, 2, -1).angleSigned({ 0, -7, 0 }, { 0, 0, 1 });
  double vec3AngleSignedGlm = glm::orientedAngle(glm::normalize(glm::dvec3(3, 2, -1)), glm::normalize(glm::dvec3(0, -7, 0)), glm::dvec3(0, 0, 1));
  double vec3AngleSignedDiff = std::abs(vec3AngleSigned - vec3AngleSignedGlm);
  assert(vec3AngleSignedDiff < 1e-9);

  double vec2AngleSigned = husky::Vector2d(1, 0).angleSigned({ 0, -1 });
  double vec2AngleSignedGlm = glm::orientedAngle(glm::dvec2(1, 0), glm::dvec2(0, -1));
  double vec2AngleSignedDiff = std::abs(vec2AngleSigned - vec2AngleSignedGlm);
  assert(vec2AngleSignedDiff < 1e-9);

  // TODO: Test Quaternion::fromRotationMatrix()

  husky::Matrix44d compInv;
  husky::Matrix44d comp = husky::Matrix44d::compose({ 2, 3, 4 }, husky::Matrix33d::rotate(husky::Math::pi, husky::Vector3d(1, 1, 1).normalized()), { -10, 20, -30 }, &compInv);
  double compInvDiff = matDiff(compInv, glm::make_mat4(comp.inverted().m));
  assert(compInvDiff < 1e-9);
  double compInvDiff2 = matDiff(comp * compInv, glm::dmat4(1)); // Identity matrix
  assert(compInvDiff2 < 1e-9);

  husky::Matrix33d inv3x3 = comp.get3x3();
  husky::Matrix33d inv3x3Inv = inv3x3.inverted();
  glm::dmat3 inv3x3InvGlm = glm::inverse(glm::make_mat3(inv3x3.m));
  double inv3x3Diff = matDiff(husky::Matrix44d(inv3x3Inv), glm::dmat4(inv3x3InvGlm));
  assert(inv3x3Diff < 1e-9);

  // TODO: Test Transform class (primarily ctors)

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

  husky::CoordSys csUtm33N(32633);
  husky::CoordSys csWgs(4326);
  husky::CoordSys csWgsWkt(
R"(GEOGCS["WGS 84",
    DATUM["WGS_1984",
        SPHEROID["WGS 84",6378137,298.257223563,
            AUTHORITY["EPSG","7030"]],
        AUTHORITY["EPSG","6326"]],
    PRIMEM["Greenwich",0,
        AUTHORITY["EPSG","8901"]],
    UNIT["degree",0.01745329251994328,
        AUTHORITY["EPSG","9122"]],
    AUTHORITY["EPSG","4326"]])");

  assert(csWgs.sameAs(csWgsWkt));

  const husky::Vector3d pt1(15.0, 55.0, 0.0);
  husky::Vector3d pt2 = pt1;
  husky::CoordTransformation ct1(csWgs, csUtm33N);
  husky::CoordTransformation ct2(csUtm33N, csWgs);
  assert(ct1.convert(pt2));
  assert(ct2.convert(pt2));
  assert((pt1 - pt2).length2() < 1e-9);
}

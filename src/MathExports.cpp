#include "stdafx.h"
#include "MathExports.h"
#include "OgreMath.h"
#include "OgreVector2.h"
#include "OgreVector3.h"
#include "OgreVector4.h"
#include "OgreMatrix3.h"
#include "OgreMatrix4.h"

#include "luabind/luabind.hpp"
#include "luabind/operator.hpp"
#include "luabind/wrapper_base.hpp"
#include "luabind/class.hpp"
#include "luabind/adopt_policy.hpp"

namespace scripting
{
	void MathExports::extendState(lua_State* pState)
	{
		luabind::module(pState, "Math")
				[
					luabind::class_<Ogre::Angle>("Angle")
					.def(luabind::constructor<Ogre::Real>()),

					luabind::class_<Ogre::Degree>("Degree")
					.def(luabind::constructor<Ogre::Real>())
					.def(luabind::constructor<Ogre::Radian>())
					.def(luabind::constructor<Ogre::Degree>())
					.def( "valueDegrees", &Ogre::Degree::valueDegrees )
					.def( "valueRadians", &Ogre::Degree::valueRadians )
					.def( "valueAngleUnits", &Ogre::Degree::valueAngleUnits )				
					.def(luabind::self + luabind::other<Ogre::Radian>())
					.def(luabind::self + luabind::other<Ogre::Degree>())
					.def(luabind::self - luabind::other<Ogre::Radian>())
					.def(luabind::self - luabind::other<Ogre::Degree>())
					.def(luabind::self * luabind::other<Ogre::Degree>())
					.def(luabind::self * luabind::other<Ogre::Real>())
					.def(luabind::self / luabind::other<Ogre::Real>())
					.def(luabind::self < luabind::other<Ogre::Degree>())
					.def(luabind::self <= luabind::other<Ogre::Degree>())
					.def(luabind::self == luabind::other<Ogre::Degree>()),

					luabind::class_<Ogre::Radian>("Radian")
					.def(luabind::constructor<Ogre::Real>())
					.def(luabind::constructor<Ogre::Radian>())
					.def(luabind::constructor<Ogre::Degree>())
					.def( "valueDegrees", &Ogre::Radian::valueDegrees )
					.def( "valueRadians", &Ogre::Radian::valueRadians )
					.def( "valueAngleUnits", &Ogre::Radian::valueAngleUnits )
					.def(luabind::self + luabind::other<Ogre::Radian>())
					.def(luabind::self + luabind::other<Ogre::Degree>())
					.def(luabind::self - luabind::other<Ogre::Radian>())
					.def(luabind::self - luabind::other<Ogre::Degree>())
					.def(luabind::self * luabind::other<Ogre::Radian>())
					.def(luabind::self * luabind::other<Ogre::Real>())
					.def(luabind::self / luabind::other<Ogre::Real>())
					.def(luabind::self < luabind::other<Ogre::Radian>())
					.def(luabind::self <= luabind::other<Ogre::Radian>())
					.def(luabind::self == luabind::other<Ogre::Radian>()),

					luabind::class_<Ogre::Vector2>("Vector2")
					.def(luabind::constructor<>())
					.def(luabind::constructor<float, float>())
					.def(luabind::constructor<Ogre::Vector2>())
					.def( "length", &Ogre::Vector2::length )
					.def( "squaredLength", &Ogre::Vector2::squaredLength )
					.def( "dotProduct", &Ogre::Vector2::dotProduct )
					.def( "normalise", &Ogre::Vector2::normalise )
					.def( "midPoint", &Ogre::Vector2::midPoint )
					.def( "makeFloor", &Ogre::Vector2::makeFloor )
					.def( "makeCeil", &Ogre::Vector2::makeCeil )
					.def( "perpendicular", &Ogre::Vector2::perpendicular )
					.def( "crossProduct", &Ogre::Vector2::crossProduct )
					.def( "randomDeviant", &Ogre::Vector2::randomDeviant )
					.def( "isZeroLength", &Ogre::Vector2::isZeroLength )
					.def( "normalisedCopy", &Ogre::Vector2::normalisedCopy )
					.def( "reflect", &Ogre::Vector2::reflect )
					.def(luabind::self + luabind::other<Ogre::Vector2>())
					.def(luabind::self - luabind::other<Ogre::Vector2>())
					.def(luabind::self * luabind::other<Ogre::Vector2>())
					.def(luabind::self / luabind::other<Ogre::Vector2>())
					.def(luabind::self * luabind::other<Ogre::Real>())
					.def(luabind::self + luabind::other<Ogre::Real>())
					.def(luabind::self - luabind::other<Ogre::Real>())
					.def(luabind::self / luabind::other<Ogre::Real>())
					.def(luabind::self == luabind::other<Ogre::Vector2>())
					.def_readwrite( "x", &Ogre::Vector2::x )
					.def_readwrite( "y", &Ogre::Vector2::y ),

					luabind::class_<Ogre::Vector3>("Vector3")
					.def(luabind::constructor<>())
					.def(luabind::constructor<float, float, float>())
					.def(luabind::constructor<Ogre::Vector3>())
					.def( "normalise", &Ogre::Vector3::normalise )
					.def( "length", &Ogre::Vector3::length )
					.def( "squaredLength", &Ogre::Vector3::squaredLength )
					.def( "distance", &Ogre::Vector3::distance )
					.def( "squaredDistance", &Ogre::Vector3::squaredDistance )
					.def( "dotProduct", &Ogre::Vector3::dotProduct )
					.def( "absDotProduct", &Ogre::Vector3::absDotProduct )
					.def( "crossProduct", &Ogre::Vector3::crossProduct )
					.def( "midPoint", &Ogre::Vector3::midPoint )
					.def( "makeFloor", &Ogre::Vector3::makeFloor )
					.def( "makeCeil", &Ogre::Vector3::makeCeil )
					.def( "perpendicular", &Ogre::Vector3::perpendicular )
					.def( "randomDeviant", &Ogre::Vector3::randomDeviant )
					.def( "angleBetween", &Ogre::Vector3::angleBetween )
					.def( "getRotationTo", &Ogre::Vector3::getRotationTo )
					.def( "isZeroLength", &Ogre::Vector3::isZeroLength )
					.def( "normalisedCopy", &Ogre::Vector3::normalisedCopy )
					.def( "reflect", &Ogre::Vector3::reflect )
					.def( "positionEquals", &Ogre::Vector3::positionEquals )
					.def( "positionCloses", &Ogre::Vector3::positionCloses )
					.def( "directionEquals", &Ogre::Vector3::directionEquals )
					.def(luabind::self - luabind::other<Ogre::Vector3>())
					.def(luabind::self + luabind::other<Ogre::Vector3>())
					.def(luabind::self * luabind::other<Ogre::Vector3>())
					.def(luabind::self / luabind::other<Ogre::Vector3>())
					.def(luabind::self * luabind::other<Ogre::Real>())
					.def(luabind::self + luabind::other<Ogre::Real>())
					.def(luabind::self - luabind::other<Ogre::Real>())
					.def(luabind::self / luabind::other<Ogre::Real>())
					.def(luabind::self == luabind::other<Ogre::Vector3>())
					.def_readwrite( "x", &Ogre::Vector3::x )
					.def_readwrite( "y", &Ogre::Vector3::y )
					.def_readwrite( "z", &Ogre::Vector3::z ),

					luabind::class_<Ogre::Vector4>("Vector4")
					.def(luabind::constructor<>())
					.def(luabind::constructor<Ogre::Real const>())
					.def(luabind::constructor<float, float, float, float>())
					.def(luabind::constructor<Ogre::Vector4>())
					.def(luabind::constructor<Ogre::Vector3>())					 
					.def( "dotProduct", &Ogre::Vector4::dotProduct )
					.def( "swap", &Ogre::Vector4::swap )
					.def( "isNaN", &Ogre::Vector4::isNaN )
					.def(luabind::self + luabind::other<Ogre::Vector4>())
					.def(luabind::self - luabind::other<Ogre::Vector4>())
					.def(luabind::self * luabind::other<Ogre::Vector4>())
					.def(luabind::self / luabind::other<Ogre::Vector4>())
					.def(luabind::self * luabind::other<Ogre::Real>())
					.def(luabind::self + luabind::other<Ogre::Real>())
					.def(luabind::self - luabind::other<Ogre::Real>())
					.def(luabind::self / luabind::other<Ogre::Real>())
					.def(luabind::self == luabind::other<Ogre::Vector4>())
					.def_readwrite( "x", &Ogre::Vector4::x )
					.def_readwrite( "y", &Ogre::Vector4::y )
					.def_readwrite( "z", &Ogre::Vector4::z )
					.def_readwrite( "w", &Ogre::Vector4::w ),

					luabind::class_<Ogre::Matrix3>("Matrix3")
					.def(luabind::constructor<>())
					.def(luabind::constructor<Ogre::Real, Ogre::Real, Ogre::Real, Ogre::Real, Ogre::Real, Ogre::Real, Ogre::Real, Ogre::Real, Ogre::Real>())
					.def( "GetColumn", &Ogre::Matrix3::GetColumn )
					.def( "SetColumn", &Ogre::Matrix3::SetColumn )
					.def( "FromAxes", &Ogre::Matrix3::FromAxes )
					.def( "Transpose", &Ogre::Matrix3::Transpose )
					.def( "InverseByMatrix", (bool(Ogre::Matrix3::*)(Ogre::Matrix3&, Ogre::Real)const) &Ogre::Matrix3::Inverse )
					.def( "Inverse", (Ogre::Matrix3(Ogre::Matrix3::*)(Ogre::Real)const) &Ogre::Matrix3::Inverse )
					.def( "Determinant", &Ogre::Matrix3::Determinant )
					.def( "SingularValueDecomposition", &Ogre::Matrix3::SingularValueDecomposition )
					.def( "SingularValueComposition", &Ogre::Matrix3::SingularValueComposition )
					.def( "Orthonormalize", &Ogre::Matrix3::Orthonormalize )
					.def( "QDUDecomposition", &Ogre::Matrix3::QDUDecomposition )
					.def( "SpectralNorm", &Ogre::Matrix3::SpectralNorm )			
					.def( "ToAxisAngleFromRadian", (void (Ogre::Matrix3::*)(Ogre::Vector3& , Ogre::Radian&) const)&Ogre::Matrix3::ToAngleAxis )
					.def( "ToAxisAngleFromDegree", (void (Ogre::Matrix3::*)(Ogre::Vector3& , Ogre::Degree&) const)&Ogre::Matrix3::ToAngleAxis )
					.def( "FromAxisAngle", &Ogre::Matrix3::FromAngleAxis )
					.def( "ToEulerAnglesXYZ", &Ogre::Matrix3::ToEulerAnglesXYZ )
					.def( "ToEulerAnglesXZY", &Ogre::Matrix3::ToEulerAnglesXZY )
					.def( "ToEulerAnglesYXZ", &Ogre::Matrix3::ToEulerAnglesYXZ )
					.def( "ToEulerAnglesYZX", &Ogre::Matrix3::ToEulerAnglesYZX )
					.def( "ToEulerAnglesZXY", &Ogre::Matrix3::ToEulerAnglesZXY )
					.def( "ToEulerAnglesZYX", &Ogre::Matrix3::ToEulerAnglesZYX )
					.def( "FromEulerAnglesXYZ", &Ogre::Matrix3::FromEulerAnglesXYZ )
					.def( "FromEulerAnglesXZY", &Ogre::Matrix3::FromEulerAnglesXZY )
					.def( "FromEulerAnglesYXZ", &Ogre::Matrix3::FromEulerAnglesYXZ )
					.def( "FromEulerAnglesYZX", &Ogre::Matrix3::FromEulerAnglesYZX )
					.def( "FromEulerAnglesZXY", &Ogre::Matrix3::FromEulerAnglesZXY )
					.def( "FromEulerAnglesZYX", &Ogre::Matrix3::FromEulerAnglesZYX )
					.def( "EigenSolveSymmetric", &Ogre::Matrix3::EigenSolveSymmetric )
					.def( "TensorProduct", &Ogre::Matrix3::TensorProduct )
					.def( "hasScale", &Ogre::Matrix3::hasScale )				
					.def(luabind::self + luabind::other<Ogre::Matrix3>())
					.def(luabind::self - luabind::other<Ogre::Matrix3>())
					.def(luabind::self * luabind::other<Ogre::Matrix3>())
					.def(luabind::self * luabind::other<Ogre::Vector3>())
					.def(luabind::self * luabind::other<Ogre::Real>())
					.def(luabind::self == luabind::other<Ogre::Matrix3>()),

					luabind::class_<Ogre::Quaternion>("Quaternion")
					.def(luabind::constructor<Ogre::Matrix3&>())
					.def(luabind::constructor<Ogre::Real, Ogre::Real, Ogre::Real, Ogre::Real>())
					.def(luabind::constructor<Ogre::Radian&, Ogre::Vector3&>())
					.def(luabind::constructor<Ogre::Vector3&, Ogre::Vector3&, Ogre::Vector3&>())
					.def(luabind::constructor<Ogre::Vector3*>())
					.def(luabind::constructor<Ogre::Real*>())
					.def( "FromRotationMatrix", &Ogre::Quaternion::FromRotationMatrix )
					.def( "ToRotationMatrix", &Ogre::Quaternion::ToRotationMatrix )
					.def( "FromAngleAxis", &Ogre::Quaternion::FromAngleAxis )
					.def( "xAxis", &Ogre::Quaternion::xAxis )
					.def( "yAxis", &Ogre::Quaternion::yAxis )
					.def( "zAxis", &Ogre::Quaternion::zAxis )
					.def( "Dot", &Ogre::Quaternion::Dot )
					.def( "Norm", &Ogre::Quaternion::Norm )
					.def( "normalise", &Ogre::Quaternion::normalise )
					.def( "Inverse", &Ogre::Quaternion::Inverse )
					.def( "UnitInverse", &Ogre::Quaternion::UnitInverse )
					.def( "getRoll", &Ogre::Quaternion::getRoll )
					.def( "getPitch", &Ogre::Quaternion::getPitch )
					.def( "getYaw", &Ogre::Quaternion::getYaw )
					.def( "ToAngleAxisFromRadian", (void(Ogre::Quaternion::*)(Ogre::Radian&,Ogre::Vector3&)const)&Ogre::Quaternion::ToAngleAxis )
					.def( "ToAngleAxisFromDegree", (void(Ogre::Quaternion::*)(Ogre::Degree&,Ogre::Vector3&)const)&Ogre::Quaternion::ToAngleAxis )
					.def(luabind::self + luabind::other<Ogre::Quaternion>())
					.def(luabind::self - luabind::other<Ogre::Quaternion>())
					.def(luabind::self * luabind::other<Ogre::Quaternion>())
					.def(luabind::self * luabind::other<Ogre::Real>())
					.def(luabind::self == luabind::other<Ogre::Quaternion>())
					.def(luabind::self * luabind::other<Ogre::Vector3>())
					.def_readwrite( "x", &Ogre::Quaternion::x )
					.def_readwrite( "y", &Ogre::Quaternion::y )
					.def_readwrite( "z", &Ogre::Quaternion::z )
					.def_readwrite( "w", &Ogre::Quaternion::w )
					.scope
					[ luabind::def("slerp", &Ogre::Quaternion::Slerp) ]	
					.scope
					[ luabind::def("nlerp", &Ogre::Quaternion::nlerp) ]	
					.scope
					[ luabind::def("slerpExtraSpins", &Ogre::Quaternion::SlerpExtraSpins) ]	
					.scope
					[ luabind::def("squad", &Ogre::Quaternion::Squad) ],
						
					luabind::class_<Ogre::Matrix4>("Matrix4")
					.def(luabind::constructor<>())
					.def(luabind::constructor<Ogre::Matrix3&>())
					.def(luabind::constructor<Ogre::Quaternion&>())
					.def( "concatenate", &Ogre::Matrix4::concatenate )
					.def( "transpose", &Ogre::Matrix4::transpose )
					.def( "setTrans", &Ogre::Matrix4::setTrans )
					.def( "extract3x3Matrix", &Ogre::Matrix4::extract3x3Matrix )
					.def( "hasScale", &Ogre::Matrix4::hasScale )
					.def( "extractQuaternion", &Ogre::Matrix4::extractQuaternion )
					.def( "adjoint", &Ogre::Matrix4::adjoint )
					.def( "determinant", &Ogre::Matrix4::determinant )
					.def( "inverse", &Ogre::Matrix4::inverse )
					.def( "makeInverseTransform", &Ogre::Matrix4::makeInverseTransform )
					.def( "makeTransform", &Ogre::Matrix4::makeTransform )
					.def( "isAffine", &Ogre::Matrix4::isAffine )
					.def( "inverseAffine", &Ogre::Matrix4::inverseAffine )
					.def( "concatenateAffine", &Ogre::Matrix4::concatenateAffine )
					.def( "decomposition", &Ogre::Matrix4::decomposition )
					.def( "hasNegativeScale", &Ogre::Matrix4::hasNegativeScale )
					.def(luabind::self * luabind::other<Ogre::Matrix4>())
					.def(luabind::self * luabind::other<Ogre::Vector3>())
					.def(luabind::self * luabind::other<Ogre::Vector4>())
					.def(luabind::self * luabind::other<Ogre::Real>())
					.def(luabind::self + luabind::other<Ogre::Matrix4>())
					.def(luabind::self - luabind::other<Ogre::Matrix4>())
					.def(luabind::self == luabind::other<Ogre::Matrix4>())
				]; 
	}
}
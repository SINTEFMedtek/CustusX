/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTRANSFORM3D_H_
#define CXTRANSFORM3D_H_

#include "cxResourceExport.h"
#include "cxPrecompiledHeader.h"

#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>
#include <QString>
#include "cxVector3D.h"
#include "cxDefinitions.h"


/** implementation functions used the Eigen extensions.
 *
 */
namespace cx_transform3D_internal
{
/** provide an array of the transform indices, vtk / row-major ordering
 */
cxResource_EXPORT boost::array<double, 16> flatten(const Eigen::Affine3d* self);

cxResource_EXPORT void fill(Eigen::Affine3d* self, vtkMatrix4x4Ptr m);
cxResource_EXPORT void fill(Eigen::Affine3d* self, float m[4][4]);
cxResource_EXPORT void fill(Eigen::Affine3d* self, const double* raw);
cxResource_EXPORT vtkMatrix4x4Ptr getVtkMatrix(const Eigen::Affine3d* self);
cxResource_EXPORT std::ostream& put(const Eigen::Affine3d* self, std::ostream& s, int indent, char newline);
cxResource_EXPORT Eigen::Affine3d fromString(const QString& text, bool* _ok);
cxResource_EXPORT vtkTransformPtr getVtkTransform(const Eigen::Affine3d* self);
}

//! @cond Doxygen_Suppress
namespace Eigen
{

template<typename _Scalar, int _Dim, int _Mode, int _Options>
std::ostream& operator<<(std::ostream& s, const Eigen::Transform<_Scalar, _Dim, _Mode, _Options>& t)
{
	s << t.matrix().format(IOFormat()); // hack: force OK output even when the default in sscMathBase.h is Veector3D-centered.
	//  t.put(s);
	return s;
}

template<typename _Scalar, int _Dim, int _Mode, int _Options>
Vector3d Transform<_Scalar, _Dim, _Mode, _Options>::vector(const Vector3d& v) const ///< transform a free vector [x,y,z,0]
{
	return this->linear() * v;
}

template<typename _Scalar, int _Dim, int _Mode, int _Options>
Vector3d Transform<_Scalar, _Dim, _Mode, _Options>::unitVector(const Vector3d& v) const
{
	return (this->linear() * v).normal();
}

template<typename _Scalar, int _Dim, int _Mode, int _Options>
Vector3d Transform<_Scalar, _Dim, _Mode, _Options>::coord(const Vector3d& v) const
{
	return (*this) * v;
}

template<typename _Scalar, int _Dim, int _Mode, int _Options>
Transform<_Scalar, _Dim, _Mode, _Options> Transform<_Scalar, _Dim, _Mode, _Options>::inv() const
{
	return this->inverse();
}

template<typename _Scalar, int _Dim, int _Mode, int _Options>
boost::array<double, 16> Transform<_Scalar, _Dim, _Mode, _Options>::flatten() const
{
	return cx_transform3D_internal::flatten(this);
}

template<typename _Scalar, int _Dim, int _Mode, int _Options>
Transform<_Scalar, _Dim, _Mode, _Options>::Transform(vtkMatrix4x4* m)
{
	cx_transform3D_internal::fill(this, m);
}

template<typename _Scalar, int _Dim, int _Mode, int _Options>
Transform<_Scalar, _Dim, _Mode, _Options>::Transform(double* raw)
{
	cx_transform3D_internal::fill(this, raw);
}

template<typename _Scalar, int _Dim, int _Mode, int _Options>
vtkMatrix4x4Ptr Transform<_Scalar, _Dim, _Mode, _Options>::getVtkMatrix() const
{
	return cx_transform3D_internal::getVtkMatrix(this);
}

template<typename _Scalar, int _Dim, int _Mode, int _Options>
vtkTransformPtr Transform<_Scalar, _Dim, _Mode, _Options>::getVtkTransform() const
{
	return cx_transform3D_internal::getVtkTransform(this);
}

template<typename _Scalar, int _Dim, int _Mode, int _Options>
std::ostream& Transform<_Scalar, _Dim, _Mode, _Options>::put(std::ostream& s, int indent, char newline) const
{
	return cx_transform3D_internal::put(this, s, indent, newline);
}

template<typename _Scalar, int _Dim, int _Mode, int _Options>
Transform<_Scalar, _Dim, _Mode, _Options> Transform<_Scalar, _Dim, _Mode, _Options>::fromString(const QString& text,
	bool* _ok)
{
	return cx_transform3D_internal::fromString(text, _ok);
}

template<typename _Scalar, int _Dim, int _Mode, int _Options>
Transform<_Scalar, _Dim, _Mode, _Options> Transform<_Scalar, _Dim, _Mode, _Options>::fromVtkMatrix(vtkMatrix4x4Ptr m)
{
	Transform<_Scalar, _Dim, _Mode, _Options> retval;
	cx_transform3D_internal::fill(&retval, m);
	return retval;
}

template<typename _Scalar, int _Dim, int _Mode, int _Options>
Transform<_Scalar, _Dim, _Mode, _Options> Transform<_Scalar, _Dim, _Mode, _Options>::fromFloatArray(float m[4][4])
{
    Transform<_Scalar, _Dim, _Mode, _Options> retval;
    cx_transform3D_internal::fill(&retval, m);
    return retval;
}


} // namespace Eigen
//! @endcond

// --------------------------------------------------------
namespace cx
{
class DoubleBoundingBox3D;

/**
 * \addtogroup cx_resource_core_math
 * @{
 */

/**\brief Transform3D is a representation of an affine 3D transform.
 *
 * Transform3D is implemented using the Eigen library type Eigen::Affine3d,
 * and extended with convenience methods using the Eigen plugin system.
 *
 * Some of the extension exist for backward compatibility with
 * the old inhouse Transform3D.
 *
 */
typedef Eigen::Affine3d Transform3D;

cxResource_EXPORT bool similar(const Transform3D& a, const Transform3D& b, double tol = 1.0E-4);

/** Transform bb using the transform m.
 * Only the two defining corners are actually transformed.
 */
cxResource_EXPORT DoubleBoundingBox3D transform(const Transform3D& m, const DoubleBoundingBox3D& bb);

/** Normalize volume defined by in to volume defined by out.
 *
 * This is intended for creating transforms from one viewport to another, i.e.
 * the two boxes should be aligned and differ only in translation + scaling.
 */
cxResource_EXPORT Transform3D createTransformNormalize(const DoubleBoundingBox3D& in, const DoubleBoundingBox3D& out);

/**Create a transform representing a scale in x,y,z
 */
cxResource_EXPORT Transform3D createTransformScale(const Vector3D& scale);

/** Create a transform representing a translation
 */
cxResource_EXPORT Transform3D createTransformTranslate(const Vector3D& translation);

/** Create a transform representing a rotation about the X-axis with an input angle.
 */
cxResource_EXPORT Transform3D createTransformRotateX(const double angle);

/** Create a transform representing a rotation about the Y-axis with an input angle.
 */
cxResource_EXPORT Transform3D createTransformRotateY(const double angle);

/** Create a transform representing a rotation about the Z-axis with an input angle.
 */
cxResource_EXPORT Transform3D createTransformRotateZ(const double angle);

/** Create a transform to a space defined by an origin and two perpendicular unit vectors that
 * for the x-y plane.
 * The original space is A and the space defined by ijc are B
 * The returned transform M_AB converts a point in B to A:
 * 		p_A = M_AB * p_B
 */
cxResource_EXPORT Transform3D createTransformIJC(const Vector3D& ivec, const Vector3D& jvec, const Vector3D& center);

cxResource_EXPORT Transform3D createTransformRotationBetweenVectors(Vector3D from, Vector3D to);


/**
  * Convert from left-posterior-superior (LPS) or right-anterior-superior (RAS).
  * LPS is used by DICOM and CustusX,
  * RAS is used by Slicer, ITK-Snap, NifTI,
  */
cxResource_EXPORT Transform3D createTransformLPS2RAS();

/**
  * Create a transform from the internal (LPS) space to a given
  * external (LPS or RAS) space. The return value is on the form
  * sMr, where s=external space, r=internal ref space.
  */
cxResource_EXPORT Transform3D createTransformFromReferenceToExternal(PATIENT_COORDINATE_SYSTEM external);



// --------------------------------------------------------
typedef boost::shared_ptr<Transform3D> Transform3DPtr;

/**
 * Utility function used to convert Transform3D to a single line string.
 *
 * Functions like getDisplayFriendlyInfo() in MeshHelpers and VolumeHelpers
 * don't display the whole matrix if it contain newlines.
 */
cxResource_EXPORT std::string matrixAsSingleLineString(Transform3D transform);

/**
 * @}
 */

} // namespace cx
// --------------------------------------------------------

#endif /*CXTRANSFORM3D_H_*/

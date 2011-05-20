/*
 * sscTransformEigenAddons.h
 *
 * Plugin extension for the eigen::Transform type
 *
 *
 *  Created on: Apr 24, 2011
 *      Author: christiana
 */

Vector3d vector(const Vector3d& v) const;      ///< transform a free vector [x,y,z,0]
Vector3d unitVector(const Vector3d& v) const;  ///< transform a unit vector [x,y,z,0], force |v|=1 after transform.
Vector3d coord(const Vector3d& v) const;       ///< transform a coordinate [x,y,z,1].
Transform inv() const;                       ///< return an inverted transform M^-1
boost::array<double, 16> flatten() const;      ///< return matrix as a flat array, vtk ordering
explicit Transform(vtkMatrix4x4* m);
explicit Transform(double* m);
vtkMatrix4x4Ptr getVtkMatrix() const;
std::ostream& put(std::ostream& s, int indent=0, char newline='\n') const;
static Transform fromString(const QString& text, bool* ok=0); ///< construct a transform matrix from a string containing 16 whitespace-separated numbers, vtk ordering
static Transform fromVtkMatrix(vtkMatrix4x4Ptr m);


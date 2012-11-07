// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

#ifndef SSCMESH_
#define SSCMESH_

#include <set>
#include <boost/shared_ptr.hpp>

#include "vtkForwardDeclarations.h"

#include <QColor>
#include "sscData.h"

class QDomNode;
class QDomDocument;

namespace ssc
{

/**\brief A mesh data set.
 *
 * A mesh is implemented as a vtkPolyData, and
 * thus can represent whatever that class can.
 *
 * \ingroup sscData
 */
class Mesh: public Data
{
Q_OBJECT
public:
	Mesh(const QString& uid, const QString& name = "");
	Mesh(const QString& uid, const QString& name, const vtkPolyDataPtr& polyData);
	virtual ~Mesh();

	void setVtkPolyData(const vtkPolyDataPtr& polyData);

	virtual vtkPolyDataPtr getVtkPolyData() const;

	void addXml(QDomNode& dataNode); ///< adds xml information about the image and its variabels
	virtual void parseXml(QDomNode& dataNode);///< Use a XML node to load data. \param dataNode A XML data representation of this object.
	virtual QString getType() const
	{
		return "mesh";
	}

	virtual DoubleBoundingBox3D boundingBox() const;
	void setColor(const QColor& color);///< Set the color of the mesh
	QColor getColor();///< Get the color of the mesh (Values are range 0 - 255)
	void setIsWireframe(bool on);///< Set rep to wireframe, false means surface
	bool getIsWireframe() const;///< true=wireframe, false=surface
	vtkPolyDataPtr getTransformedPolyData(ssc::Transform3D tranform);///< Create a new transformed polydata

signals:
	void meshChanged();
private:
	vtkPolyDataPtr mVtkPolyData;
	QColor mColor;
	bool mWireframe;
};

typedef boost::shared_ptr<Mesh> MeshPtr;

} // namespace ssc

#endif /*SSCMESH_*/

/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXMESHPROPERTYDATA_H
#define CXMESHPROPERTYDATA_H

#include "cxResourceExport.h"
#include "cxPrecompiledHeader.h"
#include "cxColorProperty.h"
#include "cxBoolProperty.h"
#include "cxDoubleProperty.h"
#include "cxStringProperty.h"

class QDomNode;

namespace cx
{

/**
 * Mesh data used in a vtkProperty
 */
class cxResource_EXPORT MeshPropertyData : public QObject
{
	Q_OBJECT
public:
	MeshPropertyData();
	void addXml(QDomNode& dataNode);
	void parseXml(QDomNode dataNode);

	std::vector<PropertyPtr> mProperties;

	ColorPropertyPtr mColor;
	BoolPropertyBasePtr mUseColorFromPolydataScalars;
	DoublePropertyPtr mVisSize;
	BoolPropertyPtr mBackfaceCulling;
	BoolPropertyPtr mFrontfaceCulling;
	StringPropertyPtr mRepresentation;

	BoolPropertyPtr mEdgeVisibility;
	ColorPropertyPtr mEdgeColor;

	DoublePropertyPtr mAmbient;
	DoublePropertyPtr mDiffuse;
	DoublePropertyPtr mSpecular;
	DoublePropertyPtr mSpecularPower;

signals:
	void changed();
private:
	void addProperty(PropertyPtr property);
	void initialize();
};

} // namespace cx

#endif // CXMESHPROPERTYDATA_H

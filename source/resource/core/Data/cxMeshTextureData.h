/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef MESHTEXTUREDATA_H
#define MESHTEXTUREDATA_H

#include "cxResourceExport.h"
#include "cxStringProperty.h"
#include "cxSelectDataStringProperty.h"


class QDomNode;

namespace cx
{

/**
 * Mesh texture data used in a vtkProperty
 */
class cxResource_EXPORT MeshTextureData : public QObject
{
	Q_OBJECT
public:
	MeshTextureData(PatientModelServicePtr patientModelService);
	void addXml(QDomNode& dataNode);
	void parseXml(QDomNode &dataNode);

	StringPropertyPtr getTextureShape() const;
	StringPropertySelectImagePtr getTextureImage() const;
	DoublePropertyPtr getScaleX() const;
	DoublePropertyPtr getScaleY() const;
	std::vector<PropertyPtr> getProperties() const;
	DoublePropertyPtr getPositionX() const;
	DoublePropertyPtr getPositionY() const;
	BoolPropertyPtr getRepeat() const;

	QString getCylinderText() const;
	QString getPlaneText() const;
	QString getSphereText() const;

signals:
	void changed();
private:
	void addProperty(PropertyPtr property);
	void initialize();
	std::vector<PropertyPtr> mProperties;
	StringPropertyPtr mTextureShape;
	StringPropertySelectImagePtr mTextureImage;
	DoublePropertyPtr mScaleX;
	DoublePropertyPtr mScaleY;
	DoublePropertyPtr mPositionX;
	DoublePropertyPtr mPositionY;
	BoolPropertyPtr mRepeat;
	PatientModelServicePtr mPatientModelService;
	QString mCylinderText;
	QString mPlaneText;
	QString mSphereText;
};

} // namespace cx

#endif // MESHTEXTUREDATA_H

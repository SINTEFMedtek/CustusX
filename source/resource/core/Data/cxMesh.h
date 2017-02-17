/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/


#ifndef SSCMESH_
#define SSCMESH_

#include "cxResourceExport.h"
#include "cxPrecompiledHeader.h"
#include "cxMeshTextureData.h"

#include <set>
#include <boost/shared_ptr.hpp>
#include "cxPatientModelService.h"
#include "cxSpaceProvider.h"

#include "vtkForwardDeclarations.h"
#include "cxMeshPropertyData.h"

#include <QColor>
#include "cxData.h"

class QDomNode;
class QDomDocument;

namespace cx
{


/** \brief A mesh data set.
 *
 * A mesh is implemented as a vtkPolyData, and
 * thus can represent whatever that class can.
 *
 * \ingroup cx_resource_core_data
 */
class cxResource_EXPORT Mesh: public Data
{
	Q_OBJECT
public:
	static MeshPtr create(const QString& uid, const QString& name = "", PatientModelServicePtr patientModelService = PatientModelService::getNullObject(),
						  SpaceProviderPtr spaceProvider = SpaceProvider::getNullObject());
	Mesh(const QString& uid, const QString& name="", vtkPolyDataPtr polyData=vtkPolyDataPtr(), PatientModelServicePtr patientModelService = PatientModelService::getNullObject(),
		 SpaceProviderPtr spaceProvider = SpaceProvider::getNullObject());
	virtual ~Mesh();

	void setVtkPolyData(const vtkPolyDataPtr& polyData);

	virtual vtkPolyDataPtr getVtkPolyData() const;
	virtual vtkTexturePtr getVtkTexture() const;

	void addXml(QDomNode& dataNode); ///< adds xml information about the image and its variabels
	virtual void parseXml(QDomNode& dataNode);///< Use a XML node to load data. \param dataNode A XML data representation of this object.
	virtual bool load(QString path);
	virtual QString getType() const
	{
		return getTypeName();
	}
	static QString getTypeName()
	{
		return "mesh";
	}
	virtual QIcon getIcon() {return QIcon(":/icons/surface.png");}

	virtual DoubleBoundingBox3D boundingBox() const;
	void setColor(const QColor& color);///< Set the color of the mesh
	QColor getColor();///< Get the color of the mesh (Values are range 0 - 255)
	void setUseColorFromPolydataScalars(bool on);
	bool getUseColorFromPolydataScalars() const;
	bool getBackfaceCulling();///< Get backface culling
	bool getFrontfaceCulling();///< Get frontface culling
	void setIsWireframe(bool on);///< Set rep to wireframe, false means surface
	bool getIsWireframe() const;///< true=wireframe, false=surface
	vtkPolyDataPtr getTransformedPolyData(Transform3D tranform);///< Create a new transformed polydata
	bool isFiberBundle() const;
	bool showGlyph();
	bool hasGlyph();
	double getVisSize();
	const char * getOrientationArray();
	const char * getColorArray();
	const char * getGlyphLUT();
	bool hasTexture() const;
	QString getTextureShape();
	QStringList getOrientationArrayList();
	QStringList getColorArrayList();
	const MeshPropertyData& getProperties() const;
	const MeshTextureData& getTextureData() const;

	virtual void save(const QString &basePath);
signals:
	void meshChanged();
public slots:
	void setBackfaceCullingSlot(bool backfaceCulling);///< Set backface culling on/off in mesh visualization
	void setFrontfaceCullingSlot(bool backfaceCulling);///< Set frontface culling on/off in mesh visualization
	void setShowGlyph(bool val);
	void setVisSize(double size);
	void setOrientationArray(const char * orientationArray);
	void setColorArray(const char * colorArray);
	void setGlyphLUT(const char * glyphLUT);
	void updateVtkPolyDataWithTexture();
private:
	PatientModelServicePtr mPatientModelService;
	SpaceProviderPtr mSpaceProvider;
	vtkPolyDataPtr mVtkPolyData;
	vtkTexturePtr mVtkTexture;
	bool createTextureMapper(vtkDataSetAlgorithmPtr &tMapper);
	bool mHasGlyph;
	bool mShowGlyph;
	bool shouldGlyphBeEnableByDefault();
	std::string mOrientationArray;
	std::string mColorArray;
	std::string mGlyphLUT;
	QStringList mOrientationArrayList;
	QStringList mColorArrayList;
	MeshPropertyData mProperties;
	MeshTextureData mTextureData;
};

typedef boost::shared_ptr<Mesh> MeshPtr;

} // namespace cx

#endif /*SSCMESH_*/

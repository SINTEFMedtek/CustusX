/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
	virtual bool load(QString path, FileManagerServicePtr filemanager);
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
	vtkPolyDataPtr getTransformedPolyDataCopy(Transform3D tranform);///< Create a new transformed polydata
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

	virtual void save(const QString &basePath, FileManagerServicePtr fileManager);
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
	vtkPolyDataPtr mVtkPolyDataOriginal;
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

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


#ifndef CXDATA_H_
#define CXDATA_H_

#include "cxResourceExport.h"
#include "cxPrecompiledHeader.h"

#include <string>
#include <set>

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include <QObject>
#include <QIcon>
#include "vtkForwardDeclarations.h"
#include "cxTransform3D.h"
#include "cxForwardDeclarations.h"
#include "cxDefinitions.h"

#include <QDateTime>
class QDomNode;

namespace cx
{
struct CoordinateSystem;
typedef boost::weak_ptr<class Rep> RepWeakPtr;
typedef boost::shared_ptr<class RegistrationHistory> RegistrationHistoryPtr;
typedef boost::shared_ptr<class Landmarks> LandmarksPtr;

struct cxResource_EXPORT TimeInfo
{
	QDateTime mAcquisitionTime;///< Possibly modified time stamp
	QDateTime mSoftwareAcquisitionTime;///< Time stamp created in at arrival in CX or CX related software
	QDateTime mOriginalAcquisitionTime;///< Original aquisition time (from hardware if possible)

	TimeInfo() :
		mAcquisitionTime(QDateTime()),
		mSoftwareAcquisitionTime(QDateTime()),
		mOriginalAcquisitionTime(QDateTime())
	{
	}
	TimeInfo(double acquisitionTime) :
		mSoftwareAcquisitionTime(QDateTime()),
		mOriginalAcquisitionTime(QDateTime())
	{
		this->setAcquisitionTime(acquisitionTime);
	}

	double getAcquisitionTime() const
	{
		return mAcquisitionTime.toMSecsSinceEpoch();
	}
	void setAcquisitionTime(double mSecsSinceEpoch)
	{
		mAcquisitionTime.setMSecsSinceEpoch(mSecsSinceEpoch);
	}

	double getSoftwareAcquisitionTime() const
	{
		return mSoftwareAcquisitionTime.toMSecsSinceEpoch();
	}
	double getScannerAcquisitionTime() const
	{
		return mOriginalAcquisitionTime.toMSecsSinceEpoch();
	}

};


/** \brief Superclass for all data objects.
 *
 * Example suclasses: Image and Mesh.
 *
 * \ingroup cx_resource_core_data
 */
class cxResource_EXPORT Data: public QObject
{
Q_OBJECT
public:
	Data(const QString& uid, const QString& name = "");
	virtual ~Data();

	void setUid(const QString& uid);
	void setName(const QString& name);
	void setAcquisitionTime(QDateTime time);
	void setSoftwareAcquisitionTime(QDateTime time);
	void setOriginalAcquisitionTime(QDateTime time);
	virtual RegistrationHistoryPtr get_rMd_History();
	LandmarksPtr getLandmarks();

	virtual QString getUid() const; ///< \return unique id for this instance
	virtual QString getName() const; /// \return a descriptive name for this instance
	virtual Transform3D get_rMd() const; ///< \return the transform M_rd from the data object's space (d) to the reference space (r).
	virtual QDateTime getAcquisitionTime() const;
	virtual TimeInfo getAdvancedTimeInfo() const;
	virtual QString getType() const
	{
        return "data";
	}
	virtual QIcon getIcon() {return QIcon();}

	virtual QString getFilename() const;
	virtual void setFilename(QString val);

	virtual QString getSpace();
	virtual QString getParentSpace();
	virtual DoubleBoundingBox3D boundingBox() const = 0;
	virtual std::vector<Vector3D> getPointCloud() const; // get a point cloud spanning volume occupied by data, in data space.

	virtual void addXml(QDomNode& dataNode); ///< adds xml information about the data and its variabels
	virtual void parseXml(QDomNode& dataNode);///< Use a XML node to load data. \param dataNode A XML data representation of this object.

	//Consider removing this, as these are only implemented in Mesh and Image
	virtual bool load(QString path) = 0;
	virtual void save(const QString& basePath) = 0;

	virtual CoordinateSystem getCoordinateSystem();

	//Moved from Image
	// methods for defining and storing clip planes. Data does not use these data, this is up to the mapper
	virtual void addPersistentClipPlane(vtkPlanePtr plane);
	virtual std::vector<vtkPlanePtr> getAllClipPlanes();
	virtual void clearPersistentClipPlanes();
	virtual void setInteractiveClipPlane(vtkPlanePtr plane); ///< set a plane that is not saved

	void addInteractiveClipPlane(vtkPlanePtr plane);
	void removeInteractiveClipPlane(vtkPlanePtr plane);

	virtual void updateFromSingleLineString(QStringList& paramterList) {}
	//virtual QString getDefaultUid() const { return ""; }
signals:
	void transformChanged(); ///< emitted when transform is changed
	void propertiesChanged(); ///< emitted when one of the metadata properties (uid, name etc) changes
	void clipPlanesChanged();

protected slots:
	virtual void transformChangedSlot()
	{
	}

protected:
	QString mUid;
	QString mName;
	QString mFilename;
	TimeInfo mTimeInfo;
	LandmarksPtr mLandmarks;

	REGISTRATION_STATUS mRegistrationStatus;
	RegistrationHistoryPtr m_rMd_History;
	std::vector<vtkPlanePtr> mPersistentClipPlanes;
	std::vector<vtkPlanePtr> mInteractiveClipPlanes;
	vtkPlanePtr mInteractiveClipPlane;

private:
	Data(const Data& other);
	Data& operator=(const Data& other);

	void addPlane(vtkPlanePtr plane, std::vector<vtkPlanePtr> &planes);
};

typedef boost::shared_ptr<Data> DataPtr;

} // end namespace cx

#endif /*CXDATA_H_*/

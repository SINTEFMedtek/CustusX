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

#ifndef SSCDATA_H_
#define SSCDATA_H_

#include <string>
#include <set>

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include <QObject>
#include "vtkForwardDeclarations.h"
#include "sscTransform3D.h"
#include "sscCoordinateSystemHelpers.h"

#include <QDateTime>
class QDomNode;

namespace ssc
{
typedef boost::weak_ptr<class Rep> RepWeakPtr;
typedef boost::shared_ptr<class RegistrationHistory> RegistrationHistoryPtr;

/**Denotes the various states
 * of registration an object is in.
 */
enum REGISTRATION_STATUS
{
	rsNOT_REGISTRATED, rsIMAGE_REGISTRATED, rsPATIENT_REGISTRATED
};

/**\brief Superclass for all data objects.
 *
 * Example suclasses: Image and Mesh.
 *
 * \ingroup sscData
 */
class Data: public QObject
{
Q_OBJECT
public:
//	Data(); //TODO remove?
	Data(const QString& uid, const QString& name = "");
	virtual ~Data();

	void setUid(const QString& uid);
	void setName(const QString& name);
	void setFilePath(const QString& filePath);///< Set current file path to file
	void setAcquisitionTime(QDateTime time);
	void setRegistrationStatus(REGISTRATION_STATUS regStat);
	virtual RegistrationHistoryPtr get_rMd_History();

	virtual QString getUid() const; ///< \return unique id for this instance
	virtual QString getName() const; /// \return a descriptive name for this instance
	virtual QString getFilePath() const; ///< \return the corresponding file path
	virtual REGISTRATION_STATUS getRegistrationStatus() const; ///< \return what kind of registration that has been performed on this data object.
	virtual Transform3D get_rMd() const; ///< \return the transform M_rd from the data object's space (d) to the reference space (r).
	virtual void setShadingOn(bool on);
	virtual bool getShadingOn() const;
//	virtual void setShading(bool on);
//	virtual bool getShading() const;
	virtual QDateTime getAcquisitionTime() const;
	virtual QString getType() const
	{
        return "data";
	}

	virtual QString getSpace();
	virtual QString getParentSpace();
	virtual DoubleBoundingBox3D boundingBox() const = 0;

	void connectToRep(const RepWeakPtr& rep); ///< called by Rep when connecting to an Image
	void disconnectFromRep(const RepWeakPtr& rep); ///< called by Rep when disconnecting from an Image

	virtual void addXml(QDomNode& dataNode); ///< adds xml information about the data and its variabels
	virtual void parseXml(QDomNode& dataNode);///< Use a XML node to load data. \param dataNode A XML data representation of this object.

	virtual CoordinateSystem getCoordinateSystem() { return ssc::CoordinateSystem(ssc::csDATA, this->getUid()); }

signals:
	void transformChanged(); ///< emitted when transform is changed
	void propertiesChanged(); ///< emitted when one of the metadata properties (uid, name etc) changes

protected slots:
	virtual void transformChangedSlot()
	{
	}

protected:
	QString mUid;
	QString mName;
	QString mFilePath;
	QDateTime mAcquisitionTime;

	REGISTRATION_STATUS mRegistrationStatus;
	RegistrationHistoryPtr m_rMd_History;

	std::set<RepWeakPtr> mReps; ///< links to Rep users.

};

typedef boost::shared_ptr<Data> DataPtr;

} // end namespace ssc

#endif /*SSCDATA_H_*/

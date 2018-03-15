/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#ifndef CXLANDMARK_H_
#define CXLANDMARK_H_

#include "cxResourceExport.h"
#include "cxPrecompiledHeader.h"

#include <string>
#include <map>
#include <QObject>
#include <QDateTime>
#include "cxVector3D.h"
#include "boost/shared_ptr.hpp"

class QDomNode;

namespace cx
{


/** \brief One landmark, or fiducial, coordinate.
 *
 * The ssc Landmark concept is one point with an unique uid that
 * can exist in several coordinate spaces. This class represent
 * the coordinate in one of these spaces.
 *
 * \ingroup cx_resource_core_data
 */
class cxResource_EXPORT Landmark
{
public:
	explicit Landmark(QString uid = "", Vector3D coord = Vector3D(0, 0, 0));
	~Landmark();

	QString getUid() const;
	Vector3D getCoord() const;
	QDateTime getTimestamp() const;

	void addXml(QDomNode& dataNode) const;
	void parseXml(QDomNode& dataNode);

private:
	QString mUid;
	Vector3D mCoord; ///< the landmarks coordinate
	QDateTime mTimestamp; ///< the time at which the landmark was sample
};
typedef std::map<QString, Landmark> LandmarkMap;

cxResource_EXPORT bool operator<(const Landmark& lhs, const Landmark& rhs);

typedef boost::shared_ptr<class Landmarks> LandmarksPtr;
/** A collection of all landmarks in a given space.
  */
class cxResource_EXPORT Landmarks : public QObject
{
	Q_OBJECT
public:
	static LandmarksPtr create();

	LandmarkMap getLandmarks();
	void addXml(QDomNode dataNode) const;
	void parseXml(QDomNode dataNode);
	void clear();

public slots:
	void setLandmark(Landmark landmark);
	void removeLandmark(QString uid);

signals:
	void landmarkRemoved(QString uid);
	void landmarkAdded(QString uid);

private:
	Landmarks();
	LandmarkMap mLandmarks; ///< map with all landmarks always in a specific space.
};


class cxResource_EXPORT LandmarkProperty
{
public:
	explicit LandmarkProperty(const QString& uid = "", const QString& name = "", bool active = true);
	bool getActive() const;
	QString getName() const;
	QString getUid() const;
	void setName(const QString& name);
	void setActive(bool active);

	void addXml(QDomNode& dataNode);
	void parseXml(QDomNode& dataNode);

private:
	QString mUid;
	QString mName;
	bool mActive;
};
typedef std::map<QString, LandmarkProperty> LandmarkPropertyMap;

} //namespace cx

#endif /* CXLANDMARK_H_ */

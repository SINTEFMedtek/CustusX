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

#ifndef SSCLANDMARK_H_
#define SSCLANDMARK_H_

#include <string>
#include <map>
#include <QObject>
#include <QDateTime>
#include "sscVector3D.h"

class QDomNode;

/*
 * sscLandmark.h
 *
 *  Created on: Apr 19, 2010
 *      Author: jbake
 */
namespace ssc
{


/**\brief One landmark, or fiducial, coordinate.
 *
 * The ssc Landmark concept is one point with an unique uid that
 * can exist in several coordinate spaces. This class represent
 * the coordinate in one of these spaces.
 *
 * \ingroup sscData
 */
class Landmark
{
public:
	explicit Landmark(QString uid = "", Vector3D coord = Vector3D(0, 0, 0));
	~Landmark();

	QString getUid() const;
	ssc::Vector3D getCoord() const;
	QDateTime getTimestamp() const;

	void addXml(QDomNode& dataNode);
	void parseXml(QDomNode& dataNode);

private:
	QString mUid;
	ssc::Vector3D mCoord; ///< the landmarks coordinate
	QDateTime mTimestamp; ///< the time at which the landmark was sample
};
typedef std::map<QString, Landmark> LandmarkMap;

bool operator<(const Landmark& lhs, const Landmark& rhs);


class LandmarkProperty
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

} //namespace ssc

#endif /* SSCLANDMARK_H_ */

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


#ifndef CXLANDMARK_H_
#define CXLANDMARK_H_

#include <string>
#include <map>
#include <QObject>
#include <QDateTime>
#include "cxVector3D.h"
#include "boost/shared_ptr.hpp"

class QDomNode;

/*
 * sscLandmark.h
 *
 *  Created on: Apr 19, 2010
 *      Author: jbake
 */
namespace cx
{


/**\brief One landmark, or fiducial, coordinate.
 *
 * The ssc Landmark concept is one point with an unique uid that
 * can exist in several coordinate spaces. This class represent
 * the coordinate in one of these spaces.
 *
 * \ingroup cx_resource_core_data
 */
class Landmark
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

bool operator<(const Landmark& lhs, const Landmark& rhs);

typedef boost::shared_ptr<class Landmarks> LandmarksPtr;
/** A collection of all landmarks in a given space.
  */
class Landmarks : public QObject
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

} //namespace cx

#endif /* CXLANDMARK_H_ */

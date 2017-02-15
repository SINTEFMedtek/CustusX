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

#ifndef CXTFRAMEMETRIC_H_
#define CXTFRAMEMETRIC_H_

#include "cxResourceExport.h"
#include "cxPrecompiledHeader.h"

#include "cxFrameMetricBase.h"

namespace cx
{

typedef boost::shared_ptr<class FrameMetric> FrameMetricPtr;

/** \brief Data class that represents a single frame (transform).
 *
 * The transform is attached to a specific coordinate system / frame.
 *
 * \ingroup cx_resource_core_data
 * \date Aug 25, 2013
 * \author Ole Vegard Solberg, SINTEF
 */
class cxResource_EXPORT FrameMetric: public cx::FrameMetricBase
{
Q_OBJECT
public:
	virtual ~FrameMetric();
	static FrameMetricPtr create(QString uid, QString name, PatientModelServicePtr dataManager, SpaceProviderPtr spaceProvider);
	virtual QString getType() const
	{
		return getTypeName();
	}
	static QString getTypeName()
	{
		return getDefaultUidPrefix() + getMetricTypeSuffix();
	}
	virtual QString getUidPrefix() const
	{
		return getDefaultUidPrefix();
	}
	static QString getDefaultUidPrefix()
	{
		return "frame";
	}
	static QString getDefaultUid()
	{
		return getDefaultUidPrefix() + "%1";
	}

	virtual QIcon getIcon() {return QIcon(":/icons/metric_frame.png");}
	virtual void addXml(QDomNode& dataNode); ///< adds xml information about the data and its variabels
	virtual void parseXml(QDomNode& dataNode); ///< Use a XML node to load data. \param dataNode A XML data representation of this object.
	virtual QString getAsSingleLineString() const;
private:
	FrameMetric(const QString& uid, const QString& name, PatientModelServicePtr dataManager, SpaceProviderPtr spaceProvider);
};

} //namespace cx

#endif /* CXTFRAMEMETRIC_H_ */

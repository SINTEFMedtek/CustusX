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

#ifndef CXMETRICREFERENCEARGUMENTLIST_H
#define CXMETRICREFERENCEARGUMENTLIST_H

#include "cxResourceExport.h"
#include "cxPrecompiledHeader.h"

#include "cxForwardDeclarations.h"
#include <vector>
#include <map>
#include "cxVector3D.h"
#include "cxTransform3D.h"
class QDomNode;

namespace cx
{

typedef boost::shared_ptr<class MetricReferenceArgumentList> MetricReferenceArgumentListPtr;
/** \brief Collection of Metric arguments that refer to another metric
 *
 *
 * \ingroup cx_resource_core_data
 * \date 2014-02-11
 * \author Christian Askeland, SINTEF
 */
class cxResource_EXPORT MetricReferenceArgumentList : public QObject
{
	Q_OBJECT
public:
	explicit MetricReferenceArgumentList(QStringList descriptions);
	void setValidArgumentTypes(QStringList types);

	unsigned getCount() const;
	void set(int index, DataPtr p);
	DataPtr get(int index);
	bool validArgument(DataPtr p) const;
	std::vector<Vector3D> getRefCoords() const;
	QString getDescription(int index) const;
    std::vector<Transform3D> getRefFrames() const;
	virtual void addXml(QDomNode& dataNode);
	virtual void parseXml(QDomNode& dataNode, const std::map<QString, DataPtr>& data);
	QString getAsSingleLineString() const;
signals:
	void argumentsChanged();
private:
	std::vector<DataPtr> mArgument;
	QStringList mDescriptions;
	QStringList mValidTypes;
};


} // namespace cx

#endif // CXMETRICREFERENCEARGUMENTLIST_H

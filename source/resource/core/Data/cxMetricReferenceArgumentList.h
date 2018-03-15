/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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

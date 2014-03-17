// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#ifndef CXMETRICREFERENCEARGUMENTLIST_H
#define CXMETRICREFERENCEARGUMENTLIST_H

#include "cxForwardDeclarations.h"
#include <vector>
#include <map>
#include "cxVector3D.h"
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
class MetricReferenceArgumentList : public QObject
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
	virtual void addXml(QDomNode& dataNode); ///< adds xml information about the data and its variabels
	virtual void parseXml(QDomNode& dataNode, const std::map<QString, DataPtr>& data); ///< Use a XML node to load data. \param dataNode A XML data representation of this object.
signals:
	void argumentsChanged();
private:
	std::vector<DataPtr> mArgument;
	QStringList mDescriptions;
	QStringList mValidTypes;
};


} // namespace cx

#endif // CXMETRICREFERENCEARGUMENTLIST_H

/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXLAYOUTREPOSITORY_H
#define CXLAYOUTREPOSITORY_H

#include "cxResourceVisualizationExport.h"

#include <QObject>
#include "cxForwardDeclarations.h"
#include <vector>
#include "cxXmlOptionItem.h"
#include "cxLayoutData.h"

namespace cx
{
class LayoutData;

/** Repository for View Layouts.
 *
 * Each layout is a separate configuration of 2D/3D/Video views, laid out
 * in a specific way on screen.
 *
 * \ingroup cx_resource_view
 * \date 2014-02-07
 * \author christiana
 */
class cxResourceVisualization_EXPORT LayoutRepository : public QObject
{
	Q_OBJECT
public:
	LayoutRepository();

	LayoutData get(const QString uid) const;
	std::vector<QString> getAvailable() const;
	void insert(const LayoutData& data);
	QString generateUid() const;
	void erase(const QString uid);
	bool isCustom(const QString& uid) const;

	void load(XmlOptionFile file);
	void save(XmlOptionFile file);

	void addDefault(LayoutData data);
	bool exists(const QString uid) const;
signals:
	void layoutChanged(QString uid);
private:
	void addDefaults();
	unsigned indexOf(const QString uid) const;

	typedef std::vector<LayoutData> LayoutDataVector;
	LayoutDataVector mLayouts;
	std::vector<QString> mDefaultLayouts;
};

} // namespace cx


#endif // CXLAYOUTREPOSITORY_H

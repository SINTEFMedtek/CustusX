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
#ifndef CXLAYOUTREPOSITORY_H
#define CXLAYOUTREPOSITORY_H

#include "cxForwardDeclarations.h"
#include <vector>
#include "sscXmlOptionItem.h"

namespace cx
{
class LayoutData;

/** Repository for View Layouts.
 *
 * Each layout is a separate configuration of 2D/3D/Video views, laid out
 * in a specific way on screen.
 *
 * \ingroup cxServiceVisualization
 * \date 2014-02-07
 * \author christiana
 */
class LayoutRepository
{
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

private:
	void addDefault(LayoutData data);
	void addDefaults();
	unsigned indexOf(const QString uid) const;

	typedef std::vector<LayoutData> LayoutDataVector;
	LayoutDataVector mLayouts;
	std::vector<QString> mDefaultLayouts;
};

typedef boost::shared_ptr<class LayoutRepository> LayoutRepositoryPtr;

} // namespace cx


#endif // CXLAYOUTREPOSITORY_H

/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXLAYOUTDATA_H_
#define CXLAYOUTDATA_H_

#include "cxResourceVisualizationExport.h"

#include <vector>
class QDomNode;
#include <QString>
#include "cxDefinitions.h"
#include "cxForwardDeclarations.h"
#include "cxView.h"

namespace cx
{
/**
* \file
* \addtogroup cx_resource_view
* @{
*/

struct cxResourceVisualization_EXPORT LayoutPosition
{
	LayoutPosition() : row(0), col(0) {}
	LayoutPosition(int r, int c) : row(r), col(c) {}
	int row; ///< row
	int col; ///< column
};

struct cxResourceVisualization_EXPORT LayoutRegion
{
	LayoutRegion() {}
	LayoutRegion(int r, int c, int rSpan = 1, int cSpan = 1) :
		pos(r, c), span(rSpan, cSpan) {}
	LayoutPosition pos; ///< start position of region
	LayoutPosition span; ///< size of region

	bool contains(LayoutPosition p) const
	{
		return pos.row <= p.row && p.row < pos.row + span.row && pos.col <= p.col && p.col < pos.col + span.col;
	}
};

cxResourceVisualization_EXPORT LayoutRegion merge(LayoutRegion a, LayoutRegion b);

/** Describes the layout and content of one view.
 */
struct LayoutViewData
{
	LayoutViewData() : mGroup(-1), mPlane(ptCOUNT), mType(View::VIEW), mRegion(-1, -1, 1, 1) {}
	LayoutViewData(int row, int col, int rowSpan = 1, int colSpan = 1) :
		mGroup(-1), mPlane(ptCOUNT), mType(View::VIEW), mRegion(row, col, rowSpan, colSpan) {}
	int mGroup; ///< what group to connect to. -1 means not set.
	PLANE_TYPE mPlane; ///< ptNOPLANE means 3D
	View::Type mType;
	LayoutRegion mRegion;

	void addXml(QDomNode node) const; ///< save state to xml
	void parseXml(QDomNode node);///< load state from xml
	bool isValid() const { return (( mGroup>=0 )&&( mPlane!=ptCOUNT )); }
};

/** Represents one specific layout of the views, and what kind of information they contain,
 *  i.e 3D/2D, slice plane definition, image group.
 */
class cxResourceVisualization_EXPORT LayoutData
{
public:
	typedef std::vector<LayoutViewData> ViewDataContainer;
	typedef ViewDataContainer::iterator iterator;
	typedef ViewDataContainer::const_iterator const_iterator;
	static const int MaxGridSize = 20;

public:
	static LayoutData createHeader(QString uid, QString name);
	static LayoutData create(QString uid, QString name, int rows, int cols);
	LayoutData();
	QString getUid() const { return mUid; }
	QString getName() const { return mName; }
	bool getOffScreenRendering() const { return mOffScreenRendering; }
	void setOffScreenRendering(bool val) { mOffScreenRendering = val; }
	void setName(const QString& name) { mName = name; }
	void resetUid(const QString& uid);
	void setView(int group, PLANE_TYPE type, LayoutRegion region);
	void setView(int group, View::Type type, LayoutRegion region);
	LayoutViewData& get(LayoutPosition pos);
	iterator begin() { return mView.begin(); }
	iterator end() { return mView.end(); }
	const_iterator begin() const { return mView.begin(); }
	const_iterator end() const { return mView.end(); }
	iterator find(LayoutPosition pos);
	bool isEmpty() const { return mSize.col == 0 || mSize.row == 0; }

	void split(iterator iter);
	void split(LayoutRegion region);
	bool merge(LayoutRegion region);
	void resize(int rows, int cols);
	LayoutPosition size() const { return mSize; }

	void addXml(QDomNode node) const; ///< save state to xml
	void parseXml(QDomNode node);///< load state from xml

private:
	QString mUid;
	QString mName;
	LayoutPosition mSize;
	std::vector<LayoutViewData> mView;
	bool mOffScreenRendering;
};

/**
* @}
*/
} // namespace cx

#endif /* CXLAYOUTDATA_H_ */

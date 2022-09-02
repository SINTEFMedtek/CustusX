/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxLayoutData.h"
#include <iostream>
#include <QDomElement>
#include "cxLogger.h"
#include "cxView.h"
#include "cxEnumConversion.h"

namespace cx
{

LayoutRegion merge(LayoutRegion a, LayoutRegion b)
{
	if (a.pos.col<0 || a.pos.row<0)
		return b;
	if (b.pos.col<0 || b.pos.row<0)
		return a;

	int r1 = std::min(a.pos.row, b.pos.row);
	int c1 = std::min(a.pos.col, b.pos.col);
	int r2 = std::max(a.pos.row + a.span.row - 1, b.pos.row + b.span.row - 1);
	int c2 = std::max(a.pos.col + a.span.col - 1, b.pos.col + b.span.col - 1);
	return LayoutRegion(r1, c1, r2 - r1 + 1, c2 - c1 + 1);
}

void LayoutViewData::addXml(QDomNode node) const
{
	QDomElement elem = node.toElement();
	elem.setAttribute("group", qstring_cast(mGroup));
	elem.setAttribute("type", qstring_cast(mPlane));
	elem.setAttribute("view", qstring_cast(mType));
	elem.setAttribute("row", qstring_cast(mRegion.pos.row));
	elem.setAttribute("col", qstring_cast(mRegion.pos.col));
	elem.setAttribute("rowSpan", qstring_cast(mRegion.span.row));
	elem.setAttribute("colSpan", qstring_cast(mRegion.span.col));
}

void LayoutViewData::parseXml(QDomNode node)
{
	QDomElement elem = node.toElement();
	mGroup = elem.attribute("group").toInt();
	mPlane = string2enum<PLANE_TYPE> (elem.attribute("type"));
	//  mType = string2enum<View::Type>(elem.attribute("view"));
	mType = static_cast<View::Type> (elem.attribute("view").toInt());
	mRegion.pos.row = elem.attribute("row").toInt();
	mRegion.pos.col = elem.attribute("col").toInt();
	mRegion.span.row = elem.attribute("rowSpan").toInt();
	mRegion.span.col = elem.attribute("colSpan").toInt();
}

LayoutData LayoutData::createHeader(QString uid, QString name)
{
	return create(uid, name, 0, 0);
}

LayoutData LayoutData::create(QString uid, QString name, int rows, int cols)
{
	LayoutData retval;
	retval.resetUid(uid);
	retval.setName(name);
	retval.resize(rows, cols);
	return retval;
}

LayoutData::LayoutData() :
	mName("unnamed"),
	mOffScreenRendering(false)
{
	mSize = LayoutPosition(0, 0);
	this->resize(0, 0);
}

void LayoutData::resetUid(const QString& uid)
{
	mUid = uid;
	mName = mName + " " + uid;
}

/**Allows easy definition of a view inside the layout.
 * region must be inside layout bounds.
 */
void LayoutData::setView(int group, PLANE_TYPE type, LayoutRegion region)
{
	if (!this->merge(region))
		return;
	LayoutViewData& view = get(region.pos);
	view.mGroup = group;
	view.mPlane = type;
	view.mType = View::VIEW_2D;
}

void LayoutData::setView(int group, View::Type type, LayoutRegion region)
{
	if (!this->merge(region))
		return;
	LayoutViewData& view = get(region.pos);
	view.mGroup = group;
	view.mPlane = ptNOPLANE;
	view.mType = type;
}

/** Merge all views inside input region.
 *  Previously merged views partially inside the new region will be split.
 *
 *  Prerequisite: input is inside old boundaries.
 */
bool LayoutData::merge(LayoutRegion region)
{
	if (region.pos.row + region.span.row > mSize.row || region.pos.col + region.span.col > mSize.col)
	{
		reportError("Attempted to merge a region outside allocated space in LayoutData.");
		return false;
	}

	//std::cout << "merge views " << std::endl;
	this->split(region); // split all existing merges in order to keep consistency

	// create new merged view based on the ul corner view.
	LayoutViewData current = this->get(region.pos);
	current.mRegion = region;

	// erase all views within region (all should now be 1x1)
	for (int r = region.pos.row; r < region.pos.row + region.span.row; ++r)
	{
		for (int c = region.pos.col; c < region.pos.col + region.span.col; ++c)
		{
			iterator iter = this->find(LayoutPosition(r, c));
			if (iter != this->end())
				mView.erase(iter);
		}
	}

	// add merged view.
	mView.push_back(current);
	return true;
}

/**Split all views wholly or partially within input region
 * into 1x1 views.
 */
void LayoutData::split(LayoutRegion region)
{
	// reset all primitives in region
	for (int r = region.pos.row; r < region.pos.row + region.span.row; ++r)
	{
		for (int c = region.pos.col; c < region.pos.col + region.span.col; ++c)
		{
			iterator iter = this->find(LayoutPosition(r, c));
			this->split(iter);
		}
	}
}

/**Split the region given by iter into 1x1 views.
 *
 */
void LayoutData::split(iterator iter)
{
	if (iter == this->end())
		return;
	if (iter->mRegion.span.row == 1 && iter->mRegion.span.col == 1)
		return; // nothing to split

	LayoutViewData newView = *iter;
	LayoutRegion region = iter->mRegion;

	// erase old region
	mView.erase(iter);

	// insert new 1x1 views in the erased region.
	for (int r = region.pos.row; r < region.pos.row + region.span.row; ++r)
	{
		for (int c = region.pos.col; c < region.pos.col + region.span.col; ++c)
		{
			newView.mRegion = LayoutRegion(r, c, 1, 1);
			mView.push_back(newView);
		}
	}
}

/**Resize layout to be of dimension rows x cols.
 *
 */
void LayoutData::resize(int rows, int cols)
{
	mSize = LayoutPosition(rows, cols);

	if (mSize.row == 0 || mSize.col == 0)
	{
		mView.clear();
		return;
	}

	// erase all views outside region
	// TODO: also consider nontrivial regions.
	for (iterator iter = this->begin(); iter != this->end();)
	{
		if (iter->mRegion.pos.row >= rows || iter->mRegion.pos.col >= cols)
		{
			iter = mView.erase(iter);
		}
		else
		{
			++iter;
		}
	}

	// add new views (brute force: optimize if needed)
	for (int r = 0; r < rows; ++r)
	{
		for (int c = 0; c < cols; ++c)
		{
			if (this->find(LayoutPosition(r, c)) == this->end())
			{
				mView.push_back(LayoutViewData(r, c, 1, 1));
			}
		}
	}
}

//const LayoutData::ViewData& LayoutData::get(LayoutPosition pos) const
//{
//  return *(this->find(pos));
//}

LayoutViewData& LayoutData::get(LayoutPosition pos)
{
	return *(this->find(pos));
}

LayoutData::iterator LayoutData::find(LayoutPosition pos)
{
	for (iterator iter = this->begin(); iter != this->end(); ++iter)
	{
		if (iter->mRegion.contains(pos))
			return iter;
	}

	return this->end();
}

void LayoutData::addXml(QDomNode node) const
{
	QDomDocument doc = node.ownerDocument();
	QDomElement elem = node.toElement();

	elem.setAttribute("uid", mUid);
	elem.setAttribute("name", mName);
	elem.setAttribute("offScreenRendering", mOffScreenRendering);

	QDomElement size = doc.createElement("size");
	size.setAttribute("row", mSize.row);
	size.setAttribute("col", mSize.col);
	elem.appendChild(size);

	for (const_iterator iter = this->begin(); iter != this->end(); ++iter)
	{
		QDomElement view = doc.createElement("view");
		iter->addXml(view);
		elem.appendChild(view);
	}
}

void LayoutData::parseXml(QDomNode node)
{
	if (node.isNull())
		return;

	QDomElement elem = node.toElement();
	mUid = elem.attribute("uid");
	mName = elem.attribute("name");
	mOffScreenRendering = elem.attribute("offScreenRendering").toInt();

	QDomElement size = elem.namedItem("size").toElement();
	mSize.row = size.attribute("row").toInt();
	mSize.col = size.attribute("col").toInt();

	mView.clear();
	// iterate over all views
	QDomElement currentElem = elem.firstChildElement("view");
	for (; !currentElem.isNull(); currentElem = currentElem.nextSiblingElement("view"))
	{
		LayoutViewData viewData;
		viewData.parseXml(currentElem);
		mView.push_back(viewData);
	}
}

} // namespace cx

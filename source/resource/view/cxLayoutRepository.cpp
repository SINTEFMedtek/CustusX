/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxLayoutRepository.h"
#include "cxLayoutData.h"
#include "cxTypeConversions.h"
#include "cxLogger.h"

namespace cx
{

LayoutRepository::LayoutRepository()
{
	this->addDefaults();
}

LayoutData LayoutRepository::get(const QString uid) const
{
	unsigned pos = this->indexOf(uid);
	if (pos != mLayouts.size())
		return mLayouts[pos];
	if(!uid.isEmpty())
		CX_LOG_WARNING() << "Layout don't exist: " << uid;
	return LayoutData();
}

bool LayoutRepository::exists(const QString uid) const
{
	unsigned pos = this->indexOf(uid);
	if (pos != mLayouts.size())
		return true;
	else
		return false;
}

std::vector<QString> LayoutRepository::getAvailable() const
{
	std::vector<QString> retval;
	for (unsigned i = 0; i < mLayouts.size(); ++i)
	{
		retval.push_back(mLayouts[i].getUid());
	}
	return retval;
}

void LayoutRepository::insert(const LayoutData& data)
{
	unsigned pos = this->indexOf(data.getUid());
	if (pos == mLayouts.size())
		mLayouts.push_back(data);
	else
		mLayouts[pos] = data;
	emit layoutChanged(data.getUid());
}

QString LayoutRepository::generateUid() const
{
	int count = 0;

	for (LayoutDataVector::const_iterator iter = mLayouts.begin(); iter != mLayouts.end(); ++iter)
	{
		if (iter->getUid() == qstring_cast(count))
			count = iter->getUid().toInt() + 1;
	}
	return qstring_cast(count);
}

void LayoutRepository::erase(const QString uid)
{
	mLayouts.erase(mLayouts.begin() + indexOf(uid));
	emit layoutChanged(uid);
}

unsigned LayoutRepository::indexOf(const QString uid) const
{
	for (unsigned i = 0; i < mLayouts.size(); ++i)
	{
		if (mLayouts[i].getUid() == uid)
			return i;
	}
	return mLayouts.size();
}

bool LayoutRepository::isCustom(const QString& uid) const
{
	bool isLayout = false;
	for (unsigned i = 0; i < mLayouts.size(); ++i)
	{
		if (uid == mLayouts[i].getUid())
		{
			isLayout = true;
			break;
		}
	}

	bool isDefaultLayout = std::count(mDefaultLayouts.begin(), mDefaultLayouts.end(), uid);

	bool retval = false;
	if (isLayout && !isDefaultLayout)
		retval = true;

	return retval;
}

void LayoutRepository::load(XmlOptionFile file)
{
	// load custom layouts:
	mLayouts.clear();

	this->blockSignals(true);

	QDomElement layouts = file.getElement("layouts");
	QDomNode layout = layouts.firstChild();
	for (; !layout.isNull(); layout = layout.nextSibling())
	{
		if (layout.toElement().tagName() != "layout")
			continue;

		LayoutData data;
		data.parseXml(layout);

		this->insert(data);
	}

	std::vector<QString> custom = this->getAvailable();
	this->addDefaults(); // ensure we overwrite loaded layouts

	this->blockSignals(false);

	for (unsigned i=0; i<custom.size(); ++i)
		emit layoutChanged(custom[i]);
}

void LayoutRepository::save(XmlOptionFile file)
{
	XmlOptionFile layoutsNode = file.descend("layouts");
	layoutsNode.removeChildren();
	for (LayoutDataVector::iterator iter = mLayouts.begin(); iter != mLayouts.end(); ++iter)
	{
		if (!this->isCustom(iter->getUid()))
			continue; // dont store default layouts - they are created automatically.

		QDomElement layoutNode = file.getDocument().createElement("layout");
		layoutsNode.getElement().appendChild(layoutNode);
		iter->addXml(layoutNode);
	}
}

/** insert the hardcoded layouts into mLayouts.
 *
 */
void LayoutRepository::addDefaults()
{
	mDefaultLayouts.clear();

	/*
	 *
	 3D______________

	 3D
	 3D AD
	 3D ACS

	 Oblique ________

	 3D AnyDual x1
	 3D AnyDual x2
	 AnyDual x3

	 Orthogonal______

	 3D ACS x1
	 3D ACS x2
	 ACS x3

	 RT______________

	 RT
	 Us Acq
	 */

	// ------------------------------------------------------
	// --- group of 3D-based layouts ------------------------
	// ------------------------------------------------------
	this->addDefault(LayoutData::createHeader("LAYOUT_GROUP_3D", "3D"));
	{
		// 3D only
		LayoutData layout = LayoutData::create("LAYOUT_3D", "3D", 1, 1);
		layout.setView(0, View::VIEW_3D, LayoutRegion(0, 0));
		this->addDefault(layout);
	}
	{
		// 3D ACS
		LayoutData layout = LayoutData::create("LAYOUT_3D_ACS", "3D ACS", 3, 4);
		layout.setView(0, View::VIEW_3D, LayoutRegion(0, 0, 3, 3));
		layout.setView(1, ptAXIAL, LayoutRegion(0, 3));
		layout.setView(1, ptCORONAL, LayoutRegion(1, 3));
		layout.setView(1, ptSAGITTAL, LayoutRegion(2, 3));
		this->addDefault(layout);
	}
    {
        // A 3DCS
        LayoutData layout = LayoutData::create("LAYOUT_A_3DCS", "A 3DCS", 3, 4);
        layout.setView(1, ptAXIAL, LayoutRegion(0, 0, 3, 3));
        layout.setView(0, View::VIEW_3D, LayoutRegion(0, 3));
        layout.setView(1, ptCORONAL, LayoutRegion(1, 3));
        layout.setView(1, ptSAGITTAL, LayoutRegion(2, 3));
        this->addDefault(layout);
    }
    {
        // 3D 3DAC
        LayoutData layout = LayoutData::create("LAYOUT_3D_3DAC", "3D 3DAC", 3, 5);
        layout.setView(0, View::VIEW_3D, LayoutRegion(0, 0, 3, 3));
        layout.setView(1, View::VIEW_3D, LayoutRegion(0, 3, 1, 2));
        layout.setView(2, ptAXIAL, LayoutRegion(1, 3, 1, 2));
        layout.setView(2, ptCORONAL, LayoutRegion(2, 3, 1, 2));
        this->addDefault(layout);
    }
	{
		// 3D Any
		LayoutData layout = LayoutData::create("LAYOUT_3D_AD", "3D AnyDual", 2, 4);
		layout.setView(0, View::VIEW_3D, LayoutRegion(0, 0, 2, 3));
		layout.setView(1, ptANYPLANE, LayoutRegion(0, 3));
		layout.setView(1, ptSIDEPLANE, LayoutRegion(1, 3));
		this->addDefault(layout);
	}
	{
		// 3D ACS in a single view group
		LayoutData layout = LayoutData::create("LAYOUT_3D_ACS_SINGLE", "3D ACS Connected", 3, 4);
		layout.setView(0, View::VIEW_3D, LayoutRegion(0, 0, 3, 3));
		layout.setView(0, ptAXIAL, LayoutRegion(0, 3));
		layout.setView(0, ptCORONAL, LayoutRegion(1, 3));
		layout.setView(0, ptSAGITTAL, LayoutRegion(2, 3));
		this->addDefault(layout);
	}
	{
		// 3D Any in a single view group
		LayoutData layout = LayoutData::create("LAYOUT_3D_AD_SINGLE", "3D AnyDual Connected", 2, 4);
		layout.setView(0, View::VIEW_3D, LayoutRegion(0, 0, 2, 3));
		layout.setView(0, ptANYPLANE, LayoutRegion(0, 3));
		layout.setView(0, ptSIDEPLANE, LayoutRegion(1, 3));
		this->addDefault(layout);
	}

	// ------------------------------------------------------
	// --- group of oblique (Anyplane-based) layouts --------
	// ------------------------------------------------------
	this->addDefault(LayoutData::createHeader("LAYOUT_GROUP_Oblique", "Oblique"));
	{
		LayoutData layout = LayoutData::create("LAYOUT_OBLIQUE_3DAnyDual_x1", "3D Any Dual x1", 1, 3);
		layout.setView(0, View::VIEW_3D, LayoutRegion(0, 0));
		layout.setView(1, ptANYPLANE, LayoutRegion(0, 1));
		layout.setView(1, ptSIDEPLANE, LayoutRegion(0, 2));
		this->addDefault(layout);
	}
	{
		LayoutData layout = LayoutData::create("LAYOUT_OBLIQUE_3DAnyDual_x2", "3D Any Dual x2", 2, 3);
		layout.setView(0, View::VIEW_3D, LayoutRegion(0, 0, 2, 1));
		layout.setView(1, ptANYPLANE, LayoutRegion(0, 1));
		layout.setView(1, ptSIDEPLANE, LayoutRegion(1, 1));
		layout.setView(2, ptANYPLANE, LayoutRegion(0, 2));
		layout.setView(2, ptSIDEPLANE, LayoutRegion(1, 2));
		this->addDefault(layout);
	}
	{
		LayoutData layout = LayoutData::create("LAYOUT_OBLIQUE_AnyDual_x3", "Any Dual x3", 2, 3);
		layout.setView(0, ptANYPLANE, LayoutRegion(0, 0));
		layout.setView(0, ptSIDEPLANE, LayoutRegion(1, 0));
		layout.setView(1, ptANYPLANE, LayoutRegion(0, 1));
		layout.setView(1, ptSIDEPLANE, LayoutRegion(1, 1));
		layout.setView(2, ptANYPLANE, LayoutRegion(0, 2));
		layout.setView(2, ptSIDEPLANE, LayoutRegion(1, 2));
		this->addDefault(layout);
	}

	// ------------------------------------------------------
	// --- group of orthogonal (ACS-based) layouts ----------
	// ------------------------------------------------------
	this->addDefault(LayoutData::createHeader("LAYOUT_GROUP_Orthogonal", "Orthogonal"));
	{
		LayoutData layout = LayoutData::create("LAYOUT_ORTHOGONAL_3DACS_x1", "3D ACS x1", 2, 2);
		layout.setView(0, View::VIEW_3D, LayoutRegion(0, 0));
		layout.setView(1, ptAXIAL, LayoutRegion(0, 1));
		layout.setView(1, ptCORONAL, LayoutRegion(1, 0));
		layout.setView(1, ptSAGITTAL, LayoutRegion(1, 1));
		this->addDefault(layout);
	}
	{
		LayoutData layout = LayoutData::create("LAYOUT_ORTHOGONAL_3DACS_x2", "3D ACS x2", 3, 3);
		layout.setView(0, View::VIEW_3D, LayoutRegion(0, 0, 3, 1));
		layout.setView(1, ptAXIAL, LayoutRegion(0, 1));
		layout.setView(1, ptCORONAL, LayoutRegion(1, 1));
		layout.setView(1, ptSAGITTAL, LayoutRegion(2, 1));
		layout.setView(2, ptAXIAL, LayoutRegion(0, 2));
		layout.setView(2, ptCORONAL, LayoutRegion(1, 2));
		layout.setView(2, ptSAGITTAL, LayoutRegion(2, 2));
		this->addDefault(layout);
	}
	{
		LayoutData layout = LayoutData::create("LAYOUT_ORTHOGONAL_3DACS_x3", "ACS x3", 3, 3);
		layout.setView(0, ptAXIAL, LayoutRegion(0, 0));
		layout.setView(0, ptCORONAL, LayoutRegion(1, 0));
		layout.setView(0, ptSAGITTAL, LayoutRegion(2, 0));
		layout.setView(1, ptAXIAL, LayoutRegion(0, 1));
		layout.setView(1, ptCORONAL, LayoutRegion(1, 1));
		layout.setView(1, ptSAGITTAL, LayoutRegion(2, 1));
		layout.setView(2, ptAXIAL, LayoutRegion(0, 2));
		layout.setView(2, ptCORONAL, LayoutRegion(1, 2));
		layout.setView(2, ptSAGITTAL, LayoutRegion(2, 2));
		this->addDefault(layout);
	}

	// ------------------------------------------------------
	// --- group of RTsource-based layouts - single viewgroup
	// ------------------------------------------------------
	this->addDefault(LayoutData::createHeader("LAYOUT_GROUP_RT", "Realtime Source"));
	{
		LayoutData layout = LayoutData::create("LAYOUT_RT_1X1", "RT", 1, 1);
		layout.setView(0, View::VIEW_REAL_TIME, LayoutRegion(0, 0));
		this->addDefault(layout);
	}
	{
		LayoutData layout = LayoutData::create("LAYOUT_US_Acquisition", "US Acquisition", 2, 3);
		layout.setView(0, ptANYPLANE, LayoutRegion(1, 2, 1, 1));
		layout.setView(0, View::VIEW_3D, LayoutRegion(0, 2, 1, 1));
		layout.setView(0, View::VIEW_REAL_TIME, LayoutRegion(0, 0, 2, 2));
		this->addDefault(layout);
	}
}

void LayoutRepository::addDefault(LayoutData data)
{
	mDefaultLayouts.push_back(data.getUid());
	mLayouts.push_back(data);
}

} // namespace cx



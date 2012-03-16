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

#ifndef CXPROBECONFIGWIDGET_H_
#define CXPROBECONFIGWIDGET_H_

#include "cxBaseWidget.h"
//#include "cxBoundingBoxWidget.h"
#include "cxToolDataAdapters.h"

namespace cx
{

/**
 *
 * \brief Widget that displays/edits a probe configuration
 * \ingroup cxGUI
 *
 * \date Mar 16, 2012
 * \author Christian Askeland, SINTEF
 */
class ProbeConfigWidget : public BaseWidget
{
	Q_OBJECT
public:
	ProbeConfigWidget(QWidget* parent=NULL);
	virtual ~ProbeConfigWidget();
private:
	virtual QString defaultWhatsThis() const;

//	BoundingBoxWidget* mBBWidget;
	ActiveProbeConfigurationStringDataAdapterPtr mActiveProbeConfig;

//	ssc::Vector3D mOrigin_p; ///< probe origin in pixel space p. (upper-left corner origin)
//	ssc::Vector3D mSpacing;
//	ssc::DoubleBoundingBox3D mClipRect_p; ///< sector clipping rect, in addition to the standard sector definition. The probe sector is the intersection of the sector definition and the clip rect.


//	ssc::ProbeData mData; ///< current sector data
};

}

#endif /* CXPROBECONFIGWIDGET_H_ */

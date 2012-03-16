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

#ifndef CXBOUNDINGBOXWIDGET_H_
#define CXBOUNDINGBOXWIDGET_H_

#include "sscForwardDeclarations.h"
#include "cxDoubleSpanSlider.h"
#include "sscBoundingBox3D.h"

namespace cx
{

/**
 * \brief Widget displays/edits a ssc::BoundingBox3D
 * \ingroup cxGUI
 *
 * \date Mar 16, 2012
 * \author Christian Askeland, SINTEF
 */
class BoundingBoxWidget: public QWidget
{
Q_OBJECT
public:
	BoundingBoxWidget(QWidget* parent=NULL);
//	void setValue(const ssc::DoubleBoundingBox3D& bb);
//	void setRange(const ssc::DoubleBoundingBox3D& bb);
	void setValue(const ssc::DoubleBoundingBox3D& value, const ssc::DoubleBoundingBox3D& range);
	ssc::DoubleBoundingBox3D getValue() const;
signals:
	void changed();
private:
	SliderRangeGroupWidget* mXRange;
	SliderRangeGroupWidget* mYRange;
	SliderRangeGroupWidget* mZRange;
};

}

#endif /* CXBOUNDINGBOXWIDGET_H_ */

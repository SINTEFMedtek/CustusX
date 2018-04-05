/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXBOUNDINGBOXWIDGET_H_
#define CXBOUNDINGBOXWIDGET_H_

#include "cxResourceWidgetsExport.h"

#include <QWidget>
#include <vector>
#include "cxForwardDeclarations.h"
#include "cxBoundingBox3D.h"

namespace cx
{
class SliderRangeGroupWidget;


/**
 * \brief Widget displays/edits a BoundingBox3D
 * \ingroup cx_resource_widgets
 *
 * \date Mar 16, 2012
 * \author Christian Askeland, SINTEF
 */
class cxResourceWidgets_EXPORT BoundingBoxWidget: public QWidget
{
Q_OBJECT
public:
	BoundingBoxWidget(QWidget* parent=NULL, QStringList inCaptions = QStringList());
	void setValue(const DoubleBoundingBox3D& value, const DoubleBoundingBox3D& range);
	DoubleBoundingBox3D getValue() const;
	void showDim(int dim, bool visible);

signals:
	void changed();
private:
	std::vector<SliderRangeGroupWidget*> mRange;
};

}

#endif /* CXBOUNDINGBOXWIDGET_H_ */

/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/
#ifndef CXVECTOR3DWIDGET_H_
#define CXVECTOR3DWIDGET_H_

#include "cxResourceWidgetsExport.h"

#include "cxVector3D.h"
#include <QWidget>
class QBoxLayout;
#include "cxVector3DDataAdapter.h"
#include "cxDoubleDataAdapterXml.h"

namespace cx
{


/**
 * \brief Widget for displaying a Vector3D
 *
 * \ingroup cx_resource_widgets
 *
 */
class cxResourceWidgets_EXPORT Vector3DWidget: public QWidget
{
Q_OBJECT
public:
	Vector3DWidget(QWidget* parent, Vector3DDataAdapterPtr data);
	virtual ~Vector3DWidget() {}

	static Vector3DWidget* createSmallHorizontal(QWidget* parent, Vector3DDataAdapterPtr data);
	static Vector3DWidget* createVerticalWithSliders(QWidget* parent, Vector3DDataAdapterPtr data);
	void showDim(int dim, bool visible);

private:
	void addSliderControlsForIndex(QString name, QString help, int index, QBoxLayout* layout);
	void addSmallControlsForIndex(QString name, QString help, int index, QBoxLayout* layout);

	Vector3DDataAdapterPtr mData;
	boost::array<DoubleDataAdapterPtr, 3> mDoubleAdapter;
	boost::array<QWidget*, 3> mWidgets;
};

}

#endif /* CXVECTOR3DWIDGET_H_ */

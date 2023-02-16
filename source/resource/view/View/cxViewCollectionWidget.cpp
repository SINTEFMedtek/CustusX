/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxViewCollectionWidget.h"
#include <QGridLayout>

#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"


#include "cxViewCollectionWidgetUsingViewWidgets.h"
#include "cxViewCollectionWidgetUsingViewContainer.h"
#include "cxViewCollectionWidgetMixed.h"

#include "cxViewContainer.h"

namespace cx
{

QPointer<ViewCollectionWidget> ViewCollectionWidget::createViewWidgetLayout(QWidget *parent)
{
//	return new LayoutWidgetUsingViewCollection(); // testing
	return new LayoutWidgetUsingViewWidgets(parent);
//	return new ViewCollectionWidgetMixed(); //testing
}

QPointer<ViewCollectionWidget> ViewCollectionWidget::createOptimizedLayout(QWidget *parent)
{
//	return new ViewCollectionWidgetUsingViewContainer();
	return new ViewCollectionWidgetMixed(parent);
}

} // namespace cx


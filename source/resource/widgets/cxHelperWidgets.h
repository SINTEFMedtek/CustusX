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


#ifndef CXHELPERWIDGETS_H_
#define CXHELPERWIDGETS_H_

#include "cxResourceWidgetsExport.h"

#include <QWidget>
#include "cxProperty.h"
class QGridLayout;
class QHBoxLayout;

namespace cx
{

/**
* \file
* \addtogroup cx_resource_widgets
* @{
*/


/**\brief A widget that floats above the other windows. Used as basis for the floating controls.
 *
 */
class cxResourceWidgets_EXPORT sscFloatingWidget: public QWidget
{
Q_OBJECT

public:
	virtual ~sscFloatingWidget()
	{
	}
	sscFloatingWidget(QWidget* parent = 0) :
		QWidget(parent)
	{
		Qt::WindowFlags flags = Qt::FramelessWindowHint;
		flags |= Qt::Window;
		flags |= Qt::WindowStaysOnTopHint;
		this->setWindowFlags(flags);
	}
};

/** Create a layout with zero margin/spacing
  * containing the input widgets.
  *
  * This is used in a hack for the Property
  * widgets that add to a QGridLayout.
  *
  * The Property Widget itself, which is invisible
  * if a gridlayout is used, is added to the
  * gridlayout this way. See ScalarInteractionWidget for
  * an example.
  * Failure to do this would cause a dangling widget.
  *
  *   \author christiana
  *   \date Nov 25, 2012
  */
cxResourceWidgets_EXPORT QHBoxLayout* mergeWidgetsIntoHBoxLayout(QWidget* first, QWidget* second);
/** Add on hack for mergeWidgetsIntoHBoxLayout(),
  * create a small extent for the widget so that is has a visible region
  *   \author christiana
  *   \date Dec 5, 2012
  */
cxResourceWidgets_EXPORT QWidget* addDummyMargin(QWidget* widget);

/**\brief Create a widget capable of displaying the input data.
 *
 * If a gridLayout is provided, the widget will insert its components
 * into a row in that layout
 *
 */
cxResourceWidgets_EXPORT QWidget* sscCreateDataWidget(QWidget* parent, PropertyPtr data, QGridLayout* gridLayout = 0, int row = 0);

typedef boost::shared_ptr<class PatientModelService> PatientModelServicePtr;
typedef boost::shared_ptr<class ViewService> ViewServicePtr;

/**\brief Create a widget capable of displaying the input data.
 *
 * If a gridLayout is provided, the widget will insert its components
 * into a row in that layout
 *
 * \ingroup cx_gui
 */
cxResourceWidgets_EXPORT QWidget* createDataWidget(ViewServicePtr viewService, PatientModelServicePtr patientModelService, QWidget* parent, cx::PropertyPtr data, QGridLayout* gridLayout = 0, int row = 0);


} // namespace cx


/**
* @}
*/

#endif

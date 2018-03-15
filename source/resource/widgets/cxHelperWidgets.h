/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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

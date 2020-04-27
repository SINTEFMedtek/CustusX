/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXFILTERWIDGET_H
#define CXFILTERWIDGET_H

#include "cxGuiExport.h"

#include "cxBaseWidget.h"
#include "cxFilter.h"
#include "cxFilterTimedAlgorithm.h"
#include "cxOptionsWidget.h"

namespace cx
{
typedef boost::shared_ptr<class WidgetObscuredListener> WidgetObscuredListenerPtr;
typedef boost::shared_ptr<class VisServices> VisServicesPtr;
class TimedAlgorithmProgressBar;
class FilterPresetWidget;

/** Helper widget for displaying the input/output/options part of a Filter.
 * Intended to be included in other Filter widgets.
 *
 * \ingroup cx_gui
 * \date Nov 18, 2012
 * \author Christian Askeland, SINTEF
 * \author Janne Beate Bakeng, SINTEF
 */
class cxGui_EXPORT FilterSetupWidget : public BaseWidget
{
	Q_OBJECT
public:
	FilterSetupWidget(VisServicesPtr services, QWidget* parent, XmlOptionFile options, bool addFrame);

	void setFilter(FilterPtr filter);
	QString generateHelpText() const;

	void setCompact(bool on); ///< Compact Mode: one group, hide main input/output
	void toggleDetailed();

private slots:
	void obscuredSlot(bool obscured);
	void showAdvancedOptions(int state);
	void rebuildOptions();

private:
	void setVisibilityOfOptionsAndAdvancedOptions();

	VisServicesPtr mServices;
	XmlOptionFile mOptions;//Not used?
	FilterPtr mCurrentFilter;

	OptionsWidget* mInputsWidget;
	OptionsWidget* mOutputsWidget;
	OptionsWidget* mOptionsWidget;
	FilterPresetWidget*  mPresetWidget;
	QGroupBox* 	   mOptionsGroupBox;
	QCheckBox*	   mAdvancedButton;
	QGroupBox* 	   mFrame;
	boost::shared_ptr<WidgetObscuredListener> mObscuredListener;
};

}

#endif // CXFILTERWIDGET_H

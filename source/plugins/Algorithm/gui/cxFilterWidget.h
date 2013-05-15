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
#ifndef CXFILTERWIDGET_H
#define CXFILTERWIDGET_H

#include "cxBaseWidget.h"
#include "cxFilter.h"
#include "cxFilterTimedAlgorithm.h"
#include "cxOptionsWidget.h"

namespace cx
{
typedef boost::shared_ptr<class WidgetObscuredListener> WidgetObscuredListenerPtr;
class TimedAlgorithmProgressBar;
class FilterPresetWidget;

/** Helper widget for displaying the input/output/options part of a Filter.
 * Intended to be included in other Filter widgets.
 *
 * \ingroup cxPluginAlgorithm
 * \date Nov 18, 2012
 * \author Christian Askeland, SINTEF
 * \author Janne Beate Bakeng, SINTEF
 */
class FilterSetupWidget : public BaseWidget
{
	Q_OBJECT
public:
	FilterSetupWidget(QWidget* parent, ssc::XmlOptionFile options, bool addFrame);
	void setFilter(FilterPtr filter);
	QString defaultWhatsThis() const;
	/** Compact Mode: one group, hide main input/output
	  */
	void setCompact(bool on);
	void toggleDetailed();

private slots:
	void obscuredSlot(bool obscured);
	void showAdvancedOptions(int state);
	void rebuildOptions();

private:

	ssc::XmlOptionFile mOptions;
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

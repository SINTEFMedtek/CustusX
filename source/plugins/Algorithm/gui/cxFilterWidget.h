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

#include <QStackedLayout>

#include "cxBaseWidget.h"
#include "sscStringDataAdapterXml.h"
#include "cxFilter.h"
#include "cxFilterTimedAlgorithm.h"
#include "cxThresholdPreview.h"

namespace cx
{
class TimedAlgorithmProgressBar;

/** Widget for displaying a list of DataAdapter instances.
 * A stack of widgets is created if the setOptions() is called with different uids.
 *
 * \ingroup cxPluginAlgorithm
 * \date Nov 20, 2012
 * \author christiana
 */
class OptionsWidget : public QWidget
{
    Q_OBJECT
public:
    OptionsWidget(QWidget* parent);

    /**
      * Set options for a given uid. Setting different uid will change the widget
      * content but store the previous contents, making it easy to swap between uids.
      */
    void setOptions(QString uid, std::vector<DataAdapterPtr> options);
    void setOptions(QString uid, std::vector<SelectDataStringDataAdapterBasePtr> options);
    QString getCurrentUid();
//    std::vector<DataAdapterPtr> getOptions(QString uid);
//    std::vector<DataAdapterPtr> getCurrentOptions();

private:
    QStackedLayout* mStackedLayout;
//    std::map<QString, std::vector<DataAdapterPtr> > mOptions;
};

/** Helper widget for displaying the input/output/options part of a Filter.
 * Intended to be included in other Filter widgets.
 *
 * \ingroup cxPluginAlgorithm
 * \date Nov 18, 2012
 * \author christiana
 * \author Janne Beate Bakeng
 */
class FilterSetupWidget : public BaseWidget
{
    Q_OBJECT
public:
    FilterSetupWidget(QWidget* parent, ssc::XmlOptionFile options, bool addFrame);
    void setFilter(FilterPtr filter);
    QString defaultWhatsThis() const;
    /**
      * Add a GroupBox around the widget with the algo name.
      */
    void setNamedFrame(bool on);

private slots:
    void obscuredSlot(bool obscured);
private:

    ssc::XmlOptionFile mOptions;
    FilterPtr mCurrentFilter;

    OptionsWidget* mInputsWidget;
    OptionsWidget* mOutputsWidget;
    OptionsWidget* mOptionsWidget;
    QGroupBox* mFrame;
    boost::shared_ptr<WidgetObscuredListener> mObscuredListener;
};


/** Widget for selecting and running a Filter.
 *
 *  Select one filter from a drop-down list, then set it up
 *  and run it. All available filters in the system should be
 *  in this widget.
 *
 * \ingroup cxPluginAlgorithm
 * \date Nov 18, 2012
 * \author christiana
 * \author Janne Beate Bakeng
 */
class AllFiltersWidget : public BaseWidget
{
    Q_OBJECT
public:
    AllFiltersWidget(QWidget* parent);
    QString defaultWhatsThis() const;

private slots:
    void filterChangedSlot();
    void runFilterSlot();
    void finishedSlot();
private:
    FilterGroupPtr mFilters;
    FilterPtr mCurrentFilter;
    ssc::StringDataAdapterXmlPtr mFilterSelector;
    FilterTimedAlgorithmPtr mThread;

    FilterSetupWidget* mSetupWidget;
    TimedAlgorithmProgressBar* mTimedAlgorithmProgressBar;
};

}

#endif // CXFILTERWIDGET_H

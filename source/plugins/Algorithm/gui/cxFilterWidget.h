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
    std::vector<DataAdapterPtr> getOptions(QString uid);
    std::vector<DataAdapterPtr> getCurrentOptions();

private:
    QStackedLayout* mStackedLayout;
    std::map<QString, std::vector<DataAdapterPtr> > mOptions;
};

/** Dummy implementation of Filter
 *
 * \ingroup cxPluginAlgorithm
 * \date Nov 18, 2012
 * \author christiana
 */
class FilterWidget : public BaseWidget
{
    Q_OBJECT
public:
    FilterWidget(QWidget* parent);

protected:
    QString defaultWhatsThis() const;
private slots:
    void filterChangedSlot();
    void runFilterSlot();
    void finishedSlot();
    void obscuredSlot(bool obscured);
private:
//    DataAdapterPtr createDataAdapter(Filter::ArgumentType type);
    QGroupBox* wrapInGroupBox(QWidget* base, QString name);

    ssc::XmlOptionFile mOptions;
    FilterPtr mCurrentFilter;
    ssc::StringDataAdapterXmlPtr mFilterSelector;
    std::vector<FilterPtr> mAvailableFilters;
    FilterTimedAlgorithmPtr mThread;

    OptionsWidget* mInputsWidget;
    OptionsWidget* mOutputsWidget;
    OptionsWidget* mOptionsWidget;
    TimedAlgorithmProgressBar* mTimedAlgorithmProgressBar;
    boost::shared_ptr<WidgetObscuredListener> mObscuredListener;
};

}

#endif // CXFILTERWIDGET_H

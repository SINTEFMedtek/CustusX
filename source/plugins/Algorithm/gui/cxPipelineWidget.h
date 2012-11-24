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
#ifndef CXPIPELINEWIDGET_H
#define CXPIPELINEWIDGET_H

#include "cxBaseWidget.h"
#include "cxPipeline.h"
class QButtonGroup;
class QRadioButton;
class QAction;
#include "cxFilterWidget.h"

namespace cx
{
class TimedAlgorithmProgressBar;


class PipelineWidgetFilterLine : public BaseWidget
{
    Q_OBJECT
public:
    PipelineWidgetFilterLine(QWidget* parent, FilterPtr filter, QButtonGroup *buttonGroup);
    QString defaultWhatsThis() const;

    QRadioButton* mRadioButton;
    QLabel* mAlgoNameLabel;
    QAction* mAction;
    TimedAlgorithmProgressBar* mTimedAlgorithmProgressBar;
    FilterPtr mFilter;

signals:
    void requestRunFilter();
    void filterSelected(QString uid);

private slots:
    void radioButtonSelectedSlot(bool on);
protected:
    virtual void mousePressEvent(QMouseEvent* event);
};

/** GUI for sequential execution of Filters.
 *
 * \ingroup cxPluginAlgorithms
 * \date Nov 22, 2012
 * \author christiana
 * \author Janne Beate Bakeng, SINTEF
 */
class PipelineWidget : public BaseWidget
{
    Q_OBJECT
public:
    PipelineWidget(QWidget* parent, PipelinePtr pipeline);
    QString defaultWhatsThis() const;
private slots:
    void runFilterSlot();
    void filterSelectedSlot(QString uid);
    void finishedSlot();
private:
    void selectFilter(int index);
    PipelinePtr mPipeline;
    QButtonGroup* mButtonGroup;
    std::vector<PipelineWidgetFilterLine*> mAlgoLines;
    FilterSetupWidget* mSetupWidget;

    FilterTimedAlgorithmPtr mThread;
    PipelineWidgetFilterLine* mCurrentlyRunningPipelineWidgetFilterLine;
};


} // namespace cx

#endif // CXPIPELINEWIDGET_H

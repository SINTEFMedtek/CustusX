/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#ifndef CXOPTIMIZEDUPDATEWIDGET_H_
#define CXOPTIMIZEDUPDATEWIDGET_H_

#include "cxResourceWidgetsExport.h"

#include <QWidget>

namespace cx
{

/** \brief Interface for all classes following the modified/prepaint paradigm.
 *
 * Use this class when the widget is modified several times
 * between each repaint. Instead of doing expensive computations
 * upon setModified(), the computations are rather deferred to
 * a pre paint step.
 * This is similar to the VTK modified/update paradigm.
 *
 * NOTE:
 *  In order for this to work, the widget itself must have a visible region.
 *  If the widget is completely covered by child widgets or other widgets,
 *  no paintEvent will come, and the prePaintEvent will not be called.
 *  See ScalarInteractionWidget for an example of how to hack this if necessary.
 *
 *   \ingroup cx_resource_widgets
 *   \author christiana
 *   \date Nov 15, 2012
 */
class cxResourceWidgets_EXPORT OptimizedUpdateWidget: public QWidget
{
Q_OBJECT
public:
    OptimizedUpdateWidget(QWidget* parent = NULL);
    virtual ~OptimizedUpdateWidget() {}
	void forcePrePaint(); // use to explicitly call the prePaintEvent. Use only for testing!

public slots:
    /**
      * Call to trigger a call to prePaintEvent() prior to next paintEvent()
      */
    virtual void setModified();

protected:
    /**
      * Implement to perform expensive operations that need only be called once
      * per paint. Use setModified() to trigger this method.
      */
    virtual void prePaintEvent() {}
    /**
      * Override. Calls prePaintEvent() provided that setModified()
      * has been called.
      */
    virtual void paintEvent(QPaintEvent* event);

private:
    void prePaintEventPrivate();

    bool mModified; ///< use to compute only prior to paintEvent()
};

} // namespace cx

#endif // CXOPTIMIZEDUPDATEWIDGET_H_

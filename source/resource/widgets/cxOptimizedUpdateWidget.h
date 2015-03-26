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

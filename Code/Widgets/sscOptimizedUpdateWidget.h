// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

#ifndef SSCOPTIMIZEDUPDATEWIDGET_H
#define SSCOPTIMIZEDUPDATEWIDGET_H

#include <QWidget>

namespace ssc
{

/** \brief Interface for all classes following the modified/prepaint paradigm.
 *
 * Use this class when the widget is modified several times
 * between each repaing. Instead of doing expensive computations
 * upon setModified(), the computations are rather deferred to
 * a pre paint step.
 * This is similar to the VTK modified/update paradigm.
 *
 *   \author christiana
 *   \date Nov 15, 2012
 *   \ingroup sscWidget
 */
class OptimizedUpdateWidget: public QWidget
{
Q_OBJECT
public:
    OptimizedUpdateWidget(QWidget* parent = NULL);
    virtual ~OptimizedUpdateWidget() {}

public slots:
    /**
      * Call to trigger a call to prePaintEvent() prior to next paintEvent()
      */
    void setModified();

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

} // namespace ssc

#endif // SSCOPTIMIZEDUPDATEWIDGET_H

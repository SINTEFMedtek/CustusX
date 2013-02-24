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

#ifndef CXTHRESHOLDPREVIEW_H_
#define CXTHRESHOLDPREVIEW_H_

#include <QObject>
class QTimer;
#include "sscForwardDeclarations.h"

namespace cx
{
/**
 * \file
 * \addtogroup cxServicePatient
 * @{
 */

typedef boost::shared_ptr<class ThresholdPreview> ThresholdPreviewPtr;

/**
  * Listen to a widget, then emit signals when it is obscured.
  * This hack solves the issue of tabbed widgets no receiving hide() events,
  * or being !visible at all.
  */
class WidgetObscuredListener : public QObject
{
    Q_OBJECT
public:
    WidgetObscuredListener(QWidget* listenedTo);
    bool isObscured() const;

signals:
    void obscured(bool visible);
private slots:
    void timeoutSlot();
private:
    bool mObscuredAtLastCheck;
    QWidget* mWidget;
    QTimer *mRemoveTimer; ///< Timer for removing segmentation preview coloring if widget is not visible
};

/**
 * \brief Use transfer function to preview a threshold in the selected volume. Used by widgets: segmentation and surface generation
 * \ingroup cxServicePatient
 *
 * \date 12. okt. 2011
 * \author Ole Vegard Solberg, SINTEF
 */
class ThresholdPreview: public QObject
{
Q_OBJECT
public:
	ThresholdPreview();

    void setPreview(ssc::ImagePtr image, double setValue);
    void removePreview();

private:
	void revertTransferFunctions();

	ssc::ImagePtr mModifiedImage; ///< image that have its TF changed temporarily
//	QWidget* mFromWidget; ///< The calling widget
    ssc::ImageTF3DPtr mTF3D_original; ///< original TF of modified image.
	ssc::ImageLUT2DPtr mTF2D_original; ///< original TF of modified image.
	bool mShadingOn_original; ///< Was shading originally enabled in image
//	QTimer *mRemoveTimer; ///< Timer for removing segmentation preview coloring if widget is not visible
};

/**
 * @}
 */
}

#endif /* CXTHRESHOLDPREVIEW_H_ */

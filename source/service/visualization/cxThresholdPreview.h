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

#ifndef CXTHRESHOLDPREVIEW_H_
#define CXTHRESHOLDPREVIEW_H_

#include "cxVisualizationServiceExport.h"

#include <QObject>
class QTimer;
#include "cxForwardDeclarations.h"
#include "cxMathBase.h"

namespace cx
{
/**
 * \file
 * \addtogroup cx_service_visualization
 * @{
 */

typedef boost::shared_ptr<class ThresholdPreview> ThresholdPreviewPtr;

/**
  * Listen to a widget, then emit signals when it is obscured.
  * This hack solves the issue of tabbed widgets no receiving hide() events,
  * or being !visible at all.
  */
class cxVisualizationService_EXPORT WidgetObscuredListener : public QObject
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
 * \ingroup cx_service_visualization
 *
 * \date 12. okt. 2011
 * \author Ole Vegard Solberg, SINTEF
 */
class cxVisualizationService_EXPORT ThresholdPreview: public QObject
{
Q_OBJECT
public:
	ThresholdPreview();

		void setPreview(ImagePtr image, double lower);
		void setPreview(ImagePtr image, const Eigen::Vector2d &threshold);
    void removePreview();

private:
	void revertTransferFunctions();
	void storeOriginalTransferfunctions(ImagePtr image);

	ImagePtr mModifiedImage; ///< image that have its TF changed temporarily
//	QWidget* mFromWidget; ///< The calling widget
    ImageTF3DPtr mTF3D_original; ///< original TF of modified image.
	ImageLUT2DPtr mTF2D_original; ///< original TF of modified image.
	bool mShadingOn_original; ///< Was shading originally enabled in image
//	QTimer *mRemoveTimer; ///< Timer for removing segmentation preview coloring if widget is not visible
};

/**
 * @}
 */
}

#endif /* CXTHRESHOLDPREVIEW_H_ */

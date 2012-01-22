/*
 * cxThresholdPreview.h
 *
 *  Created on: Oct 10, 2011
 *      Author: olevs
 */

#ifndef CXTHRESHOLDPREVIEW_H_
#define CXTHRESHOLDPREVIEW_H_

#include <QTimer>
#include "sscImage.h"

namespace cx
{
/**
* \file
* \addtogroup cxServicePatient
* @{
*/

typedef boost::shared_ptr<class ThresholdPreview> ThresholdPreviewPtr;

/**
 * \brief Use transfer function to preview a threshold in the selected volume. Used by widgets: segmentation and surface generation
 * \ingroup cxServicePatient
 *
 * \date 12. okt. 2011
 * \author: Ole Vegard Solberg, SINTEF
 */
class ThresholdPreview : public QObject
{
  Q_OBJECT
public:
	ThresholdPreview();

	void setPreview(QWidget* fromWidget, ssc::ImagePtr image, double setValue);
	void removePreview(QWidget* fromWidget);

private slots:
	void removeIfNotVisibleSlot();

private:
	void revertTransferFunctions();

  ssc::ImagePtr mModifiedImage; ///< image that have its TF changed temporarily
  QWidget* mFromWidget; ///< The calling widget
  ssc::ImageTF3DPtr mTF3D_original; ///< original TF of modified image.
  ssc::ImageLUT2DPtr mTF2D_original; ///< original TF of modified image.
  bool mShadingOn_original; ///< Was shading originally enabled in image
  QTimer *mRemoveTimer;///< Timer for removing segmentation preview coloring if widget is not visible
};


/**
* @}
*/
}

#endif /* CXTHRESHOLDPREVIEW_H_ */

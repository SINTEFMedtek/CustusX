/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXRECORDSESSIONWIDGET_H_
#define CXRECORDSESSIONWIDGET_H_

#include "org_custusx_acquisition_Export.h"

#include "cxBaseWidget.h"
#include "cxAcquisitionService.h"

class QPushButton;
class QLineEdit;
class QLabel;

namespace cx
{
/**
* \file
* \addtogroup org_custusx_acquisition
* @{
*/

typedef boost::shared_ptr<class AcquisitionService> AcquisitionServicePtr;
typedef boost::shared_ptr<class RecordSessionWidget> RecordSessionWidgetPtr;

/**
 * \class RecordSessionWidget
 *
 * \brief
 *
 * \date Dec 8, 2010
 * \author Janne Beate Bakeng
 */
class org_custusx_acquisition_EXPORT  RecordSessionWidget : public BaseWidget
{
  Q_OBJECT

public:
  RecordSessionWidget(AcquisitionServicePtr base, QWidget* parent,
					  AcquisitionService::TYPES context,
					  QString category);
  virtual ~RecordSessionWidget();

  void setDescription(QString text);
  void setCurrentSession(QString uid); ///< set a session that the next recording adds to

private slots:
  void startStopSlot(bool);
  void cancelSlot();
  void recordStateChangedSlot();
  void onReadinessChanged();

private:

  AcquisitionServicePtr mAcquisitionService;
  QLabel* mInfoLabel;
  QPushButton* mStartStopButton;
  QPushButton* mCancelButton;
  AcquisitionService::TYPES mContext;
  QString mCategory;
  QString mCurrentSession;
};

/**
* @}
*/
}//namespace cx
#endif /* CXRECORDSESSIONWIDGET_H_ */

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

#ifndef CXPOINTSAMPLINGWIDGET_H_
#define CXPOINTSAMPLINGWIDGET_H_

#include "cxBaseWidget.h"

#include <vector>
#include <QtWidgets>

#include "cxForwardDeclarations.h"
#include "cxLandmark.h"

class QVBoxLayout;
class QTableWidget;
class QPushButton;

namespace cx
{

/**
 * \class PointSamplingWidget
 *
 * Early version of metric/label system.
 * Replaced by MetricWidget.
 *
 * \ingroup cx_gui
 * \ingroup cxNotUsed
 *
 * \sa MetricWidget
 * \date 2010.05.05
 * \author Christian Askeland, SINTEF
 */
class PointSamplingWidget : public BaseWidget
{
  Q_OBJECT

public:
  PointSamplingWidget(QWidget* parent);
  virtual ~PointSamplingWidget();

  virtual QString defaultWhatsThis() const;

signals:

protected slots:
	void updateSlot();
	void itemSelectionChanged();

  void addButtonClickedSlot();
  void editButtonClickedSlot();
  void removeButtonClickedSlot();
  void gotoButtonClickedSlot();
  void loadReferencePointsSlot();

protected:
  virtual void showEvent(QShowEvent* event); ///<updates internal info before showing the widget
  virtual void hideEvent(QHideEvent* event);
  void setManualTool(const Vector3D& p_r);
  Vector3D getSample() const;
  void enablebuttons();
  void addPoint(Vector3D point);

  QVBoxLayout* mVerticalLayout; ///< vertical layout is used
  QTableWidget* mTable; ///< the table widget presenting the landmarks
  typedef std::vector<Landmark> LandmarkVector;
  LandmarkVector mSamples;
  QString mActiveLandmark; ///< uid of surrently selected landmark.

  QPushButton* mAddButton; ///< the Add Landmark button
  QPushButton* mEditButton; ///< the Edit Landmark button
  QPushButton* mRemoveButton; ///< the Remove Landmark button
  QPushButton* mLoadReferencePointsButton; ///< button for loading a reference tools reference points

//private:
//  PointSamplingWidget();

};

}//end namespace cx


#endif /* CXPOINTSAMPLINGWIDGET_H_ */

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

#ifndef CXRECONSTRUCTIONWIDGET_H_
#define CXRECONSTRUCTIONWIDGET_H_

#include "org_custusx_usreconstruction_Export.h"

#include <QtWidgets>

#include "cxDoubleWidgets.h"
#include "cxXmlOptionItem.h"
#include "cxLabeledComboBoxWidget.h"
#include "cxFileSelectWidget.h"
#include "cxBaseWidget.h"


namespace cx
{
class TimedAlgorithmProgressBar;
class UsReconstructionService;
typedef boost::shared_ptr<class UsReconstructionService> UsReconstructionServicePtr;

/**
 * \file
 * \addtogroup cx_module_usreconstruction
 * @{
 */

/**
 *  sscReconstructionWidget.h
 *
 *  Created by Ole Vegard Solberg on 5/4/10.
 *
 */
class org_custusx_usreconstruction_EXPORT ReconstructionWidget: public BaseWidget
{
Q_OBJECT
public:
	ReconstructionWidget(QWidget* parent, UsReconstructionServicePtr reconstructer);
	UsReconstructionServicePtr reconstructer()
	{
		return mReconstructer;
	}

public slots:
	void selectData(QString inputfile);
	void reconstruct();
	void reload();
	void paramsChangedSlot();
private slots:
	void inputDataSelected(QString mhdFileName);
	/** Add the widgets for the current algorithm to a stacked widget.*/
	void repopulateAlgorithmGroup();
	void reconstructStartedSlot();
	void reconstructFinishedSlot();
	void toggleDetailsSlot();

	void reconstructAboutToStartSlot();

private:
	UsReconstructionServicePtr mReconstructer;

	FileSelectWidget* mFileSelectWidget;
	QPushButton* mReconstructButton;
	QPushButton* mReloadButton;
	QLineEdit* mExtentLineEdit;
	QLineEdit* mInputSpacingLineEdit;
//	SpinBoxGroupWidget* mMaxVolSizeWidget;
	SpinBoxGroupWidget* mSpacingWidget;
	SpinBoxGroupWidget* mDimXWidget;
	SpinBoxGroupWidget* mDimYWidget;
	SpinBoxGroupWidget* mDimZWidget;
	TimedAlgorithmProgressBar* mTimedAlgorithmProgressBar;

	QFrame* mAlgorithmGroup;
	QStackedLayout* mAlgoLayout;
	std::vector<QWidget*> mAlgoWidgets;

	QWidget* mOptionsWidget;
	QWidget* createOptionsWidget();
	QString getCurrentPath();
	void updateComboBox();
	QString defaultWhatsThis() const;
	void createNewStackedWidget(QString algoName);
};

/**
 * @}
 */
}//namespace
#endif //CXRECONSTRUCTIONWIDGET_H_

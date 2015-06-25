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

#ifndef CXELASTIXWIDGET_H_
#define CXELASTIXWIDGET_H_

#include "cxRegistrationBaseWidget.h"
#include "cxStringPropertyBase.h"
#include "cxFileSelectWidget.h"
#include "cxElastixManager.h"
#include "cxFilePreviewWidget.h"
#include "cxBoolProperty.h"

class QSpinBox;
class QPushButton;
class QLabel;
class QLineEdit;

namespace cx
{
typedef boost::shared_ptr<class FilePathProperty> FilePathPropertyPtr;
class TimedAlgorithmProgressBar;

/**
 * \file
 * \addtogroup org_custusx_registration_method_commandline
 * @{
 */

/**
 * \brief Widget for interfacing to the ElastiX registration package.
 *
 * \date Feb 4, 2012
 * \author Christian Askeland, SINTEF
 */
class ElastixWidget: public RegistrationBaseWidget
{
Q_OBJECT
public:
	ElastixWidget(RegServices services, QWidget* parent = NULL);
	virtual ~ElastixWidget();

public slots:

private slots:
	void registerSlot();
	void userParameterFileSelected(QString filename);
	void elastixChangedSlot();
	void toggleDetailsSlot();
	void savePresetSlot();
	void deletePresetSlot();

protected:
	virtual void prePaintEvent();
private:
	void createUI();
	QWidget* createOptionsWidget();
	FilePathPropertyPtr getExecutable(QDomElement root);
	void recurseParameterFolders(QString root, QStringList* retval);

	QPushButton* mRegisterButton;
	ElastixManagerPtr mElastixManager;

	StringPropertyBasePtr mFixedImage;
	StringPropertyBasePtr mMovingImage;
	FileSelectWidget* mParameterFileWidget0;
	FilePreviewWidget* mFilePreviewWidget;
	cx::TimedAlgorithmProgressBar* mTimedAlgorithmProgressBar;
	QWidget* mOptionsWidget;
};

/**
 * @}
 */
}//namespace cx


#endif /* CXELASTIXWIDGET_H_ */

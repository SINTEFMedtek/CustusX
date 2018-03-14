/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
	ElastixWidget(RegServicesPtr services, QWidget* parent = NULL);
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

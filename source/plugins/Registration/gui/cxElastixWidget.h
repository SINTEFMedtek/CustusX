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

#ifndef CXELASTIXWIDGET_H_
#define CXELASTIXWIDGET_H_

#include "cxRegistrationBaseWidget.h"
#include "sscStringDataAdapter.h"
#include "sscFileSelectWidget.h"
#include "cxElastixManager.h"
#include "cxFilePreviewWidget.h"
#include "sscBoolDataAdapterXml.h"

class QSpinBox;
class QPushButton;
class QLabel;
class QLineEdit;

namespace cx
{
class TimedAlgorithmProgressBar;

/**
 * \file
 * \addtogroup cxPluginRegistration
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
	ElastixWidget(RegistrationManagerPtr regManager, QWidget* parent = NULL);
	virtual ~ElastixWidget();
	virtual QString defaultWhatsThis() const;

public slots:

private slots:
	void registerSlot();
	void userParameterFileSelected(QString filename);
	void browseExecutableSlot();
	void elastixChangedSlot();
	void executableEditFinishedSlot();
	void toggleDetailsSlot();
	void savePresetSlot();
	void deletePresetSlot();

private:
	QWidget* createOptionsWidget();

	QPushButton* mRegisterButton;
	ElastixManagerPtr mElastixManager;

	StringDataAdapterPtr mFixedImage;
	StringDataAdapterPtr mMovingImage;
	FileSelectWidget* mParameterFileWidget0;
	FilePreviewWidget* mFilePreviewWidget;
	QLineEdit* mExecutableEdit;
	cx::TimedAlgorithmProgressBar* mTimedAlgorithmProgressBar;
	//  QPushButton* mLaunchServerButton;
	QWidget* mOptionsWidget;
};

/**
 * @}
 */
}//namespace cx


#endif /* CXELASTIXWIDGET_H_ */

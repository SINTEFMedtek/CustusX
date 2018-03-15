/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTRANSFERFUNCTIONPRESETWIDGET_H_
#define CXTRANSFERFUNCTIONPRESETWIDGET_H_

#include "cxGuiExport.h"

#include "cxPresetWidget.h"

#include "cxTransferFunctions3DPresets.h"
#include "cxDataInterface.h"

class QComboBox;

namespace cx {

/*
 * cxTransferFunctionPresetWidget.h
 *
 * \date Mar 8, 2013
 * \author Janne Beate Bakeng, SINTEF
 */
class cxGui_EXPORT TransferFunctionPresetWidget: public PresetWidget
{

	Q_OBJECT

public:
	TransferFunctionPresetWidget(PatientModelServicePtr patientModelService, QWidget* parent, bool is3D);
	virtual ~TransferFunctionPresetWidget() {}

public slots:
	virtual void resetSlot();
	virtual void saveSlot();
	virtual void deleteSlot();
	virtual void populatePresetListSlot(); ///< Fill the preset list with the available presets (matching active images modality)

	void presetsBoxChangedSlot(const QString& presetName);
	void toggleSlot();

private:
	bool use2D() const;
	bool use3D() const;
	void updateToggles();

	QAction* mToggleAction;
	bool mIs3D; ///< true if 3D, false if 2D
	bool mApplyToAll; ///< true if presets are to be applied to both 2D and 3D
	ActiveImageProxyPtr mActiveImageProxy;
	ActiveDataPtr mActiveData;

};

} //namespace cx

#endif /* CXTRANSFERFUNCTIONPRESETWIDGET_H_ */

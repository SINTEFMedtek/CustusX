/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXGENERALTAB_H_
#define CXGENERALTAB_H_

#include "cxGuiExport.h"

#include <QComboBox>
#include "cxPreferenceTab.h"
#include "cxBoolProperty.h"
class QToolButton;

namespace cx
{

/**
 * \class GeneralTab
 *
 * \brief Tab for general settings in the system
 *
 * \date Jan 25, 2010
 * \author Janne Beate Bakeng, SINTEF
 * \author Ole Vegard Solberg, SINTEF
 * \author Frank Lindseth, SINTEF
 */
class cxGui_EXPORT GeneralTab : public PreferenceTab
{
  Q_OBJECT

public:
  GeneralTab(ViewServicePtr viewService, PatientModelServicePtr patientModelService, QWidget *parent = 0);
  virtual ~GeneralTab();
  void init();

public slots:
  void saveParametersSlot();

private slots:
  void browsePatientDataFolderSlot();
  void browseVLCPathSlot();
  void onAddProfile();
	void onProfileSelected();
	void onProfileChanged();
	void selectProfile(QString uid);

private:
	void rejectDialog();
  void searchForVLC(QStringList searchPaths = QStringList());
  QToolButton* createAddProfileButton();
  StringPropertyPtr getProfileSelector();

  QComboBox* mPatientDataFolderComboBox;
  QComboBox* mVLCPathComboBox;

  QComboBox* mToolConfigFolderComboBox;

  BoolPropertyPtr mFilterToolPositions;
  DoublePropertyPtr mFilterToolPositionsCutoff;

  QString mGlobalPatientDataFolder;
  QString mVLCPath;
  ViewServicePtr mViewService;
  PatientModelServicePtr mPatientModelService;
  StringPropertyPtr mSelector;

};
} /* namespace cx */
#endif /* CXGENERALTAB_H_ */

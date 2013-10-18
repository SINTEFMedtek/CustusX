#ifndef CXGENERALTAB_H_
#define CXGENERALTAB_H_

#include <QComboBox>
#include "cxPreferenceTab.h"

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
class GeneralTab : public PreferenceTab
{
  Q_OBJECT

public:
  GeneralTab(QWidget *parent = 0);
  virtual ~GeneralTab();
  void init();

public slots:
  void saveParametersSlot();

private slots:
  void browsePatientDataFolderSlot();

  void currentApplicationChangedSlot(int index);
  void applicationStateChangedSlot();

private:
  void setApplicationComboBox();

  QComboBox* mPatientDataFolderComboBox;

  QComboBox* mToolConfigFolderComboBox;
  QComboBox* mChooseApplicationComboBox;

  QString mGlobalPatientDataFolder;
};
} /* namespace cx */
#endif /* CXGENERALTAB_H_ */

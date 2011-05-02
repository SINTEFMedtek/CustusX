#ifndef cxLayoutEditorTab_h__
#define cxLayoutEditorTab_h__

#include "cxPreferencesDialog.h"
#include "cxLayoutEditorWidget.h"

class QLabel;
class QComboBox;

namespace cx
{
/**
* \class LayoutEditorTab
*
* \brief
*
* \date Apr 29, 2011
* \author Janne Beate Bakeng, SINTEF
*/
class LayoutEditorTab : public PreferencesTab
{
  Q_OBJECT

public:
  LayoutEditorTab(QWidget* parent = 0);
  virtual ~LayoutEditorTab();
  
  virtual void init();

public slots:
  virtual void saveParametersSlot();

private slots:
  void layoutToEditChangedSlot(const QString& uid);

private:
  void populateEditableLayouts(QString selectedLayoutUid);

  LayoutEditorWidget* mLayoutEditorWidget;
  QLabel*             mLayoutLabel;
  QComboBox*          mLayoutList;
};

}//namespace cx
#endif // cxLayoutEditorTab_h__


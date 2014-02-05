#ifndef cxLayoutEditorTab_h__
#define cxLayoutEditorTab_h__

#include "cxPreferencesDialog.h"
#include "cxLayoutEditorWidget.h"

class QLabel;
class QComboBox;
class QPushButton;

namespace cx
{
/**
* \class LayoutEditorTab
*
* \brief
* \ingroup cxGUI
*
* \date Apr 29, 2011
* \author Janne Beate Bakeng, SINTEF
*/
class LayoutEditorTab : public PreferenceTab
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
  void deleteCustomLayoutSlot();
  void saveLayoutSlot();

private:
  void populateEditableLayouts(QString selectedLayoutUid);
  void addCustomLayoutToDisplayList( QString displayName, QString uid );
  void setToActiveLayout();
  LayoutData updateLayoutToEdit(const QString& name);

  LayoutEditorWidget* mLayoutEditorWidget;
  QLabel*             mLayoutLabel;
  QComboBox*          mLayoutList;
  QPushButton*        mDeleteButton;
  QPushButton*        mSaveLayoutButton;
};

}//namespace cx
#endif // cxLayoutEditorTab_h__


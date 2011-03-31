#ifndef CXSELECTIONGROUPBOX_H_
#define CXSELECTIONGROUPBOX_H_

#include <QGroupBox>

class QButtonGroup;
class QHBoxLayout;

namespace cx
{
/**
 * SelectionGroupBox
 *
 * \brief
 *
 * \date Mar 30, 2011
 * \author Janne Beate Bakeng, SINTEF
 */

class SelectionGroupBox : public QGroupBox
{
  Q_OBJECT

public:
  SelectionGroupBox(QString title, QStringList selectionList, bool exclusive = false, QWidget* parent = NULL);
  virtual ~SelectionGroupBox();

  virtual QStringList getSelected(); ///< get a list of selected button text
  virtual void setSelected(QStringList selectedlist); ///< set the selected buttons

  void setEnabledButtons(bool value); ///< enables or disables all buttons

signals:
  void selectionChanged(); ///< emitted when the selection of application changed
  void userClicked(); ///< emitted when a user has clicked a button inside the groupbox

private:
  void populate(bool exclusive); ///< Populates the group box
  void filter(QStringList filter); ///< Selects the buttons with text matching the incoming list

  QHBoxLayout* mLayout;

  QStringList     mSelectionList;
  QButtonGroup*   mButtonGroup;
};
}//namespace cx
#endif /* CXSELECTIONGROUPBOX_H_ */

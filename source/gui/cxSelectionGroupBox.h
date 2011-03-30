#ifndef CXSELECTIONGROUPBOX_H_
#define CXSELECTIONGROUPBOX_H_

#include <QGroupBox>

class QButtonGroup;

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

signals:
  void selectionChanged(); ///< emitted when the selection of application changed

private:
  void populate(bool exclusive); ///< Populates the group box

  QStringList     mSelectionList;
  QButtonGroup*   mButtonGroup;
};
}//namespace cx
#endif /* CXSELECTIONGROUPBOX_H_ */

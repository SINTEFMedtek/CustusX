#ifndef SSCLABELEDLINEEDITWIDGET_H_
#define SSCLABELEDLINEEDITWIDGET_H_

#include <QWidget>
#include <QSlider>
#include <QLineEdit>
#include <QLabel>
#include <QGridLayout>
#include "sscStringDataAdapter.h"

namespace ssc
{

/**Composite widget for string selection.
 * Consists of <namelabel, combobox>.
 * Insert a subclass of ssc::StringDataAdStringDataAdapter to connect to data.
 */
class LabeledLineEditWidget : public QWidget
{
  Q_OBJECT
public:
  LabeledLineEditWidget(QWidget* parent, EditableStringDataAdapterPtr, QGridLayout* gridLayout=0, int row=0);
private slots:
  void dataChanged();
  void editingFinished();
private:
  QLabel* mLabel;
  QLineEdit* mLine;
  EditableStringDataAdapterPtr mData;
};

} // namespace ssc

#endif /* SSCLABELEDLINEEDITWIDGET_H_ */

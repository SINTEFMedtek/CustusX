#ifndef SSCLABELCOMBOBOXWIDGET_H_
#define SSCLABELCOMBOBOXWIDGET_H_

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
class LabeledComboBoxWidget : public QWidget
{
  Q_OBJECT
public:
  LabeledComboBoxWidget(QWidget* parent, StringDataAdapterPtr, QGridLayout* gridLayout=0, int row=0);
private slots:
  void dataChanged();
  void comboIndexChanged(int val);
private:
  QLabel* mLabel;
  QComboBox* mCombo;
  StringDataAdapterPtr mData;
};

} // namespace ssc

#endif /* SSCLABELCOMBOBOXWIDGET_H_ */

/*
 * sscStringWidgets.h
 *
 *  Created on: Jun 23, 2010
 *      Author: christiana
 */
#ifndef SSCSTRINGWIDGETS_H_
#define SSCSTRINGWIDGETS_H_

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
 * Insert a subclass of ssc::StringDataAdStringDataAdaptero connect to data.
 */
class ComboGroupWidget : public QWidget
{
  Q_OBJECT
public:
  ComboGroupWidget(QWidget* parent, StringDataAdapterPtr, QGridLayout* gridLayout=0, int row=0);
private slots:
  void dataChanged();
  void comboIndexChanged(int val);
private:
  QLabel* mLabel;
  QComboBox* mCombo;
  StringDataAdapterPtr mData;
};

} // namespace ssc

#endif /* SSCSTRINGWIDGETS_H_ */

/*
 * cxVolumePropertiesWidget.h
 *
 *  Created on: Aug 20, 2010
 *      Author: christiana
 */

#ifndef CXVOLUMEPROPERTIESWIDGET_H_
#define CXVOLUMEPROPERTIESWIDGET_H_

#include <QWidget>
#include "sscForwardDeclarations.h"
#include "sscStringDataAdapter.h"
class QComboBox;

namespace cx
{

/** Adapter that connects to the current active image.
 */
class ActiveImageStringDataAdapter : public ssc::StringDataAdapter
{
  Q_OBJECT
public:
  static ssc::StringDataAdapterPtr New() { return ssc::StringDataAdapterPtr(new ActiveImageStringDataAdapter()); }
  ActiveImageStringDataAdapter();
  virtual ~ActiveImageStringDataAdapter() {}

public: // basic methods
  virtual QString getValueName() const;
  virtual bool setValue(const QString& value);
  virtual QString getValue() const;

public: // optional methods
  virtual QString getHelp() const;
  virtual QStringList getValueRange() const;
  virtual QString convertInternal2Display(QString internal);
};

/** Widget that contains a select active image combo box.
 */
class ActiveVolumeWidget : public QWidget
{
  Q_OBJECT
public:
  ActiveVolumeWidget(QWidget* parent);
  ~ActiveVolumeWidget() {}
};


/** Widget for displaying and manipulating various 3D Volume properties.
 *
 */
class VolumePropertiesWidget : public QWidget
{
  Q_OBJECT
public:
  VolumePropertiesWidget(QWidget* parent);

};

}//namespace cx


#endif /* CXVOLUMEPROPERTIESWIDGET_H_ */

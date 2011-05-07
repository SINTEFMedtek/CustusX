/*
 * cxClippingWidget.h
 *
 *  Created on: Aug 25, 2010
 *      Author: christiana
 */

#ifndef CXCLIPPINGWIDGET_H_
#define CXCLIPPINGWIDGET_H_

#include <QWidget>
#include "sscForwardDeclarations.h"
#include "sscStringDataAdapter.h"
#include "cxViewManager.h"
class QCheckBox;

namespace cx
{

/** Adapter that connects to the current active image.
 */
class ClipPlaneStringDataAdapter : public ssc::StringDataAdapter
{
  Q_OBJECT
public:
  static ssc::StringDataAdapterPtr New(InteractiveClipperPtr clipper) { return ssc::StringDataAdapterPtr(new ClipPlaneStringDataAdapter(clipper)); }
  ClipPlaneStringDataAdapter(InteractiveClipperPtr clipper);
  virtual ~ClipPlaneStringDataAdapter() {}

public: // basic methods
  virtual QString getValueName() const;
  virtual bool setValue(const QString& value);
  virtual QString getValue() const;

public: // optional methods
  virtual QString getHelp() const;
  virtual QStringList getValueRange() const;

  InteractiveClipperPtr mInteractiveClipper;
};


/**
 *
 */
class ClippingWidget : public QWidget
{
  Q_OBJECT
public:
  ClippingWidget(QWidget* parent);
private:
  InteractiveClipperPtr mInteractiveClipper;

  QCheckBox* mUseClipperCheckBox;
  QCheckBox* mInvertPlaneCheckBox;
  ssc::StringDataAdapterPtr mPlaneAdapter;
private slots:
  void clipperChangedSlot();
  void clearButtonClickedSlot();
  void saveButtonClickedSlot();
};

}//namespace cx

#endif /* CXCLIPPINGWIDGET_H_ */

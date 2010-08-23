/*
 * cxCroppingWidget.h
 *
 *  Created on: Aug 20, 2010
 *      Author: christiana
 */

#ifndef CXCROPPINGWIDGET_H_
#define CXCROPPINGWIDGET_H_

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
class CroppingWidget : public QWidget
{
  Q_OBJECT
public:
  CroppingWidget(QWidget* parent);
private:
  InteractiveClipperPtr mInteractiveClipper;

  QCheckBox* mUseClipperCheckBox;
  QCheckBox* mInvertPlaneCheckBox;
  ssc::StringDataAdapterPtr mPlaneAdapter;
private slots:
  void clipperChangedSlot();
};

}//namespace cx


#endif /* CXCROPPINGWIDGET_H_ */

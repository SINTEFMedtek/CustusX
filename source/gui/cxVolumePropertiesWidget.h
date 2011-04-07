#ifndef CXVOLUMEPROPERTIESWIDGET_H_
#define CXVOLUMEPROPERTIESWIDGET_H_

#include "cxWhatsThisWidget.h"
#include "sscForwardDeclarations.h"
#include "cxDataInterface.h"

namespace cx
{
/**
 * \class ActiveVolumeWidget
 * \brief Widget that contains a select active image combo box.
 * \date Aug 20, 2010
 * \author christiana
 */
class ActiveVolumeWidget : public WhatsThisWidget
{
  Q_OBJECT
public:
  ActiveVolumeWidget(QWidget* parent);
  ~ActiveVolumeWidget() {}

  virtual QString defaultWhatsThis() const;
};

/**Widget for displaying various volume information.
 * Part of the VolumePropertiesWidget.
 */
class VolumeInfoWidget : public WhatsThisWidget
{
  Q_OBJECT

public:
  VolumeInfoWidget(QWidget* parent);
  virtual ~VolumeInfoWidget();

  virtual QString defaultWhatsThis() const;

protected slots:
  void updateSlot();
  void deleteDataSlot();

private:
  ParentFrameStringDataAdapterPtr mParentFrameAdapter;
  DataNameEditableStringDataAdapterPtr mNameAdapter;
  DataUidEditableStringDataAdapterPtr mUidAdapter;
};

/**
 * \class VolumePropertiesWidget
 * \brief Widget for displaying and manipulating various 3D Volume properties.
 * \date Aug 20, 2010
 * \author christiana
 */
class VolumePropertiesWidget : public WhatsThisWidget
{
  Q_OBJECT
public:
  VolumePropertiesWidget(QWidget* parent);
  virtual ~VolumePropertiesWidget(){};

  virtual QString defaultWhatsThis() const;
};

}//namespace cx


#endif /* CXVOLUMEPROPERTIESWIDGET_H_ */

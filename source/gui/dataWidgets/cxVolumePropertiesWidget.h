#ifndef CXVOLUMEPROPERTIESWIDGET_H_
#define CXVOLUMEPROPERTIESWIDGET_H_

#include "cxBaseWidget.h"
#include "sscForwardDeclarations.h"
#include "cxDataInterface.h"
#include "cxActiveImageProxy.h"

#include <QTextBrowser>

namespace cx
{
/**
 * \class ActiveVolumeWidget
 * \brief Widget that contains a select active image combo box.
 * \ingroup cxGUI
 * \date Aug 20, 2010
 * \author christiana
 */
class ActiveVolumeWidget : public BaseWidget
{
  Q_OBJECT
public:
  ActiveVolumeWidget(QWidget* parent);
  ~ActiveVolumeWidget() {}

  virtual QString defaultWhatsThis() const;
};

/**\brief Widget for displaying various volume information.
 *
 * \ingroup cxGUI
 *
 * Part of the VolumePropertiesWidget.
 */
class VolumeInfoWidget : public BaseWidget
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
  DataModalityStringDataAdapterPtr mModalityAdapter;
  ImageTypeStringDataAdapterPtr mImageTypeAdapter;
  ActiveImageProxyPtr mActiveImageProxy;

  QTextBrowser* mTextBrowser;
};

/**
 * \class VolumePropertiesWidget
 * \brief Widget for displaying and manipulating various 3D Volume properties.
 * \ingroup cxGUI
 *
 * \date Aug 20, 2010
 * \author christiana
 */
class VolumePropertiesWidget : public BaseWidget
{
  Q_OBJECT
public:
  VolumePropertiesWidget(QWidget* parent);
  virtual ~VolumePropertiesWidget(){};

  virtual QString defaultWhatsThis() const;
};

}//namespace cx


#endif /* CXVOLUMEPROPERTIESWIDGET_H_ */

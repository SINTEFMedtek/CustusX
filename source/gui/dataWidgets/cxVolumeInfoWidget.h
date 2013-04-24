#ifndef CXVOLUMEINFOWIDGET_H_
#define CXVOLUMEINFOWIDGET_H_

#include "cxBaseWidget.h"

#include "sscForwardDeclarations.h"
#include "cxDataInterface.h"

namespace cx
{

/**\brief Widget for displaying various volume information.
 *
 * \ingroup cxGUI
 *
 * \author Christian Askeland, SINTEF
 * \author Janne Beate Bakeng, SINTEF
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
  //void deleteDataSlot();

private:
  ParentFrameStringDataAdapterPtr mParentFrameAdapter;
  DataNameEditableStringDataAdapterPtr mNameAdapter;
  DataUidEditableStringDataAdapterPtr mUidAdapter;
  DataModalityStringDataAdapterPtr mModalityAdapter;
  ImageTypeStringDataAdapterPtr mImageTypeAdapter;
  ActiveImageProxyPtr mActiveImageProxy;
};

}//namespace cx



#endif /* CXVOLUMEINFOWIDGET_H_ */

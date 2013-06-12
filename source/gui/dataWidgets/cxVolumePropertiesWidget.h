#ifndef CXVOLUMEPROPERTIESWIDGET_H_
#define CXVOLUMEPROPERTIESWIDGET_H_

#include "cxBaseWidget.h"
#include "sscForwardDeclarations.h"
#include "cxDataInterface.h"
#include "cxActiveImageProxy.h"

namespace cx
{
/**
 * \class ActiveVolumeWidget
 * \brief Widget that contains a select active image combobox.
 * \ingroup cxGUI
 * \date Aug 20, 2010
 * \author Christian Askeland, SINTEF
 */
class ActiveVolumeWidget : public BaseWidget
{
  Q_OBJECT
public:
  ActiveVolumeWidget(QWidget* parent);
  ~ActiveVolumeWidget() {}

  virtual QString defaultWhatsThis() const;
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

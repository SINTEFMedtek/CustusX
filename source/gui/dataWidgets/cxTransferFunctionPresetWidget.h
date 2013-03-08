#ifndef CXTRANSFERFUNCTIONPRESETWIDGET_H_
#define CXTRANSFERFUNCTIONPRESETWIDGET_H_

#include "cxPresetWidget.h"

#include "sscPresetTransferFunctions3D.h"
#include "cxDataInterface.h"

class QComboBox;

namespace cx{

/*
 * cxTransferFunctionPresetWidget.h
 *
 * \date Mar 8, 2013
 * \author Janne Beate Bakeng, SINTEF
 */
class TransferFunctionPresetWidget : public PresetWidget
{
  Q_OBJECT

public:
  TransferFunctionPresetWidget(QWidget* parent, bool is3D);
  virtual ~TransferFunctionPresetWidget() {}
  virtual QString defaultWhatsThis() const;

public slots:
	void generatePresetListSlot(); ///< Fill the preset list with the available presets (matching active images modality)
  void presetsBoxChangedSlot(const QString& presetName);
  void resetSlot();
  void saveSlot();
  void deleteSlot();
  void toggleSlot();

protected:
  bool use2D() const;
  bool use3D() const;
  void updateToggles();

  QVBoxLayout* mLayout;
  QComboBox* mPresetsComboBox;
  ssc::PresetTransferFunctions3DPtr mPresets;
  QAction* mToggleAction;
  bool mIs3D; ///< true if 3D, false if 2D
  bool mApplyToAll; ///< true if presets are to be applied to both 2D and 3D

  ActiveImageProxyPtr mActiveImageProxy;
};

}//namespace cx

#endif /* CXTRANSFERFUNCTIONPRESETWIDGET_H_ */

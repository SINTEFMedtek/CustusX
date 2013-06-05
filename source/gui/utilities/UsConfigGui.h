#ifndef USCONFIGGUI_H_
#define USCONFIGGUI_H_

#include <QWidget>
#include "ProbeXmlConfigParser.h"

class QGridLayout;
class QComboBox;
/**
 * \class UsConfigGui.h
 *
 * \brief A class that gives a gui for displaying a ProbeCalibConfigs.xml file
 * \ingroup cxGUI
 *
 * \date 11. mai 2010
 * \\author jbake
 */
class UsConfigGui : public QWidget
{
    Q_OBJECT

public:
  UsConfigGui(QWidget* parent); ///< makes a widget and tries to fill it with info from ProbeCalibConfigs.xml (if it exists inside the same folder as the application)
  virtual ~UsConfigGui();
  
  QStringList getConfigurationString(); ///< returns a combo of the currently selected scanner, probe, rtsource and configId
  ProbeXmlConfigParser::Configuration getConfiguration(); ///< returns the currently selected configuration
  void setXml(QString filename); ///< use this if you don't want to use the default xml file from the applications folder
  void setEditMode(bool on); ///< when in edit mode scanner, probe and config comboboxes are editable
  
signals:
  void probeSelected(const QString& probe); ///< emitted when a user selects a probe
  void configurationChanged(); ///< emmited whenever the configuration changed
  
public slots:
  void RTsourceDetected(const QString& source);
  
protected slots:
  void scannerChanged(const QString& scanner); ///< reacts when the user selected a new scanner from a combobox
  void probeChanged(const QString& probe); ///< reacts when the user selected a new probe from a combobox
  void rtSourceChanged(const QString& rtSource); ///< reacts when the user selected a new resource from a combobox
  void configIdChanged(const QString& configId); ///< reacts when the user selected a new configid from a combobox

protected:
  void initComboBoxes(); ///< initializes the comboboxs, tries to use values saved in Settings
  void populateScannerBox(const QString& tryToSelect); ///< fills the scanner combobox and tries to set a specific item as selected
  void populateProbeBox(const QString& tryToSelect); ///< fills the probe combobox and tries to set a specific item as selected
  void populateRtSourceBox(const QString& tryToSelect); ///< fills the rtsource combobox and tries to set a specific item as selected
  void populateConfigIdBox(const QString& tryToSelect); ///< fills the configid combobox and tries to set a specific item as selected

  QString               mXmlFileName;	///< filename of the currently used ProbeCalibConfigs.xml file
  ProbeXmlConfigParser* mXml; ///< the xml parser

  QGridLayout*          mLayout; ///< the used lauout

  QComboBox*            mScannerBox; ///< List of available US scanners
  QComboBox*            mProbeBox; ///< List of probes
  QComboBox*            mRtSourceBox; ///< List of video grabber sources
  QComboBox*            mConfigIdBox; ///< List of probe config parameters
};

#endif /* USCONFIGGUI_H_ */

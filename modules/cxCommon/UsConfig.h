/*
 *  UsConfig.h
 *  CustusX3
 *
 *  Created by Ole Vegard Solberg on 3/11/10.
 *  Copyright 2010 SINTEF. All rights reserved.
 *
 */
#ifndef USCONFIG_H_
#define USCONFIG_H_


#include <qwidget.h>
#include <qcombobox.h>

class QDomDocument;
class QGridLayout;
class QDomNode;
class QString;
class QSettings;

class UsConfig : public QWidget
{
    Q_OBJECT

public:
  UsConfig(QWidget* parent);
  ~UsConfig();

  QStringList getConfiguration();

  QString getCurrentScanner() {return mScannerBox->currentText();}; ///< \return currently selected scanner
  QString getCurrentProbe()   {return mProbeBox->currentText();};   ///< \return currently selected probe
  QString getCurrentRtSource(){return mRtSourceBox->currentText();};///< \return currently selected RT source
  QString getCurrentConfig()  {return mConfigBox->currentText();};  ///< \return currently selected probe config

  int getWidthDeg()   {return mWidthDeg;};  ///< Get selecetd width
  int getDepth()      {return mDepth;};     ///< Get selecetd depth
  int getOffset()     {return mOffset;};    ///< Get selecetd Offset
  int getLeftEdge()   {return mLeftEdge;};  ///< Get selecetd LeftEdge
  int getRightEdge()  {return mRightEdge;}; ///< Get selecetd RightEdge
  int getTopEdge()    {return mTopEdge;};   ///< Get selecetd TopEdge
  int getBottomEdge() {return mBottomEdge;};///< Get selecetd BottomEdge
  int getOriginCol()  {return mOriginCol;}; ///< Get selecetd Origin.Col
  int getOriginRow()  {return mOriginRow;}; ///< Get selecetd Origin.Row
  
signals:
  void probeSelected(const QString& probe); ///< signal that tells what probe is currently selected by the user

protected slots:
  void populateProbeNamesSlot(const QString& scanner);///< Populate the probe combo box for a selected scanner
  void populateRtSourceSlot(const QString& probe);///< Populate the source combo boxe for a selecetd probe
  void populateProbePropertiesSlot(const QString& rtSource);///< Populate the probe parameters combo boxes for a selecetd source
  void readConfigParametersSlot(const QString& config);///< Read the config parameters for a selected configuration
  void probeSelectedSlot(const QString& probe); ///< sends out a signal that tells everyone that a new probe has been selected
  
protected:
  void initScannerList();///< Polulates the scanner list

  QDomNode findScanner(QString scannerName); ///< Find named ultrasound scanner \return the QDomNode for the scanner. If not found returns an empty node, check with isNull().
  QDomNode findProbe(QString scannerName, QString probeName); ///< Find named ultrasound probe \return the QDomNode for the probe. If not found returns an empty node, check with isNull().
  QDomNode findRtSource(QString scannerName, QString probeName, QString rtSource); ///< Find named RT source. \return QDomNode for source.
  QDomNode findConfig(QString configName); ///< Find named config setting \return QDomNode for config.

  QDomDocument* mDoc; ///< The ultrasound config XML document
  QGridLayout* mLayout; ///< The used lauout
  
  QComboBox* mScannerBox; ///< List of available US scanners
  QComboBox* mProbeBox; ///< List of probes
  QComboBox* mRtSourceBox; ///< List of video grabber sources
  QComboBox* mConfigBox; ///< List of probe config parameters

  typedef std::pair<int,int> ColRowPair;
  int mWidthDeg;  ///< Selecetd width
  int mDepth;     ///< Selected depth
  int mOffset;    ///< Selecetd Offset
  int mOriginCol; ///< Selecetd Origin.Col
  int mOriginRow; ///< Selecetd Origin.Row
  int mNCorners;  ///< Selected number of corners
  std::vector<ColRowPair> mCorners; ///< Selected corners <col,row>
  int mLeftEdge;  ///< Selecetd LeftEdge
  int mRightEdge; ///< Selecetd RightEdge
  int mTopEdge;   ///< Selecetd TopEdge
  int mBottomEdge;///< Selecetd BottomEdge
  double mPixelWidth;///< Selected Pixel width
  double mPixelHeight;///< Selected Pixel height

  QSettings* mSettings; ///< Settings that should be saved from session to session
};
#endif //USCONFIG_H_

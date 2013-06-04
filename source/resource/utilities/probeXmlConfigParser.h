#ifndef PROBEXMLCONFIGPARSER_H_
#define PROBEXMLCONFIGPARSER_H_

#include <math.h>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <QDomDocument>
#include "sscXmlOptionItem.h"


/**
* \file
* \addtogroup cxResourceUtilities
* @{
*/

/**
 * \brief Class that works as a interface to a ProbeCalibConfigs.xml
 *
 * \date 11. mai 2010
 * \author jbake
 * \author Ole Vegard Solberg, SINTEF
 */
class ProbeXmlConfigParser
{
public:
  typedef std::pair<double,double> ColRowPair;
  struct Configuration ///< a easy-to-work-with struct for a specific xml configuration
  {
    QString             mUsScanner;  ///<  scanner
    QString             mUsProbe;    ///<  probe
    QString             mRtSource;   ///<  realtime source
    QString             mConfigId;   ///<  config id
    QString             mName;       ///<  Name of config set
    int                     mImageWidth;      ///< Width of the used image format (x dim)
    int                     mImageHeight;     ///< Height of the used image format (y dim)
    double                   mWidthDeg;   ///<  width in degrees
    double                   mDepth;      ///<  depth
    double                   mOffset;     ///<  Offset
    double                   mOriginCol;  ///<  Origin.Col
    double                   mOriginRow;  ///<  Origin.Row
    int                     mNCorners;   ///<  number of corners
    std::vector<ColRowPair> mCorners;    ///<  corners <col,row>
    int                     mLeftEdge;   ///<  LeftEdge
    int                     mRightEdge;  ///<  RightEdge
    int                     mTopEdge;    ///<  TopEdge
    int                     mBottomEdge; ///<  BottomEdge
    double                  mPixelWidth; ///<  Pixel width
    double                  mPixelHeight;///<  Pixel height
    int                     mHorizontalOffset; ///< parameter for the grabber
    QString                 mNotes; ///< useful information
    double                  mTemporalCalibration; ///< delay in timestamping in grabber source relative to master clock.
//    int                     mColumns; ///< X dim
//    int                     mRows;    ///< Y dim
    
    bool mEmpty;
    
    Configuration() :
    	mImageWidth(0),
    	mImageHeight(0),
    	mWidthDeg(0),
    	mDepth(0),
    	mOffset(0),
    	mOriginCol(0),
    	mOriginRow(0),
    	mNCorners(0),
    	mLeftEdge(0),
    	mRightEdge(0),
    	mTopEdge(0),
    	mBottomEdge(0),
    	mPixelWidth(0),
    	mPixelHeight(0),
    	mHorizontalOffset(0),
    	mTemporalCalibration(0),
    	mEmpty(true)
    {};
    bool isEmpty(){return mEmpty;};
  };

  //ProbeXmlConfigParser(QString& pathToXml) {}; ///< opens the file and reads it onto the QDomDocument
  //~ProbeXmlConfigParser() {};
  virtual QString getFileName() = 0;
  virtual void removeConfig(QString scanner, QString probe, QString rtsource, QString configId) = 0;
  virtual void saveCurrentConfig(Configuration config) = 0;
  virtual QStringList getScannerList() = 0; ///< get a list of all scanner in the xml
  virtual QStringList getProbeList(QString scanner) = 0; ///< get a list of all probes for that scanner
  virtual QStringList getRtSourceList(QString scanner, QString probe) = 0; ///< get a list of rt sources for that scanner/probe combo
  virtual QStringList getConfigIdList(QString scanner, QString probe, QString rtSource) = 0; ///< get a list of config ids for that scanner/probe/rsource combo
  virtual Configuration getConfiguration(QString scanner, QString probe, QString rtsource, QString configId) = 0; ///< get a easy-to-work-with struct of a specific config

};
typedef boost::shared_ptr<ProbeXmlConfigParser> ProbeXmlConfigParserPtr;

#endif /* PROBEXMLCONFIGPARSER_H_ */

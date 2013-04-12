#ifndef PROBEXMLCONFIGPARSER_H_
#define PROBEXMLCONFIGPARSER_H_

#include <math.h>
#include <vector>
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
 * \\author jbake
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

  ProbeXmlConfigParser(QString& pathToXml); ///< opens the file and reads it onto the QDomDocument
  ~ProbeXmlConfigParser();

	void removeConfig(QString scanner, QString probe, QString rtsource, QString configId);
	void saveCurrentConfig(Configuration config);

  QStringList getScannerList(); ///< get a list of all scanner in the xml
  QStringList getProbeList(QString scanner); ///< get a list of all probes for that scanner
  QStringList getRtSourceList(QString scanner, QString probe); ///< get a list of rt sources for that scanner/probe combo
  QStringList getConfigIdList(QString scanner, QString probe, QString rtSource); ///< get a list of config ids for that scanner/probe/rsource combo
  
  Configuration getConfiguration(QString scanner, QString probe, QString rtsource, QString configId); ///< get a easy-to-work-with struct of a specific config

protected:
//  template<class TYPE> bool readTextNode(TYPE* retval, QDomNode parent, QString name);
//  template<> bool readTextNode<QString>(QString* retval, QDomNode parent, QString name);


  QList<QDomNode> getScannerNodes(QString scanner = "ALL"); ///< get a list of ALL scanner nodes or just the one you are looking for
  QList<QDomNode> getProbeNodes(QString scanner, QString probe = "ALL"); ///< get a list of ALL probenodes for that scanner, or just the one you are looking for
  QList<QDomNode> getRTSourceNodes(QString scanner, QString probe, QString rtSource="ALL"); ///< get a list of ALL rtsourcenodes for that scanner/probe combo, or just the one you are looking for
  QList<QDomNode> getConfigNodes(QString scanner, QString probe, QString rtsource, QString config="ALL"); ///< get a list of ALL confignodes for that scanner/probe/rtsource combo, or just the one you are looking for
  
  QList<QDomNode> nodeListToListOfNodes(QDomNodeList list); ///< converts a QDomNodeList to a QList<QDomNode> 
  void addTextElement(QDomElement parent, QString element, QString text);

//  QDomDocument mDomDoc; ///< the dom document to parse
  ssc::XmlOptionFile mFile;
};


//------------------------------------------------------------------------------
/*
http://stackoverflow.com/questions/1709283/how-can-i-sort-a-coordinate-list-for-a-rectangle-counterclockwise

I need to sort a coordinate list for a rectangle counterclockwise, and make the north-east corner the first coordinate. 

solution seems pretty straightforward:

>>> import math
>>> mlat = sum(x['lat'] for x in l) / len(l)
>>> mlng = sum(x['lng'] for x in l) / len(l)
>>> def algo(x):
    return (math.atan2(x['lat'] - mlat, x['lng'] - mlng) + 2 * math.pi) % 2*math.pi

>>> l.sort(key=algo)
basically, algo normalises the input into the [0, 2pi] space and it would be naturally sorted "counter-clockwise".
*/

//------------------------------------------------------------------------------
class Angular_less
{
public:
  Angular_less(ProbeXmlConfigParser::ColRowPair center) : mCenter(center)
  {
  }
  
  bool operator()(const ProbeXmlConfigParser::ColRowPair& rhs, const ProbeXmlConfigParser::ColRowPair& lhs)
  {
    return angle(rhs) < angle(lhs);
  }
  
  int angle(ProbeXmlConfigParser::ColRowPair pair1)
  {
	double pi = atan(1.0)*4;
    return fmod(  ( ( atan2((double)(pair1.first - mCenter.first), (double)(pair1.second - mCenter.second)) ) + 3*pi/4) , 2*pi);
  }
  ProbeXmlConfigParser::ColRowPair mCenter;
};

/**
* @}
*/


#endif /* PROBEXMLCONFIGPARSER_H_ */

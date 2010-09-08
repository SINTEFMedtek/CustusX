#ifndef PROBEXMLCONFIGPARSER_H_
#define PROBEXMLCONFIGPARSER_H_

#include <math.h>
#include <vector>
#include <QDomDocument>

/**
 * \class probeXmlConfigParser.h
 *
 * \brief Class that works as a interface to a ProbeCalibConfigs.xml
 *
 * \date 11. mai 2010
 * \author: jbake
 */
class ProbeXmlConfigParser
{
public:
  typedef std::pair<int,int> ColRowPair;
  struct Configuration ///< a easy-to-work-with struct for a specific xml configuration
  {
    std::string             mUsScanner;  ///<  scanner
    std::string             mUsProbe;    ///<  probe
    std::string             mRtSource;   ///<  realtime source
    std::string             mConfigId;   ///<  config id
    float                   mWidthDeg;   ///<  width in degrees
    int                     mDepth;      ///<  depth
    float                   mOffset;     ///<  Offset
    float                   mOriginCol;  ///<  Origin.Col
    float                   mOriginRow;  ///<  Origin.Row
    int                     mNCorners;   ///<  number of corners
    std::vector<ColRowPair> mCorners;    ///<  corners <col,row>
    int                     mLeftEdge;   ///<  LeftEdge
    int                     mRightEdge;  ///<  RightEdge
    int                     mTopEdge;    ///<  TopEdge
    int                     mBottomEdge; ///<  BottomEdge
    double                  mPixelWidth; ///<  Pixel width
    double                  mPixelHeight;///<  Pixel height
  };

  ProbeXmlConfigParser(QString& pathToXml); ///< opens the file and reads it onto the QDomDocument
  ~ProbeXmlConfigParser();

  QStringList getScannerList(); ///< get a list of all scanner in the xml
  QStringList getProbeList(QString scanner); ///< get a list of all probes for that scanner
  QStringList getRtSourceList(QString scanner, QString probe); ///< get a list of rt sources for that scanner/probe combo
  QStringList getConfigIdList(QString scanner, QString probe, QString rtSource); ///< get a list of config ids for that scanner/probe/rsource combo
  
  Configuration getConfiguration(QString scanner, QString probe, QString rtsource, QString configId); ///< get a easy-to-work-with struct of a specific config

protected:
  QList<QDomNode> getScannerNodes(QString scanner = "ALL"); ///< get a list of ALL scanner nodes or just the one you are looking for
  QList<QDomNode> getProbeNodes(QString scanner, QString probe = "ALL"); ///< get a list of ALL probenodes for that scanner, or just the one you are looking for
  QList<QDomNode> getRTSourceNodes(QString scanner, QString probe, QString rtSource="ALL"); ///< get a list of ALL rtsourcenodes for that scanner/probe combo, or just the one you are looking for
  QList<QDomNode> getConfigNodes(QString scanner, QString probe, QString rtsource, QString config="ALL"); ///< get a list of ALL confignodes for that scanner/probe/rtsource combo, or just the one you are looking for
  
  QList<QDomNode> nodeListToListOfNodes(QDomNodeList list); ///< converts a QDomNodeList to a QList<QDomNode> 

  QDomDocument mDomDoc; ///< the dom document to parse
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
    double pi = atan(1)*4;
    return fmod(  ( ( atan2((pair1.first - mCenter.first), (pair1.second - mCenter.second)) ) + 3*pi/4) , 2*pi);
  }
  ProbeXmlConfigParser::ColRowPair mCenter;
};


#endif /* PROBEXMLCONFIGPARSER_H_ */

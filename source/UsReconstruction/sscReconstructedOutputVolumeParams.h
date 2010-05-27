/*
 * sscReconstructedOutputVolumeParams.h
 *
 *  Created on: May 27, 2010
 *      Author: christiana
 */
#ifndef SSCRECONSTRUCTEDOUTPUTVOLUMEPARAMS_H_
#define SSCRECONSTRUCTEDOUTPUTVOLUMEPARAMS_H_

#include <QDomElement>
#include <QStringList>

namespace ssc
{

/** Represents one option of the string type.
 *  The data are stored within a xml document.
 *
 *  The option node has this layout:
 *   <option>
 *     <id>Processor</id>
 *     <name>Nice name for Processor, optional</name>
 *     <help>Preferred type of processor to use during reconstruction</help>
 *     <value>CPU</value>
 *     <type>string</type>
 *     <range>"GPU" "CPU"</range>
 *   </option>
 */
class StringOptionItem
{
public:
  /** find and return the setting with id==uid among the children of root.
   */
  static StringOptionItem fromName(const QString& uid, QDomNode root)
  {
    QDomNodeList settings = root.childNodes();
    for (int i=0; i<settings.size(); ++i)
    {
      StringOptionItem item(settings.item(i).toElement());
      //std::cout << "getnamed("<<uid<<") "<< item.getId() << std::endl;
      if (item.getId()==uid)
        return item;
    }
    return StringOptionItem(QDomElement());
  }

  StringOptionItem(QDomElement element) : mElement(element) {}
  QString getId() const
  {
    return mElement.attribute("id");
    //return mElement.namedItem("id").toElement().text();
  }
  QString getName() const
  {
    QString name = mElement.namedItem("name").toElement().text();
    if (!name.isEmpty())
      return name;
    return this->getId();
  }
  QString getHelp() const
  {
    return mElement.namedItem("help").toElement().text();
  }
  QString getValue() const
  {
    return mElement.namedItem("value").toElement().text();
  }
  void setValue(const QString& val)
  {
    QDomText text = mElement.namedItem("value").firstChild().toText();
    if (text.isNull())
    {
      std::cout << "setvalue: null" << std::endl;
      text = mElement.ownerDocument().createTextNode("");
      mElement.namedItem("value").appendChild(text);
    }
    std::cout << "setvalue: " << val.toStdString() << std::endl;
    text.setData(val);
  }
  QStringList getRange() const
  {
    QString str = mElement.namedItem("range").toElement().text();
    // split sequences of type "a" "bb" "ff f"
    QStringList retval = str.split(QRegExp("\"|\"\\s+\""), QString::SkipEmptyParts);
    return retval;
  }
private:
  QDomElement mElement;
};

class ReconstructionParams
{
  QDomDocument mDocument;
  QString mProcessor; // GPU, CPU,
  QString mOrientation; // PatientReference, MiddleFrame,
  QString mAlgorithm;
};



/** Helper struct for sending and controlling output volume properties.
 */
class OutputVolumeParams
{
public:
  // constants, set only based on input data
  ssc::DoubleBoundingBox3D mExtent;
  double mInputSpacing;
  ssc::Vector3D mInputDim;

  OutputVolumeParams() :
    mExtent(0,0,0,0,0,0),
    mInputSpacing(0),
    mInputDim(0,0,0),
    mMaxVolumeSize(0),
    mDim(0,0,0),
    mSpacing(0)
  {
  }
  /** Initialize the volue parameters with sensible defaults.
   */
  OutputVolumeParams(ssc::DoubleBoundingBox3D extent, double inputSpacing, ssc::Vector3D inputDim) :
    mExtent(extent),
    mInputSpacing(inputSpacing),
    mInputDim(inputDim),
    mMaxVolumeSize(1024*1024*32)
  {
    // Calculate optimal output image spacing and dimensions based on US frame spacing
    setSpacing(mInputSpacing);
    constrainVolumeSize(mMaxVolumeSize);
  }

  unsigned long getVolumeSize() const
  {
    return mDim[0]*mDim[1]*mDim[2];;
  }

  /** Set a spacing, recalculate dimensions.
   */
  void setSpacing(double spacing)
  {
    mSpacing = spacing;
    mDim = mExtent.range() / mSpacing;
    this->roundDim();
  }
  double getSpacing() const
  {
    return mSpacing;
  }
  /** Set one of the dimensions explicitly, recalculate other dims and spacing.
   */
  void setDim(int index, int val)
  {
    setSpacing(mExtent.range()[index] / val);
   }
  ssc::Vector3D getDim() const
  {
    return mDim;
  }
  /** Increase spacing in order to keep size below a max size
   */
  void constrainVolumeSize(double maxSize)
  {
    this->setSpacing(mInputSpacing); // reset to default values

    mMaxVolumeSize = maxSize;
    // Reduce output volume size if optimal volume size is too large
    unsigned long volumeSize = getVolumeSize();
    if (volumeSize > mMaxVolumeSize)
    {
      double scaleFactor = pow(volumeSize/double(mMaxVolumeSize),1/3.0);
      std::cout << "Downsampled volume - Used scaleFactor : " << scaleFactor << std::endl;
//      mDim /= scaleFactor;
//      mSpacing *= scaleFactor;
      this->setSpacing(mSpacing*scaleFactor);
    }
  }
  unsigned long getMaxVolumeSize() const
  {
    return mMaxVolumeSize;
  }

private:
  // controllable data, set only using the setters
  unsigned long mMaxVolumeSize;
  ssc::Vector3D mDim;
  double mSpacing;

  void roundDim()
  {
    for (int i=0; i<3; ++i)
      mDim[i] = ceil(mDim[i]);
  }
};

} // namespace ssc


#endif /* SSCRECONSTRUCTEDOUTPUTVOLUMEPARAMS_H_ */

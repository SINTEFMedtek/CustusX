#include "sscImage.h"

#include <QDomDocument>
#include <vtkImageAccumulate.h>
#include <vtkImageReslice.h>
#include <vtkImageData.h>
#include <vtkMatrix4x4.h>
#include <vtkImageLuminance.h>
#include <vtkPlane.h>
#include <vtkPlanes.h>
#include <vtkImageChangeInformation.h>
#include <vtkImageClip.h>
#include "sscImageTF3D.h"
#include "sscBoundingBox3D.h"
#include "sscImageLUT2D.h"
#include "sscRegistrationTransform.h"
#include "sscLandmark.h"
#include "sscMessageManager.h"
#include "sscDataManager.h"
#include "sscTypeConversions.h"
#include "sscUtilHelpers.h"

typedef vtkSmartPointer<vtkImageChangeInformation> vtkImageChangeInformationPtr;

namespace ssc
{

Image::ShadingStruct::ShadingStruct()
{
	on = false;
	ambient = 0.2;
	diffuse = 0.9;
	specular = 0.3;
	specularPower = 15.0;
}

double Image::ShadingStruct::loadAttribute(QDomNode dataNode, QString name, double defVal)
{
  QString text = dataNode.toElement().attribute(name);
  bool ok;
  double val = text.toDouble(&ok);
  if (ok)
    return val;
  return defVal;
}

void Image::ShadingStruct::addXml(QDomNode dataNode)
{
	QDomElement elem = dataNode.toElement();
	elem.setAttribute("on", on);
	elem.setAttribute("ambient", ambient);
	elem.setAttribute("diffuse", diffuse);
	elem.setAttribute("specular", specular);
	elem.setAttribute("specularPower", specularPower);
}

void Image::ShadingStruct::parseXml(QDomNode dataNode)
{
	if (dataNode.isNull())
		return;

	on = dataNode.toElement().attribute("on").toInt();
	std::cout << "attrib on: " << dataNode.toElement().attribute("on")  << " : " << on << std::endl;
	ambient = loadAttribute(dataNode, "ambient", ambient);
	diffuse = loadAttribute(dataNode, "diffuse", diffuse);
	specular = loadAttribute(dataNode, "specular", specular);
	specularPower = loadAttribute(dataNode, "specularPower", specularPower);
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------
  
Image::~Image()
{}

Image::Image(const QString& uid, const vtkImageDataPtr& data,
    const QString& name) :
  Data(uid, name),
	mBaseImageData(data)
{
  mUseCropping = false;
  mCroppingBox_d = DoubleBoundingBox3D(0,0,0,0,0,0);

//  mShading.on = false;
//  mShading.ambient = 0.2;
//  mShading.diffuse = 0.9;
//  mShading.specular = 0.3;
//  mShading.specularPower = 15.0;
  
  this->resetTransferFunctions();
}
  
void Image::resetTransferFunctions()
{
  //messageManager()->sendDebug("Image::reset called");
  
  if(!mBaseImageData)
  {
    messageManager()->sendWarning("ssc::Image has no image data");
    return;
  }
  
  //mBaseImageData->Update();
	mBaseImageData->GetScalarRange();	// this line updates some internal vtk value, and (on fedora) removes 4.5s in the second render().
  
	this->resetTransferFunction(ImageTF3DPtr(new ImageTF3D(mBaseImageData)), ImageLUT2DPtr(new ImageLUT2D(mBaseImageData)));
//
//	if(mImageTransferFunctions3D)
//    disconnect(mImageTransferFunctions3D.get(), SIGNAL(transferFunctionsChanged()),
//               this, SIGNAL(transferFunctionsChanged()));
//  if(mImageLookupTable2D)
//    disconnect(mImageLookupTable2D.get(), SIGNAL(transferFunctionsChanged()),
//               this, SIGNAL(transferFunctionsChanged()));
//
//  mImageTransferFunctions3D.reset(new ImageTF3D(mBaseImageData));
//	mImageLookupTable2D.reset(new ImageLUT2D(mBaseImageData));
//
//	// Add initial values to the 3D transfer functions
//	mImageTransferFunctions3D->addAlphaPoint(this->getMin(), 0);
//	mImageTransferFunctions3D->addAlphaPoint(this->getMax(), this->getMaxAlphaValue());
//	mImageTransferFunctions3D->addColorPoint(this->getMin(), Qt::black);
//	mImageTransferFunctions3D->addColorPoint(this->getMax(), Qt::white);
//
//  connect(mImageTransferFunctions3D.get(), SIGNAL(transferFunctionsChanged()),
//          this, SIGNAL(transferFunctionsChanged()));
//  connect(mImageLookupTable2D.get(), SIGNAL(transferFunctionsChanged()),
//					this, SIGNAL(transferFunctionsChanged()));
//
//  emit transferFunctionsChanged();
}

void Image::resetTransferFunction(ImageTF3DPtr imageTransferFunctions3D, ImageLUT2DPtr imageLookupTable2D)
{
  if(!mBaseImageData)
  {
    messageManager()->sendWarning("ssc::Image has no image data");
    return;
  }

  if(mImageTransferFunctions3D)
  {
    disconnect(mImageTransferFunctions3D.get(), SIGNAL(transferFunctionsChanged()), this, SIGNAL(transferFunctionsChanged()));
  }
  if(mImageLookupTable2D)
  {
    disconnect(mImageLookupTable2D.get(), SIGNAL(transferFunctionsChanged()), this, SIGNAL(transferFunctionsChanged()));
  }

  mImageTransferFunctions3D = imageTransferFunctions3D;
  mImageLookupTable2D = imageLookupTable2D;

  if(mImageTransferFunctions3D)
  {
    mImageTransferFunctions3D->setVtkImageData(mBaseImageData);
    connect(mImageTransferFunctions3D.get(), SIGNAL(transferFunctionsChanged()), this, SIGNAL(transferFunctionsChanged()));
  }
  if(mImageLookupTable2D)
  {
    mImageLookupTable2D->setVtkImageData(mBaseImageData);
    connect(mImageLookupTable2D.get(), SIGNAL(transferFunctionsChanged()), this, SIGNAL(transferFunctionsChanged()));
  }

  emit transferFunctionsChanged();
}

  
//void Image::set_rMd(Transform3D rMd)
//{
//	bool changed = !similar(rMd, m_rMd);
//
//	Data::set_rMd(rMd);
//	//std::cout << "Image::setTransform(): \n" << rMd << std::endl;
//	if (!changed)
//	{
//		return;
//	}
//}

void Image::transformChangedSlot()
{
	if (mReferenceImageData)
	{
		Transform3D rMd = get_rMd();
		mOrientatorMatrix->DeepCopy(rMd.inv().matrix());
		mReferenceImageData->Update();		
	}
  //std::cout << "Image::transformChangedSlot()\n" << rMd << std::endl;
}

void Image::setVtkImageData(const vtkImageDataPtr& data)
{
	//messageManager()->sendDebug("Image::setVtkImageData()");
	mBaseImageData = data;
	mBaseGrayScaleImageData = NULL;
	
	if (mOrientator)
	{
		mOrientator->SetInput(mBaseImageData);		
	}

  this->resetTransferFunctions();
	emit vtkImageDataChanged();
}
vtkImageDataPtr Image::getGrayScaleBaseVtkImageData()
{
	if (mBaseGrayScaleImageData)
	{
		return mBaseGrayScaleImageData;
	}
	
	mBaseGrayScaleImageData = getBaseVtkImageData();
	
	// if the volume is color, run it through a luminance filter in order to get a
	// finning grayscale representation.
	if (mBaseGrayScaleImageData->GetNumberOfScalarComponents()>2) // color
	{
		vtkSmartPointer<vtkImageLuminance> luminance = vtkSmartPointer<vtkImageLuminance>::New();
		luminance->SetInput(mBaseGrayScaleImageData);
		mBaseGrayScaleImageData = luminance->GetOutput();		
	}
	
	mBaseGrayScaleImageData->Update();
	return mBaseGrayScaleImageData;
}
ImageTF3DPtr Image::getTransferFunctions3D()
{
	return mImageTransferFunctions3D;
}
void Image::setTransferFunctions3D(ImageTF3DPtr transferFuntion)
{
  mImageTransferFunctions3D = transferFuntion;
  emit transferFunctionsChanged();
}
ImageLUT2DPtr Image::getLookupTable2D()
{
	return mImageLookupTable2D;
}
//See ssc::Data
/*REGISTRATION_STATUS Image::getRegistrationStatus() const
{
	return mRegistrationStatus;
}*/
vtkImageDataPtr Image::getBaseVtkImageData()
{
	return mBaseImageData;
}
vtkImageDataPtr Image::getRefVtkImageData()
{
	if (!mReferenceImageData) // optimized: dont init it if you dont need it.
	{
		// provide a resampled volume for algorithms requiring that (such as proberep)
		mOrientatorMatrix = vtkMatrix4x4Ptr::New();
		mOrientator = vtkImageReslicePtr::New();
		mOrientator->SetInput(mBaseImageData);
		mOrientator->SetInterpolationModeToLinear();
		mOrientator->SetOutputDimensionality( 3);
		mOrientator->SetResliceAxes(mOrientatorMatrix);
		mOrientator->AutoCropOutputOn();
		mReferenceImageData = mOrientator->GetOutput();

		mReferenceImageData->Update();		

		this->transformChangedSlot(); // update transform
		std::cout << "Warning: ssc::Image::getRefVtkImageData() called. Expensive. Do not use." << std::endl;
	}
	
	return mReferenceImageData;
}

LandmarkMap Image::getLandmarks()
{
	return mLandmarks;
}
void Image::setLandmark(Landmark landmark)
{
  //std::cout << "Image::setLandmark" << std::endl;
  mLandmarks[landmark.getUid()] = landmark;
  emit landmarkAdded(landmark.getUid());
}
void Image::removeLandmark(QString uid)
{
  mLandmarks.erase(uid);
  emit landmarkRemoved(uid);
}
/** If index is found, it's treated as an edit operation, else
 * it's an add operation.
 * @param x
 * @param y
 * @param z
 * @param index
 */
/*void Image::addLandmarkSlot(double x, double y, double z, unsigned int index)
{
	double addLandmark[4] = {x, y, z, (double)index};

	int numberOfLandmarks = mLandmarks->GetNumberOfTuples();
	//if index exists, we treat it as an edit operation
	for(int i=0; i<= numberOfLandmarks-1; i++)
	{
		double* landmark = mLandmarks->GetTuple(i);
		if(landmark[3] == index)
		{
			mLandmarks->SetTupleValue(i, addLandmark);
			emit landmarkAdded(x, y, z, index);
			return;
		}
	}
	//else it's an add operation
	mLandmarks->InsertNextTupleValue(addLandmark);
	emit landmarkAdded(x, y, z, index);
}*/
/** If index is found that tuple(landmark) is removed from the array, else
 * it's just ignored.
 * @param x
 * @param y
 * @param z
 * @param index
 */
/*void Image::removeLandmarkSlot(double x, double y, double z, unsigned int index)
{
	int numberOfLandmarks = mLandmarks->GetNumberOfTuples();
	for(int i=0; i<= numberOfLandmarks-1; i++)
	{
		double* landmark = mLandmarks->GetTuple(i);
		if(landmark[3] == index)
		{
			mLandmarks->RemoveTuple(i);
			emit landmarkRemoved(x, y, z, index);
		}
	}
}*/
//
//void Image::transferFunctionsChangedSlot()
//{
//	emit vtkImageDataChanged();
//}
void Image::printLandmarks()
{
	std::cout << "Landmarks: " << std::endl;
	/*for(int i=0; i<= mLandmarks->GetNumberOfTuples()-1; i++)
	{
		double* landmark = mLandmarks->GetTuple(i);
		QStringstream stream;
		stream << i << ": (";
		stream << landmark[0];
		stream << ",";
		stream << landmark[1];
		stream << ",";
		stream << landmark[2];
		stream << ",";
		stream << landmark[3];
		stream << ")";
		std::cout << stream.str() << std::endl;
	}*/
}
DoubleBoundingBox3D Image::boundingBox() const
{
  mBaseImageData->UpdateInformation();
	DoubleBoundingBox3D bounds(mBaseImageData->GetBounds());
	return bounds;
}
vtkImageAccumulatePtr Image::getHistogram()
{
	if (mHistogramPtr.GetPointer() == NULL)
	{
		mHistogramPtr = vtkImageAccumulatePtr::New();
		mHistogramPtr->SetInput(mBaseImageData);
		mHistogramPtr->IgnoreZeroOn(); // required for Sonowand CT volumes, where data are placed between 31K and 35K.
		// Set up only a 1D histogram for now, so y and z values are set to 0
		mHistogramPtr->SetComponentExtent(this->getMin(), this->getMax(),0,0,0,0);
		mHistogramPtr->SetComponentOrigin(this->getMin(), 0, 0);
		mHistogramPtr->SetComponentSpacing(1, 0, 0);
	}
	mHistogramPtr->Update();
	return mHistogramPtr;
}
int Image::getMax()
{
	// Alternatively create max from histogram
	//IntIntMap::iterator iter = this->getHistogram()->end();
	//iter--;
	//return (*iter).first;
	return (int)mImageTransferFunctions3D->getScalarMax();
}
int Image::getMin()
{
	// Alternatively create min from histogram
	//IntIntMap::iterator iter = this->getHistogram()->begin();
	//return (*iter).first;
	return (int)mImageTransferFunctions3D->getScalarMin();
}
int Image::getPosMax()
{
	return (int)mImageTransferFunctions3D->getScalarMax() - getMin();
}
int Image::getPosMin()
{
	return 0;
}
  
int Image::getRange()
{
	return this->getMax() - this->getMin();
}
int Image::getMaxAlphaValue()
{
	return 255;
}
void Image::addXml(QDomNode& dataNode)
{
  Data::addXml(dataNode);
  QDomNode imageNode = dataNode;
  QDomDocument doc = dataNode.ownerDocument();
//  QDomElement imageNode = doc.createElement("image");
//  parentNode.appendChild(imageNode);

//  m_rMd_History->addXml(imageNode); //TODO: should be in the superclass

//  QDomElement uidNode = doc.createElement("uid");
//  uidNode.appendChild(doc.createTextNode(mUid.c_str()));
//  imageNode.appendChild(uidNode);
//
//  QDomElement nameNode = doc.createElement("name");
//  nameNode.appendChild(doc.createTextNode(mName.c_str()));
//  imageNode.appendChild(nameNode);
//
//  QDomElement filePathNode = doc.createElement("filePath");
//  filePathNode.appendChild(doc.createTextNode(mFilePath.c_str()));
//  imageNode.appendChild(filePathNode);
  
  QDomElement tf3DNode = doc.createElement("transferfunctions");
  mImageTransferFunctions3D->addXml(tf3DNode);
  imageNode.appendChild(tf3DNode);

  QDomElement lut2DNode = doc.createElement("lookuptable2D");
  mImageLookupTable2D->addXml(lut2DNode);
  imageNode.appendChild(lut2DNode);

  QDomElement shadingNode = doc.createElement("shading");
  mShading.addXml(shadingNode);
  imageNode.appendChild(shadingNode);

//
//  shadingNode.appendChild(doc.createTextNode(qstring_cast(mShading.on)));
//  imageNode.appendChild(shadingNode);
//  //std::cout << "created shading" << std::endl;
//
//  QDomElement shadingAmbientNode = doc.createElement("shadingAmbient");
//  shadingAmbientNode.appendChild(doc.createTextNode(qstring_cast(mShading.ambient)));
//  imageNode.appendChild(shadingAmbientNode);
//
//  QDomElement shadingDiffuseNode = doc.createElement("shadingDiffuse");
//  shadingDiffuseNode.appendChild(doc.createTextNode(qstring_cast(mShading.diffuse)));
//  imageNode.appendChild(shadingDiffuseNode);
//
//  QDomElement shadingSpecularNode = doc.createElement("shadingSpecular");
//  shadingSpecularNode.appendChild(doc.createTextNode(qstring_cast(mShading.specular)));
//  imageNode.appendChild(shadingSpecularNode);
//
//  QDomElement shadingSpecularPowerNode = doc.createElement("shadingSpecularPower");
//  shadingSpecularPowerNode.appendChild(doc.createTextNode(qstring_cast(mShading.specularPower)));
//  imageNode.appendChild(shadingSpecularPowerNode);

  QDomElement landmarksNode = doc.createElement("landmarks");
  LandmarkMap::iterator it = mLandmarks.begin();
  for(; it != mLandmarks.end(); ++it)
  {
    QDomElement landmarkNode = doc.createElement("landmark");
    it->second.addXml(landmarkNode);
    landmarksNode.appendChild(landmarkNode);
  }
  imageNode.appendChild(landmarksNode);

  QDomElement cropNode = doc.createElement("crop");
  cropNode.setAttribute("use", mUseCropping);
  //std::cout << "qstring_cast(mCroppingBox_r) " << qstring_cast(mCroppingBox_r) << std::endl;
  cropNode.appendChild(doc.createTextNode(qstring_cast(mCroppingBox_d)));
  imageNode.appendChild(cropNode);

  QDomElement clipNode = doc.createElement("clip");
  for (unsigned i=0; i<mClipPlanes.size(); ++i)
  {
    QDomElement planeNode = doc.createElement("plane");
    Vector3D normal(mClipPlanes[i]->GetNormal());
    Vector3D origin(mClipPlanes[i]->GetOrigin());
    planeNode.setAttribute("normal", qstring_cast(normal));
    planeNode.setAttribute("origin", qstring_cast(origin));
    clipNode.appendChild(planeNode);
  }
  imageNode.appendChild(clipNode);
}

void Image::parseXml(QDomNode& dataNode)
{
  Data::parseXml(dataNode);

  // image node must be parsed in the data manager to create this Image object
  // Only subnodes are parsed here

	if (dataNode.isNull())
		return;
	
//  QDomNode registrationHistory = dataNode.namedItem("registrationHistory");
//  m_rMd_History->parseXml(registrationHistory);

	//transferefunctions
	QDomNode transferfunctionsNode = dataNode.namedItem("transferfunctions");
	if (!transferfunctionsNode.isNull())
		mImageTransferFunctions3D->parseXml(transferfunctionsNode);
	else
	{
		std::cout << "Warning: Image::parseXml() found no transferfunctions";
		std::cout << std::endl;
	}

	mImageLookupTable2D->parseXml(dataNode.namedItem("lookuptable2D"));

	// backward compatibility:
	mShading.on = dataNode.namedItem("shading").toElement().text().toInt();
  //Assign default values if the shading nodes don't exists to allow backward compability
  if(!dataNode.namedItem("shadingAmbient").isNull())
    mShading.ambient = dataNode.namedItem("shadingAmbient").toElement().text().toDouble();
  //else
  //  mShading.ambient = 0.2;
  if(!dataNode.namedItem("shadingDiffuse").isNull())
    mShading.diffuse = dataNode.namedItem("shadingDiffuse").toElement().text().toDouble();
  //else
  //  mShading.diffuse = 0.9;
  if(!dataNode.namedItem("shadingSpecular").isNull())
    mShading.specular = dataNode.namedItem("shadingSpecular").toElement().text().toDouble();
  //else
  //  mShading.specular = 0.3;
  if(!dataNode.namedItem("shadingSpecularPower").isNull())
    mShading.specularPower = dataNode.namedItem("shadingSpecularPower").toElement().text().toDouble();
  //else
  //  mShading.specularPower = 15.0;
  
  // new way:
  mShading.parseXml(dataNode.namedItem("shading"));

	QDomNode landmarksNode = dataNode.namedItem("landmarks");
	QDomElement landmarkNode = landmarksNode.firstChildElement("landmark");
	for (; !landmarkNode.isNull(); landmarkNode = landmarkNode.nextSiblingElement("landmark"))
	{
	  Landmark landmark;
	  landmark.parseXml(landmarkNode);
	  this->setLandmark(landmark);
  }

  QDomElement cropNode = dataNode.namedItem("crop").toElement();
  if (!cropNode.isNull())
  {
    mUseCropping = cropNode.attribute("use").toInt();
    mCroppingBox_d = DoubleBoundingBox3D::fromString(cropNode.text());
  }

  QDomElement clipNode = dataNode.namedItem("clip").toElement();
  QDomElement clipPlaneNode = clipNode.firstChildElement("plane");
  for (; !clipPlaneNode.isNull(); clipPlaneNode = clipPlaneNode.nextSiblingElement("plane"))
  {
    Vector3D normal = Vector3D::fromString(clipPlaneNode.attribute("normal"));
    Vector3D origin = Vector3D::fromString(clipPlaneNode.attribute("origin"));
    vtkPlanePtr plane = vtkPlanePtr::New();
    plane->SetNormal(normal.begin());
    plane->SetOrigin(origin.begin());
    mClipPlanes.push_back(plane);
  }

}

void Image::setShadingOn(bool on)
{
  mShading.on = on;
  emit transferFunctionsChanged();
}

bool Image::getShadingOn() const
{
  return mShading.on;
}

void Image::setShadingAmbient(double ambient)
{
  mShading.ambient = ambient;
  emit transferFunctionsChanged();
}
  
void Image::setShadingDiffuse(double diffuse)
{
  mShading.diffuse = diffuse;
  emit transferFunctionsChanged();
}
  
void Image::setShadingSpecular(double specular)
{
  mShading.specular = specular;
  emit transferFunctionsChanged();
}

void Image::setShadingSpecularPower(double specularPower)
{
  mShading.specularPower = specularPower;
  emit transferFunctionsChanged();
}

double Image::getShadingAmbient()
{return mShading.ambient;}
double Image::getShadingDiffuse()
{return mShading.diffuse;}
double Image::getShadingSpecular()
{return mShading.specular;}
double Image::getShadingSpecularPower()
{return mShading.specularPower;}  

Image::ShadingStruct Image::getShading()
{
  return mShading;
}

void Image::setShading(Image::ShadingStruct shading )
{
  mShading = shading;
  emit transferFunctionsChanged();
}

// methods for defining and storing a cropping box. Image does not use these data, this is up to the mapper
void Image::setCropping(bool on)
{
  if (mUseCropping==on)
    return;

  mUseCropping = on;
  if (similar(mCroppingBox_d, DoubleBoundingBox3D(0,0,0,0,0,0)))
    mCroppingBox_d = this->boundingBox();
  emit cropBoxChanged();
}

bool Image::getCropping() const
{
  return mUseCropping;
}

void Image::setCroppingBox(const DoubleBoundingBox3D& bb_d)
{
  if (similar(mCroppingBox_d, bb_d))
    return;
  mCroppingBox_d = bb_d;
  emit cropBoxChanged();
}

DoubleBoundingBox3D Image::getCroppingBox() const
{
  return mCroppingBox_d;
}

// methods for defining and storing clip planes. Image does not use these data, this is up to the mapper
void Image::addClipPlane(vtkPlanePtr plane)
{
  if (std::count(mClipPlanes.begin(), mClipPlanes.end(), plane))
    return;
  mClipPlanes.push_back(plane);
  emit clipPlanesChanged();
}

std::vector<vtkPlanePtr> Image::getClipPlanes()
{
  return mClipPlanes;
}

void Image::clearClipPlanes()
{
  mClipPlanes.clear();
  emit clipPlanesChanged();
}

/**Do the following operations on mBaseVtkImageData:
 *  * Reset the origin to zero.
 *  * Reset the extent to have its lower-left corner in zero.
 * The shift introduced by these  two operations are inserted
 * as a translation into the matrix rMd.
 *
 * This operation is needed because ssc::Image dont support vtkImageData
 * with a nonzero origin or nonzero extent. These must be removed during creation.
 */
void Image::mergevtkOriginIntosscTransform()
{
//  std::cout << "REMOVE ORIGIN START:" << std::endl;
//  mBaseImageData->Print(std::cout);

  // the internal CustusX format does not handle extents starting at non-zero.
  // Move extent to zero and change rMd.
  Vector3D origin(mBaseImageData->GetOrigin());
  Vector3D spacing(mBaseImageData->GetSpacing());
  IntBoundingBox3D extent(mBaseImageData->GetExtent());
  Vector3D extentShift = multiply_elems(extent.corner(0,0,0), spacing);

  vtkImageChangeInformationPtr info = vtkImageChangeInformationPtr::New();
  info->SetInput(mBaseImageData);
  info->SetOutputExtentStart(0,0,0);
  info->SetOutputOrigin(0,0,0);
  mBaseImageData = info->GetOutput();

  mBaseImageData->ComputeBounds();
  mBaseImageData->Update();
  mBaseImageData->UpdateInformation();

  this->get_rMd_History()->setRegistration(this->get_rMd() * createTransformTranslate(origin + extentShift));

//  std::cout << "REMOVED ORIGIN END:" << std::endl;
//  mBaseImageData->Print(std::cout);

  emit vtkImageDataChanged();
  emit transferFunctionsChanged();
  emit clipPlanesChanged();
  emit cropBoxChanged();
}


ImagePtr Image::CropAndClipImage(QString outputBasePath)
{
  vtkImageDataPtr rawResult = this->CropAndClipImageTovtkImageData();

//  // the internal CustusX format does not handle extents starting at non-zero.
//  // Move extent to zero and change rMd.
//  IntBoundingBox3D extent(rawResult->GetExtent());
//  int diff[3];
//  diff[0] = extent[0];
//  diff[1] = extent[2];
//  diff[2] = extent[4];
//  extent[0] -= diff[0];
//  extent[1] -= diff[0];
//  extent[2] -= diff[1];
//  extent[3] -= diff[1];
//  extent[4] -= diff[2];
//  extent[5] -= diff[2];
//
////  std::cout << "cropped volume pre move:" << std::endl;
////  rawResult->Print(std::cout);
//
//  rawResult->SetExtent(extent.begin());
//  rawResult->SetWholeExtent(extent.begin());
//  rawResult->SetUpdateExtentToWholeExtent();
//  rawResult->ComputeBounds();
////  std::cout << "cropped volume pre update:" << std::endl;
////  rawResult->Print(std::cout);
//  rawResult->Update();
//
//  vtkImageDataPtr copyData = vtkImageDataPtr::New();
//  copyData->DeepCopy(rawResult);
//  copyData->Update();
//  rawResult = copyData;

  QString uid = changeExtension(this->getUid(), "") + "_clip%1";
  QString name = this->getName()+" clipped %1";
  //std::cout << "clipped volume: " << uid << ", " << name << std::endl;
  ImagePtr result = dataManager()->createImage(rawResult,uid, name);
  result->mergevtkOriginIntosscTransform();
  result->resetTransferFunction(this->getTransferFunctions3D()->createCopy(), this->getLookupTable2D()->createCopy());
  messageManager()->sendInfo("Created volume " + result->getName());

//  std::cout << "cropped volume:" << std::endl;
//  rawResult->Print(std::cout);

//  DoubleBoundingBox3D bb = image->getCroppingBox();
//  clip->SetInput(this->getBaseVtkImageData());
//  DoubleBoundingBox3D bb_orig = image->boundingBox();
//  Vector3D shift = this->getCroppingBox().corner(0,0,0) - this->boundingBox().corner(0,0,0);
////  std::cout << "shift: " << shift << std::endl;
//
//  result->get_rMd_History()->setRegistration(this->get_rMd() * createTransformTranslate(shift));
  result->setParentFrame(this->getUid());
  dataManager()->loadData(result);
//  QString outputBasePath = stateManager()->getPatientData()->getActivePatientFolder();
  dataManager()->saveImage(result, outputBasePath);
  return result;
}

vtkImageDataPtr Image::CropAndClipImageTovtkImageData()
{
  //vtkPlanesPtr planes = vtkPlanesPtr::New();
  //planes->SetBounds(this->getCroppingBox().begin());
  //TODO: Insert clip planes also
  //this->getClipPlanes();

//  vtkCutterPtr cutter = vtkCutterPtr::New();
//  cutter->SetCutFunction(planes);
//  cutter->SetInput(this->getBaseVtkImageData());
//  vtkClipVolumePtr clipper = vtkClipVolumePtr::New();
//  clipper->SetClipFunction(planes);
//  clipper->SetInput(this->getBaseVtkImageData());
//  return clipper->GetOutput();

  vtkImageClipPtr clip = vtkImageClipPtr::New();
  DoubleBoundingBox3D bb = this->getCroppingBox();
  clip->SetInput(this->getBaseVtkImageData());

//  DoubleBoundingBox3D bb_orig = this->boundingBox();
//  Vector3D shift = bb.corner(0,0,0) - bb_orig.corner(0,0,0);
//  Vector3D c_orig(this->getBaseVtkImageData()->GetOrigin());
//  this->getBaseVtkImageData()->Print(std::cout);
//  std::cout << "bb_orig" << bb_orig << std::endl;
//  std::cout << "bb_CLIP" << bb << std::endl;
//  std::cout << "c_orig" << c_orig << std::endl;
//  std::cout << "shift" << shift << std::endl;

  double* sp = this->getBaseVtkImageData()->GetSpacing();

  clip->SetOutputWholeExtent(
      static_cast<int>(bb[0]/sp[0]+0.5), static_cast<int>(bb[1]/sp[1]+0.5),
      static_cast<int>(bb[2]/sp[1]+0.5), static_cast<int>(bb[3]/sp[1]+0.5),
      static_cast<int>(bb[4]/sp[2]+0.5), static_cast<int>(bb[5]/sp[2]+0.5));

  clip->ClipDataOn();
  vtkImageDataPtr retVal = clip->GetOutput();

  retVal->Update();
  retVal->ComputeBounds();
//  Vector3D c_new(retVal->GetOrigin());
//  std::cout << "bb_new" << DoubleBoundingBox3D(retVal->GetBounds()) << std::endl;
//  std::cout << "c_new" << c_new << std::endl;
//  retVal->Print(std::cout);
//
//  IntBoundingBox3D extent(retVal->GetExtent());
//  int diff[3];
//  diff[0] = extent[0];
//  diff[1] = extent[2];
//  diff[2] = extent[4];
//  extent[0] -= diff[0];
//  extent[1] -= diff[0];
//  extent[2] -= diff[1];
//  extent[3] -= diff[1];
//  extent[4] -= diff[2];
//  extent[5] -= diff[2];
//
//  retVal->SetExtent(extent.begin());
//  retVal->SetWholeExtent(extent.begin());
//  retVal->SetUpdateExtentToWholeExtent();
//  retVal->ComputeBounds();
//  std::cout << "flyttet data" << std::endl;
//  retVal->Update();
//  retVal->Print(std::cout);

/*
  int* in = this->getBaseVtkImageData()->GetWholeExtent();
  int* ret = retVal->GetWholeExtent();
  std::cout << "in wholeExtent:     " << in[0] << " " << in[1]<< " " << in[2]<< " " << in[3]<< " " << in[4]<< " " << in[5] << std::endl;
  std::cout << "retVal wholeExtent: " << ret[0] << " " << ret[1]<< " " << ret[2]<< " " << ret[3]<< " " << ret[4]<< " " << ret[5] << std::endl;

  double* inSpacing = this->getBaseVtkImageData()->GetSpacing();
  std::cout << "inSpacing: " << inSpacing[0] << " " << inSpacing[1]<< " " << inSpacing[2] << std::endl;
  int* inExtent = this->getBaseVtkImageData()->GetExtent();
  std::cout << "inExtent : " << inExtent[0] << " " << inExtent[1]<< " " << inExtent[2] << inExtent[3]<< " " << inExtent[4]<< " " << inExtent[5] << std::endl;


  double* bounds = retVal->GetBounds();
  std::cout << "retBounds: " << bounds[0] << " " << bounds[1]<< " " << bounds[2]<< " " << bounds[3]<< " " << bounds[4]<< " " << bounds[5] << std::endl;

  int* extent = retVal->GetExtent();
  std::cout << "out Extent[5]: " <<  extent[5] << std::endl;*/

  return retVal;
}

} // namespace ssc

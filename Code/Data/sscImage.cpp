#include "sscImage.h"

#include <vtkImageReslice.h>
#include <vtkImageData.h>
#include <vtkMatrix4x4.h>
#include <vtkDoubleArray.h>
#include <vtkLookupTable.h>
#include <sstream>
#include <QDomDocument>

namespace ssc
{

Image::~Image()
{
}

Image::Image(const std::string& uid, const vtkImageDataPtr& data) :
	mImageTransferFunctions3D(new ImageTF3D(data)),
	mImageLookupTable2D(data),
	mUid(uid), mName(uid), mBaseImageData(data),
	mLandmarks(vtkDoubleArray::New())
{
	std::cout << "Image::Image() " << std::endl;
	mOutputImageData = mBaseImageData;
#ifdef USE_TRANSFORM_RESCLICER
	//mBaseImageData->ReleaseDataFlagOn();
	mOrientator = vtkImageReslicePtr::New();
	mOrientator->AutoCropOutputOn();
	mOrientator->SetInput(mBaseImageData);
	mOutputImageData = mOrientator->GetOutput();
	//mOutputImageData->Update();
	mOutputImageData->UpdateInformation();
#endif
	mLandmarks->SetNumberOfComponents(4);
	mOutputImageData->GetScalarRange();	// this line updates some internal vtk value, and (on fedora) removes 4.5s in the second render().
	mAlpha = 0.5;
	mTreshold = 1.0;
	
	// Add initial values to the transfer functions
	mImageTransferFunctions3D->addAlphaPoint(this->getMin(), 0);
	mImageTransferFunctions3D->addAlphaPoint(this->getMax(), 
																					//1.0);
																					this->getMaxAlphaValue());
	mImageTransferFunctions3D->addColorPoint(this->getMin(), Qt::black);
	mImageTransferFunctions3D->addColorPoint(this->getMax(), Qt::white);
	
  connect(mImageTransferFunctions3D.get(), SIGNAL(transferFunctionsChanged()),
					this, SLOT(transferFunctionsChangedSlot()));
	
	//setTransform(createTransformTranslate(Vector3D(0,0,0.1)));
}

void Image::set_rMd(Transform3D rMd)
{
	Data::set_rMd(rMd);
	//std::cout << "Image::setTransform(): \n" << trans << std::endl;
	if (similar(rMd, m_rMd))
	{
		return;
	}

#ifdef USE_TRANSFORM_RESCLICER
	mOrientator->SetResliceAxes(m_rMd.inv().matrix());
	mOutputImageData->UpdateInformation();
	mOutputImageData->GetScalarRange();	// this line updates some internal vtk value, and (on fedora) removes 4.5s in the second render().
#endif

	emit transformChanged();
}

void Image::setVtkImageData(const vtkImageDataPtr& data)
{
	std::cout << "Image::setVtkImageData() " << std::endl;
	mBaseImageData = data;
#ifdef USE_TRANSFORM_RESCLICER
	mOrientator->SetInput(mBaseImageData);
	mOrientator->SetResliceAxes(m_rMd.inv().matrix());
	mOutputImageData->Update();
	mOutputImageData->UpdateInformation();
	mOutputImageData->GetScalarRange();	// this line updates some internal vtk value, and (on fedora) removes 4.5s in the second render().
#else
	mOutputImageData = mBaseImageData;
#endif
	mImageTransferFunctions3D->setVtkImageData(data);
	mImageLookupTable2D = ImageLUT2D(data);

	emit vtkImageDataChanged();
}
void Image::setClut(vtkLookupTablePtr clut)
{
	mImageLookupTable2D.setLookupTable( clut );
	std::cout<<"ssc::Image, set new lut"<<std::endl;
}

double Image::treshold()
{
	return mTreshold;
}

void Image::setTreshold( double val )
{
	if( similar(mTreshold ,val))
		return;
	std::cout<<"ssc::Image, got threshold :"<<val<<std::endl;
	mTreshold = val;
	emit thresholdChange(val);
}

double Image::getAlpha()
{
	return mAlpha;
}

void Image::setAlpha(double val)
{
	if (similar(mAlpha, val))
		return;
	mAlpha = val;
	emit alphaChange();
}

ImageTF3DPtr Image::getTransferFunctions3D()
{
	return mImageTransferFunctions3D;
}

ImageLUT2D& Image::getLookupTable2D()
{
	return mImageLookupTable2D;
}

void Image::setName(const std::string& name)
{
	mName = name;
}

std::string Image::getUid() const
{
	return mUid;
}

std::string Image::getName() const
{
	return mName;
}

REGISTRATION_STATUS Image::getRegistrationStatus() const
{
	return rsNOT_REGISTRATED;
}

vtkImageDataPtr Image::getBaseVtkImageData()
{
	return mBaseImageData;
}

vtkImageDataPtr Image::getRefVtkImageData()
{
	return mOutputImageData;
}

vtkDoubleArrayPtr Image::getLandmarks()
{
	return mLandmarks;
}
void Image::connectRep(const RepWeakPtr& rep)
{
	mReps.insert(rep);
}
void Image::disconnectRep(const RepWeakPtr& rep)
{
	mReps.erase(rep);
}
/** If index is found, it's treated as an edit operation, else
 * it's an add operation.
 * @param x
 * @param y
 * @param z
 * @param index
 */
void Image::addLandmarkSlot(double x, double y, double z, unsigned int index)
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
}
/** If index is found that tuple(landmark) is removed from the array, else
 * it's just ignored.
 * @param x
 * @param y
 * @param z
 * @param index
 */
void Image::removeLandmarkSlot(double x, double y, double z, unsigned int index)
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
}

void Image::transferFunctionsChangedSlot()
{
	emit vtkImageDataChanged();
}

void Image::printLandmarks()
{
	std::cout << "Landmarks: " << std::endl;
	for(int i=0; i<= mLandmarks->GetNumberOfTuples()-1; i++)
	{
		double* landmark = mLandmarks->GetTuple(i);
		std::stringstream stream;
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
	}
}

DoubleBoundingBox3D Image::boundingBox() const
{
	mOutputImageData->UpdateInformation();
	DoubleBoundingBox3D bounds(mOutputImageData->GetBounds());
	return bounds;
}

vtkImageAccumulatePtr Image::getHistogram()
{
	if (mHistogramPtr.GetPointer() == NULL)
	{
		mHistogramPtr = vtkImageAccumulate::New();
		mHistogramPtr->SetInput(mBaseImageData);
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
	return mImageTransferFunctions3D->getScalarMax();
}
int Image::getMin()
{
	// Alternatively create min from histogram
	//IntIntMap::iterator iter = this->getHistogram()->begin();
	//return (*iter).first;
	return mImageTransferFunctions3D->getScalarMin();
}
int Image::getRange()
{
	return this->getMax() - this->getMin();
}
int Image::getMaxAlphaValue()
{
	return 255;
}

QDomNode Image::getXml(QDomDocument& doc)
{
	QDomElement imageNode = doc.createElement("image");
	
	QDomElement uidNode = doc.createElement("uid");
	uidNode.appendChild(doc.createTextNode(mUid.c_str()));
	imageNode.appendChild(uidNode);
	
	QDomElement nameNode = doc.createElement("name");
	nameNode.appendChild(doc.createTextNode(mName.c_str()));
	imageNode.appendChild(nameNode);
	
	// Add submodes
	// Add node for 3D transferfunctions
	imageNode.appendChild(mImageTransferFunctions3D->getXml(doc));
	
	return imageNode;
}
void Image::parseXml(QDomNode& dataNode)
{
	if (dataNode.isNull())
		return;
	
	// image node must be parsed in the data manager to create this Image object
	// Only subnodes are parsed here
	QDomNode transferfunctionsNode = dataNode.namedItem("transferfunctions");
	if (!transferfunctionsNode.isNull())
		mImageTransferFunctions3D->parseXml(transferfunctionsNode);
	else
	{
		std::cout << "Warning: Image::parseXml() found no transferfunctions";
		std::cout << std::endl;
	}
}
} // namespace ssc

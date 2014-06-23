// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

#ifndef CXIMAGE_H_
#define CXIMAGE_H_

#include <map>
#include <vector>
#include <boost/shared_ptr.hpp>
#include "cxBoundingBox3D.h"
#include "vtkForwardDeclarations.h"
#include "cxForwardDeclarations.h"
#include "cxData.h"

typedef boost::shared_ptr<std::map<int, int> > HistogramMapPtr;

class QDomNode;
class QDomDocument;


namespace cx
{
//typedef boost::shared_ptr<class Landmarks> LandmarksPtr;

/**\brief A volumetric data set.
 *
 * One volumetric data set, represented as a vtkImageData,
 * along with auxiliary data.
 *
 * \ingroup cx_resource_core_data
 */
class Image : public Data
{
Q_OBJECT

public:
	struct ShadingStruct
	{
		bool on;
		double ambient;
		double diffuse;
		double specular;
		double specularPower;

		ShadingStruct();
		void addXml(QDomNode dataNode);
		void parseXml(QDomNode dataNode);

	private:
		double loadAttribute(QDomNode dataNode, QString name, double defVal);
	};

	static ImagePtr create(const QString& uid, const QString& name);
	ImagePtr copy();

	virtual ~Image();
	Image(const QString& uid, const vtkImageDataPtr& data, const QString& name = "");

	virtual void setVtkImageData(const vtkImageDataPtr& data);

	virtual vtkImageDataPtr getBaseVtkImageData(); ///< \return the vtkimagedata in the data coordinate space
	virtual vtkImageDataPtr getGrayScaleVtkImageData(); ///< as getBaseVtkImageData(), but constrained to 1 component if multicolor.
	virtual vtkImageDataPtr get8bitGrayScaleVtkImageData();///< Have never been used or tested. Create a test for it
//	virtual vtkImageDataPtr getRefVtkImageData(); ///< \return the vtkimagedata in the reference coordinate space
//	LandmarksPtr getLandmarks();
	/** Return a version of this, containing image data and transfer functions converted to unsigned.
	  * This is used for the 3D texture slicer that doesnt handle signed data.
	  */
	virtual ImagePtr getUnsigned(ImagePtr self);

	virtual QString getModality() const;
	virtual void setModality(const QString& val);
	virtual QString getImageType() const;
	virtual void setImageType(const QString& val);

	virtual ImageTF3DPtr getTransferFunctions3D();
	virtual void setTransferFunctions3D(ImageTF3DPtr transferFuntion);
	virtual ImageLUT2DPtr getLookupTable2D();
	virtual void setLookupTable2D(ImageLUT2DPtr imageLookupTable2D);

	virtual void setInitialWindowLevel(double width, double level);
	double getInitialWindowLevel() const { return mInitialWindowLevel; }
	double getInitialWindowWidth() const { return mInitialWindowWidth; }

	virtual DoubleBoundingBox3D boundingBox() const; ///< bounding box in image space
	virtual Eigen::Array3d getSpacing() const;
	virtual vtkImageAccumulatePtr getHistogram();///< \return The histogram for the image
	virtual int getMax();	///< \return Return highest used value in the image
	virtual int getMin();	///< \return Return lowest used value in the image
	virtual int getRange();///< For convenience: getMax() - getMin()
	virtual int getMaxAlphaValue();///<Max alpha value (probably 255)
	virtual void setShadingOn(bool on);
	virtual bool getShadingOn() const;
	virtual void setShadingAmbient(double ambient); ///<Set shading ambient parmeter
	virtual void setShadingDiffuse(double diffuse); ///<Set shading diffuse parmeter
	virtual void setShadingSpecular(double specular); ///<Set shading specular parmeter
	virtual void setShadingSpecularPower(double specularPower); ///<Set shading specular power parmeter
	virtual double getShadingAmbient(); ///<Get shading ambient parmeter
	virtual double getShadingDiffuse(); ///<Get shading diffuse parmeter
	virtual double getShadingSpecular(); ///<Get shading specular parmeter
	virtual double getShadingSpecularPower(); ///<Get shading specular power parmeter
	virtual Image::ShadingStruct getShading();
	virtual void setShading(Image::ShadingStruct shading);

	void addXml(QDomNode& dataNode); ///< adds xml information about the image and its variabels \param dataNode Data node in the XML tree \return The created subnode
	virtual void parseXml(QDomNode& dataNode);///< Use a XML node to load data. \param dataNode A XML data representation of this object.
	virtual bool load(QString path);
	virtual QString getType() const
	{
		return getTypeName();
	}
	static QString getTypeName()
	{
		return "image";
	}

	// methods for defining and storing a cropping box. Image does not use these data, this is up to the mapper
	virtual void setCropping(bool on);
	virtual bool getCropping() const;
	virtual void setCroppingBox(const DoubleBoundingBox3D& bb_d);
	virtual DoubleBoundingBox3D getCroppingBox() const;

	// methods for defining and storing clip planes. Image does not use these data, this is up to the mapper
	virtual void addPersistentClipPlane(vtkPlanePtr plane);
	virtual std::vector<vtkPlanePtr> getAllClipPlanes();
	virtual void clearPersistentClipPlanes();
	virtual void setInteractiveClipPlane(vtkPlanePtr plane); ///< set a plane that is not saved

	void mergevtkSettingsIntosscTransform();

	void resetTransferFunctions(bool _2D=true, bool _3D=true); ///< Resets the transfer functions and creates new defaut values.

	void moveThisAndChildrenToThread(QThread* thread); ///< Move this and all children to thread. Use the thread is generated in a worker thread and the result is to be used in the main thread.

	static vtkImageDataPtr createDummyImageData(int axisSize, int maxVoxelValue); ///< Create a moc object of vtkImageData

	void setInterpolationTypeToNearest();
	void setInterpolationTypeToLinear();
	void setInterpolationType(int val);
	int getInterpolationType() const;

	vtkImageDataPtr resample(long maxVoxels);

signals:
	void vtkImageDataChanged(); ///< emitted when the vktimagedata are invalidated and must be retrieved anew.
	void transferFunctionsChanged(); ///< emitted when image transfer functions in 2D or 3D are changed.
	void clipPlanesChanged();
	void cropBoxChanged();

protected slots:
	virtual void transformChangedSlot();

protected:
	vtkImageDataPtr mBaseImageData; ///< image data in data space
	vtkImageDataPtr mBaseGrayScaleImageData; ///< image data in data space
//	vtkImageReslicePtr mOrientator; ///< converts imagedata to outputimagedata
//	vtkMatrix4x4Ptr mOrientatorMatrix;
//	vtkImageDataPtr mReferenceImageData; ///< imagedata after filtering through the orientatior, given in reference space
	vtkImageAccumulatePtr mHistogramPtr;///< Histogram
	ImagePtr mUnsigned; ///< version of this containing unsigned data.

//	LandmarksPtr mLandmarks;

	ShadingStruct mShading;

	bool mUseCropping; ///< image should be cropped using mCroppingBox
	DoubleBoundingBox3D mCroppingBox_d; ///< box defining the cropping size.
	std::vector<vtkPlanePtr> mPersistentClipPlanes;
	vtkPlanePtr mInteractiveClipPlane;
	QString mModality; ///< modality of the image, defined as DICOM tag (0008,0060), Section 3, C.7.3.1.1.1
	QString mImageType; ///< type of the image, defined as DICOM tag (0008,0008) (mainly value 3, but might be a merge of value 4), Section 3, C.7.6.1.1.2
	double mMaxRGBIntensity;
	int mInterpolationType; ///< mirror the interpolationType in vtkVolumeProperty


private:
	Image(const Image& other);
	Image& operator=(const Image& other);
	void resetTransferFunction(ImageTF3DPtr imageTransferFunctions3D, ImageLUT2DPtr imageLookupTable2D);
	void resetTransferFunction(ImageLUT2DPtr imageLookupTable2D);
	void resetTransferFunction(ImageTF3DPtr imageTransferFunctions3D);
	DoubleBoundingBox3D getInitialBoundingBox() const;
	double loadAttribute(QDomNode dataNode, QString name, double defVal);

	double computeResampleFactor(long maxVoxels);

	ImageTF3DPtr mImageTransferFunctions3D;
	ImageLUT2DPtr mImageLookupTable2D;

	double mInitialWindowWidth;
	double mInitialWindowLevel;
};

} // end namespace cx

#endif /*CXIMAGE_H_*/

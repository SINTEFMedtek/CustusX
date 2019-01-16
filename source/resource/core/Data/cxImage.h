/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#ifndef CXIMAGE_H_
#define CXIMAGE_H_

#include "cxResourceExport.h"
#include "cxPrecompiledHeader.h"

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
typedef std::map<int, int> IntIntMap;
typedef std::map<int, QColor> ColorMap;

/** \brief A volumetric data set.
 *
 * One volumetric data set, represented as a vtkImageData,
 * along with auxiliary data.
 *
 * \ingroup cx_resource_core_data
 */
class cxResource_EXPORT Image : public Data
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

	/** Initialize image with properties from parent image, but with separate vtkImageData and name.
	 */
	virtual void intitializeFromParentImage(ImagePtr parentImage);
	virtual void setVtkImageData(const vtkImageDataPtr& data, bool resetTransferFunctions = true);

	virtual vtkImageDataPtr getBaseVtkImageData(); ///< \return the vtkimagedata in the data coordinate space
	virtual vtkImageDataPtr getGrayScaleVtkImageData(); ///< as getBaseVtkImageData(), but constrained to 1 component if multicolor.
	virtual vtkImageDataPtr get8bitGrayScaleVtkImageData();///< Have never been used or tested. Create a test for it
	/** Return a version of this, containing image data and transfer functions converted to unsigned.
	  * This is used for the 3D texture slicer that doesnt handle signed data.
	  */
	virtual ImagePtr getUnsigned(ImagePtr self);

	virtual IMAGE_MODALITY getModality() const;
	virtual void setModality(const IMAGE_MODALITY &val);
	virtual IMAGE_SUBTYPE getImageType() const;
	virtual void setImageType(const IMAGE_SUBTYPE &val);

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
	virtual bool load(QString path, FileManagerServicePtr filemanager);
	virtual QString getType() const
	{
		return getTypeName();
	}
	static QString getTypeName()
	{
		return "image";
	}
	virtual QIcon getIcon() {return QIcon(":/icons/volume.png");}

	// methods for defining and storing a cropping box. Image does not use these data, this is up to the mapper
	virtual void setCropping(bool on);
	virtual bool getCropping() const;
	virtual void setCroppingBox(const DoubleBoundingBox3D& bb_d);
	virtual DoubleBoundingBox3D getCroppingBox() const;

	void mergevtkSettingsIntosscTransform();

	void resetTransferFunctions(bool _2D=true, bool _3D=true); ///< Resets the transfer functions and creates new default values.

	void moveThisAndChildrenToThread(QThread* thread); ///< Move this and all children to thread. Use the thread is generated in a worker thread and the result is to be used in the main thread.

	static vtkImageDataPtr createDummyImageData(int axisSize, int maxVoxelValue); ///< Create a moc object of vtkImageData

//	void setInterpolationTypeToNearest();
//	void setInterpolationTypeToLinear();
	void setInterpolationType(int val);
	int getInterpolationType() const;

	vtkImageDataPtr resample(long maxVoxels);

	virtual void save(const QString &basePath, FileManagerServicePtr filemanager);

	void startThresholdPreview(const Eigen::Vector2d& threshold);
	void stopThresholdPreview();
	double getVTKMinValue();
	double getVTKMaxValue();
	bool is2D();

signals:
	void vtkImageDataChanged(QString uid = QString()); ///< emitted when the vktimagedata are invalidated and must be retrieved anew.
	void transferFunctionsChanged(); ///< emitted when image transfer functions in 2D or 3D are changed.
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

	IMAGE_MODALITY mModality; ///< modality of the image, defined as DICOM tag (0008,0060), Section 3, C.7.3.1.1.1
	IMAGE_SUBTYPE mImageType; ///< type of the image, defined as DICOM tag (0008,0008) (mainly value 3, but might be a merge of value 4), Section 3, C.7.6.1.1.2
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

	ColorMap createPreviewColorMap(const Eigen::Vector2d &threshold);
	IntIntMap createPreviewOpacityMap(const Eigen::Vector2d &threshold);
	void createThresholdPreviewTransferFunctions3D(const Eigen::Vector2d &threshold);
	void createThresholdPreviewLookupTable2D(const Eigen::Vector2d &threshold);

	ImageTF3DPtr getUnmodifiedTransferFunctions3D();
	ImageLUT2DPtr getUnmodifiedLookupTable2D();

	ImageTF3DPtr mImageTransferFunctions3D;
	ImageLUT2DPtr mImageLookupTable2D;

	double mInitialWindowWidth;
	double mInitialWindowLevel;

	bool mThresholdPreview;
	ImageTF3DPtr mTresholdPreviewTransferfunctions3D;
	ImageLUT2DPtr mTresholdPreviewLookupTable2D;
};

} // end namespace cx

#endif /*CXIMAGE_H_*/

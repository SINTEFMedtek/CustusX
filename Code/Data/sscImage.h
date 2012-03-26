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

#ifndef SSCIMAGE_H_
#define SSCIMAGE_H_

#include <map>
#include <vector>
#include <boost/shared_ptr.hpp>
#include "sscLandmark.h"
#include "sscBoundingBox3D.h"
#include "vtkForwardDeclarations.h"
#include "sscForwardDeclarations.h"
#include "sscData.h"

typedef boost::shared_ptr<std::map<int, int> > HistogramMapPtr;
typedef std::map<QString, class Landmark> LandmarkMap;

class QDomNode;
class QDomDocument;

//#define USE_TRANSFORM_RESCLICER

namespace ssc
{

/**\brief A volumetric data set.
 *
 * One volumetric data set, represented as a vtkImageData,
 * along with auxiliary data.
 *
 * \warning Landmarks are only used by SINTEF atm, so they can change the code
 * at any given point.
 *
 * \ingroup sscData
 */
class Image: public Data
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

	virtual ~Image();
	Image(const QString& uid, const vtkImageDataPtr& data, const QString& name = "");
	void setVtkImageData(const vtkImageDataPtr& data);

	virtual vtkImageDataPtr getBaseVtkImageData(); ///< \return the vtkimagedata in the data coordinate space
	virtual vtkImageDataPtr getGrayScaleBaseVtkImageData(); ///< as getBaseVtkImageData(), but constrained to 1 component if multicolor.
	virtual vtkImageDataPtr getRefVtkImageData(); ///< \return the vtkimagedata in the reference coordinate space
	virtual LandmarkMap getLandmarks(); ///< \return all landmarks defined on the image.

	virtual QString getModality() const;
	virtual void setModality(const QString& val);
	virtual QString getImageType() const;
	virtual void setImageType(const QString& val);

	ImageTF3DPtr getTransferFunctions3D();
	void setTransferFunctions3D(ImageTF3DPtr transferFuntion);
	ImageLUT2DPtr getLookupTable2D();

	void printLandmarks(); //TODO: JUST FOR TESTING
	virtual DoubleBoundingBox3D boundingBox() const; ///< bounding box in image space
	vtkImageAccumulatePtr getHistogram();///< \return The histogram for the image
	int getMax();///< \return Max alpha position in the histogram = max key value in map.
	int getMin();///< \return Min alpha position in the histogram = min key value in map.
	int getRange();///< For convenience: getMax() - getMin()
	int getMaxAlphaValue();///<Max alpha value (probably 255)
	virtual void setShadingOn(bool on);
	virtual bool getShadingOn() const;
	void setShadingAmbient(double ambient); ///<Set shading ambient parmeter
	void setShadingDiffuse(double diffuse); ///<Set shading diffuse parmeter
	void setShadingSpecular(double specular); ///<Set shading specular parmeter
	void setShadingSpecularPower(double specularPower); ///<Set shading specular power parmeter
	double getShadingAmbient(); ///<Get shading ambient parmeter
	double getShadingDiffuse(); ///<Get shading diffuse parmeter
	double getShadingSpecular(); ///<Get shading specular parmeter
	double getShadingSpecularPower(); ///<Get shading specular power parmeter
	Image::ShadingStruct getShading();
	void setShading(Image::ShadingStruct shading);

	void addXml(QDomNode& dataNode); ///< adds xml information about the image and its variabels \param dataNode Data node in the XML tree \return The created subnode
	virtual void parseXml(QDomNode& dataNode);///< Use a XML node to load data. \param dataNode A XML data representation of this object.
	virtual QString getType() const
	{
		return "image";
	}

	// methods for defining and storing a cropping box. Image does not use these data, this is up to the mapper
	virtual void setCropping(bool on);
	virtual bool getCropping() const;
	virtual void setCroppingBox(const DoubleBoundingBox3D& bb_d);
	virtual DoubleBoundingBox3D getCroppingBox() const;

	// methods for defining and storing clip planes. Image does not use these data, this is up to the mapper
	virtual void addClipPlane(vtkPlanePtr plane);
	virtual std::vector<vtkPlanePtr> getClipPlanes();
	virtual void clearClipPlanes();
	void mergevtkSettingsIntosscTransform();

//	void resetTransferFunctions();///< Resets the transfer functions and creates new defaut values.
	void resetTransferFunctions(bool _2D=true, bool _3D=true); ///< Resets the transfer functions and creates new defaut values.
	void resetTransferFunction(ImageTF3DPtr imageTransferFunctions3D, ImageLUT2DPtr imageLookupTable2D);
	void resetTransferFunction(ImageLUT2DPtr imageLookupTable2D);
	void resetTransferFunction(ImageTF3DPtr imageTransferFunctions3D);

	void moveThisAndChildrenToThread(QThread* thread); ///< Move this and all children to thread. Use the thread is generated in a worker thread and the result is to be used in the main thread.

signals:
	void landmarkRemoved(QString uid);
	void landmarkAdded(QString uid);
	void vtkImageDataChanged(); ///< emitted when the vktimagedata are invalidated and must be retrieved anew.
	void transferFunctionsChanged(); ///< emitted when image transfer functions in 2D or 3D are changed.
	void clipPlanesChanged();
	void cropBoxChanged();

public slots:
	void setLandmark(Landmark landmark);
	void removeLandmark(QString uid);

protected slots:
	//	void transferFunctionsChangedSlot();
	virtual void transformChangedSlot();

protected:

	ImageTF3DPtr mImageTransferFunctions3D;
	ImageLUT2DPtr mImageLookupTable2D;

	vtkImageDataPtr mBaseImageData; ///< image data in data space
	vtkImageDataPtr mBaseGrayScaleImageData; ///< image data in data space
	vtkImageReslicePtr mOrientator; ///< converts imagedata to outputimagedata
	vtkMatrix4x4Ptr mOrientatorMatrix;
	vtkImageDataPtr mReferenceImageData; ///< imagedata after filtering through the orientatior, given in reference space
	vtkImageAccumulatePtr mHistogramPtr;///< Histogram

	LandmarkMap mLandmarks; ///< map with all landmarks always in space d (data).

	ShadingStruct mShading;

	bool mUseCropping; ///< image should be cropped using mCroppingBox
	DoubleBoundingBox3D mCroppingBox_d; ///< box defining the cropping size.
	std::vector<vtkPlanePtr> mClipPlanes;
	QString mModality; ///< modality of the image, defined as DICOM tag (0008,0060), Section 3, C.7.3.1.1.1
	QString mImageType; ///< type of the image, defined as DICOM tag (0008,0008) (mainly value 3, but might be a merge of value 4), Section 3, C.7.6.1.1.2




};

} // end namespace ssc

#endif /*SSCIMAGE_H_*/

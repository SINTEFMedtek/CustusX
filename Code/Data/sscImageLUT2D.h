/*
 * sscImageLookupTable2D.h
 *
 *  Created on: Jan 9, 2009
 *      Author: christiana
 */

#ifndef SSCIMAGELOOKUPTABLE2D_H_
#define SSCIMAGELOOKUPTABLE2D_H_

#include <boost/shared_ptr.hpp>
#include <QColor>
#include <QObject>
#include <vtkUnsignedCharArray.h> 
#include "vtkSmartPointer.h" 
typedef vtkSmartPointer<class vtkLookupTable> vtkLookupTablePtr;
typedef vtkSmartPointer<class vtkWindowLevelLookupTable> vtkWindowLevelLookupTablePtr;
typedef vtkSmartPointer<class vtkScalarsToColors> vtkScalarsToColorsPtr;
typedef vtkSmartPointer<class vtkImageData> vtkImageDataPtr;
typedef vtkSmartPointer<class vtkPiecewiseFunction> vtkPiecewiseFunctionPtr;
typedef vtkSmartPointer<class vtkImageMapToColors> vtkImageMapToColorsPtr;
namespace ssc
{

/**Handler for the lookup table used in 2d image slices.
 * This lookup-handler has default lut grayscale
 * Or use stored lut from file.
 * This class will also store differenst windowlevel values and color.
 */
class ImageLUT2D : public QObject
{
	Q_OBJECT
public:
	ImageLUT2D(vtkImageDataPtr base);
	void setVtkImageData(vtkImageDataPtr base);
	void setBaseLookupTable(vtkLookupTablePtr lut);
	vtkLookupTablePtr getOutputLookupTable(); 
	void setLLR(double val);
	void setWindow(double val);
	void setLevel(double val);
	void setAlpha(double val);
	double getWindow() const;
	double getLLR() const;
	double getLevel() const;
	double getAlpha() const;
	
	double getScalarMax() const;
	
signals:
	void transferFunctionsChanged();	
	
private:
	void refreshOutput();
//	void changeOpacity(double index, double opacity);
	void testMap(double val);
	double mapThroughLUT(double x);
	
	vtkLookupTablePtr mOutputLUT; ///< the sum of all internal values
	vtkImageDataPtr mBase; ///< image data
	double mLLR; ///< used to modify the output
	double mWindow; ///< used to modify the output
	double mLevel; ///< used to modify the output
	double mAlpha; ///< not used by output, use BlenderRep to solve this.
	vtkLookupTablePtr mBaseLUT;	///< basis for generating the output lut
};

typedef boost::shared_ptr<ImageLUT2D> ImageLUT2DPtr;


}

#endif /* SSCIMAGELOOKUPTABLE2D_H_ */

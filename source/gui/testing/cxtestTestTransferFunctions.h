#ifndef CXTESTTESTTRANSFERFUNCTIONS_H_
#define CXTESTTESTTRANSFERFUNCTIONS_H_

#include <vtkImageData.h>
#include "sscImage.h"
#include "sscImageTF3D.h"
#include "sscImageLUT2D.h"

namespace cxtest {

/*
 * TestTransferFunctions
 *
 * \brief Helper class for testing 2D/3D transfer functions
 *
 * \date Jul 31, 2013
 * \author Ole Vegard Solberg, SINTEF
 */
class TestTransferFunctions {
public:
	TestTransferFunctions();
	bool hasValid3DTransferFunction();
	bool hasValid2DTransferFunction();
	void Corrupt3DTransferFunctionWindowWidth();
	void Corrupt3DTransferFunctionWindowLevel();
	void Corrupt2DTransferFunctionWindowWidth();
	void Corrupt2DTransferFunctionWindowLevel();
	void setNewCorrupt3DTranferFunction();
	void setNewCorrupt2DTranferFunction();

private:
	vtkImageDataPtr mDummyImageData;
	ssc::ImagePtr mDummyImage;

	void createDummyImage();
};

} /* namespace cxtest */
#endif /* CXTESTTESTTRANSFERFUNCTIONS_H_ */

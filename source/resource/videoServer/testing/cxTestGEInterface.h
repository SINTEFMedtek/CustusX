#ifndef CXTESTGEINTERFACE_H_
#define CXTESTGEINTERFACE_H_

#ifdef CX_USE_ISB_GE
#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include "cxImageStreamer.h"

namespace cxtest
{

/**Unit tests that test the acquisition plugin
 */
class TestGEInterface
{
public:
	void setUp();
	void tearDown();

	void testGEStreamer();///< Test GEStreamer directly. Don't use cxImageSenderGE

	int getValue(vtkSmartPointer<vtkImageData> img, int x, int y, int z);
	void validateData(vtkSmartPointer<vtkImageData> img);
	void validateBMode3D(vtkSmartPointer<vtkImageData> img);
	void testStream(cx::StringMap args);///< Grab a short stream

private:
};

} //namespace cxtest

#endif //CX_USE_ISB_GE
#endif /*CXTESTGEINTERFACE_H_*/

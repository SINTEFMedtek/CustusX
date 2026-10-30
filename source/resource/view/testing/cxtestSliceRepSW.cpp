/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"
#include "cxSlicerRepSW.h"
#include "cxSliceProxy.h"
#include "cxImage.h"
#include "cxRegistrationTransform.h"
#include "cxtestUtilities.h"
#include "cxtestRenderTester.h"
#include <vtkImageData.h>
#include <vtkImageSlice.h>
#include <vtkPointData.h>
#include <vtkDataArray.h>
#include "cxBasicVideoSource.h"
#include "cxTrackedStream.h"
#include "cxImageLUT2D.h"
#include "cxProbeImpl.h"
#include "cxProbeDefinition.h"
#include "cxVolumeHelpers.h"

namespace cxtest
{

TEST_CASE("SliceRepSW: a 2D image is in the slice plane only when parallel and close to it", "[unit][resource][visualization]")
{
	cx::Vector3D center(10, 10, 0);
	CHECK(cx::SliceRepSW::isInSlicePlane(cx::Transform3D::Identity(), center));
	CHECK(cx::SliceRepSW::isInSlicePlane(cx::createTransformRotateZ(0.7) * cx::createTransformTranslate(cx::Vector3D(30, -5, 0.2)), center));
	CHECK(cx::SliceRepSW::isInSlicePlane(cx::createTransformRotateX(M_PI), center)); // seen from the other side
	CHECK_FALSE(cx::SliceRepSW::isInSlicePlane(cx::createTransformTranslate(cx::Vector3D(0, 0, 2)), center));
	CHECK_FALSE(cx::SliceRepSW::isInSlicePlane(cx::createTransformRotateX(0.1), center));
	CHECK_FALSE(cx::SliceRepSW::isInSlicePlane(cx::createTransformRotateY(M_PI / 2), center)); // a line in the slice
}

TEST_CASE("SliceRepSW: a 2D image near the slice plane is moved exactly into it", "[unit][resource][visualization]")
{
	cx::Vector3D center(10, 10, 0);
	cx::Transform3D sMd = cx::createTransformRotateX(0.005) * cx::createTransformTranslate(cx::Vector3D(30, -5, 0.3));
	cx::Transform3D moved = cx::SliceRepSW::moveIntoSlicePlane(sMd, center);
	CHECK(moved.vector(cx::Vector3D(0, 0, 1)).normal()[2] == Approx(1.0));
	cx::Vector3D movedCenter = moved.coord(center);
	cx::Vector3D originalCenter = sMd.coord(center);
	CHECK(movedCenter[0] == Approx(originalCenter[0]));
	CHECK(movedCenter[1] == Approx(originalCenter[1]));
	CHECK(std::abs(movedCenter[2]) < 1e-9);

	cx::Transform3D flipped = cx::SliceRepSW::moveIntoSlicePlane(cx::createTransformRotateX(M_PI) * sMd, center);
	CHECK(flipped.vector(cx::Vector3D(0, 0, 1)).normal()[2] == Approx(-1.0));
}

TEST_CASE("SliceRepSW: draws a 2D image in the slice plane directly, and slices other images", "[unit][resource][visualization]")
{
	cx::SimpleSliceProxyPtr slicer(new cx::SimpleSliceProxy());
	cx::SliceRepSWPtr rep = cx::SliceRepSW::New("rep");
	rep->setSliceProxy(slicer);

	cx::ImagePtr image2D = cxtest::Utilities::create3DImage(Eigen::Array3i(10, 10, 1), 100);
	rep->setImage(image2D);
	CHECK(rep->isDrawnDirectly());

	slicer->set_sMr(cx::createTransformRotateX(M_PI / 2)); // e.g. a view across the image
	rep->update();
	CHECK_FALSE(rep->isDrawnDirectly());

	image2D->get_rMd_History()->setRegistration(cx::createTransformRotateX(-M_PI / 2)); // the image follows the plane
	rep->update();
	CHECK(rep->isDrawnDirectly());

	rep->setImage(cxtest::Utilities::create3DImage(Eigen::Array3i(10, 10, 10), 100));
	CHECK_FALSE(rep->isDrawnDirectly());
}

namespace
{
cx::ImagePtr createImageWithGradient()
{
	cx::ImagePtr image = cxtest::Utilities::create3DImage(Eigen::Array3i(40, 20, 1), cx::Vector3D(1, 1, 1), 0);
	vtkImageDataPtr data = image->getBaseVtkImageData();
	for (int x = 0; x < 40; ++x)
	{
		for (int y = 0; y < 20; ++y)
		{
			data->SetScalarComponentFromDouble(x, y, 0, 0, 50 + 5 * x);
		}
	}
	data->Modified();
	return image;
}

class TestSliceRepSW : public cx::SliceRepSW
{
public:
	static std::shared_ptr<TestSliceRepSW> New()
	{
		return wrap_new(new TestSliceRepSW(), "rep");
	}
	using cx::SliceRepSW::getActor;
};

vtkImageDataPtr renderSlice(std::shared_ptr<TestSliceRepSW> rep, bool drawDirectly)
{
	rep->setDirectDrawingEnabled(drawDirectly);
	CHECK(rep->isDrawnDirectly() == drawDirectly);
	rep->update();
	cxtest::RenderTesterPtr tester = cxtest::RenderTester::create();
	tester->addProp(rep->getActor());
	vtkImageDataPtr viewArea = cxtest::Utilities::create3DVtkImageData(Eigen::Array3i(120, 120, 1));
	viewArea->SetOrigin(-60, -60, 0);
	tester->alignRenderWindowWithImage(viewArea);
	vtkImageDataPtr retval = tester->renderToImage();
	CHECK(cxtest::Utilities::getFractionOfVoxelsAboveThreshold(retval, 0) > 0.01);
	return retval;
}
/** The brightest rendered pixel */
double shownValue(std::shared_ptr<TestSliceRepSW> rep)
{
	rep->update();
	cxtest::RenderTesterPtr tester = cxtest::RenderTester::create();
	tester->addProp(rep->getActor());
	vtkImageDataPtr viewArea = cxtest::Utilities::create3DVtkImageData(Eigen::Array3i(40, 40, 1));
	viewArea->SetOrigin(-15, -15, 0);
	tester->alignRenderWindowWithImage(viewArea);
	vtkImageDataPtr rendered = tester->renderToImage();
	double range[2];
	rendered->GetPointData()->GetScalars()->GetRange(range, 0);
	return range[1];
}

cx::ImagePtr createFrame(unsigned value)
{
	return cxtest::Utilities::create3DImage(Eigen::Array3i(10, 10, 1), cx::Vector3D(1, 1, 1), value);
}

cx::ImagePtr createStreamFrame(unsigned value)
{
	// a stream sends all its frames with the same uid
	return cx::ImagePtr(new cx::Image("us", cx::generateVtkImageData(Eigen::Array3i(10, 10, 1), cx::Vector3D(1, 1, 1), value)));
}

/** As the 2D view does when a video source is added to the stream */
void showStream(std::shared_ptr<TestSliceRepSW> rep, cx::TrackedStreamPtr stream)
{
	cx::ImagePtr image = stream->getChangingImage();
	image->getLookupTable2D()->setWindow(255);
	image->getLookupTable2D()->setLevel(127);
	image->getLookupTable2D()->setLLR(0);
	rep->setImage(image);
}

/** As the video connection does when the first image arrives */
cx::BasicVideoSourcePtr connectVideo(cx::ProbeImplPtr probe, cx::TrackedStreamPtr stream)
{
	cx::BasicVideoSourcePtr source(new cx::BasicVideoSource());
	source->start();
	source->setInput(createStreamFrame(50));
	probe->setRTSource(source);
	stream->setVideoSource(probe->getRTSource("us"));
	return source;
}

/** As the video connection does when it is stopped */
void disconnectVideo(cx::ProbeImplPtr probe, cx::BasicVideoSourcePtr source)
{
	source->stop();
	source->setInput(cx::ImagePtr());
	probe->removeRTSource(source);
}
} // namespace

TEST_CASE("SliceRepSW: a 2D image drawn directly looks like the sliced image", "[unit][resource][visualization]")
{
	cx::SimpleSliceProxyPtr slicer(new cx::SimpleSliceProxy());
	slicer->set_sMr(cx::createTransformRotateX(M_PI / 2));
	cx::ImagePtr image = createImageWithGradient();
	cx::Transform3D sMd = cx::createTransformRotateZ(0.5) * cx::createTransformTranslate(cx::Vector3D(-15, -5, 0));
	image->get_rMd_History()->setRegistration(slicer->get_sMr().inv() * sMd);

	std::shared_ptr<TestSliceRepSW> rep = TestSliceRepSW::New();
	rep->setSliceProxy(slicer);
	rep->setImage(image);

	vtkImageDataPtr direct = renderSlice(rep, true);
	vtkImageDataPtr sliced = renderSlice(rep, false);
	cxtest::RenderTesterPtr tester = cxtest::RenderTester::create();
	tester->setImageErrorThreshold(1.0);
	CHECK(tester->findDifference(direct, sliced));

	image->get_rMd_History()->setRegistration(slicer->get_sMr().inv() * cx::createTransformTranslate(cx::Vector3D(5, 0, 0)) * sMd);
	vtkImageDataPtr moved = renderSlice(rep, true);
	CHECK(Eigen::Array3i(moved->GetDimensions()).isApprox(Eigen::Array3i(sliced->GetDimensions())));
	CHECK_FALSE(tester->findDifference(moved, sliced));
}

TEST_CASE("SliceRepSW: a video stream is shown after it is stopped and started, and after a reconnect", "[unit][resource][visualization]")
{
	std::shared_ptr<cx::BasicVideoSource> source(new cx::BasicVideoSource("us"));
	source->overrideTimeout(false);
	cx::TrackedStreamPtr stream(new cx::TrackedStream("stream", "stream", cx::ToolPtr(), source));
	cx::SimpleSliceProxyPtr slicer(new cx::SimpleSliceProxy());
	std::shared_ptr<TestSliceRepSW> rep = TestSliceRepSW::New();
	rep->setSliceProxy(slicer);
	showStream(rep, stream);

	source->setInput(createFrame(50));
	source->start();
	double first = shownValue(rep);
	CHECK(rep->isDrawnDirectly());
	source->setInput(createFrame(200));
	double second = shownValue(rep);
	CHECK(second != Approx(first));

	source->stop();
	source->start();
	source->setInput(createFrame(50));
	CHECK(shownValue(rep) == Approx(first));

	source->setInput(cx::ImagePtr()); // disconnected
	source->stop();
	source->setInput(createFrame(200));
	source->start();
	CHECK(shownValue(rep) == Approx(second));
	CHECK(rep->isDrawnDirectly());

	// a reconnect can also give the stream a new video source
	std::shared_ptr<cx::BasicVideoSource> newSource(new cx::BasicVideoSource("us"));
	newSource->overrideTimeout(false);
	stream->setVideoSource(newSource);
	showStream(rep, stream);
	newSource->setInput(createFrame(50));
	newSource->start();
	CHECK(shownValue(rep) == Approx(first));
	newSource->setInput(createFrame(200));
	CHECK(shownValue(rep) == Approx(second));
}

namespace
{
/** As the video connection does when it is stopped and started, e.g. from the video connection widget */
void checkStreamAfterVideoRestart(bool drawDirectly)
{
	cx::ProbeImplPtr probe = cx::ProbeImpl::New("", "");
	cx::ProbeDefinition definition;
	definition.setUid("us");
	definition.setSpacing(cx::Vector3D(1, 1, 1));
	definition.setSize(QSize(10, 10));
	probe->setProbeDefinition(definition);
	cx::TrackedStreamPtr stream(new cx::TrackedStream("stream", "stream", cx::ToolPtr(), cx::VideoSourcePtr()));
	cx::BasicVideoSourcePtr source = connectVideo(probe, stream);

	cx::SimpleSliceProxyPtr slicer(new cx::SimpleSliceProxy());
	std::shared_ptr<TestSliceRepSW> rep = TestSliceRepSW::New();
	rep->setSliceProxy(slicer);
	rep->setDirectDrawingEnabled(drawDirectly);
	showStream(rep, stream);
	double first = shownValue(rep);
	source->setInput(createStreamFrame(200));
	double second = shownValue(rep);
	CHECK(second != Approx(first));

	disconnectVideo(probe, source);
	source = connectVideo(probe, stream);
	probe->setProbeDefinition(definition); // sent again by the streamer
	showStream(rep, stream);
	CHECK(shownValue(rep) == Approx(first));
	source->setInput(createStreamFrame(200));
	CHECK(shownValue(rep) == Approx(second));
}
} // namespace

TEST_CASE("SliceRepSW: a probe video stream is shown after the video connection is stopped and started", "[unit][resource][visualization]")
{
	checkStreamAfterVideoRestart(true);
	checkStreamAfterVideoRestart(false);
}

} // namespace cxtest

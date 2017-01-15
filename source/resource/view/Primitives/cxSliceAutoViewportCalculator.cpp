#include "cxSliceAutoViewportCalculator.h"

#include "cxUtilHelpers.h"
#include "cxLogger.h"


namespace cx
{

SliceAutoViewportCalculator::ReturnType SliceAutoViewportCalculator::calculate()
{
	ReturnType retval;
//	retval.center_shift_s = Vector3D::Zero();
//	retval.zoom = 0;
//	CX_LOG_CHANNEL_DEBUG("CA") << "rois" << mROI_s;

	if (mFollowType==ftFIXED_CENTER)
	{

		// find the smallest viewport fitting the roi, by changing zoom and center
		if (this->isAutoZoom())
			return this->calculateZoomCenter_SmallestFitROI();
		// move center only, in order to keep the tool visible.
		else
			return this->calculateCenter_FitToolIntoViewport();
	}
	else
	{
		// find the smallest viewport fitting the roi, by changing zoom only
		if (this->isAutoZoom())
			return this->calculateZoom_FitROIKeepCenter();
	}

	return retval;
}

bool SliceAutoViewportCalculator::isAutoZoom() const
{
	return !similar(mROI_s, DoubleBoundingBox3D::zero());
}

Vector3D SliceAutoViewportCalculator::findCenterShift_s()
{
	Vector3D shift = Vector3D::Zero();

	Vector3D pt_s = this->findVirtualTooltip_s();
	DoubleBoundingBox3D BB_s = this->findStaticBox();
	shift = this->findShiftFromBoxToTool_s(BB_s, pt_s);

//	if (!similar(shift, Vector3D::Zero()))
//	{
//		std::cout << "type: " << enum2string(mSliceProxy->getComputer().getPlaneType()) << std::endl;
//		std::cout << "mBB_s: " << mBB_s << std::endl;
//		std::cout << "BB_s: " << BB_s << std::endl;
//		std::cout << "pt_s: " << pt_s << std::endl;
//		Vector3D pt_r = rMpr * prMt.coord(Vector3D(0,0,tool->getTooltipOffset()));
//		std::cout << "pt_r: " << pt_r << std::endl;
//		std::cout << "shift: " << shift << std::endl;
//	}

	return shift;
}

Vector3D SliceAutoViewportCalculator::findVirtualTooltip_s()
{
	Vector3D pt_s = mTooltip_s;
	pt_s[2] = 0; // project into plane
	return pt_s;
}

DoubleBoundingBox3D SliceAutoViewportCalculator::findStaticBox()
{
	double followTooltipBoundary = mFollowTooltipBoundary;
	followTooltipBoundary = constrainValue(followTooltipBoundary, 0.0, 0.5);
	Transform3D S = createTransformScale(Vector3D::Ones()*(1.0-2.0*followTooltipBoundary));
	Transform3D T = createTransformTranslate(mBB_s.center());
	DoubleBoundingBox3D BB_s = transform(T*S*T.inv(), mBB_s);
	return BB_s;
}

/**
 * Find the shift required to move BB_s to include pt_s.
 */
Vector3D SliceAutoViewportCalculator::findShiftFromBoxToTool_s(DoubleBoundingBox3D BB_s, Vector3D pt_s)
{
	Vector3D shift = Vector3D::Zero();

	for (unsigned i=0; i<2; ++i) // loop over two first dimensions, check if pt outside of bb
	{
		if (pt_s[i] < BB_s[2*i])
			shift[i] += pt_s[i] - BB_s[2*i];
		if (pt_s[i] > BB_s[2*i+1])
			shift[i] += pt_s[i] - BB_s[2*i+1];
	}

	return shift;
}

/**
 * Find the shift required to move BB_s to include roi_s.
 */
Vector3D SliceAutoViewportCalculator::findShiftFromBoxToROI(DoubleBoundingBox3D bb, DoubleBoundingBox3D roi)
{
	Vector3D shift = Vector3D::Zero();

	for (unsigned i=0; i<2; ++i) // loop over two first dimensions, check if roi outside of bb
	{
		if (roi[2*i  ] < bb[2*i  ])
			shift[i] += roi[2*i  ] - bb[2*i ];
		if (roi[2*i+1] > bb[2*i+1])
			shift[i] += roi[2*i+1] - bb[2*i+1];
	}

	return shift;
}

SliceAutoViewportCalculator::ReturnType SliceAutoViewportCalculator::calculateZoomCenter_SmallestFitROI()
{
	// zoom to centered box
	// shift pos to box

	// roi: bb defining region of interest
	// box: viewable section
	//
	// find centered roi
	// find zoom needed to see entire centered roi
	// apply zoom to box and emit zoom
	// find shift required to see entire roi
	// set center based on shift

	ReturnType retval;
	DoubleBoundingBox3D roi_s = mROI_s;
	Transform3D T = createTransformTranslate(-roi_s.center());
	DoubleBoundingBox3D roi_sc = transform(T, roi_s);
	//		CX_LOG_CHANNEL_DEBUG("CA") << "roi_s " << roi_s;
	//		CX_LOG_CHANNEL_DEBUG("CA") << "roi_sc " << roi_sc;
	//		CX_LOG_CHANNEL_DEBUG("CA") << "mBB_s " << mBB_s;

	double zoom = this->findZoomRequiredToIncludeRoi(mBB_s, roi_sc);
	//		CX_LOG_CHANNEL_DEBUG("CA") << "autozoom zoom " << zoom;
	Transform3D S = createTransformScale(Vector3D::Ones()*zoom);
	DoubleBoundingBox3D bb_zoomed = transform(S, mBB_s);
	//		CX_LOG_CHANNEL_DEBUG("CA") << "bb_zoomed " << bb_zoomed;

	// find shift
	Vector3D shift = this->findShiftFromBoxToROI(bb_zoomed, roi_s);
	//		CX_LOG_CHANNEL_DEBUG("CA") << "autozoom shift " << shift;

	retval.center_shift_s = shift;
	retval.zoom = 1.0/zoom;

	return retval;
}

SliceAutoViewportCalculator::ReturnType SliceAutoViewportCalculator::calculateCenter_FitToolIntoViewport()
{
	ReturnType retval;
	if (mFollowTooltip)
	{
		retval.center_shift_s = this->findCenterShift_s();
	}
	return retval;
}

SliceAutoViewportCalculator::ReturnType SliceAutoViewportCalculator::calculateZoom_FitROIKeepCenter()
{
	ReturnType retval;

	DoubleBoundingBox3D roi_s = mROI_s;
	// zoom to actual box

	// find zoom needed to see entire box
	// emit zoom
	double zoom = this->findZoomRequiredToIncludeRoi(mBB_s, roi_s);
//	CX_LOG_CHANNEL_DEBUG("CA") << "autozoom zoom only " << zoom;
	retval.zoom = 1.0/zoom; //???? check inversion

	return retval;
}

//SliceAutoViewportCalculator::ReturnType SliceAutoViewportCalculator::autoZoom()
//{
//	ReturnType retval;
//	retval.center_shift_s = Vector3D::Zero();
//	retval.zoom = 0;

//	DoubleBoundingBox3D roi_s = mROI_s;
//	//DoubleBoundingBox3D roi_s = this->getROI_BB_s();
//	// autozoom
//	// if orthogonal: zoom to centered box
//	//                shift pos to box
//	//
//	// if oblique:    zoom to actual box
//	//
//	if (mFollowType==ftFIXED_CENTER)
//	{
//		// roi: bb defining region of interest
//		// box: viewable section
//		//
//		// find centered roi
//		// find zoom needed to see entire centered roi
//		// apply zoom to box and emit zoom
//		// find shift required to see entire roi
//		// set center based on shift

////		CX_LOG_CHANNEL_DEBUG("CA") << "";

//		Transform3D T = createTransformTranslate(-roi_s.center());
//		DoubleBoundingBox3D roi_sc = transform(T, roi_s);
////		CX_LOG_CHANNEL_DEBUG("CA") << "roi_s " << roi_s;
////		CX_LOG_CHANNEL_DEBUG("CA") << "roi_sc " << roi_sc;
////		CX_LOG_CHANNEL_DEBUG("CA") << "mBB_s " << mBB_s;

//		double zoom = this->findZoomRequiredToIncludeRoi(mBB_s, roi_sc);
////		CX_LOG_CHANNEL_DEBUG("CA") << "autozoom zoom " << zoom;
//		Transform3D S = createTransformScale(Vector3D::Ones()*zoom);
//		DoubleBoundingBox3D bb_zoomed = transform(S, mBB_s);
////		CX_LOG_CHANNEL_DEBUG("CA") << "bb_zoomed " << bb_zoomed;

//		// find shift
//		Vector3D shift = this->findShiftFromBoxToROI(bb_zoomed, roi_s);
////		CX_LOG_CHANNEL_DEBUG("CA") << "autozoom shift " << shift;

////		Vector3D newcenter_r = this->findShiftedCenter_r(shift);

//		retval.center_shift_s = shift;
//		retval.zoom = 1.0/zoom;

////		if (!similar(zoom, 1.0))
////		{
////			CX_LOG_CHANNEL_DEBUG("CA") << this << " autozoom zoom " << zoom;
////			emit newZoom(1.0/zoom);
////		}
////		if (!similar(shift, Vector3D::Zero()))
////		{
////			CX_LOG_CHANNEL_DEBUG("CA") << this << "autozoom shift " << shift;
////			mDataManager->setCenter(newcenter_r);
////		}
//	}
//	else // ftFOLLOW_TOOL
//	{
//		// find zoom needed to see entire box
//		// emit zoom
//		double zoom = this->findZoomRequiredToIncludeRoi(mBB_s, roi_s);
//		CX_LOG_CHANNEL_DEBUG("CA") << "autozoom zoom " << zoom;
////		emit newZoom(zoom);
//		retval.zoom = 1.0/zoom; //???? check inversion
//	}

//	return retval;
//}

double SliceAutoViewportCalculator::findZoomRequiredToIncludeRoi(DoubleBoundingBox3D base, DoubleBoundingBox3D roi)
{
	double scale = 0;
	// find zoom in x and y
	for (int i=0; i<2; ++i)
	{
		double base_max = fabs(std::max(base[2*i], base[2*i+1]));
		double roi_max = fabs(std::max(roi[2*i], roi[2*i+1]));
		scale = std::max(scale, roi_max/base_max);
	}

	return scale;
}


} // namespace cx

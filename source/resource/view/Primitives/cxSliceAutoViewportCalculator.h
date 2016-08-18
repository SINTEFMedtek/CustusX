#ifndef CXSLICEAUTOVIEWPORTCALCULATOR_H
#define CXSLICEAUTOVIEWPORTCALCULATOR_H

#include "cxResourceVisualizationExport.h"
#include "cxVector3D.h"
#include "cxBoundingBox3D.h"
#include "cxTransform3D.h"
#include "cxDefinitions.h"

namespace cx
{

/**
 * Based on a given slice definition, and a (center, zoom)
 * find a (center,zoom) such that the tool and roi is visible.
 */
class cxResourceVisualization_EXPORT SliceAutoViewportCalculator
{
public:

	struct ReturnType
	{
		ReturnType(Vector3D center_shift_s_=Vector3D::Zero(), double zoom_=1) :
			center_shift_s(center_shift_s_),
			zoom(zoom_)
		{
		}

		double zoom;
		Vector3D center_shift_s;
	};
	// find a (center,zoom) change according the the input config.
	SliceAutoViewportCalculator::ReturnType calculate();

	// MUST set all these parameters before call to calculate() is valid.
	FOLLOW_TYPE mFollowType;
	Vector3D mTooltip_s;
	double mFollowTooltipBoundary;
	bool mFollowTooltip;
	DoubleBoundingBox3D mBB_s;
	DoubleBoundingBox3D mROI_s;

private:
	Vector3D findCenterShift_s();
	ReturnType autoZoom();
	Vector3D findVirtualTooltip_s();
	DoubleBoundingBox3D findStaticBox();
	Vector3D findShiftFromBoxToTool_s(DoubleBoundingBox3D BB_s, Vector3D pt_s);
	Vector3D findShiftFromBoxToROI(DoubleBoundingBox3D bb, DoubleBoundingBox3D roi);
	double findZoomRequiredToIncludeRoi(DoubleBoundingBox3D base, DoubleBoundingBox3D roi);
	bool isAutoZoom() const;
	ReturnType calculateZoomCenter_SmallestFitROI();
	ReturnType calculateCenter_FitToolIntoViewport();
	ReturnType calculateZoom_FitROIKeepCenter();
};

} // namespace cx

#endif // CXSLICEAUTOVIEWPORTCALCULATOR_H

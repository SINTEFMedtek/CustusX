#ifndef SSCORIENTATIONANNOTATION2DREP_H_
#define SSCORIENTATIONANNOTATION2DREP_H_

#include "sscRepImpl.h"
#include "sscDefinitions.h"
#include "vtkForwardDeclarations.h"
#include "sscForwardDeclarations.h"
#include "sscVector3D.h"
#include "sscTransform3D.h"
#include <vector>

namespace ssc
{

typedef boost::shared_ptr<class OrientationAnnotationSmartRep> OrientationAnnotationSmartRepPtr;
typedef vtkSmartPointer<class OrientationAnnotation> OrientationAnnotationPtr;

/**\brief A class that annotated 2D views with otientation information.
 * \ingroup cxServiceVisualizationRep
 *
 * The slice proxy is used to find the orientation of a slice in space r,
 * which is assumed to be a valid DICOM space, and combinations of
 * the letters
 * 	Anterior - Posterior
 *  Left - Right
 *  Superior - Inferior
 * are used. Combinations of the letters are used for oblique angles.
 */
class OrientationAnnotationSmartRep: public ssc::RepImpl
{
Q_OBJECT
public:
	static OrientationAnnotationSmartRepPtr New(const QString& uid, const QString& name);
	virtual ~OrientationAnnotationSmartRep();
	virtual QString getType() const	{ return "vm::OrientationAnnotationSmartRep"; }

	void setSliceProxy(ssc::SliceProxyPtr slicer);
	/**The maximum angular deviation from a major axis
	 * for annotation to be displayed.
	 */
	void ThresholdAngle(double angle);
	double ThresholdAngle() const;

	void setVisible(bool visible);
private slots:
	void transformChangedSlot();
protected:
	OrientationAnnotationSmartRep(const QString& uid, const QString& name);
	virtual void addRepActorsToViewRenderer(ssc::View* view);
	virtual void removeRepActorsFromViewRenderer(ssc::View* view);

	QString determineAnnotation(ssc::Vector3D planeDir_s, ssc::Transform3D rMs);
	void createAnnotation();

	double mAngle;
	SliceProxyPtr mSlicer;
	OrientationAnnotationPtr mOrientation;
	std::map<QString, ssc::Vector3D> mDCMDirections_r; ///< directions of DICOM labels APSILR
	std::vector<ssc::Vector3D> mPlaneDirections_s; ///< the four directions in the slice plane
};

}

#endif /*SSCORIENTATIONANNOTATION2DREP_H_*/


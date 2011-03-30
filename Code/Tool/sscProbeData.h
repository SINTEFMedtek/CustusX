#ifndef SSCPROBEDATA_H_
#define SSCPROBEDATA_H_

#include <QSize>
class QDomNode;
#include "sscVector3D.h"
#include "sscBoundingBox3D.h"

namespace ssc
{

/**Definition of characteristics for an Ultrasound Probe.
 *
 *  * Definitions of probe geometry:
 *
 *  Sector probe:
 *
 *                    *   -------------- origin of t space
 *                  '   '
 *                '       '
 *              '           '
 *            '  '         '  '
 *          '      ' ' ' '      '  ----- depth start
 *        '                       '
 *      '                           '
 *        ' ' '               ' ' '
 *              ' ' ' ' ' ' '  --------- depth end
 *
 *             width in radians
 *
 *
 * Linear probe:
 *
 *         ' ' ' ' ' '*' ' ' ' ' ------- depth start, origin (*) of t space
 *         '                   '
 *         '                   '
 *         '                   '
 *         '                   '
 *         '                   '
 *         '                   '
 *         ' ' ' ' ' ' ' ' ' ' ' ------- depth end
 *
 *              width in mm
 *
 *
 * Associated image:
 *
 *            x                   coordinate space v: origin upper left corner
 *      v*---->
 *       |
 *       | ' ' ' ' ' ' ' ' ' ' '  size: pixel dimensions (width, height)
 *      y. '                   '  spacing: pixel size (width, height)
 *         '         *         '  origin_u: calibration center in space u, dimensions mm. origin of t space.
 *         '                   '
 *      y^ '                   '
 *       | '                   '
 *       | ' ' ' ' ' ' ' ' ' ' '
 *      u*---->                  coordinate space u: origin lower left corner
 *            x
 *
 *
 */
class ProbeData
{
public: 
  struct ProbeImageData
  {
//    ssc::Vector3D mOrigin_u; ///< probe origin in image space u. (lower-left corner origin)
    ssc::Vector3D getOrigin_u() const; ///< probe origin in image space u. (lower-left corner origin)
    ssc::DoubleBoundingBox3D getClipRect_u() const; ///< probe origin in image space u. (lower-left corner origin)
    ssc::Vector3D mOrigin_p; ///< probe origin in pixel space p. (upper-left corner origin)
    ssc::Vector3D mSpacing;
    ssc::DoubleBoundingBox3D mClipRect_p; ///< sector clipping rect, in addition to the standard sector definition. The probe sector is the intersection of the sector definition and the clip rect.
    QSize mSize;
    Vector3D transform_p_to_u(const Vector3D& q_p) const;
  };

  enum TYPE { tNONE=0,   ///< undefined
				tSECTOR=1, ///< US beam is emitted radially in a flat cone. 
				tLINEAR=2  ///< US beam is emitted straight forward.
				};	
public:
  ProbeData();
  ProbeData(TYPE type, double depthStart, double depthEnd, double width);
  void addXml(QDomNode& dataNode) const; ///< write internal state to node

	TYPE mType; ///< type of probe
	double mDepthStart; ///< start of sector, mm
	double mDepthEnd; ///< end of sector, mm
	double mWidth; ///< width of sector in radians (SECTOR) or millimeters (LINEAR).
  double mTemporalCalibration;
	ProbeImageData mImage;
};

} // namespace ssc

#endif /*SSCPROBEDATA_H_*/

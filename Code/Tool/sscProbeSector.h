#ifndef SSCPROBESECTOR_H_
#define SSCPROBESECTOR_H_

#include <QSize>
class QDomNode;
#include "sscVector3D.h"

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
class ProbeSector
{
public: 
  struct ProbeImageData
  {
    ssc::Vector3D mOrigin_u; ///< probe origin in image space u. (lower-left corner origin)
    ssc::Vector3D mSpacing;
    QSize mSize;
  };

  enum TYPE { tNONE=0,   ///< undefined
				tSECTOR=1, ///< US beam is emitted radially in a flat cone. 
				tLINEAR=2  ///< US beam is emitted straight forward.
				};	
public:
	ProbeSector();
	ProbeSector(TYPE type, double depthStart, double depthEnd, double width);
  void addXml(QDomNode& dataNode) const; ///< write internal state to node

	TYPE mType; ///< type of probe
	double mDepthStart; ///< start of sector, mm
	double mDepthEnd; ///< end of sector, mm
	double mWidth; ///< width of sector in radians (SECTOR) or millimeters (LINEAR).
	ProbeImageData mImage;
};

} // namespace ssc

#endif /*SSCPROBESECTOR_H_*/

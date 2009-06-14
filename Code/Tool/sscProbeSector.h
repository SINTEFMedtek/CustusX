#ifndef SSCPROBESECTOR_H_
#define SSCPROBESECTOR_H_

namespace ssc
{

/**Definition of characteristics for an Ultrasound Probe.
 */
class ProbeSector
{
public: 
	enum TYPE { tNONE=0,   ///< undefined
				tSECTOR=1, ///< US beam is emitted radially in a flat cone. 
				tLINEAR=2  ///< US beam is emitted straight forward.
				};	
public:
	ProbeSector();
	ProbeSector(TYPE type, double depthStart, double depthEnd, double width);
	
	TYPE mType; ///< type of probe
	double mDepthStart; ///< start of sector, mm
	double mDepthEnd; ///< end of sector, mm
	double mWidth; ///< width of sector in radians (SECTOR) or millimeters (LINEAR). 
};

} // namespace ssc

#endif /*SSCPROBESECTOR_H_*/

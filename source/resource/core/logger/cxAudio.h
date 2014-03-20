#ifndef CXAUDIO_H_
#define CXAUDIO_H_

#include "boost/shared_ptr.hpp"

namespace cx
{

/**\brief Interface for playing sounds
 *
 * \date Mar 11, 2011
 * \author Janne Beate Bakeng, SINTEF
 *
 * \ingroup cx_resource_core_logger
 */
class Audio
{
public:
	Audio(){};
	virtual ~Audio(){};

	virtual void playStartSound() = 0;
	virtual void playStopSound() = 0;
	virtual void playCancelSound() = 0;

	virtual void playSuccessSound() = 0;
	virtual void playWarningSound() = 0;
	virtual void playErrorSound() = 0;

	virtual void playScreenShotSound() = 0;
	virtual void playSampleSound() = 0;
};

typedef boost::shared_ptr<Audio> AudioPtr;
}

#endif /* CXAUDIO_H_ */

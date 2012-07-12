#ifndef SSCAUDIO_H_
#define SSCAUDIO_H_

#include "boost/shared_ptr.hpp"

namespace ssc
{

/**\brief Interface for playing sounds
 *
 * \date Mar 11, 2011
 * \author Janne Beate Bakeng, SINTEF
 *
 * \ingroup sscUtility
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

#endif /* SSCAUDIO_H_ */

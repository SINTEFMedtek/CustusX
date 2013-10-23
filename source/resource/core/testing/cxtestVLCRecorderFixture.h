#ifndef CXTESTVLCRECORDERFIXTURE_H_
#define CXTESTVLCRECORDERFIXTURE_H_

#include <QString>

namespace cxtest
{

/*
 * VLCRecorderFixture
 *
 * Helper functions for testing VLCRecorder.
 *
 * \date Oct 23, 2013
 * \author: Janne Beate Bakeng, SINTEF
 */

class VLCRecorderFixture
{
public:
	VLCRecorderFixture();
	~VLCRecorderFixture();

	void checkThatVLCCanRecordTheScreen(int secondsToRecord);

	void checkIsMovieFilePlayable();

private:
	void createMovieFilePath();
	QString mMoviePath;

};

} /* namespace cxtest */
#endif /* CXTESTVLCRECORDERFIXTURE_H_ */

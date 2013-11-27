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

	/** NOTE: will work on Windows only if VLCs "Allow only one instance" is checked in VLC gui (--one-instance commandline option is broken for windows) */
	void checkThatVLCCanRecordTheScreen(int secondsToRecord);

	void checkIsMovieFileOfValidSize();

private:
	void createMovieFilePath();
	QString mMoviePath;

};

} /* namespace cxtest */
#endif /* CXTESTVLCRECORDERFIXTURE_H_ */

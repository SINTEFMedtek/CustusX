#ifndef SSCUTILHELPERS_H_
#define SSCUTILHELPERS_H_

#include <sstream>
#include <iomanip>
#include <QString>

namespace ssc
{

/**
 * \addtogroup sscUtility
 * \{
 */


/**stream the range |begin,end> to the ostream.
 * insert separator between each element..
 */
template<class ITER> std::ostream& stream_range(std::ostream& s, ITER begin, ITER end, char separator=' ')
{
	if (begin==end)
		return s;

	std::ostringstream ss; // avoid changing state of input stream
	ss << std::setprecision(3) << std::fixed;

	ss << std::setw(10) << *begin;
	++begin;
	for (; begin!=end; ++begin)
		ss << separator << std::setw(10) << *begin;

	s << ss.str();

	return s;
}
// --------------------------------------------------------

double constrainValue(double val, double min, double max);
int sign(double x);
QString changeExtension(QString name, QString ext);

/**
 * \}
 */

} // namespace ssc

#endif /*SSCUTILHELPERS_H_*/

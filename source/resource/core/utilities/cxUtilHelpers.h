/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXUTILHELPERS_H_
#define CXUTILHELPERS_H_

#include "cxResourceExport.h"

#include <sstream>
#include <iomanip>
#include <QString>

namespace cx
{

/**
 * \addtogroup cx_resource_core_utilities
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

cxResource_EXPORT double constrainValue(double val, double min, double max);
cxResource_EXPORT int constrainValue(int val, int min, int max);
cxResource_EXPORT int sign(double x);
cxResource_EXPORT QString changeExtension(QString name, QString ext);
cxResource_EXPORT void sleep_ms(int ms);

/**
 * \}
 */

} // namespace cx

#endif /*CXUTILHELPERS_H_*/

#ifndef SSCUTILHELPERS_H_
#define SSCUTILHELPERS_H_

/**stream the range |begin,end> to the ostream.
 * insert separator between each element..
 */
template<class ITER> std::ostream& stream_range(std::ostream& s, ITER begin, ITER end, char separator=' ')
{
	if (begin==end)
		return s;
	s << *begin;
	++begin;
	for (; begin!=end; ++begin)
		s << separator << *begin;
	return s;
}
// --------------------------------------------------------

#endif /*SSCUTILHELPERS_H_*/

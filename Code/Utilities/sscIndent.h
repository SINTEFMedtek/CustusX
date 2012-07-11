#ifndef SSCINDENT_H_
#define SSCINDENT_H_

#include <vtkIndent.h>

namespace ssc
{

/**\brief Formatting class for debug printing of the ssc library.
 *
 * Based on vtkIndent.
 *
 * \ingroup sscUtility
 */
class Indent// : public vtkIndent
{
public:
	enum DETAIL { 
		dSTRUCTURE, ///< print overall structure only 
		dNORMAL, ///< print all contents of ssc structures
		dDETAILS ///< include printing of vtk objects
		};

public:
	Indent();
	explicit Indent(int ind, DETAIL det=dNORMAL);
	~Indent();

	int getIndent() const;
	vtkIndent getVtkIndent() const;
	Indent stepDown() const;
	bool includeDetails() const;
	bool includeNormal() const;

	DETAIL mDetail; ///< how much info to print
	int mIndent;
};

ostream& operator<<(ostream &os, const Indent &val);


}

#endif /*SSCINDENT_H_*/

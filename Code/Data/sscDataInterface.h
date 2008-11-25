#ifndef SSCDATA_H_
#define SSCDATA_H_

#include <string>
#include "vtkSmartPointer.h"
typedef vtkSmartPointer<class vtkMatrix4x4> vtkMatrix4x4Ptr;
#include "sscTransform3D.h"

namespace ssc
{

/**Denotes the various states
 * of registration an object is in.
 */
enum REGISTRATION_STATUS
{
	rsNOT_REGISTRATED,
	rsIMAGE_REGISTRATED,
	rsPATIENT_REGISTRATED
};

/**Superclass for all data objects.
 * Example suclassess: Image and Surface.
 */
struct Data
{
	virtual ~Data() {}
	virtual std::string getUid() const = 0; ///< \return unique id for this instance
	virtual std::string getName() const = 0; /// \return a descriptive name for this instance
	virtual Transform3D getTransform() const = 0; ///< \return the transform M_rd from the data object's space (d) to the reference space (r).
	virtual REGISTRATION_STATUS getRegistrationStatus() const = 0; ///< \return what kind of registration that has been performed on this data object.

};

} // end namespace ssc

#endif /*SSCDATA_H_*/

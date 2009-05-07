#ifndef SSCDATA_H_
#define SSCDATA_H_

#include <string>
#include <set>

#include <boost/shared_ptr.hpp>

#include <QObject>
#include "vtkSmartPointer.h"
typedef vtkSmartPointer<class vtkMatrix4x4> vtkMatrix4x4Ptr;
#include "sscTransform3D.h"

#include "sscRep.h"

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
class Data : public QObject
{
public:
	Data();
	virtual ~Data();

	void setUID(const std::string& uid);
	void setName(const std::string& name);
	void setRegistrationStatus(REGISTRATION_STATUS regStat);
	virtual void set_rMd(Transform3D rMd);

	virtual std::string getUID() const; ///< \return unique id for this instance
	virtual std::string getName() const; /// \return a descriptive name for this instance
	virtual REGISTRATION_STATUS getRegistrationStatus() const; ///< \return what kind of registration that has been performed on this data object.
	virtual Transform3D get_rMd() const; ///< \return the transform M_rd from the data object's space (d) to the reference space (r).

	void connectToRep(const RepWeakPtr& rep); ///< called by Rep when connecting to an Image
	void disconnectFromRep(const RepWeakPtr& rep); ///< called by Rep when disconnecting from an Image

protected:
	std::string mUID;
	std::string mName;

	REGISTRATION_STATUS mRegistrationStatus;
	Transform3D m_rMd; ///< the transform from data to reference space

	std::set<RepWeakPtr> mReps; ///< links to Rep users.

};

} // end namespace ssc

#endif /*SSCDATA_H_*/

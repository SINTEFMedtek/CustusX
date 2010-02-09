#ifndef SSCSPACEORGANIZER_H_
#define SSCSPACEORGANIZER_H_

#include <string>
#include <map>
#include <vector>
#include <set>
#include <boost/weak_ptr.hpp>
#include <QDateTime>
#include "sscTransform3D.h"
#include "sscTypeConversions.h"
//#include "Utilities.h"
//#include "pdInterface.h"

namespace ssc
{

/**Container for all properties accosiated with a coordinate space.
 */
class SpaceProperties
{
public:
	SpaceProperties();
	explicit SpaceProperties(const QString& uid);
	SpaceProperties(const QString& uid, const QString& ref, const Transform3D& M, const QString& timestamp, bool aligned, bool dependOnPatientFrame);
	QString mUid; ///< unique id for this space
	QString mDICOM_UID; ///< FrameOfReference Uid compatible with DICOM. For this space,
	Transform3D m_rMs; ///< transform from this space s to ref space
	QString mRef; ///< unique id for ref space
	QString mTimestamp; ///< when was the space last defined.
	bool mAligned; ///< true if space conforms to the DICOM standard axis definitions.
	bool mDependOnPatientFrame; ///< true if the space is (originally) defined in terms of the patient space. It must then be newer than it in order to be up-to-date.
	void put(std::ostream& s, int indent) const;
};
typedef std::pair<bool,SpaceProperties> SignedSpaceProperties; // bool signifies forward/backward transform


/**Helper class for determining properties of spaces. 
 * Define a space properties object for each space and add it,
 * then query the organizer.
 * 
 * The global space need not be added, it is implicitly there,
 * patient space is implicitly there as a null space.
 * 
 */
class SpaceOrganizer
{
public:
	SpaceOrganizer(const QString& session, const QString& sessionTimestamp);
	
	bool isAligned(const QString& space) const; ///< true means use ACS-notation, false means use XYZ-style.
//	bool isUpToDate(const QString& space) const; ///< true means space definition is usable within this session.
	bool isNavigable(const QString& space) const; ///< can navigate  using a navigator.
	bool isImageRegistrated(const QString& space) const; ///< is connected to global space.
	QString worldSpace() const; ///< return the uid of the world space
	SpaceProperties relateToWorld(const QString& space) const; ///< return a formatted space relative to the world space
	QString get_DICOM_UID_ForSpace(const QString& space) const; ///< get DICOM uid for space, a unique uid if space not existent.
//	enum dicomlib_volume_alignment getPatientRegistrationType(const QString& space) const;
	
	void addSpace(const SpaceProperties& space);
	
	void put(std::ostream& s) const;
		
private:
	class SpaceNode;
	typedef boost::shared_ptr<SpaceNode> SpaceNodePtr;
	typedef boost::weak_ptr<SpaceNode> SpaceNodeWeakPtr;
	typedef std::map<QString, SpaceNodePtr> SpaceMap;
	class SpaceNode
	{
	public:
		explicit SpaceNode(const SpaceProperties& props) : mProps(props) {}

	//private:
		SpaceProperties mProps;
		std::set<SpaceNodeWeakPtr> mConnections;
		SpaceNodeWeakPtr mReference;
	};

	//typedef std::map<QString, SpaceProperties> SpaceMap;

	QString timestampFormat() const;
	QDateTime convertTimestamp2QDateTime(const QString& timestamp) const; ///< converter for the custom sonowand timestamp format
	QString generateDICOMFrameOfReferenceUID() const;

//	bool isDependentOnPatientSpace(const QString& space) const;
	bool newer(const QString& lhs, const QString& rhs) const;
	bool isConnected(const QString& a, const QString& b) const;
	std::vector<SignedSpaceProperties> getPath(const QString& a, const QString& b) const;
//	bool isUpToDate(const QString& a, const QString& b) const;
	const SpaceProperties& get(const QString& space) const;
	bool isValid(const QString& space) const;

	QString mSessionUid;
	QString mSessionTimestamp;
	SpaceMap mSpaces;
	const SpaceProperties mNullSpace;
};

typedef boost::shared_ptr<SpaceOrganizer> SpaceOrganizerPtr;

std::ostream& operator<<(std::ostream& s, const SpaceOrganizer& val); 

} // namespace ssc

#endif /*SSCSPACEORGANIZER_H_*/

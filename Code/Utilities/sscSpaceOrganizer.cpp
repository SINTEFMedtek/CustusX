#include "sscSpaceOrganizer.h"
//#include "QtUtilities.h"
//#include "DICOMLib/DICOMLib.h"

namespace ssc
{

SpaceProperties::SpaceProperties() :
	mAligned(false),
	mDependOnPatientFrame(false)
{
}

SpaceProperties::SpaceProperties(const QString& uid) :
	mUid(uid),
	mAligned(false),
	mDependOnPatientFrame(false)
{
}

SpaceProperties::SpaceProperties(const QString& uid, const QString& ref, const Transform3D& M, const QString& timestamp, bool aligned, bool dependOnPatientFrame) :
	mUid(uid),
	m_rMs(M),
	mRef(ref),
	mTimestamp(timestamp),
	mAligned(aligned),
	mDependOnPatientFrame(dependOnPatientFrame)
{
}

void SpaceProperties::put(std::ostream& s, int indent) const
{
	QString ind(indent, ' ');
	s << ind << "mUid      :\t" << mUid << std::endl;
	s << ind << "mRef      :\t" << mRef << std::endl;
	s << ind << "mDICOM_UID:\t" << mDICOM_UID << std::endl;
	if (!mRef.isEmpty())
	{
		s << ind << "m_rMs" << std::endl;
		m_rMs.put(s, indent+3);
		s << std::endl;
		s << ind << "mTimestamp:\t" << mTimestamp << std::endl;
	}
	s << ind << "mAligned  :\t" << mAligned << std::endl;
	s << ind << "mDependOnPatientFrame: " << mDependOnPatientFrame << std::endl;
}

std::ostream& operator<<(std::ostream& s, const SpaceProperties& val)
{
	val.put(s, 0);
	return s;
}


// --------------------------------------------------------
// --------------------------------------------------------


SpaceOrganizer::SpaceOrganizer(const QString& session, const QString& sessionTimestamp) : mSessionUid(session), mSessionTimestamp(sessionTimestamp)
{
	// always have an uncalibrated patient system available.
	addSpace(SpaceProperties("patient", "", Transform3D(), "", false, false));
	// always have an uncalibrated global system available.
	addSpace(SpaceProperties ("global", "", Transform3D(), "", false, false));
}

void SpaceOrganizer::put(std::ostream& s) const
{
	s << "session TS: \t" << mSessionTimestamp << std::endl;
	s << "world(): \t" << worldSpace() << std::endl;

	for (SpaceMap::const_iterator iter=mSpaces.begin(); iter!=mSpaces.end(); ++iter)
	{
		s << "<space " << iter->first << ">" << std::endl;
		iter->second->mProps.put(s, 3);
		s << "   isAligned(): " << isAligned(iter->first) 
//		  << " / isUpToDate(): " << isUpToDate(iter->first)
		  << " / isNavigable(): " << isNavigable(iter->first) 
		  << std::endl;
		s << "<\\space>" << std::endl;
	}
}

/**True if the world space is aligned, and the input space is connected to it,
 * OR if the space is unconnected and aligned.
 * Use for display of ACS text / XYZ text.
 */
bool SpaceOrganizer::isAligned(const QString& space) const
{
	bool connected = isConnected(space, worldSpace());

	if (connected)
	{
		return get(worldSpace()).mAligned;
	}
	else
	{
		return get(space).mAligned;
	}
}

///**true if space timestamp if newer than the session timestamp.
// *
// *  * Not used: remove
// *
// */
//bool SpaceOrganizer::isUpToDate(const QString& space) const
//{
////	std::cout << "utd: " << space << std::endl;
//	if (get(space).mUid.isEmpty())
//	{
//		return false;
//	}
//
//	bool retval = true;
//
//	// special case for spaces depending in the newness of the patient frame
//	// f.ex gravity must be newer than patient in order to be ok.
//	if (get(space).mDependOnPatientFrame)
//	{
//		retval = retval && newer(get(space).mTimestamp, get("patient").mTimestamp);
//	}
//
//	// special case for patient space: if older than session timestamp, this indicates
//	// that it is too old. (Other spaces are ok if older than session ts).
//	// TODO: a better way to solve this is to remove patient space (or stamp it as new).
//	if (space=="patient" || get(space).mDependOnPatientFrame)
//	{
//		retval = retval && newer(get(space).mTimestamp, mSessionTimestamp);
//	}
//
//	return retval;
//}

/** defines the format Sonowand uses as its timestamp format
 *  - in conjunction with QString.
 */
QString SpaceOrganizer::timestampFormat() const
{
	return QString("yyyyMMdd'T'hhmmss");
}


QDateTime SpaceOrganizer::convertTimestamp2QDateTime(const QString& timestamp) const ///< converter for the custom sonowand timestamp format
{
	return QDateTime::fromString(timestamp, timestampFormat());
}

/**Return true if ts a is newer than ts b.
 * 
 * if a or b is invalid, return true.
 * I.e: is a space does not have a timestamp, it has no negative effect on the age.
 */
bool SpaceOrganizer::newer(const QString& lhs, const QString& rhs) const
{
	QDateTime left = convertTimestamp2QDateTime(lhs);
	QDateTime right = convertTimestamp2QDateTime(rhs);
	if (!left.isValid() || !right.isValid())
		return true;	
//	return lhs > rhs;
	return left >= right;
}

///**check up-to-date status of all spaces along a path.
// *
// *  * Not used: remove
// *
// */
//bool SpaceOrganizer::isUpToDate(const QString& a, const QString& b) const
//{
//	if (a==b)
//	{
//		return true;
//	}
//
//	std::vector<SignedSpaceProperties> path = getPath(a, b);
//
//	for (unsigned i=0; i<path.size(); ++i)
//	{
//		if (!isUpToDate(path[i].second.mUid))
//			return false;
//	}
//
//	return true;
//}

///** return true is the space is connected directly to the patient space,
// *  not walking through global space.
// *
// * Not used: remove
// */
//bool SpaceOrganizer::isDependentOnPatientSpace(const QString& space) const
//{
//	std::vector<SignedSpaceProperties> path = getPath(space, "patient");
//
//	for (unsigned i=0; i<path.size(); ++i)
//	{
//		if (path[i].second.mUid == "patient")
//			return true;
//		if (path[i].second.mUid == "global")
//			return false;
//	}
//
//	return false;
//}

/**true if there is a valid path from dataspace to patient space p, and
 * all spaces along the path are upToDate()
 * 
 * True means that the space is connected to the patient frame and
 * a navigator tool can be used to navigate the data.
 * True also means use Green color, Red if false.
 */
bool SpaceOrganizer::isNavigable(const QString& space) const
{
//	bool depOnPat = isDependentOnPatientSpace(space);
//	bool newerThanPatient = true;
//
//	// if the patient is older than the session ts: no navigation at all.
//	bool patientNewerThanSession = newer(get("patient").mTimestamp, mSessionTimestamp);
//	
//	// handles the us case: all us images(+gravity etc) must be newer than the latest patient registration in order to be ok.
//	if (depOnPat)
//	{
//		std::vector<pd::SignedSpaceProperties> path = getPath(space, "patient");
//		
//		for (unsigned i=0; i<path.size(); ++i)
//		{
//			newerThanPatient = newerThanPatient && newer(get(path[i].second.mUid).mTimestamp, get("patient").mTimestamp);
//			// this additionally handles the case where no patient reg exist, but us images must be diabled cause of
//			// old outdated session.
//			patientNewerThanSession = patientNewerThanSession && newer(get(path[i].second.mUid).mTimestamp, mSessionTimestamp);
//		}
//	}
//	
//	//bool newerThanPatient = newer(get(space).mTimestamp, get("patient").mTimestamp);
//	return isConnected(space, "patient") && newerThanPatient && patientNewerThanSession && isConnected(space, worldSpace()) ;
	
	// space itself must be uptodate: applies to US volumes.
	bool spaceUTD = true;
	if (get(space).mDependOnPatientFrame)
	{
		spaceUTD = spaceUTD && newer(get(space).mTimestamp, get("patient").mTimestamp); // newer than patreg
		spaceUTD = spaceUTD && newer(get(space).mTimestamp, mSessionTimestamp);		// newer than session
	}
	
	// patient must be update relative to session
	bool patNewerThanSession = newer(get("patient").mTimestamp, mSessionTimestamp);
	// must also be connected to patient and world.
	return isConnected(space, "patient") && spaceUTD && patNewerThanSession && isConnected(space, worldSpace()) ;
}

/**Image registrated but no necessarily patient registrated.
 */
bool SpaceOrganizer::isImageRegistrated(const QString& space) const
{
	return isConnected(space, "global");	
}

QString SpaceOrganizer::get_DICOM_UID_ForSpace(const QString& space) const
{
	QString retval = get(space).mDICOM_UID;
	if (retval.isEmpty())
		retval = generateDICOMFrameOfReferenceUID();
	return retval;
}

/**return the space acting as the world space
 * 
 * If patient valid, use first aligned data space that is connected to patient
 * Else, if patientOrientation valid, use that.
 * Else, use the patient space This enable display of tools and us images 
 * internally consistent, while nonregistrated images will be shown directly 
 * attached to p, but with red text.
 */
QString SpaceOrganizer::worldSpace() const
{
	// no patient registration: use the first volume connected to global. This handles image registration.
	if (!isValid("patient"))
	{
		for (SpaceMap::const_iterator iter=mSpaces.begin(); iter!=mSpaces.end(); ++iter)
		{
			if (iter->second->mProps.mAligned
					&& iter->second->mProps.mUid!="patientOrientation"
					&& isConnected("global", iter->second->mProps.mUid))
			{
				return iter->second->mProps.mUid;
			}
		}		
	}
	
	// patient registration present: find first volume space connected to patient.
	if (isValid("patient"))
	{
		for (SpaceMap::const_iterator iter=mSpaces.begin(); iter!=mSpaces.end(); ++iter)
		{
			if (iter->second->mProps.mAligned
					&& iter->second->mProps.mUid!="patientOrientation"
					&& isConnected("patient", iter->second->mProps.mUid))
			{
				return iter->second->mProps.mUid;
			}
		}
	}
	
	// use pator if nothing else available
	if (isValid("patientOrientation"))
	{
		return "patientOrientation";
	}
	
	// use patient in 3D direct case
	return get("patient").mUid;
}

SpaceProperties SpaceOrganizer::relateToWorld(const QString& space) const
{
	QString world = worldSpace();
	std::vector<SignedSpaceProperties> path = getPath(space, world);
	
	
	if (path.empty())
	{
		SpaceProperties retval;
		retval.mUid = space;
		//Logger::log("nav.log", "SpaceOrganizer::relateToWorld(" + space + ") failure ws=" + world);
		return retval;
	}
	
	Transform3D wMs;
	for (unsigned i=0; i<path.size(); ++i)
	{
		if (path[i].first)
		{
			wMs = path[i].second.m_rMs * wMs;
		}
		else
		{
			wMs = path[i].second.m_rMs.inv() * wMs;			
		}
	}

	SpaceProperties retval = get(space);
	retval.m_rMs = wMs;
	retval.mRef = world;
	//Logger::log("nav.log", "SpaceOrganizer::relateToWorld(" + space + ") success ws=" + world);
	return retval;	
}


QString SpaceOrganizer::generateDICOMFrameOfReferenceUID() const
{
	return "undefined";
	//return DICOMLib_FrameOfReferenceUID();
}

/**return the data associated with the input space if it exists,
 * return a null space if not.
 */
const SpaceProperties& SpaceOrganizer::get(const QString& space) const
{
	if (mSpaces.count(space))
	{
		return mSpaces.find(space)->second->mProps;
	}
	else
	{
		return mNullSpace;
	}
}

bool SpaceOrganizer::isConnected(const QString& a, const QString& b) const
{
	return !getPath(a,b).empty();
}

//void SpaceOrganizer::addSpace(const SpaceProperties& space)
//{
//	mSpaces[space.mUid] = space;
//	if (space.mRef.isEmpty())
//	{
//		mSpaces[space.mUid].mTimestamp = ""; // ensure invalid spaces have invalid timestamps.
//	}
//	if (!space.mRef.isEmpty() && space.mDICOM_UID.isEmpty())
//	{
//		// quick fix for this uid: create anew each time the organizer object is initialized.
//		// This requires the creator to keep the object for as long as he wishes the
//		// uids to be invariant.
//		mSpaces[space.mUid].mDICOM_UID = generateDICOMFrameOfReferenceUID();
//	}
//}

void SpaceOrganizer::addSpace(const SpaceProperties& space)
{
	SpaceNodePtr ref;
	// create an empty reference node if not present
	if (!mSpaces.count(space.mRef) && !space.mRef.isEmpty())
	{
		ref.reset(new SpaceNode(SpaceProperties(space.mRef)));
		mSpaces[ref->mProps.mUid] = ref;
	}
	ref = mSpaces[space.mRef];

	SpaceNodePtr current;
	// create the node if not present
	if (!mSpaces.count(space.mUid))
	{
		current.reset(new SpaceNode(space));
		mSpaces[current->mProps.mUid] = current;
	}
	current->mProps = space; // set in case of change
	current = mSpaces[space.mUid];

	// set some invariants
	if (current->mProps.mRef.isEmpty())
	{
		current->mProps.mTimestamp = ""; // ensure invalid spaces have invalid timestamps.
	}
	if (!current->mProps.mRef.isEmpty() && current->mProps.mDICOM_UID.isEmpty())
	{
		// quick fix for this uid: create anew each time the organizer object is initialized.
		// This requires the creator to keep the object for as long as he wishes the
		// uids to be invariant.
		current->mProps.mDICOM_UID = generateDICOMFrameOfReferenceUID();
	}

	// connect to the reference node
	current->mReference = ref;

	// clear all existing connections
	for (SpaceMap::iterator iter=mSpaces.begin(); iter!=mSpaces.end(); ++iter)
	{
		current->mConnections.clear();
	}

	// rebuild all connections
	for (SpaceMap::iterator iter=mSpaces.begin(); iter!=mSpaces.end(); ++iter)
	{
		SpaceNodePtr thisRef = mSpaces[iter->second->mProps.mRef];
		iter->second->mConnections.insert(thisRef);
	}
}

///** return a definition that leads one step further from 'from' to 'to'.
//  *
//  *
//  */
//SignedSpaceProperties SpaceOrganizer::step(const QString& from, const QString to)
//{
//	//typedef std::map<QString, SpaceProperties> SpaceMap;
//	std::vector<SignedSpaceProperties> retval;
//
//	for (SpaceMap::const_iterator iter=mSpaces.begin(); iter!=mSpaces.end(); ++iter)
//	{
////		if ()
//	}
//}
//
///** Expand the input path towards target space recursively.
//  * Return true if a path were found.
//  *
//  */
//bool SpaceOrganizer::step(std::vector<SignedSpaceProperties>* base, const QString target)
//{
//	//typedef std::map<QString, SpaceProperties> SpaceMap;
//	std::vector<SignedSpaceProperties> retval;
//
//	for (SpaceMap::const_iterator iter=mSpaces.begin(); iter!=mSpaces.end(); ++iter)
//	{
////		if ()
//	}
//}

/**check if there exist a path between spaces a and b.
 * 
 * An empty return value means no path was found.
 * A return value containing a single empty space means that the input spaces were equal.
 * Otherwise, a vector of spaces indicating the path from a to b is returned. 
 * The SignedSpaceProperties.first indicates the direction of the transform.
 * 
 * input p and i , 
 * spaces pMg and iMg
 * would give { (true, pMg), (false, iMg) }
 */
std::vector<SignedSpaceProperties> SpaceOrganizer::getPath(const QString& a, const QString& b) const
{
//	std::cout << find path between:  << a << "- and -" << b << std::endl;
	
	std::vector<SignedSpaceProperties> retval;
	return retval;
	
//	if (a==b)
//	{
//		retval.push_back(std::make_pair(true, SpaceProperties()));
//		return retval;
//	}
//
//	// create the path from a down to bottom
//	std::vector<QString> down;
//	QString current = a;
//	down.push_back(current);
//	while (!current.isEmpty())
//	{
//		current = get(current).mRef;
//		if (current.isEmpty())
//			break;
//		down.push_back(current);
////		std::cout << "down: " << current << std::endl;
//	}
//
//	// create the path from b down to bottom
//	std::vector<QString> up;
//	current = b;
//	up.push_back(current);
//	while (!current.isEmpty())
//	{
//		if (std::count(down.begin(), down.end(), current))
//		{
//			// found a match from the down path, finished
//			break;
//		}
//
//		current = get(current).mRef;
//		if (current.isEmpty())
//			break;
//		up.push_back(current);
////		std::cout << "up: " << current << std::endl;
//	}
//
//	if (up.empty())
//		return retval;
//
//	// remove the superfluous spaces from down.
//	while (!down.empty() && down.back()!=up.back())
//	{
//		down.pop_back();
//	}
//
//	if (down.empty() || down.back()!=up.back())
//		return retval;
//
//	// create the path from the down and up vectors.
//	for (unsigned i=1; i<down.size(); ++i)
//	{
//		retval.push_back(std::make_pair(true, get(down[i-1])));
//	}
//	for (unsigned ii=1; ii<up.size(); ++ii)
//	{
//		unsigned i = up.size()-ii;
//		retval.push_back(std::make_pair(false, get(up[i-1])));
//	}
//
////	std::cout << "path: ";
////	for (unsigned i=0; i<retval.size(); ++i)
////	{
////		std::cout << "\t" << retval[i].second.mUid;
////	}
////	std::cout << std::endl;
//
//	return retval;
}

bool SpaceOrganizer::isValid(const QString& space) const
{
	return !get(space).mRef.isEmpty();
}

std::ostream& operator<<(std::ostream& s, const SpaceOrganizer& val)
{
	val.put(s);
	return s;
}
//
///**Find a type describing the kind of registration applied to a volume,
// * conforming to DICOM standard for US volumes (Supplement 43: Storage of 3D Ultrasound Volumes)
// */
//enum dicomlib_volume_alignment SpaceOrganizer::getPatientRegistrationType(const QString& space) const
//{
//	enum dicomlib_volume_alignment retval = DICOMLIB_ALIGNMENT_TABLE;
//	if (isAligned(space))
//	{
//		retval = DICOMLIB_ALIGNMENT_REGISTRATION;
//	}
//	if (worldSpace()=="patientOrientation")
//	{
//		retval = DICOMLIB_ALIGNMENT_ESTIMATED;
//	}
//	return retval;
//}


} // namespace ssc

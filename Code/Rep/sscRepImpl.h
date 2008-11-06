#ifndef SSCREPIMPL_H_
#define SSCREPIMPL_H_

#include <set>
#include "sscRep.h"

namespace ssc
{

/**
 * Default implementation of Rep. Subclass from here to get a concrete class.
 *
 * The connection to View is handled here, but subclasses must implement
 * at least:
 * - getType()
 * - addRepActorsToViewRenderer()
 * - removeRepActorsFromViewRenderer().
 */
class RepImpl : public Rep
{
public:
	RepImpl(const std::string& uid, const std::string& name="");
	virtual ~RepImpl();
	virtual std::string getType() const = 0;
	virtual void connectToView(View *theView);
	virtual void disconnectFromView(View *theView);
	virtual bool isConnectedToView(View *theView) const;
	void setName(std::string name);
	std::string getName() const;
	std::string getUid() const;

protected:
	std::set<View *> mViews;
	std::string mName;
	std::string mUid;
	RepWeakPtr mSelf;
	//typedef std::vector<View *>::iterator ViewsIter;

	virtual void addRepActorsToViewRenderer(View* view) = 0;
	virtual void removeRepActorsFromViewRenderer(View* view) = 0;

private:
	RepImpl(); ///< not implemented
};

} // namespace ssc

#endif /*SSCREPIMPL_H_*/

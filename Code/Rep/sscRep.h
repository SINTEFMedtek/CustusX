#ifndef SSCREP_H_
#define SSCREP_H_

#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include <QObject>

namespace ssc
{
typedef boost::shared_ptr<class Rep> RepPtr;
typedef boost::weak_ptr<class Rep> RepWeakPtr;

class Indent;
class View;

/**
 * Rep (Representation) is the interface that all Representations in ssc must
 * implement.
 * The default implementation can be found in RepImpl. Inherit from that when
 * implementing concrete classes.
 */
class Rep :public QObject
{
	Q_OBJECT
public:
	virtual ~Rep() {}

	/**\return a string identifying the Rep type. Each suclass implement
	 * this with its own class name. Subclasses from other namespaces
	 * (such as cx and snw) should add that namespace as a prefix.
	 */
	virtual std::string getType() const = 0;

	/**
	 * Do not use this method! It is only used by ssc::View to set up an
	 * internal connection between the two.
	 *
	 * \param theView The view to add this representation to.
	 * \return
	 */
	virtual void connectToView(View *theView) = 0;

	/**
	 * Do not use this method! It is only used by ssc::View to tear down an
	 * internal connection between the two.
	 *
	 * \param theView The view to disconnect from.
	 * \return
	 */
	virtual void disconnectFromView(View *theView) = 0;

	/**
	 * Traverse the vector of views to see if this Representation is connected to
	 * one of them.
	 *
	 * \param theView A view pointer.
	 * \return True if \a theView was found in the internal vector of \a View pointers.
	 * false otherwise.
	 */
	virtual bool isConnectedToView(View *theView) const = 0;
	/** Set the name of this Rep. This is useful for context-menus in
	 * Views and other user interaction constructs.
	 *
	 * \param name The user-friendly name for this instance.
	 */
	virtual void setName(std::string name) = 0;
	/**\return a user-friendly name for this instance
	 */
	virtual std::string getName() const = 0;
	/**Return an unique id for this instance, i.e. unique
	 * over all Rep instances.
	 * \return the unique id.
	 */
	virtual std::string getUid() const = 0;
	
	virtual void printSelf(std::ostream & os, Indent indent) = 0;
	
};

} // namespace ssc

#endif /*SSCREP_H_*/

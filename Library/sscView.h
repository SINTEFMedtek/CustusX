#ifndef SSCVIEW_H_
#define SSCVIEW_H_

#include <boost/shared_ptr.hpp>
#include "QVTKWidget.h"
#include "vtkSmartPointer.h"

typedef vtkSmartPointer<class vtkRenderer> vtkRendererPtr;
typedef vtkSmartPointer<class vtkRenderWindow> vtkRenderWindowPtr;

namespace ssc
{

typedef boost::shared_ptr<class Rep> RepPtr;

/**
 * Base widget for displaying lists of Rep.
 *
 * The View class inherits a QVTKWidget but creates its own vtkRenderer and
 * vtkRenderWindow.
 */
class View : public QVTKWidget
{
  Q_OBJECT

public:
  enum Type
  {
	VIEW,
    VIEW_2D,
    VIEW_3D,
  };
  /**\return the View type, indicating display dimension.
   */
  virtual Type getType() const {VIEW;};

  View(QWidget *parent = NULL, Qt::WFlags f = 0);
  virtual ~View();
  /**
   * Get the renderer used by this \a View.
   *
   * \return The renderer used in this \a View
   */
  virtual vtkRendererPtr  getRenderer() const;
  /**
   * Get the vtkRenderWindow used by this \a View.
   *
   * \return The rendererwindow used in this \a View
   */
  virtual vtkRenderWindowPtr getRenderWindow() const;
  /**
   * Displays a \a Rep in this \a View and maintains a list of all
   * added \a Rep objects.
   * This function checks if \a rep is already added to this \a View.
   * If it is, this function does nothing. If not, the Rep is
   * connected to this \a View with the \a Rep::connectToView()
   * method and added to this \a View `s vector of Reps
   *
   * \param rep The \a Rep to add to the \a View.
   */
  virtual void addRep(const RepPtr& rep);
  /**
   * Remove all other \a Rep objects from this \a View and add
   * the provided Rep to this \a View.
   *
   * \param rep The Rep to show in this \a View.
   */
  virtual void setRep(const RepPtr& rep);
  /**
   * Remove a rep from this \a View.
   * This function traverses the internal list of Rep to find the
   * provided \a Rep object and executes
   * \a Rep::disconnectFromView() on this object. The found object is
   * also removed from this \a View object's list of Rep.
   * If no matching \a Rep obejct is found, this function does
   * nothing.
   * \param rep The Rep to remove.
   */
  virtual void removeRep(const RepPtr& rep);
  /**
   * Check to see if this \a View has the given \a Rep in it's
   * internal list.
   * \param rep The Rep to search for.
   * \return true if the \a Rep is found, false otherwise.
   */
  virtual bool hasRep(const RepPtr& rep) const;
  /** \return All reps in the View.
   */
  virtual std::vector<RepPtr> getReps();
  /**Remove all reps in the View.
   */
  virtual void removeReps();

protected:
	vtkRendererPtr  mRenderer;
	vtkRenderWindowPtr mRenderWindow;
	std::vector<RepPtr> mReps; ///< storage for internal reps.
	typedef std::vector<RepPtr>::iterator RepsIter; ///< iterator typedef for the internal rep vector.
};

} // namespace ssc


#endif /*SSCVIEW_H_*/

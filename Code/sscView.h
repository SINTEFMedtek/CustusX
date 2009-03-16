#ifndef SSCVIEW_H_
#define SSCVIEW_H_

#include <boost/shared_ptr.hpp>
#include "vtkSmartPointer.h"
#include "QVTKWidget.h"
#include "sscIndent.h"

typedef vtkSmartPointer<class vtkRenderer> vtkRendererPtr;
typedef vtkSmartPointer<class vtkRenderWindow> vtkRenderWindowPtr;

namespace ssc
{

typedef boost::shared_ptr<class Rep> RepPtr;

/**
 * \class View
 * \brief Base widget for displaying lists of Rep. The View class inherits a
 * QVTKWidget but creates its own vtkRenderer and vtkRenderWindow.
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
  }; ///< type describing the view
  View(QWidget *parent = NULL, Qt::WFlags f = 0);
  virtual ~View();
  virtual Type getType() const { return VIEW;}; ///< \return the View type, indicating display dimension.
  virtual std::string getUid(); ///< get a views unique id
  virtual std::string getName(); ///< get a views name
  virtual vtkRendererPtr  getRenderer() const; ///< get the renderer used by this \a View.
  virtual vtkRenderWindowPtr getRenderWindow() const; ///< get the vtkRenderWindow used by this \a View.
  virtual void addRep(const RepPtr& rep); ///< adds and connects a rep to the view
  virtual void setRep(const RepPtr& rep); ///< remove all other \a Rep objects from this \a View and add the provided Rep to this \a View.
  virtual void removeRep(const RepPtr& rep); ///< removes and disconnects the rep from the view
  virtual bool hasRep(const RepPtr& rep) const; ///< checks if the view already have the rep
  virtual std::vector<RepPtr> getReps(); ///< returns all reps in the view
  virtual void removeReps(); ///< removes all reps in the view
  virtual void clear(); ///< removes everything in the view, inluding reps.
  
  void print(std::ostream& os);  	
  virtual void printSelf(std::ostream & os, Indent indent);
  
signals:
      void resized(QSize size);
protected:
	std::string mUid; 		///< the views unique id
	std::string mName; 		///< the views name

	vtkRendererPtr  						mRenderer;
	vtkRenderWindowPtr 						mRenderWindow;
	std::vector<RepPtr> 					mReps; ///< storage for internal reps.
	typedef std::vector<RepPtr>::iterator 	RepsIter; ///< iterator typedef for the internal rep vector.
private:
    void resizeEvent(QResizeEvent * event);
};

typedef boost::shared_ptr<View> ViewPtr;

} // namespace ssc


#endif /*SSCVIEW_H_*/

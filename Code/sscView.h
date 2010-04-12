#ifndef SSCVIEW_H_
#define SSCVIEW_H_
#include "sscConfig.h"
#include <boost/shared_ptr.hpp>
#include "vtkSmartPointer.h"
#include "sscIndent.h"

#ifdef USE_GLX_SHARED_CONTEXT
#include "sscSNWQVTKWidget.h"
typedef SNWQVTKWidget ViewParent;
#else
#include "QVTKWidget.h"
typedef QVTKWidget ViewParent;
#endif




typedef vtkSmartPointer<class vtkRenderer> vtkRendererPtr;
typedef vtkSmartPointer<class vtkRenderWindow> vtkRenderWindowPtr;
typedef vtkSmartPointer<class SNWXOpenGLRenderWindow> SNWXOpenGLRenderWindowPtr;

namespace ssc
{
	typedef boost::shared_ptr<class Rep> RepPtr;

	/**
	 * \class View
	 * \brief Base widget for displaying lists of Rep. The View class inherits a
	 * QVTKWidget but creates its own vtkRenderer and vtkRenderWindow.
	 */
	class View : public ViewParent
	{
		Q_OBJECT

		typedef ViewParent inherited;

	public:
		enum Type
		{
			VIEW,
			VIEW_2D,
			VIEW_3D,
			VIEW_REAL_TIME
		}; ///< type describing the view
		View(QWidget *parent = NULL, Qt::WFlags f = 0);
		virtual ~View();
		virtual Type getType() const { return VIEW;}		///< \return the View type, indicating display dimension.
		virtual std::string getUid();				///< Get a views unique id
		virtual std::string getName();				///< Get a views name
		virtual vtkRendererPtr  getRenderer() const;		///< Get the renderer used by this \a View.
		virtual vtkRenderWindowPtr getRenderWindow() const;	///< Get the vtkRenderWindow used by this \a View.
		virtual void addRep(const RepPtr& rep);			///< Adds and connects a rep to the view
		virtual void setRep(const RepPtr& rep);			///< Remove all other \a Rep objects from this \a View and add the provided Rep to this \a View.
		virtual void removeRep(const RepPtr& rep);		///< Removes and disconnects the rep from the view
		virtual bool hasRep(const RepPtr& rep) const;		///< Checks if the view already have the rep
		virtual std::vector<RepPtr> getReps();			///< Returns all reps in the view
		virtual void removeReps();				///< Removes all reps in the view
		virtual void clear();					///< Removes everything in the view, inluding reps.

		void print(std::ostream& os);
		virtual void printSelf(std::ostream & os, Indent indent);

	signals:
		void resized(QSize size);
		void mouseMoveSignal(QMouseEvent* event);
		void mousePressSignal(QMouseEvent* event);
		void mouseReleaseSignal(QMouseEvent* event);
	  void mouseWheelSignal(QWheelEvent*);
    void showSignal(QShowEvent* event);
    void focusInSignal(QFocusEvent* event);

	protected:
		std::string mUid;					///< The view's unique id
		std::string mName;					///< The view's name
		vtkRendererPtr mRenderer;
		vtkRenderWindowPtr mRenderWindow;
		//SNWXOpenGLRenderWindowPtr mRenderWindow;
		std::vector<RepPtr> mReps;				///< Storage for internal reps.
		typedef std::vector<RepPtr>::iterator RepsIter;		///< Iterator typedef for the internal rep vector.
	private:
		virtual void showEvent(QShowEvent* event);
	  virtual void wheelEvent(QWheelEvent*);
		virtual void mouseMoveEvent(QMouseEvent *event);
		virtual void mousePressEvent(QMouseEvent *event);
		virtual void mouseReleaseEvent(QMouseEvent *event);
		virtual void focusInEvent(QFocusEvent* event);
		void resizeEvent(QResizeEvent *event);
	};

	typedef boost::shared_ptr<View> ViewPtr;
} // namespace ssc

#endif /*SSCVIEW_H_*/

#ifndef VMULTRASOUND_H_
#define VMULTRASOUND_H_

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <QtCore>

#include "sscRepImpl.h"
#include "Include/UltrasoundDefinitions.h"
#include "Utilities.h"
#include "sscVtkHelperClasses.h"
#include "snwVtkIncludes.h"

#include "vmForwardDeclarations.h"
#include "vmNavigationData.h"
#include "vmViewControlData.h"

typedef vtkSmartPointer<class vtkPlaneSource> vtkPlaneSourcePtr;
typedef vtkSmartPointer<class vtkTexture> vtkTexturePtr;
typedef vtkSmartPointer<class vtkTransformTextureCoords> vtkTransformTextureCoordsPtr;
typedef vtkSmartPointer<class vtkDataSetMapper> vtkDataSetMapperPtr;

 
//---------------------------------------------------------
namespace vm
{
//---------------------------------------------------------

typedef boost::shared_ptr<class UltrasoundData> UltrasoundDataPtr;

/**Encapulates the Ultrasound state in ViewMachine
 * 
 * activate/deactivate enters the us state.
 * start/stop/display is usable (from us) while  active.
 * 
 */
class Ultrasound
{
public:
	Ultrasound(DBusAPIPtr callback);
	virtual ~Ultrasound();

	void activate();
	bool active() const;
	void deactivate();

	void start(int shmid);
	void stop();

	void setModified();
	void refresh();
	void validateData();
	void setStatus(snw_us::ModuleStatus status, double mi, double ti);

	UltrasoundRepPtr createRep();
	ViewControlData createViewControlData(const ViewControlData& base);
	
private:
	bool mActive;
	void teardown();
	void setup(int shmtKey);
	bool finished() const;
	void display();
	
	bool mValidData;
	UltrasoundDataPtr mData;
	DBusAPIPtr mCallback;
	
};

typedef boost::shared_ptr<Ultrasound> UltrasoundPtr;

class UltrasoundRep : public ssc::RepImpl
{
	Q_OBJECT
public:
	UltrasoundRep(UltrasoundDataPtr data, const std::string& uid, const std::string& name="");
	virtual ~UltrasoundRep();
	virtual std::string getType() const { return "vm::UltrasoundRep"; }
	void setCamera();
protected:	
	virtual void addRepActorsToViewRenderer(ssc::View* view);
	virtual void removeRepActorsFromViewRenderer(ssc::View* view);	
private slots:
	void statusChangedSlot();
private:
	void initializeSize(int imageWidth, int imageHeight);
	void formatMechTermIndex();
	void setup();
	UltrasoundDataPtr mData;
	ssc::TextDisplayPtr mStatusText;
	ssc::TextDisplayPtr mInfoText;
	vtkActorPtr mPlaneActor;
	vtkPlaneSourcePtr mPlaneSource;
	vtkTexturePtr mUsTexture;	
	vtkRendererPtr mRenderer;
};

//---------------------------------------------------------
} // namespace vm
//---------------------------------------------------------


#endif /*VMULTRASOUND_H_*/

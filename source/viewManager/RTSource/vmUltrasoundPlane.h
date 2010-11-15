#ifndef ULTRASOUNDPLANE_H_
#define ULTRASOUNDPLANE_H_

#include <boost/shared_ptr.hpp>
#include <snwVtkIncludes.h>
#include <QtCore>
#include "Include/UltrasoundDefinitions.h"

typedef vtkSmartPointer<class vtkImageImport> vtkImageImportPtr;
typedef vtkSmartPointer<class vtkTimerLog> vtkTimerLogPtr;

namespace vm
{

/**Synchronize data with shared memory,
 * provide data as a vtkImageData.
 */
class UltrasoundSource
{
public:
	UltrasoundSource();
	~UltrasoundSource();
	void connect(int shmtKey);
	void reconnect();
	void disconnect();
	void update();
	bool connected() const;
	vtkImageDataPtr getVtkImageData();
	int width() const { return mImageWidth; }
	int height() const { return mImageHeight; }
	
private:
	void initializeBuffer(int newWidth, int newHeight);
	void padBox(int* x, int* y) const;
	
	int m_shmtKey;
	int mUSSession;
	uint32_t *mUSTextBuf;
	int mImageWidth;
	int mImageHeight;
	vtkImageDataPtr mImageData;
	vtkImageImportPtr mImageImport;
};
typedef boost::shared_ptr<UltrasoundSource> UltrasoundSourcePtr;

typedef boost::shared_ptr<class UltrasoundSource> UltrasoundSourcePtr;

/**Contains all data associated with one realtime US stream.
 */
class UltrasoundData : public QObject
{
	Q_OBJECT
public:
	UltrasoundData();
	std::string statusString() const;
	void setStatus(snw_us::ModuleStatus status, double mi, double ti);	
	double mechanicalIndex() const;
	double thermalIndex() const;
	void statusModified();
	void setModified();
	
	void connectSource(int shmtKey);
	void disconnectSource();
	void reconnectSource();
	bool connected() const;
	vtkImageDataPtr getVtkImageData();
	int width() const;
	int height() const;
	
	void refresh();
	bool valid();
	void startTimer();
signals:
	void statusChanged(); ///< emitted when anything else than realtime stream changes.
private slots:
	void clearDisplay();  ///< this will be triggered if the us image is older than 1.0 second
private:
	void display();
	bool mModified;		 	///< true means next refresh will be carried out.
	UltrasoundSourcePtr mSource;
	float mCurrentThermIndexVal;
	float mCurrentMechIndexVal;
	snw_us::ModuleStatus mStatus;

	QTimer *mQtimer;
	bool mValidImage;         ///< image is valid if it is not older then evalTime
	double mTimeLimit;        ///< time threshold in milliseconds,  
	
};
typedef boost::shared_ptr<UltrasoundData> UltrasoundDataPtr;

}
#endif

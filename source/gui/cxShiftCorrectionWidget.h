#ifndef CXSHIFTCORRECTIONWIDGET_H_
#define CXSHIFTCORRECTIONWIDGET_H_

#include <QWidget>
#include <QProcess>;
//class QVBoxLayout;
class QGridLayout;
class QComboBox;
class QPushButton;
class QCheckBox;
class QSpinBox;
#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkImageToVTKImageFilter.h>
#include <vtkSmartPointer.h>

typedef vtkSmartPointer<class vtkMarchingCubes> vtkMarchingCubesPtr;
typedef vtkSmartPointer<class vtkPolyData> vtkPolyDataPtr;

namespace cx
{
const unsigned int Dimension = 3;
typedef unsigned short PixelType;
typedef itk::Image< PixelType, Dimension >  ImageType;
typedef itk::ImageFileReader< ImageType  > ImageReaderType;
typedef ImageType::DirectionType ImageDirectionType;
typedef itk::ImageFileWriter<ImageType> ImageWriterType;
typedef itk::ImageToVTKImageFilter<ImageType> itkToVtkFilterType;
  
/**
 * \class ShiftCorrectionWidget
 *
 * \date Mar 15, 2010
 * \author Ole Vegard Solberg
 */
class ShiftCorrectionWidget : public QWidget
{
  Q_OBJECT
  
public:
  ShiftCorrectionWidget(QWidget* parent);
  ~ShiftCorrectionWidget();
  void init(QString& workingFolder);///< Initialize widget
  
protected:
  QString mWorkingFolder;///< All images are read from this folder
  //QVBoxLayout* mLayout;
  QGridLayout* mLayout;
  
  QPushButton* mMovingImageSegmentButton;///<Button for segmenting moving image with a threshold value
  QPushButton* mRefImageSegmentButton;///<Button for segmenting ref image with a threshold value
  QPushButton* mMovingImageProcessButton;///<Button for finding centerlines in moving image
  QPushButton* mRefImageProcessButton;///< Button for finding centerlines in ref image
  QPushButton* mRegistrationButton;///< Button for starting I2I registration
  QPushButton* mApplyUpdateButton;///< Button for applying update to another image
  
  QComboBox* mMovingImageBox;///< List of all available images, used to select moving image
  QComboBox* mRefImageBox;///< List of all available images, used to select reference image
  QComboBox* mConnectImageBox;///< List of all available images, used to select one or more connected images
  
  QCheckBox* mMovingImageSmoothingCheckBox;///< Use smoothing for moving image?
  QCheckBox* mRefImageSmoothingCheckBox;///< Use smoothing for ref image?
  
  QSpinBox* mMovingImageThresholdBox;///<The threshold to use for the segmentation of the moving image
  QSpinBox* mRefImageThresholdBox;///<The threshold to use for the segmentation of the reference image
  
  QProcess* mMovingImageProcess;///< Process that runs Vessel segmentation and centerline extraction for the moving image
  QProcess* mRefImageProcess;///< Process that runs Vessel segmentation and centerline extraction for the reference image
  QProcess* mRegistrationProcess;///<Process that runs I2I registration
  bool mMovingImageDone;///<Is moving image processed?
  bool mRefImageDone;///<Is ref image processed?
  
  void initFileComboBoxes();///<Fill file comboBoxes with file names from working folder
  void segmentImage(QString imageName, 
                    int thresholdValue,
                    bool smoothing);///<Segment image based on a threshold
  void processImage(QString imageName);///<Run command line programs for image
  void updateProcessButton(QComboBox* imageBox, QPushButton* button);///<Enable/disable button for centerline extraction if segmented file exists
  
protected slots:
  void segmentMovingImageSlot();///<Run vessel segmentation
  void segmentRefImageSlot();///<Run vessel segmentation
  void processMovingImageSlot();///<Run centerline extraction
  void processRefImageSlot();///<Run centerline extraction
  void i2iRegistrationSlot();///<Run I2I registration
  void applyUpdateSlot();///<Apply found transferfunction on another image
  void movingImageRunningSlot();///<Vessel segmentation and centerline extraction running
  void refImageRunningSlot();///<Vessel segmentation and centerline extraction running
  void registrationRunningSlot();///<I2I registration running
  void movingImageFinishedSlot(int, QProcess::ExitStatus);///<Vessel segmentation and centerline extraction finished
  void refImageFinishedSlot(int, QProcess::ExitStatus);///<Vessel segmentation and centerline extraction finished
  void registrationFinishedSlot(int, QProcess::ExitStatus);///<I2I registration finished
  void changeWorkingFolderSlot(const QString& folder);///<Change working folder
  void movingImageBoxChangedSlot(int index);
  void refImageBoxChangedSlot(int index);
};

}//namespace
#endif //CXSHIFTCORRECTIONWIDGET_H_

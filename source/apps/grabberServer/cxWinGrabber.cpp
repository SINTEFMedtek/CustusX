#ifdef WIN32

//#import <Foundation/NSAutoreleasePool.h>
//#import <QTKit/QTKit.h>

//#include "sscTime.h"
//#include "sscTypeConversions.h"
//#include <iostream>
//#include <QWidget>
//#include <QMacCocoaViewContainer>

#include "cxWinGrabber.h"
#include "sscMessageManager.h"

#include "vtkSonixVideoSource.h"

//==============================================================================
////Class that connects to the videosignal and receives frames
//@interface VideoObserver : NSObject {
//
//  @private
//    cx::WinGrabber*   mGrabber;
//}
//
//- (id)init;
//- (void)dealloc;
//- (void)setCallback:(QTCaptureOutput*)videoStream;
//- (void)setGrabber:(cx::WinGrabber*)grabber;
//@end
//
//  @implementation VideoObserver
//
//  - (id)init {
//    self = [super init];
//    return self;
//}
//
//- (void)dealloc {
//  [super dealloc];
//}
//
//- (void)setCallback:(QTCaptureDecompressedVideoOutput*)videoStream {
//  [videoStream setDelegate:self];
//
//}
//
//- (void)setGrabber:(cx::WinGrabber*)grabber {
//  mGrabber = grabber;
//}
//
//
////method that receives the frames from the grabber, 
////connected via [mObjectiveC->mVideoObserver setCallback:mObjectiveC->mCaptureDecompressedVideoOutput]
//- (void)captureOutput:(QTCaptureFileOutput *)captureOutput didOutputVideoFrame:(CVImageBufferRef)videoFrame withSampleBuffer:(QTSampleBuffer *)sampleBuffer fromConnection:(QTCaptureConnection *)connection
//{
//  //LOCK
//  CVPixelBufferLockBaseAddress(videoFrame, 0);
//
//  //width and height of the incoming image
//  int width = CVPixelBufferGetWidth(videoFrame);
//  int height = CVPixelBufferGetHeight(videoFrame);  
//  //NSLog(@"PixelBufferWidth : %d",CVPixelBufferGetWidth(videoFrame));
//  //NSLog(@"PixelBufferHeight : %d",CVPixelBufferGetHeight(videoFrame));
//
//  //finding the timetag of the image
//  QTTime  timetag = [sampleBuffer presentationTime]; //presentationTime seems to be relative to a (unknown) time, probably something driver specific which we cannot controll via QtKit
//
//  //OSType pixelFormat = CVPixelBufferGetPixelFormatType(videoFrame);
//  //CFDictionaryRef dictonary = CVPixelFormatDescriptionCreateWithPixelFormatType(kCFAllocatorDefault, pixelFormat);
//  //NSLog(@"PixelFormatType : %d",CVPixelBufferGetPixelFormatType(videoFrame));
//  //CFStringRef pixelFormatString =  UTCreateStringForOSType(CVPixelBufferGetPixelFormatType(videoFrame));
//
//  //setting up the frame
//  Frame frame;
//  //frame.mTimestamp = ((double)timetag.timeValue / (double)timetag.timeScale) * 1000;
//  frame.mTimestamp = ssc::getMilliSecondsSinceEpoch()/1000; //resmapling the timestamp because we cannot find convert the original timestamp into epoch time
//  frame.mWidth = width;
//  frame.mHeight = height;
//  frame.mPixelFormat = static_cast<int>(CVPixelBufferGetPixelFormatType(videoFrame));
//  //ssc::messageManager()->sendDebug("Pixel format: "+qstring_cast(frame.mPixelFormat));
//  frame.mFirstPixel = reinterpret_cast<char*>(CVPixelBufferGetBaseAddress(videoFrame));
//
//  mGrabber->sendFrame(frame); //results in a memcpy of the frame
//
//  //UNLOCK
//  CVPixelBufferUnlockBaseAddress(videoFrame, 0);
//
//  //   NSLog(@"PixelFormatType : %d",CVPixelBufferGetPixelFormatType(videoFrame));
//  //   NSLog(@"Pixelbuffer width : %d",CVPixelBufferGetWidth(videoFrame));
//  //   NSLog(@"Pixelbuffer height : %d",CVPixelBufferGetHeight(videoFrame));
//  //   NSLog(@"Pixelbuffer bytes per row : %d",CVPixelBufferGetBytesPerRow(videoFrame));
//  //   NSLog(@"Pixelbuffer data size : %d",CVPixelBufferGetDataSize(videoFrame));
//}
//@end
//
namespace cx
{
//==============================================================================
////Helper class for combining objective-c with c++/Qt
//class WinGrabber::ObjectiveC 
//{
//public:
//  NSAutoreleasePool*                    mPool;
//  //device
//  QTCaptureDevice*                      mSelectedDevice;
//  //input
//  QTCaptureDeviceInput*                 mCaptureDeviceInput;
//  //session
//  QTCaptureSession*                     mCaptureSession;
//  //output
//  QTCaptureDecompressedVideoOutput*     mCaptureDecompressedVideoOutput; //may not drop frames
//  //QTCaptureVideoPreviewOutput*          mVideoPreviewOutput; //may drop frames
//  //view
//  QTCaptureView*                        mCaptureView;
//  //observer
//  VideoObserver*                        mVideoObserver;
//};
//==============================================================================

WinGrabber::WinGrabber() :
  Grabber(),
  mIpAdressToConnectTo("127.0.0.1"),
  //mIpAdressToConnectTo("169.254.200.200"),
  mBufferSize(500),
  mImagingMode(0),
  mAcquisitionDataType(0x00000004)
  //mObjectiveC(new ObjectiveC),
  //mSuperVideo(false)
{
  ////allocate memory
  //mObjectiveC->mPool = [[NSAutoreleasePool alloc] init];
  //mObjectiveC->mCaptureSession = [[QTCaptureSession alloc] init];

  mSonixGrabber = vtkSonixVideoSource::New();
  mSonixGrabber->SetSonixIP(mIpAdressToConnectTo.toStdString().c_str());
  mSonixGrabber->SetImagingMode(mImagingMode);
  mSonixGrabber->SetAcquisitionDataType(mAcquisitionDataType);
  mSonixGrabber->SetFrameBufferSize(mBufferSize);  // Number of image frames in buffer
  mSonixGrabber->Initialize(); // Run initialize to set spacing and offset


  this->mSonixHelper = new SonixHelper;
  mSonixGrabber->setSonixHelper(this->mSonixHelper);
  connect(mSonixHelper, SIGNAL(frame(Frame&)), this, SIGNAL(frame(Frame&)), Qt::DirectConnection);
}

WinGrabber::~WinGrabber()
{
  //[mObjectiveC->mPool release];

  mSonixGrabber->ReleaseSystemResources();
  mSonixGrabber->Delete();
}

SonixHelper* WinGrabber::getSonixHelper()
{
  return this->mSonixHelper;
}

void WinGrabber::start()
{
  if(this->isGrabbing())
    return;

  /*if(this->findConnectedDevice())
  {
    if(!this->openDevice())
      ssc::messageManager()->sendError("Could not open the selected device. Aborting.");
    else
      this->startSession();
  } else
  {
    ssc::messageManager()->sendError("Could not find a connected device. Aborting.");
  }*/
  mSonixGrabber->Record();
}

void WinGrabber::stop()
{
  if(!this->isGrabbing())
    return;

  mSonixGrabber->Stop();

  this->stopSession();
}

void WinGrabber::displayPreview(QWidget* parent)
{
  //QMacCocoaViewContainer* container = new QMacCocoaViewContainer(0 ,parent);
  //container->setCocoaView(mObjectiveC->mCaptureView);
  //container->setFixedSize(800,600); //todo?
}

bool WinGrabber::isGrabbing()
{
  //return [mObjectiveC->mCaptureSession isRunning];
  return false;
}

bool WinGrabber::findConnectedDevice()
{
  bool found = false;

  ////find which grabber is connected to the system
  //NSArray *devices = [QTCaptureDevice inputDevicesWithMediaType:QTMediaTypeVideo];
  //int i = [devices count];
  //ssc::messageManager()->sendInfo("Number of connected grabber devices: "+qstring_cast(i));

  //if([devices count] == 0)
  //  return found;

  //NSEnumerator *enumerator = [devices objectEnumerator];
  //QTCaptureDevice* captureDevice;

  //while((captureDevice = [enumerator nextObject])) {
  //  NSString* grabberName = [captureDevice localizedDisplayName];
  //  this->reportString(grabberName);

  //  NSComparisonResult compareResult;

  //  //buildt in apple i-sight camera
  //  compareResult = [grabberName localizedCompare:@"Built-in iSight"];
  //  if (compareResult == NSOrderedSame)
  //  {
  //    mObjectiveC->mSelectedDevice = captureDevice;
  //    found = true;
  //  }

  //  //new VGA grabber (Epiphan)
  //  compareResult = [grabberName localizedCompare:@"D4U24488"];
  //  if (compareResult == NSOrderedSame)
  //  {
  //    mObjectiveC->mSelectedDevice = captureDevice;
  //    found = true;
  //  }
  //  //even newer VGA grabber (Epiphan)
  //  compareResult = [grabberName localizedCompare:@"D4U24942"];
  //  if (compareResult == NSOrderedSame)
  //  {
  //    mObjectiveC->mSelectedDevice = captureDevice;
  //    found = true;
  //  }

  //  //old VGA grabber (Epiphan)
  //  compareResult = [grabberName localizedCompare:@"V2U10443"];
  //  if (compareResult == NSOrderedSame)
  //  {
  //    mObjectiveC->mSelectedDevice = captureDevice;
  //    found = true;
  //  }

  //  //S-VHS grabber
  //  compareResult = [grabberName localizedCompare:@"S-VHS"];
  //  if (compareResult == NSOrderedSame) 
  //  {
  //    mObjectiveC->mSelectedDevice = captureDevice;
  //    found = true;
  //    mSuperVideo = true;
  //  }
  //}
  return found;
}

bool WinGrabber::openDevice()
{
  //try to open the selected device
  //NSError* error;
  bool success = false;//[mObjectiveC->mSelectedDevice open:&error];
  //if(!success)
  //  this->reportError(error);

  return success;
}

bool WinGrabber::closeDevice()
{
  return true;
}

void WinGrabber::startSession()
{
  //try to connect the device to the pipeline
  //NSError* error;
  //mObjectiveC->mCaptureDeviceInput = [[QTCaptureDeviceInput alloc] initWithDevice:mObjectiveC->mSelectedDevice];
  bool success = false;//[mObjectiveC->mCaptureSession addInput:mObjectiveC->mCaptureDeviceInput error:&error];

  if (!success)
  {
    //this->reportError(error);
    return;
  }

  this->setupGrabbing();

  //[mObjectiveC->mCaptureSession startRunning];

  //NSString* grabberName = [mObjectiveC->mSelectedDevice localizedDisplayName];
  //std::string name = std::string([grabberName UTF8String]);
  //ssc::messageManager()->sendSuccess("Started grabbing from "+qstring_cast(name));
  emit started();
}

void WinGrabber::stopSession()
{
  //[mObjectiveC->mCaptureSession stopRunning];
  ssc::messageManager()->sendSuccess("Grabbing stopped.");
  emit stopped();
}

void WinGrabber::setupGrabbing()
{
  ////catch the frames and transmitt them using a signal
  //mObjectiveC->mCaptureDecompressedVideoOutput = [[QTCaptureDecompressedVideoOutput alloc] init];

  //NSDictionary* attributes;
  //if(mSuperVideo)
  //{
  //  attributes = [NSDictionary dictionaryWithObjectsAndKeys:
  //[NSNumber numberWithDouble:768.0], (id)kCVPixelBufferWidthKey,
  //  [NSNumber numberWithDouble:576.0], (id)kCVPixelBufferHeightKey,
  //  [NSNumber numberWithUnsignedInt:k32ARGBPixelFormat], (id)kCVPixelBufferPixelFormatTypeKey,
  //  nil];
  ////NSLog(@"Set pixel format to 768, 576");
  //}else
  //{
  //  attributes = [NSDictionary dictionaryWithObjectsAndKeys:
  //[NSNumber numberWithDouble:800.0], (id)kCVPixelBufferWidthKey,
  //  [NSNumber numberWithDouble:600.0], (id)kCVPixelBufferHeightKey,
  //  [NSNumber numberWithUnsignedInt:k32ARGBPixelFormat], (id)kCVPixelBufferPixelFormatTypeKey,
  //  nil];
  //}
  //[mObjectiveC->mCaptureDecompressedVideoOutput setPixelBufferAttributes:attributes];

  //NSError* error;
  bool success = false;//[mObjectiveC->mCaptureSession addOutput:mObjectiveC->mCaptureDecompressedVideoOutput error:&error];
  if(!success)
  {
    //this->reportError(error);
    return;
  }

  ////Create a observer that listens to the videosignal from the captured decompressed video output
  //mObjectiveC->mVideoObserver = [[VideoObserver alloc] init];
  //[mObjectiveC->mVideoObserver setGrabber:this];
  //[mObjectiveC->mVideoObserver setCallback:mObjectiveC->mCaptureDecompressedVideoOutput]; //may not drop frames
  ////[mObjectiveC->mVideoObserver setCallback:mObjectiveC->mVideoPreviewOutput]; //may drop frames

  //mObjectiveC->mCaptureView = [[QTCaptureView alloc] init];
  //[mObjectiveC->mCaptureView setCaptureSession:mObjectiveC->mCaptureSession];
}

//void WinGrabber::reportError(NSError* error)
//{
//  if(!error)
//    return;
//  NSString* errorString = [error localizedDescription];
//  std::string errorStdString = std::string([errorString UTF8String]);
//  ssc::messageManager()->sendError(qstring_cast(errorStdString));
//}

//void WinGrabber::reportString(NSString* string)
//{
//  if(!string)
//    return;
//  std::string stdString = std::string([string UTF8String]);
//  ssc::messageManager()->sendInfo(qstring_cast(stdString));  
//}

void WinGrabber::sendFrame(Frame& frame)
{
  //[mObjectiveC->mCaptureView setFrameSize:NSMakeSize(frame.mWidth, frame.mHeight)];
  emit this->frame(frame);
}

//void WinGrabber::printAvailablePixelFormats()
//{
//  // Prints the available pixel formats
//  CFArrayRef pixelFormatDescriptionsArray = NULL;
//  CFIndex i;
//
//  pixelFormatDescriptionsArray =
//    CVPixelFormatDescriptionArrayCreateWithAllPixelFormatTypes(kCFAllocatorDefault);
//
//  printf("Core Video Supported Pixel Format Types:\n\n");
//
//  for (i = 0; i < CFArrayGetCount(pixelFormatDescriptionsArray); i++) {
//    CFStringRef pixelFormat = NULL;
//
//    CFNumberRef pixelFormatFourCC = (CFNumberRef)CFArrayGetValueAtIndex(pixelFormatDescriptionsArray, i);
//
//    if (pixelFormatFourCC != NULL) {
//      UInt32 value;
//
//      CFNumberGetValue(pixelFormatFourCC, kCFNumberSInt32Type, &value);
//
//      if (value <= 0x28) {
//        pixelFormat = CFStringCreateWithFormat(kCFAllocatorDefault, NULL,
//          CFSTR("Core Video Pixel Format Type: %d\n"), value);
//      } else {
//        pixelFormat = CFStringCreateWithFormat(kCFAllocatorDefault, NULL,
//          CFSTR("Core Video Pixel Format Type (FourCC):%c%c%c%c\n"), (char)(value >> 24), (char)(value >> 16),
//          (char)(value >> 8), (char)value);
//      }
//
//      CFShow(pixelFormat);
//      CFRelease(pixelFormat);
//    }
//  }  
//}

}//namespace cx
#endif /*WIN32*/

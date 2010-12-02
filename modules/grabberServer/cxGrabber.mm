#import <Foundation/NSAutoreleasePool.h>
#import <QTKit/QTKit.h>

#include "sscTypeConversions.h"
#include "cxGrabber.h"
#include <iostream>
#include <QWidget>
#include <QMacCocoaViewContainer>

#include "sscMessageManager.h"

//==============================================================================
//Class that connects to the videosignal and receives frames
@interface VideoObserver : NSObject {
    
@private
    int               mFrameCount;
    float             mFirstTimeTag;  //in milliseconds
    float             mStopTimeTag; //in milliseconds
    cx::MacGrabber*   mGrabber;
}

- (id)init;
- (void)dealloc;
- (void)setCallback:(QTCaptureOutput*)videoStream;
- (void)setGrabber:(cx::MacGrabber*)grabber;
- (int)getFrameCount;
@end

@implementation VideoObserver

- (id)init {
    self = [super init];
    mFrameCount = 0;
    mFirstTimeTag = 0;
    mStopTimeTag = std::numeric_limits<float>::max();
    return self;
}

- (void)dealloc {
    [super dealloc];
}

- (void)setCallback:(QTCaptureDecompressedVideoOutput*)videoStream {
    [videoStream setDelegate:self];
    
}

- (void)setGrabber:(cx::MacGrabber*)grabber {
  mGrabber = grabber;
}

- (int)getFrameCount {
    return mFrameCount;
}

//method that receives the frames from the grabber, 
//connected via [mObjectiveC->mVideoObserver setCallback:mObjectiveC->mCaptureDecompressedVideoOutput]
- (void)captureOutput:(QTCaptureFileOutput *)captureOutput didOutputVideoFrame:(CVImageBufferRef)videoFrame withSampleBuffer:(QTSampleBuffer *)sampleBuffer fromConnection:(QTCaptureConnection *)connection
{
  //LOCK
  CVPixelBufferLockBaseAddress(videoFrame, 0);
  
  //width and height of the incoming image
  int width = CVPixelBufferGetWidth(videoFrame);
  int height = CVPixelBufferGetHeight(videoFrame);  
  
  //finding the timetag of the image
  QTTime  timetag = [sampleBuffer presentationTime];
  float timeTagValue = ((float)timetag.timeValue / (float)timetag.timeScale) * 1000;
  
  //OSType pixelFormat = CVPixelBufferGetPixelFormatType(videoFrame);
  //CFDictionaryRef dictonary = CVPixelFormatDescriptionCreateWithPixelFormatType(kCFAllocatorDefault, pixelFormat);
  //NSLog(@"PixelFormatType : %d",CVPixelBufferGetPixelFormatType(videoFrame));
  //CFStringRef pixelFormatString =  UTCreateStringForOSType(CVPixelBufferGetPixelFormatType(videoFrame));
  
  //setting up the frame
  Frame frame;
  frame.mTimestamp = ((float)timetag.timeValue / (float)timetag.timeScale) * 1000;
  frame.mWidth = width;
  frame.mHeight = height;
  frame.mPixelFormat = static_cast<int>(CVPixelBufferGetPixelFormatType(videoFrame));
  //ssc::messageManager()->sendDebug("Pixel format: "+qstring_cast(frame.mPixelFormat));
  frame.mFirstPixel = reinterpret_cast<char*>(CVPixelBufferGetBaseAddress(videoFrame));
  
  mGrabber->sendFrame(frame); //results in a memcpy of the frame
  
  //UNLOCK
  CVPixelBufferUnlockBaseAddress(videoFrame, 0);
  
  mFrameCount++;
  
  
  //keeping the timetag for the first frame
  if (mFrameCount == 1) 
  {
    mFirstTimeTag = timeTagValue;
    //std::cout << "mFirstTimeTag is now set to: " << mFirstTimeTag << std::endl;
  }
  
//   NSLog(@"PixelFormatType : %d",CVPixelBufferGetPixelFormatType(videoFrame));
//   NSLog(@"Pixelbuffer width : %d",CVPixelBufferGetWidth(videoFrame));
//   NSLog(@"Pixelbuffer height : %d",CVPixelBufferGetHeight(videoFrame));
//   NSLog(@"Pixelbuffer bytes per row : %d",CVPixelBufferGetBytesPerRow(videoFrame));
//   NSLog(@"Pixelbuffer data size : %d",CVPixelBufferGetDataSize(videoFrame));
}
@end

namespace cx
{
//==============================================================================
//Helper class for combining objective-c with c++/Qt
class MacGrabber::ObjectiveC 
{
public:
    NSAutoreleasePool*                    mPool;
    //device
    QTCaptureDevice*                      mSelectedDevice;
    //input
    QTCaptureDeviceInput*                 mCaptureDeviceInput;
    //session
    QTCaptureSession*                     mCaptureSession;
    //output
    QTCaptureDecompressedVideoOutput*     mCaptureDecompressedVideoOutput; //may not drop frames
    //QTCaptureVideoPreviewOutput*          mVideoPreviewOutput; //may drop frames
    //view
    QTCaptureView*                        mCaptureView;
    //observer
    VideoObserver*                        mVideoObserver;
};
//==============================================================================

Grabber::Grabber()
{
  typedef cx::Frame Frame;
  qRegisterMetaType<Frame>("Frame");
}

//==============================================================================

MacGrabber::MacGrabber() :
  Grabber(),
  mObjectiveC(new ObjectiveC)
{
  //allocate memory
  mObjectiveC->mPool = [[NSAutoreleasePool alloc] init];
  mObjectiveC->mCaptureSession = [[QTCaptureSession alloc] init];
}

MacGrabber::~MacGrabber()
{
  [mObjectiveC->mPool release];
}

void MacGrabber::start()
{
  if(this->findConnectedDevice())
  {
    if(!this->openDevice())
      ssc::messageManager()->sendError("Could not open the selected device. Aborting.");
    else
      this->startSession();
  } else
  {
    ssc::messageManager()->sendError("Could not find a connected device. Aborting.");
  }
}

void MacGrabber::stop()
{
  this->stopSession();
}

QMacCocoaViewContainer* MacGrabber::getPreviewWidget(QWidget* parent)
{
  QMacCocoaViewContainer* retval = new QMacCocoaViewContainer(0 ,parent);
  retval->setCocoaView(mObjectiveC->mCaptureView);
  retval->setFixedSize(800,600); //todo?
  
  return retval;
}

bool MacGrabber::findConnectedDevice()
{
  bool found = false;
  
  //find which grabber is connected to the system
  NSArray *devices = [QTCaptureDevice inputDevicesWithMediaType:QTMediaTypeVideo];
  NSLog(@"Number of input devices %d",[devices count]);
  
  NSEnumerator *enumerator = [devices objectEnumerator];
  QTCaptureDevice* captureDevice;
  
  while((captureDevice = [enumerator nextObject])) {
      NSString *grabberName = [captureDevice localizedDisplayName];
      NSLog(@"Grabber name: %d",grabberName);
      
      NSComparisonResult compareResult;
      
      //new VGA grabber (Epiphan)
      compareResult = [grabberName localizedCompare:@"D4U24488"];
      if (compareResult == NSOrderedSame)
      {
        mObjectiveC->mSelectedDevice = captureDevice;
        found = true;
      }
      
      //old VGA grabber (Epiphan)
      compareResult = [grabberName localizedCompare:@"V2U10443"];
      if (compareResult == NSOrderedSame)
      {
        mObjectiveC->mSelectedDevice = captureDevice;
        found = true;
      }
      
      //S-VHS grabber
      compareResult = [grabberName localizedCompare:@"S-VHS"];
      if (compareResult == NSOrderedSame) 
      {
        mObjectiveC->mSelectedDevice = captureDevice;
        found = true;
      }
      
      //buildt in apple i-sight camera
      compareResult = [grabberName localizedCompare:@"Built-in iSight"];
      if (compareResult == NSOrderedSame)
      {
        mObjectiveC->mSelectedDevice = captureDevice;
        found = true;
      }
  }
  return found;
}

bool MacGrabber::openDevice()
{
  //try to open the selected device
  NSError* error;
  bool success = [mObjectiveC->mSelectedDevice open:&error];
  return success;
}

bool MacGrabber::closeDevice()
{
  return true;
}

bool MacGrabber::startSession()
{
  //try to connect the device to the pipeline
  NSError* error;
  mObjectiveC->mCaptureDeviceInput = [[QTCaptureDeviceInput alloc] initWithDevice:mObjectiveC->mSelectedDevice];
  bool success = [mObjectiveC->mCaptureSession addInput:mObjectiveC->mCaptureDeviceInput error:&error];
  
  if (!success)
    return success;
  
  this->setupGrabbing();
  
  [mObjectiveC->mCaptureSession startRunning];
  
  return success;
}

void MacGrabber::stopSession()
{
  [mObjectiveC->mCaptureSession stopRunning];
}

void MacGrabber::setupGrabbing()
{
  //=============================================================================
  // Prints the available pixel formats
    CFArrayRef pixelFormatDescriptionsArray = NULL;
    CFIndex i;

    pixelFormatDescriptionsArray =
    CVPixelFormatDescriptionArrayCreateWithAllPixelFormatTypes(kCFAllocatorDefault);

    printf("Core Video Supported Pixel Format Types:\n\n");

    for (i = 0; i < CFArrayGetCount(pixelFormatDescriptionsArray); i++) {
        CFStringRef pixelFormat = NULL;

        CFNumberRef pixelFormatFourCC = (CFNumberRef)CFArrayGetValueAtIndex(pixelFormatDescriptionsArray, i);

        if (pixelFormatFourCC != NULL) {
            UInt32 value;

            CFNumberGetValue(pixelFormatFourCC, kCFNumberSInt32Type, &value);

            if (value <= 0x28) {
                pixelFormat = CFStringCreateWithFormat(kCFAllocatorDefault, NULL,
                              CFSTR("Core Video Pixel Format Type: %d\n"), value);
            } else {
                pixelFormat = CFStringCreateWithFormat(kCFAllocatorDefault, NULL,
                              CFSTR("Core Video Pixel Format Type (FourCC):%c%c%c%c\n"), (char)(value >> 24), (char)(value >> 16),
                              (char)(value >> 8), (char)value);
            }

            CFShow(pixelFormat);
            CFRelease(pixelFormat);
        }
    }
   //==============================================================================

  //catch the frames and transmitt them using a signal
  mObjectiveC->mCaptureDecompressedVideoOutput = [[QTCaptureDecompressedVideoOutput alloc] init];
  
  //==============================================================================
  
      NSDictionary* attributes = [NSDictionary dictionaryWithObjectsAndKeys:
                                 [NSNumber numberWithDouble:800.0], (id)kCVPixelBufferWidthKey,
                                 [NSNumber numberWithDouble:600.0], (id)kCVPixelBufferHeightKey,
                                 [NSNumber numberWithUnsignedInt:k32ARGBPixelFormat], (id)kCVPixelBufferPixelFormatTypeKey,
                                 nil];

    [mObjectiveC->mCaptureDecompressedVideoOutput setPixelBufferAttributes:attributes];
    
  //==============================================================================

  NSError* error;
  bool success = [mObjectiveC->mCaptureSession addOutput:mObjectiveC->mCaptureDecompressedVideoOutput error:&error];
  if(!success)
    return; //TODO

  //Create a observer that listens to the videosignal from the captured decompressed video output
  mObjectiveC->mVideoObserver = [[VideoObserver alloc] init];
  [mObjectiveC->mVideoObserver setGrabber:this];
  [mObjectiveC->mVideoObserver setCallback:mObjectiveC->mCaptureDecompressedVideoOutput]; //may not drop frames
  //[mObjectiveC->mVideoObserver setCallback:mObjectiveC->mVideoPreviewOutput]; //may drop frames
  
  mObjectiveC->mCaptureView = [[QTCaptureView alloc] init];
  [mObjectiveC->mCaptureView setCaptureSession:mObjectiveC->mCaptureSession];
}

void MacGrabber::sendFrame(Frame& frame)
{
  //[mObjectiveC->mCaptureView setFrameSize:NSMakeSize(frame.mWidth, frame.mHeight)];
  emit this->frame(frame);
}

}//namespace cx

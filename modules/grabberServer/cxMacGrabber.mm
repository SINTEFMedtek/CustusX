#ifdef __APPLE__
#import <Foundation/NSAutoreleasePool.h>
#import <QTKit/QTKit.h>

#include "cxMacGrabber.h"

#include "sscTime.h"
#include "sscTypeConversions.h"
#include <iostream>
#include <QWidget>
#include <QMacCocoaViewContainer>

#include "igtlImageMessage.h"
#include "sscMessageManager.h"

// utility for reporting errors
void reportError(NSError* error)
{
    if(!error)
        return;
    NSString* errorString = [error localizedDescription];
    std::string errorStdString = std::string([errorString UTF8String]);
    ssc::messageManager()->sendError(qstring_cast(errorStdString));
}

// utility for reporting strings
void reportString(NSString* string)
{
    if(!string)
        return;
    std::string stdString = std::string([string UTF8String]);
    ssc::messageManager()->sendInfo(qstring_cast(stdString));
}

//==============================================================================
//Class that connects to the videosignal and receives frames
@interface VideoObserver : NSObject {
    
@private
    cx::MacGrabber*   mGrabber;
}

- (id)init;
- (void)dealloc;
- (void)setCallback:(QTCaptureOutput*)videoStream;
- (void)setGrabber:(cx::MacGrabber*)grabber;
@end

@implementation VideoObserver

- (id)init {
    self = [super init];
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


//method that receives the frames from the grabber, 
//connected via [mObjectiveC->mVideoObserver setCallback:mObjectiveC->mCaptureDecompressedVideoOutput]
- (void)captureOutput:(QTCaptureFileOutput *)captureOutput didOutputVideoFrame:(CVImageBufferRef)videoFrame withSampleBuffer:(QTSampleBuffer *)sampleBuffer fromConnection:(QTCaptureConnection *)connection
{
  //LOCK
  CVPixelBufferLockBaseAddress(videoFrame, 0);
  
  //width and height of the incoming image
  int width = CVPixelBufferGetWidth(videoFrame);
  int height = CVPixelBufferGetHeight(videoFrame);  
  //NSLog(@"PixelBufferWidth : %d",CVPixelBufferGetWidth(videoFrame));
  //NSLog(@"PixelBufferHeight : %d",CVPixelBufferGetHeight(videoFrame));
  
  //finding the timetag of the image
  QTTime  timetag = [sampleBuffer presentationTime]; //presentationTime seems to be relative to a (unknown) time, probably something driver specific which we cannot controll via QtKit
  
  //OSType pixelFormat = CVPixelBufferGetPixelFormatType(videoFrame);
  //CFDictionaryRef dictonary = CVPixelFormatDescriptionCreateWithPixelFormatType(kCFAllocatorDefault, pixelFormat);
  //NSLog(@"PixelFormatType : %d",CVPixelBufferGetPixelFormatType(videoFrame));
  //CFStringRef pixelFormatString =  UTCreateStringForOSType(CVPixelBufferGetPixelFormatType(videoFrame));
  
  //setting up the frame
  Frame frame;
  //frame.mTimestamp = ((double)timetag.timeValue / (double)timetag.timeScale) * 1000;
  frame.mTimestamp = ssc::getMilliSecondsSinceEpoch()/1000; //resmapling the timestamp because we cannot find convert the original timestamp into epoch time
  frame.mWidth = width;
  frame.mHeight = height;
  //frame.mPixelFormat = static_cast<int>(CVPixelBufferGetPixelFormatType(videoFrame));
  frame.mPixelFormat = igtl::ImageMessage::TYPE_UINT32;
  //ssc::messageManager()->sendDebug("Pixel format: "+qstring_cast(frame.mPixelFormat));
  frame.mFirstPixel = reinterpret_cast<unsigned char*>(CVPixelBufferGetBaseAddress(videoFrame));
	
  //Just initialize these with dummy values
	frame.mSpacing[0] = 0;//1;
	frame.mSpacing[1] = 0;//1;
  frame.mOrigin[0] = 0;
  frame.mOrigin[1] = 0;
  
  mGrabber->sendFrame(frame); //results in a memcpy of the frame
  
  //UNLOCK
  CVPixelBufferUnlockBaseAddress(videoFrame, 0);
  
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

MacGrabber::MacGrabber() :
  Grabber(),
  mSuperVideo(false),
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
  if(this->isGrabbing())
    return;
  
  if(this->findConnectedDevice())
  {
    if(!this->openDevice())
      ssc::messageManager()->sendError("Could not open the selected device. Aborting.");
    else
      this->startSession();
  } else
  {
    ssc::messageManager()->sendError("Could not find a connected device (must be present in SupportedGrabbers.txt). Aborting.");
  }
}

void MacGrabber::stop()
{
  if(!this->isGrabbing())
    return;
  
  this->stopSession();
}

void MacGrabber::displayPreview(QWidget* parent)
{
  QMacCocoaViewContainer* container = new QMacCocoaViewContainer(0 ,parent);
  container->setCocoaView(mObjectiveC->mCaptureView);
  container->setFixedSize(800,600); //todo?
}

bool MacGrabber::isGrabbing()
{
  return [mObjectiveC->mCaptureSession isRunning];
}

bool MacGrabber::findConnectedDevice()
{
  //Read file with supported grabbers
  NSString *path = [NSString stringWithString:@"SupportedGrabbers.txt"];
  NSError *error = nil;
  NSString *words = [[NSString alloc] initWithContentsOfFile:path
                                                 encoding:NSUTF8StringEncoding error:&error];
  reportError(error);

  NSArray* lines = [words componentsSeparatedByCharactersInSet:[NSCharacterSet newlineCharacterSet]];
  //for (NSString *line in lines)
  //  NSLog(@"Supported grabber: %@", line);

  //Report to user all found grabbers
  NSArray *devices = [QTCaptureDevice inputDevicesWithMediaType:QTMediaTypeVideo];
  int i = [devices count];
  ssc::messageManager()->sendInfo("Number of input grabber devices: "+qstring_cast(i));
  for (QTCaptureDevice *device in devices)
    reportString([device localizedDisplayName]);

  //Select the grabber with highest file priority
  bool found = false;
  for (NSString *line in lines)
  {
    for(QTCaptureDevice *device in devices)
    {
      NSString *deviceName = [device localizedDisplayName];
      NSComparisonResult compareResult = [deviceName localizedCompare:line];
      if (compareResult == NSOrderedSame)
      {
        NSString *foundtext = [NSString stringWithFormat:@"Matched grabber from file: %@", line];
        reportString(foundtext);
        mObjectiveC->mSelectedDevice = device;
        found = true;
        break;
      }
    }
    if(found)
      break;
  }

  //Report to user how to change grabber
  ssc::messageManager()->sendInfo("To change priority of grabbers or add new grabbers, edit the file \"SupportedGrabbers.txt\" that is next to the GrabberServer.");

  return found;

  /*
  bool found = false;
  
  //find which grabber is connected to the system
  NSArray *devices = [QTCaptureDevice inputDevicesWithMediaType:QTMediaTypeVideo];
  int i = [devices count];
  ssc::messageManager()->sendInfo("Number of connected grabber devices: "+qstring_cast(i));
  
  NSEnumerator *enumerator = [devices objectEnumerator];
  QTCaptureDevice* captureDevice;
  NSUInteger numberOfDevices = [devices count];
  if(numberOfDevices == 0)
    return found;
  else if(numberOfDevices == 1)
  {
    captureDevice = [enumerator nextObject];
    NSString* grabberName = [captureDevice localizedDisplayName];
    this->reportString(grabberName);
    mObjectiveC->mSelectedDevice = captureDevice;
    found = true;
  } else {
    //if more than one grabber connected, select one that is not Built-in iSight
    while((captureDevice = [enumerator nextObject])) {
        NSString* grabberName = [captureDevice localizedDisplayName];
        this->reportString(grabberName);

        NSComparisonResult compareResult = [grabberName localizedCompare:@"Built-in iSight"];
        if (compareResult != NSOrderedSame)
        {
          mObjectiveC->mSelectedDevice = captureDevice;
          found = true;
          break;
        }
    }
  }
  
  return found;
  */
}

bool MacGrabber::openDevice()
{
  //try to open the selected device
  NSError* error;
  bool success = [mObjectiveC->mSelectedDevice open:&error];
  if(!success)
    reportError(error);
    
  return success;
}

bool MacGrabber::closeDevice()
{
  return true;
}

void MacGrabber::startSession()
{
  //try to connect the device to the pipeline
  NSError* error;
  mObjectiveC->mCaptureDeviceInput = [[QTCaptureDeviceInput alloc] initWithDevice:mObjectiveC->mSelectedDevice];
  bool success = [mObjectiveC->mCaptureSession addInput:mObjectiveC->mCaptureDeviceInput error:&error];
  
  if (!success)
  {
    reportError(error);
    return;
  }
  
  this->setupGrabbing();
  
  [mObjectiveC->mCaptureSession startRunning];
  
  NSString* grabberName = [mObjectiveC->mSelectedDevice localizedDisplayName];
  std::string name = std::string([grabberName UTF8String]);
  ssc::messageManager()->sendSuccess("Started grabbing from "+qstring_cast(name));
  emit started();
}

void MacGrabber::stopSession()
{
  [mObjectiveC->mCaptureSession stopRunning];
  ssc::messageManager()->sendSuccess("Grabbing stopped.");
  emit stopped();
}

void MacGrabber::setupGrabbing()
{
  //catch the frames and transmitt them using a signal
  mObjectiveC->mCaptureDecompressedVideoOutput = [[QTCaptureDecompressedVideoOutput alloc] init];
  
  NSDictionary* attributes;
  if(mSuperVideo)
  {
    attributes = [NSDictionary dictionaryWithObjectsAndKeys:
                               [NSNumber numberWithDouble:768.0], (id)kCVPixelBufferWidthKey,
                               [NSNumber numberWithDouble:576.0], (id)kCVPixelBufferHeightKey,
                               [NSNumber numberWithUnsignedInt:k32ARGBPixelFormat], (id)kCVPixelBufferPixelFormatTypeKey,
                               nil];
    //NSLog(@"Set pixel format to 768, 576");
  }else
  {
    attributes = [NSDictionary dictionaryWithObjectsAndKeys:
                               [NSNumber numberWithDouble:800.0], (id)kCVPixelBufferWidthKey,
                               [NSNumber numberWithDouble:600.0], (id)kCVPixelBufferHeightKey,
                               [NSNumber numberWithUnsignedInt:k32ARGBPixelFormat], (id)kCVPixelBufferPixelFormatTypeKey,
                               nil];
  }
  [mObjectiveC->mCaptureDecompressedVideoOutput setPixelBufferAttributes:attributes];

  NSError* error;
  bool success = [mObjectiveC->mCaptureSession addOutput:mObjectiveC->mCaptureDecompressedVideoOutput error:&error];
  if(!success)
  {
    reportError(error);
    return;
  }

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

void MacGrabber::printAvailablePixelFormats()
{
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
}

}//namespace cx
#endif

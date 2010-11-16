#import <Foundation/NSAutoreleasePool.h>
#import <QTKit/QTKit.h>

#include "cxGrabber.h"
#include <iostream>
#include <QWidget>

//#include "sscMessageManager.h"

//==============================================================================
//Class that connects to the videosignal and receives frames
@interface VideoObserver : NSObject {
    
@private
    int               mFrameCount;
    float             mFirstTimeTag;  //in milliseconds
    float             mStopTimeTag; //in milliseconds
}

- (id)init;
- (void)dealloc;
- (void)setCallback:(QTCaptureDecompressedVideoOutput*)videoStream;
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

- (int)getFrameCount {
    return mFrameCount;
}

//method that receives the frames from the grabber, 
//connected via [mObjectiveC->mVideoObserver setCallback:mObjectiveC->mCaptureDecompressedVideoOutput]
- (void)captureOutput:(QTCaptureFileOutput *)captureOutput didOutputVideoFrame:(CVImageBufferRef)videoFrame withSampleBuffer:(QTSampleBuffer *)sampleBuffer fromConnection:(QTCaptureConnection *)connection
{  
  //width and height of the incoming image
  int width = CVPixelBufferGetWidth(videoFrame);
  int height = CVPixelBufferGetHeight(videoFrame);
  
  //extracting pixels from the image
  CVPixelBufferLockBaseAddress(videoFrame, 0);
  unsigned char* bufferAddr = reinterpret_cast<unsigned char*>(CVPixelBufferGetBaseAddress(videoFrame));
  unsigned char pixel;
  
  for (int i=0; i<(width*height); i++)
  {
    pixel = *bufferAddr;
    pixel = 255 - pixel;    // Pixel values are inverted from QuickTime (White is zero)

    //TODO
    //DO SOMETHING WITH THE IMAGE!!!!!
    //create some structure we can live with
    //vtkImage???
    //TODO
    
    bufferAddr++;
  }
  //TODO!!!!
  //call function in MacGrabber::sendFrame() which emits a signal with the frame that the server is connected to
  //TODO!!!
  CVPixelBufferUnlockBaseAddress(videoFrame, 0);
  mFrameCount++;
  std::cout << "Frame written..." << std::endl;
  
  //finding the timetag of the image
  QTTime  timetag = [sampleBuffer presentationTime];
  float timeTagValue = ((float)timetag.timeValue / (float)timetag.timeScale) * 1000;
  
  //keeping the timetag for the first frame
  if (mFrameCount == 1) 
  {
    mFirstTimeTag = timeTagValue;
    //std::cout << "Starting to save frames, mFirstTimeTag is now set to: " << mFirstTimeTag << std::endl;
  }
  
  /*
   NSLog(@"PixelFormatType : %d",CVPixelBufferGetPixelFormatType(videoFrame));
   NSLog(@"Pixelbuffer width : %d",CVPixelBufferGetWidth(videoFrame));
   NSLog(@"Pixelbuffer height : %d",CVPixelBufferGetHeight(videoFrame));
   NSLog(@"Pixelbuffer bytes per row : %d",CVPixelBufferGetBytesPerRow(videoFrame));
   NSLog(@"Pixelbuffer data size : %d",CVPixelBufferGetDataSize(videoFrame));
   */
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
    QTCaptureDevice*                      mSelectedDevice;
    QTCaptureSession*                     mCaptureSession;
    QTCaptureDeviceInput*                 mCaptureDeviceInput;
    QTCaptureDecompressedVideoOutput*     mCaptureDecompressedVideoOutput;
    VideoObserver*                        mVideoObserver;
};
//==============================================================================

MacGrabber::MacGrabber() :
  Grabber(),
  mObjectiveC(new ObjectiveC)
{
  mObjectiveC->mPool = [[NSAutoreleasePool alloc] init];
  
  //make a capture session for receiving frames to save to file
  mObjectiveC->mCaptureSession = [[QTCaptureSession alloc] init];
}

MacGrabber::~MacGrabber()
{}

void MacGrabber::start()
{
  if(this->findConnectedDevice())
  {
    if(!this->openDevice())
      std::cout << "Could not open the selected device. Aborting." << std::endl;
  } else
  {
    std::cout << "Could not find a connected device. Aborting." << std::endl;
  }
}

void MacGrabber::stop()
{}

QWidget* MacGrabber::getPreviewWidget()
{
  QWidget* retval = new QWidget();
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
//                this->setFixedSize(800, 600);
      }
      
      //old VGA grabber (Epiphan)
      compareResult = [grabberName localizedCompare:@"V2U10443"];
      if (compareResult == NSOrderedSame)
      {
        mObjectiveC->mSelectedDevice = captureDevice;
        found = true;
//                this->setFixedSize(800, 600);
      }
      
      //S-VHS grabber
      compareResult = [grabberName localizedCompare:@"S-VHS"];
      if (compareResult == NSOrderedSame) 
      {
        mObjectiveC->mSelectedDevice = captureDevice;
        found = true;
//                this->setFixedSize(768, 576);
      }
      
      //buildt in apple i-sight camera
      compareResult = [grabberName localizedCompare:@"Built-in iSight"];
      if (compareResult == NSOrderedSame)
      {
        mObjectiveC->mSelectedDevice = captureDevice;
        found = true;
//                this->setFixedSize(640, 480);
      }
  }
  return found;
}

bool MacGrabber::openDevice()
{
  //try to open the selected device
  NSError* error;
  bool success = [mObjectiveC->mSelectedDevice open:&error];
  if (!success)
    return success;
  
  //try to connect the device to the pipeline
  mObjectiveC->mCaptureDeviceInput = [[QTCaptureDeviceInput alloc] initWithDevice:mObjectiveC->mSelectedDevice];
  success = [mObjectiveC->mCaptureSession addInput:mObjectiveC->mCaptureDeviceInput error:&error];
  
  if (!success)
    return success;
  
  //set the pixel format to 8 bits (whiteIsZero)
  mObjectiveC->mCaptureDecompressedVideoOutput = [[QTCaptureDecompressedVideoOutput alloc] init];
  [mObjectiveC->mCaptureDecompressedVideoOutput setPixelBufferAttributes:[NSDictionary
                                                                          dictionaryWithObjectsAndKeys:
                                                                          [NSNumber numberWithUnsignedInt:kCVPixelFormatType_8IndexedGray_WhiteIsZero], 
                                                                          (id)kCVPixelBufferPixelFormatTypeKey,
                                                                          nil
                                                                          ]];
  success = [mObjectiveC->mCaptureSession addOutput:mObjectiveC->mCaptureDecompressedVideoOutput error:&error];
  /*if (!success) {
      [[NSAlert alertWithError:error] runModal];
      return;
  }*/
  if (!success)
    return success;

  //Create a observer that listens to the videosignal from the captured decompressed video output
  mObjectiveC->mVideoObserver = [[VideoObserver alloc] init];
  //mObjectiveC->mVideoObserver = observer;
  [mObjectiveC->mVideoObserver setCallback:mObjectiveC->mCaptureDecompressedVideoOutput];
  
  
  //TODO
  //setup preview
  //[mObjectiveC->mVideoPreView setupCaptureSession:mObjectiveC->mCaptureSession];

  
  //REMOVE???
  /*QTCaptureConnection* connection;
  NSEnumerator* connectionEnumerator = [[mObjectiveC->mCaptureMovieFileOutput connections] objectEnumerator];
  while ((connection = [connectionEnumerator nextObject])) {
      NSString* mediaType = [connection mediaType];
      QTCompressionOptions* compressionOptions = nil;
      
      if ([mediaType isEqualToString:QTMediaTypeVideo]) {
          compressionOptions = [QTCompressionOptions compressionOptionsWithIdentifier:@"QTCompressionOptionsLosslessAppleIntermediateVideo"];
      }
      [mObjectiveC->mCaptureMovieFileOutput setCompressionOptions:compressionOptions forConnection:connection];
      //insert the capturesession into a capturelayer
      //[mObjectiveC->mVideoPreView setupCaptureSession:mObjectiveC->mCaptureSession];
  }*/
  
  [mObjectiveC->mCaptureSession startRunning];
        
  return success;
}
}//namespace cx

/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/

#ifndef CXPLUSPROTOCOL_H
#define CXPLUSPROTOCOL_H

#include "org_custusx_core_openigtlink_Export.h"

#include "cxOpenIGTLinkProtocol.h"

namespace cx {

/**
 * @brief The PlusDialect class contains the knowhow on the packages sent from a
 * PlusServer. For this class to function correctly, the Plus configuration file
 * used by the PlusServer needs to set up correctly.
 *
 * ASSUMPTIONS:
 * - connected probes are linear
 * - incoming images does not contain padding (sector == imagesize)
 * - all images comes from a probe
 * - the datasource with the us must be named Probe
 * - the transformation from the probe to the tracker must be named ProbeToTracker
 * - all calibrations are transformation packages named CalibrationTo<Name>X
 *
 * Example configuration used with the Ultrasonix  L14-5 gps probe:

<PlusConfiguration version="2.1" PlusRevision="Plus-2.1.2.4115 - Win32">
  <DataCollection StartupDelaySec="1">
    <DeviceSet Name="CustusX PlusServer: Ultrasonix ultrasound imaging and tracking device" Description="Broadcasting ultrasound images and tracking data acquired from the Ultrasonix system through OpenIGTLink. If PlusServer does not run on the Ultrasonix PC then update the IP attribute in the Device element with the Ultrasonix PC&apos;s IP address." />
    <Device
      Id="VideoDevice"
      Type="SonixVideo"
      AcquisitionRate="30"
      IP="127.0.0.1"
      SharedMemoryStatus="1"
      EnableAutoClip="TRUE"
      AutoClipEnabled="TRUE"
      LocalTimeOffsetSec="-0.145"
      ImageGeometryOutputEnabled="TRUE"
      ImageToTransducerTransformName="ImageToTransducer"
      >
      <DataSources>
        <DataSource
          Type="Video"
          Id="Video"
          PortName="B"
          PortUsImageOrientation="MF" />
      </DataSources>
      <OutputChannels>
        <OutputChannel Id="VideoStream" VideoDataSourceId="Video" />
      </OutputChannels>
    </Device>
    <Device
      Id="TrackerDevice"
      Type="Ascension3DG"
      FilterAcWideNotch="1"
      ToolReferenceFrame="Tracker">
      <DataSources>
        <DataSource
          Type="Tool"
          Id="Probe"
          PortName="0"/>
        <DataSource
          Type="Tool"
          Id="Needle"
          PortName="2"/>
      </DataSources>
      <OutputChannels>
        <OutputChannel Id="TrackerStream">
          <DataSource Id="Probe" />
          <DataSource Id="Needle" />
        </OutputChannel>
      </OutputChannels>
    </Device>
    <Device Id="TrackedVideoDevice" Type="VirtualMixer">
      <InputChannels>
        <InputChannel Id="TrackerStream" />
        <InputChannel Id="VideoStream" />
      </InputChannels>
      <OutputChannels>
        <OutputChannel Id="TrackedVideoStream" />
      </OutputChannels>
    </Device>
  </DataCollection>
  <CoordinateDefinitions>
    <!-- Calibration matrix of Plus, calculated from information from Ultrasonix -->
    <!-- http://perk-software.cs.queensu.ca/plus/doc/nightly/user/DeviceSonixVideo.html -->
    <!-- Needle calibration matrix -->
    <Transform From="Calibration" To="NeedleX"
      Matrix="
        0 1 0 8
        -1 0 0 0
        0 0 1 0
        0 0 0 1" />
    <!-- Probe calibration matrix -->
    <Transform From="Calibration" To="ProbeX"
      Matrix="
        0.0018    0.9477   -0.0175   14.8449
       -1.0000    0.0016   -0.0052   15.0061
       -0.0052    0.0166    0.9998    0.1638
             0         0         0    1.0000" />
  </CoordinateDefinitions>
  <PlusOpenIGTLinkServer
  MaxNumberOfIgtlMessagesToSend="100"
  MaxTimeSpentWithProcessingMs="50"
  ListeningPort="18944"
  SendValidTransformsOnly="true"
  OutputChannelId="TrackedVideoStream">
    <DefaultClientInfo>
      <MessageTypes>
        <Message Type="TRANSFORM" />
        <Message Type="IMAGE" />
      </MessageTypes>
      <TransformNames>
        <Transform Name="ProbeToTracker" />
        <Transform Name="NeedleToTracker" />
        <!-- Start Calibration transformations -->
        <Transform Name="CalibrationToNeedleX" />
        <Transform Name="CalibrationToProbeX" />
        <!-- End Calibration tranformations -->
      </TransformNames>
      <ImageNames>
        <Image Name="Image" EmbeddedTransformToFrame="Transducer" />
      </ImageNames>
    </DefaultClientInfo>
  </PlusOpenIGTLinkServer>
</PlusConfiguration>


 */

class org_custusx_core_openigtlink_EXPORT PlusProtocol : public OpenIGTLinkProtocol
{
public:
    PlusProtocol();

    virtual QString getName() const;

    virtual void translate(const igtl::TransformMessage::Pointer body);
    virtual void translate(const igtl::ImageMessage::Pointer body);
    virtual void translate(const igtl::StringMessage::Pointer body);

private:
    double getCurrentTimestamp() const;
    double getSyncedTimestampForTransformsAndImages(double current_original_timestamp);
    void registerTransformDeviceName(QString deviceName);
    bool isCalibration(QString deviceName) const;
    QString findDeviceForCalibration(QString calibrationDeviceName) const;
    QString extractDeviceNameFromCalibrationDeviceName(QString calibrationDeviceName) const;
    QString findRegisteredTransformDeviceNameThatContains(QString deviceName) const;

    Transform3D igtltool_M_custustool; //custus defines its tool coordinate system differently than igtl
    QStringList mKnownTransformDeviceNames;
    QString mCalibrationKeyword;
    QString mProbeToTrackerName; //name of the device that contains the transform between probe and tracker

    double mLastKnownOriginalTimestamp;
    double mLastKnownLocalTimestamp;
};

}
#endif // CXPLUSPROTOCOL_H

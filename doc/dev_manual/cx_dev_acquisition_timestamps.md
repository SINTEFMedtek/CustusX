Acquisition timestamps {#dev_acquisition_timestamps}
===========================================================

Every tool position and video frame gets a timestamp: the time the device acquired it, in
milliseconds since epoch on this computer's clock. US acquisition and reconstruction pair each
video frame with the probe position at the same time, so a timestamp error becomes a position
error: 10 ms at a probe speed of 50 mm/s is 0.5 mm.

The time a sample is read or received is a poor timestamp. It includes the transfer time and
jitter from USB, serial or network reads, operating system scheduling, and frames that queue up.
CustusX therefore uses the device's own frame counter or clock where it has one, and converts it
to this computer's clock (CustusX#65).

What remains is a constant latency inside the devices, e.g. from image processing in the
scanner. Temporal calibration measures and removes that (see the Temporal Calibration Widget in
the user documentation). Temporal calibrations made before CustusX#65 may need to be redone.


Overview
-----------------------------------------------------------

* NDI tracking (org.custusx.core.tracking.system.ndi): the NDI frame number, fitted against the
  read time with FrameCounterTimestamper. A tool that was not updated in the latest frame gets the
  time of its own frame number.
* IGSTK tracking: the same, using the frame number and time in the NDI sample info.
* OpenIGTLink (org.custusx.core.openigtlink3, tracking and video): the sender's timestamp in each
  message, converted to this computer's clock with SenderClockSynchronizer. There is one
  synchronizer per message type and device name, e.g. one for the images and one for the
  transforms of a device.
* OpenIGTLink in the older video plugin (IGTLinkClientStreamer, also used for the local video
  server): SenderClockSynchronizer, but a sender on this computer keeps its sent time, see below.
* OpenCV video: the V4L2 driver's frame time (CAP_PROP_POS_MSEC), converted from the monotonic
  clock to the system clock. The grab time is used if the driver time is missing, after the
  grab, or more than 1 s before the grab.
* The bkinterface streamserver timestamps the BK images (from the scanner's timestamps) and the
  NDI frames with the same frame counter fit as FrameCounterTimestamper, in the streamer
  computer's clock, and sends them over OpenIGTLink.

getMilliSecondsSinceEpoch() has sub-millisecond resolution (std::chrono::system_clock), which the
fits use.


Frame counter fit
-----------------------------------------------------------

FrameCounterTimestamper. A device counts its frames at a fixed rate. Each frame is read some
delay after it was acquired, and the delay is short most of the time:

    received time = offset + period * frame number + delay,  delay 0 or more

Plotted as received time against frame number, the frames lie on or above a line, with most of
them close to it. The fit finds this line along the lower edge of the points, over the last 10 s
of frames:

* The period is the slope of the lower convex hull at the median frame. Frames that arrive late,
  e.g. a burst of queued frames, lie above the hull and don't change it. The hull is found again
  without the earliest 5% of the arrivals, so that a few frames that arrive too early don't tilt it.
* The offset is the 5th percentile of (received time - period * frame number). A percentile
  instead of the minimum keeps a single early outlier from moving the line.
* The timestamp is offset + period * frame number, but not later than the receive time.
  Timestamps are kept increasing by at least 0.01 ms, which can make one slightly later than
  its receive time.
* Until there are 10 frames over 500 ms, or 10 frames once the period has been measured, the
  receive time is used.

The window slides, and the fit runs from when the device connects, not only during a recording.
A recording of several minutes, e.g. a 3D US acquisition, is timestamped from the fit at each
frame. The device clock and this computer's clock drift apart, typically 1 to 6 ms per minute,
which a sliding window follows, while one line fitted over minutes would not.

The frame period of a device is very stable, while queues of late frames can last for seconds,
e.g. after a depth change on a BK scanner. The period is therefore measured once and kept:

* It is measured when the window spans 5 s and is free of queued frames: the median arrival is
  less than 30 ms later than the 5th percentile arrival.
* After that, it follows the hull slope by 5% steps, as long as the slope is within 0.5% of it.
* If the slope stays different for 20 s, longer than any queue, the period has changed and is
  measured again.

The fit is restarted, keeping the measured period, when:

* the frame number is 0 or decreases (the device restarted),
* there are more than 1 s between two reads, or the clock of this computer steps back,
* a frame arrives more than 200 ms later than its frame number says. The device clock paused,
  e.g. a BK scanner while its depth is changed.

getTimestampOfEarlierFrame() gives the time of an earlier frame number from the last timestamp and
the period. NDI updates the port handles of some tools less often than every frame.


Sender clock synchronization
-----------------------------------------------------------

SenderClockSynchronizer. An OpenIGTLink sender stamps its messages with its own clock, which has
an unknown offset to this computer's clock:

    received time - sent time = clock offset + transfer delay

The smallest difference over the last 10 s is the clock offset plus the shortest transfer delay.
The timestamp is the sent time plus this smallest difference. This keeps the sender's own
timing, without the jitter of the network and the receiving thread, and is never later than the
receive time. The shortest transfer delay remains in the timestamp, and is part of what temporal
calibration measures. A sliding minimum is kept in a deque of increasing differences.

The receive time is used instead when:

* the sent time is 0: the sender has no timestamp,
* the sent time doesn't increase, or the message is more than 1 s later than the offset says,
  e.g. a frozen ultrasound scanner whose clock stops. NetworkHandler logs this once per clock.

If this lasts for 2 s, the synchronizer starts again with a new offset, e.g. after the sender's
clock was set.

Each message type and device has its own offset, so the offset of each includes its own
shortest transfer delay. For a sender that stamps video and tracking with the same clock, a
larger shortest delay for images than for transforms shifts the images later relative to the
tracking. Temporal calibration absorbs a constant shift like this.

A sender on this computer (localhost, a loopback address or one of this computer's own
addresses) uses the same clock, so IGTLinkClientStreamer keeps its sent time instead of adding
the shortest transfer delay. The synchronizer still runs, and its offset must fit the same clock:
0 to 100 ms, i.e. only the transfer delay. Otherwise the synchronized time is used, e.g. for a
tunnel to another computer, which also has a local address. A tunnel to a clock that is less
than 100 ms behind can't be told apart from a local sender. Late or repeated frames get the
receive time, as for other senders.


Limitations
-----------------------------------------------------------

* Frames that queue up for the whole window can't be detected: the line then lies along delayed
  frames, and the timestamps are late by the queue delay until the queue drains. A BK scanner
  queues frames for up to about 10 s after its depth is changed, so don't change the depth
  during a recording.
* After a restart of the fit, e.g. at a clock pause, the first 10 frames get the receive time.
* As old frames leave the window, the offset can move slightly. How much over a recording of
  several minutes has not been measured. The intervals between the timestamps in a recording's
  .tts and .tp files show it.
* With one offset per OpenIGTLink message type, see above, the difference in the shortest
  transfer delay of images and transforms from the same sender shifts them relative to each other.
* The device's own latency, e.g. image processing in the scanner, is not removed. Temporal
  calibration measures it.

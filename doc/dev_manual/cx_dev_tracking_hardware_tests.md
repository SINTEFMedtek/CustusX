Tracking hardware tests {#dev_tracking_hardware_tests}
===========================================================

Tests of the tracking systems (NDI, IGSTK, OpenIGTLink) with real NDI Aurora/Polaris
hardware. They are run by hand when the hardware is connected, never by the default test
runs (CustusX#55).

There are two ways to run them:

* From the GUI: Window, Debugging, Tracking Test. The tool configuration is the current one.
  See the Tracking Test widget in the user documentation.
* From the command line: the Catch tests tagged [tracking_hardware], configured with
  environment variables. These use a fixed tool configuration, given as a file.

Both record the tool positions to CSV files, and write a summary with the rate, mean position,
jitter (standard deviation) and drift of each tool. The GUI shows the progress and the summary,
the command line tests print the summary. The tracking systems are run one at a time: tracking
is stopped (and the OpenIGTLink server closed) before the next one starts, as they share the
device.

Tool configurations
-----------------------------------------------------------

The Laboratory profile has two tool configurations that can be used for these tests:

* LabDefault.xml: Polaris with the NDI reference frame and pointer.
* LabAuroraTools.xml: Aurora with the reference on port 1, a pointer on port 2, and a sensor
  without calibration on ports 3 and 4.

Any other tool configuration for the tools at hand works too. For an OpenIGTLink comparison, the
tools need the OpenIGTLink transform ids that the server sends.

Build
-----------------------------------------------------------

The NDI plugin is built whenever IGSTK is not. To compare NDI with IGSTK on the same
computer, build both, on a platform that still builds IGSTK (e.g. Ubuntu 20.04/22.04):

    ./install/cxInstaller.py --full --all --build_type Release --igstk --ndi_with_igstk

With both plugins, tool configurations with the igstk tracking system go to IGSTK, and those
with ndi go to the NDI plugin. Without the IGSTK plugin, the NDI plugin also handles igstk
configurations.

For OpenIGTLink, use an OpenIGTLink tracking server application that reads the same NDI device
and sends one TRANSFORM message per tool. If the server sends the positions in tracker space,
the comparison applies the reference tool in CustusX (openigtlinkApplyRefToTools, on by default).
The OpenIGTLink tools are found by the OpenIGTLink transform id in the tool files, which must
match the device names that the server sends (port_N also works for Aurora tools on port N).

Command line
-----------------------------------------------------------

Set the environment variables, then run the tests with Catch from the build folder:

    export CX_TRACKING_TEST_CONFIG=/path/to/tool/configuration.xml
    export CX_TRACKING_TEST_SYSTEMS=igstk,ndi,openigtlink
    export CX_TRACKING_TEST_IGTL_COMMAND=/path/to/openigtlink_tracking_server
    export CX_TRACKING_TEST_IGTL_ARGS="--hostname=/dev/ttyUSB0 --tools=ref.rom,pointer.rom"
    cd build_Release/bin
    ./Catch "[tracking_hardware]"

Or run one test at a time, e.g. ./Catch "Tracking hardware: compare*".

Environment variables:

* CX_TRACKING_TEST_CONFIG: the tool configuration file (required). Its tracking system is
  replaced for each run.
* CX_TRACKING_TEST_SYSTEMS: comma separated list of tracking systems to run. The default
  depends on the test.
* CX_TRACKING_TEST_DURATION_S: seconds to record for each system, default 10.
* CX_TRACKING_TEST_OUTPUT: output folder, default cx_tracking_tests in the home folder.
* CX_TRACKING_TEST_IGTL_COMMAND and CX_TRACKING_TEST_IGTL_ARGS: OpenIGTLink tracking server
  to start for the openigtlink run. Empty: the server must already be running.
* CX_TRACKING_TEST_IGTL_HOST and CX_TRACKING_TEST_IGTL_PORT: default 127.0.0.1 and 18944.
* CX_TRACKING_TEST_IGTL_APPLY_REF: 1 (default) applies the reference tool to the OpenIGTLink
  tools in CustusX. Use 0 for a server that sends positions relative to the reference.
* CX_TRACKING_TEST_MAX_DIFF_MM: the largest allowed difference between the mean positions of a
  tool in the first and the other systems in the comparison test, default 1.0.
* CX_TRACKING_TEST_REPEATS: the number of start/stop cycles in the restart test, default 5.
* CX_TRACKING_TEST_LONG_DURATION_S: the length of the long run, default 3600.
* CX_TRACKING_TEST_NDI_DEBUG: 1 turns on ndiTrackingDebugLogging, which writes each NDI command
  with its time to NdiCommunication.txt in the logging folder. It also asks a Polaris for the 3D
  markers in each frame, so the replies are larger. Needed in the tests, as they start with a new
  settings file.

The tests:

* Compare tracking systems on the same tools: each system in turn (default igstk, ndi,
  openigtlink). Keep the tools still: the mean position of each tool (except the reference) in
  each system is compared with the first system, and must be within CX_TRACKING_TEST_MAX_DIFF_MM.
  The jitter and drift of each system are in summary.txt.
* Start and stop tracking several times: connects, tracks for 3 s and disconnects, several
  times (default ndi).
* Long run: tracks for an hour (default ndi). Watch the memory use while it runs.
* Shut down while tracking, then track again: shuts CustusX down while tracking, and checks
  that the device can be used again afterwards. Uses the tracking system in the configuration.

Output
-----------------------------------------------------------

One folder per test run, with:

* N_system_positions.csv: one row per position, visibility change or position warning.
  The columns are label, tool, event, timestamp_ms (from the tracking system), received_ms
  (when CustusX got it), visible, x, y, z (tool tip in the reference space, mm), the rotation
  r00 to r22, the position warning, and the tracking system's metadata (for NDI the RMS error,
  port status, number of markers used, frame number and the time the frame was read).
* N_system_toolconfig.xml: the tool configuration used.
* N_system_command.log: the output of the OpenIGTLink server.
* summary.txt: OK or FAILED for each run, the statistics of each tool, and the differences in
  mean position from the first run.

Hardware checklist
-----------------------------------------------------------

For each device (Aurora, Polaris), connection (serial/USB, Ethernet) and OS, record the
device, firmware (the VER reply in the log when connecting), OS, commit, and pass/fail for
each item in CustusX#55. Set ndiTrackingDebugLogging=true in the settings file to log all
commands to NdiCommunication.txt.

* Compare tracking systems: the same tools with IGSTK, NDI and OpenIGTLink. The mean positions
  of a fixed tool should agree, and the rates should be similar.
* Start and stop tracking several times.
* Long run, at least an hour: memory use, rate, dropped frames.
* Shut down while tracking (the light and the full shutdown), and quit the application while
  tracking. No errors, and no tracking thread left running.
* Unplug the cable or power off the device while tracking: the log says that the contact with
  the device was lost, the tools become invisible, and tracking can be started again. After a
  power off, changing the tool configuration or quitting should not hang, and the log should not
  say that the tracker thread was terminated.
* Rate: NDI tracks an Aurora at 40 Hz and a Polaris Spectra at 60 Hz ("Tracking at 60 Hz" in
  the log), with even intervals between the positions in the CSV file. A Polaris that can't track
  at 60 Hz (e.g. a Vicra) tracks at 20 Hz, with a warning.
* Baud rate: NdiCommunication.txt shows that COMM succeeded at 921600 baud. With
  ndiTrackingBaudRate set to a rate that isn't supported (e.g. 1228739), a slower rate is used.
* With Record positions in the Tracking Test widget: move a tool in and out of the measurement
  volume, to and from its border, and (Polaris) hide one sphere, partly or completely. The tool
  should become orange (position warning) at the border and with a hidden sphere, and red when it
  is out of view. Occlude the reference: all tools should go red, or orange when the reference
  has a warning. Tune ndiTrackingMaxErrorRMS from the recorded RMS errors if needed.
* Device in use (network): with another application (e.g. PlusServer) tracking with the device, start
  NDI tracking in CustusX. CustusX should refuse with "already tracking" in the log, and the other
  application should keep tracking. With ndiTrackingTakeOverDevice=true, CustusX connects.
* Device in use (serial/USB): the same with the device on a serial or USB port. The port should fail
  to open, and the other application should keep tracking.
* OpenIGTLink fallback: with an NDI tool configuration, and an OpenIGTLink server (e.g. PlusServer)
  using the device and sending the tool positions, Start Tracking should track the tools from the
  server ("Using the tool positions it sends over OpenIGTLink" in the log). Stop the server and start
  tracking again: the NDI plugin should connect to the device, and the server should not be used.
* Polaris marker count, with ndiTrackingDebugLogging=true: the "NDI Tracker debug" lines in the
  log show the 3D markers seen and the markers used for each tool. Use a 4-marker tool: all
  spheres visible, one sphere hidden, a sphere hidden or removed before tracking starts, and the
  tool hidden. The tool should be orange whenever fewer markers than in its .rom file are used,
  also when a sphere is hidden from the start. A tool with markers on several faces is always
  orange, as the .rom file gives the markers of all faces (e.g. "4 of 8 markers used").
* Aurora: plug a tool into a different port than in its tool file, and check that it is
  tracked, with a warning in the log that names the right ports.
* Aurora: unplug a sensor while tracking and plug it in again, then stop and start tracking: the
  tool is tracked again. Unplug a sensor, then stop and start tracking: the tool is shown as not
  initialized.
* OpenIGTLink: Start Tracking connects to the server and Stop Tracking disconnects, several times.
* Change the tool configuration in the Preferences while NDI is configured but not tracking: the
  new configuration's tools are used when tracking starts.
* A tool configuration with a tool file that is missing (e.g. renamed while CustusX runs): a
  warning when the configuration is saved or tracking starts, and no crash.
* An existing profile gets the shipped tool configurations it is missing.
* Windows: a device on COM10 or higher is found.

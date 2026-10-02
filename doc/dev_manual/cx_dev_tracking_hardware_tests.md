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

Both record the tool positions to CSV files, and write a summary with the rate, mean position
and spread of each tool.

Build
-----------------------------------------------------------

The NDI plugin is built whenever IGSTK is not. To compare NDI with IGSTK on the same
computer, build both, on a platform that still builds IGSTK (e.g. Ubuntu 20.04/22.04):

    ./install/cxInstaller.py --full --all --build_type Release --igstk --ndi_with_igstk

With both plugins, tool configurations with the igstk tracking system go to IGSTK, and those
with ndi go to the NDI plugin. Without the IGSTK plugin, the NDI plugin also handles igstk
configurations.

For OpenIGTLink, build bkinterface (holosurge) next to CustusX, and use its ndistreamer
application as the OpenIGTLink tracking server. ndistreamer streams the positions in tracker
space, so the comparison applies the reference tool in CustusX (openigtlinkApplyRefToTools).
The OpenIGTLink tools are found by the OpenIGTLink transform id in the tool files, which must
match the device names that ndistreamer sends (the names in its configuration file, or port_N
for Aurora).

Command line
-----------------------------------------------------------

Set the environment variables, then run the tests with Catch from the build folder:

    export CX_TRACKING_TEST_CONFIG=/path/to/tool/configuration.xml
    export CX_TRACKING_TEST_SYSTEMS=igstk,ndi,openigtlink
    export CX_TRACKING_TEST_IGTL_COMMAND=/path/to/bkinterface/build/apps/ndistreamer
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
* CX_TRACKING_TEST_REPEATS: the number of start/stop cycles in the restart test, default 5.
* CX_TRACKING_TEST_LONG_DURATION_S: the length of the long run, default 3600.

The tests:

* Compare tracking systems on the same tools: each system in turn (default igstk, ndi,
  openigtlink). Keep the tools still, and compare the mean positions in summary.txt.
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
  port status and number of markers used).
* N_system_toolconfig.xml: the tool configuration used.
* N_system_command.log: the output of the OpenIGTLink server.
* summary.txt: OK or FAILED for each run, and the statistics of each tool.

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
  the device was lost, the tools become invisible, and tracking can be started again.
* With Record positions in the Tracking Test widget: move a tool in and out of the measurement
  volume, to and from its border, and (Polaris) hide one sphere, partly or completely. The tool
  should become orange (position warning) at the border and with a hidden sphere, and red when it
  is out of view. Occlude the reference: all tools should go red, or orange when the reference
  has a warning. Tune ndiTrackingMaxErrorRMS from the recorded RMS errors if needed.
* Aurora: plug a tool into a different port than in its tool file, and check that it is
  tracked, with a warning in the log that names the right ports.

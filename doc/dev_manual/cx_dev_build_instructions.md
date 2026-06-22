Build instructions {#build_instructions}
===================

Recommended build procedure {#build_instructions2}
===================

Make sure you:
* Are on a \ref supported_platforms.
* Fulfill the \ref prerequisites.
* Have [Git](https://git-scm.com/) installed.

Choose a root folder for the project, e.g. `~/dev/cx`. It will be populated as described in \ref build_instructions_folder_structure.

Then run the following commands:

    mkdir dev
    mkdir dev/cx
    cd dev/cx
    git clone https://gitlab.sintef.no/custusx/CustusX.git CX/CX
    cd CX/CX
    (git checkout develop)
    ./install/cxInstaller.py --full --all --build_type Release --user_doc


Run `cxInstaller.py -h` for a list of the components in CustusX to build, and an explanation of the input arguments.
The `--full` argument is a combination of the following arguments:

 * `--checkout:` download the source code repositories of the selected components.
 * `--configure:` configure and run CMake to generate build files.
 * `--make:` build the selected components.

The `--all` argument selects all components for building. After having run the script successfully, you might want to
drop this argument and instead list the components you want to run commands on, primarily *CustusX*.

Prerequisites {#prerequisites}
------------------------

The following software must be installed prior to building *CustusX*.

Several other libraries (see \ref external_libraries) are part of the \ref dev_superbuild,
and thus does not need to be installed separately.

For convenience, setup scripts for some platforms are available in the
repository. They will help setup a machine from scratch, but might not be fully updated or give
you more than you expected. Look for your platform in
[install/platforms](https://gitlab.sintef.no/custusx/CustusX/-/tree/develop/install/platforms).

### Linux

On Ubuntu, most required packages can be installed via the package manager.
See the platform setup scripts in
[install/platforms](https://gitlab.sintef.no/custusx/CustusX/-/tree/develop/install/platforms)
for the complete list.

**Python and Git**: Make sure that *Python* and *Git* are installed and available on the command line.

**Cuda**: You might need to install *Cuda* to get *OpenCL* working. It is
recommended to download and use the regular installer from the web site.

### Practical information

To start writing code, open the file

    root_dir/CX/CX/CMakeLists.txt

in *Qt Creator.* Make sure that the build folder(s) for your selected configuration(s) (Debug/Release)
matches the build folder(s) in your build tree.

You might need to rerun the build script and CMake to get everything working.

## Running the tests

The test suite of *CustusX* can be run through the executable *Catch*, which is built as part of the
superbuild. Run it with the `-h` argument to see the options. To run a specific test:

    ./CX/build_Release/bin/Catch "test name"

Note that the test name must be in quotes. Tests are tagged. To run all the unit tests:

    ./CX/build_Release/bin/Catch [unit]~[hide]~[unstable]~[not_linux]

This will run all unit tests, but not tests tagged as hidden, unstable or not for Linux.
Run with the `-t` argument to list all the tags including the not-tags for other platforms, and `-l` to list all the tests.

The integration tests are generally more complex and should be run in separate processes.
We use a script for that:

    ./CX/install/cxRunTests.py --run_catch --wrap_in_ctest [integration]~[unstable]~[hide]~[not_linux]

Several tests uses data in the *CustusX-Data* component which is downloaded as part of the superbuild
as the *data* folder in the *CX/CX* folder.
Some tests might depend on external applications, like *Vlc*, being installed. Other tests might
depend on graphics drivers or other hardware. The complexity of the test suite means that you might
have failing tests, but it should generally not be a big concern if you are on one of the
listed \ref supported_platforms.

Some notes about running the tests on an installed (i.e. not built) version of *CustusX*:

 * At the time, some relative paths from the build machine excists in the *Catch* executable
   This means that you need to put the *data* repo in the corresponding location.
 * The script `cxRunTests.py` only works if you have installed *CustusX* in the same folder
   relative to the script, as if it was a built version. Also, the script doesn't work
   on a debug installation.

## Superbuild Folder Structure {#build_instructions_folder_structure}

The default CustusX folder structure differs from the standard CMake source+build
structure. All libraries, CustusX included, are placed within a root folder,
with source and build folders grouped according to library.

The CustusX \ref dev_superbuild defines and sets up this structure. It is fully
possible to use a different structure, in that case you must configure cmake
yourself.

|        |          |                |
| ------ | ----     | -------------- |
| root   | CX       | CX             |
|        |          | build_Release  |
|        | VTK      | VTK            |
|        |          | build_Release  |
|        | CTK      | CTK            |
|        |          | build_Release  |
|        | some_lib | some_lib       |
|        |          | build_Release  |

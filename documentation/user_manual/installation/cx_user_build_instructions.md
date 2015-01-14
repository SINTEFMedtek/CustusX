Build instructions {#build_instructions}
===================

CustusX runs on these [Supported Platforms](cx_user_supported_platforms.md).

## Prerequisites 

 * [CMake](http://www.cmake.org/)
 * [Qt5](http://qt-project.org/)
 * [openCL](https://www.khronos.org/opencl/)
 * [boost](http://www.boost.org/)
 * [doxygen](http://www.doxygen.org/)
 * [cppunit](http://sourceforge.net/projects/cppunit/)
 * ... 

These must be installed prior to installing CustusX.

CustusX depends on a lot of other libraries, which is part of the \superbuild,
and thus does not need to be installed separately. 

For convenience, setup scripts for some platforms are available in the 
repository. They will help setup a machine from scratch, but might give 
you more than you expected. Look for your platform in 
[install/platforms](install/platforms).

## Superbuild {#superbuild}

CustusX uses a Python-based superbuild. Select a root folder, then all libs 
will be downloaded and built in separate folders beneath it, as follows:

|        |          |                |
| ------ | ----     | -------------- |
| root   | CustusX  | CustusX        |
|        |          | build_Release  |
|        | VTK      | VTK            |
|        |          | build_Release  |
|        | CTK      | CTK            |
|        |          | build_Release  |
|        | some_lib | some_lib       |
|        |          | build_Release  |

Run the following lines to get it right:
```bash
cd <root_dir>
git clone git@github.com:SINTEFMedisinskTeknologi/CustusX.git CustusX/CustusX
./CustusX/CustusX/install/cxInstaller.py --root_dir . --full --all
```
The default root_dir is ~/dev/cx. Run cxInstaller.py -h for more options.

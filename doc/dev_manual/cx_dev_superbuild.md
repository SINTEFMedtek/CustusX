Superbuild {#dev_superbuild}
===========================================================

Superbuild
-----------------------------------------------------------

CustusX depends on several other libraries, and those libraries must be of 
specific versions. The superbuild handles these issues. It is implemented in 
Python, and works on top of CMake. It downloads, configures and builds CustusX 
and the libraries it 	depends upon. 

Structure: 

    Python (superbuild)
    CMake (configure each library)
    C++ (application code)

The build scripts can be found in the folder `install`:

    cxInstaller.py
    cxJenkins_job_step.py
    cxJenkinsMasterBuild.py

The `cxInstaller.py` can be used to build the system, while the other scripts 
are used for continuous integration and testing tasks. The scripts uses the 
Python library `install/cx`. 

Some important modules:

Name                            | Description
------------------------------- | -----------------------------------------------------
`cx.build.cxInstallData`        | Definitions and settings 
`cx.build.cxComponents`         | Contains one class for each library in the superbuild
`cx.build.cxComponentsAssembly` | Assembles all libraries



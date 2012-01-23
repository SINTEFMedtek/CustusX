// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

namespace ssc
{

/**\namespace ssc
 * \brief An IGT library used by Sonowand and Sintef Medical Technology.
 *
 */

/**
 * \defgroup sscGroup SSC
 * \brief An IGT library used by Sonowand and Sintef Medical Technology.
 *
 */

/**
 * \defgroup sscUtility SSC Utility
 * \brief Defines and miscellaneous utilities for the other parts of SSC.
 * \ingroup sscGroup
 *
 */

/**
 * \defgroup sscWidget SSC Widget
 * \brief Widget collection.
 * \ingroup sscGroup
 *
 * Much of the \ref sscWidget module is built around DataAdapter. Start there.
 *
 */

/**
 * \defgroup sscMath SSC Math
 * \brief The SSC Math library, based on Eigen.
 * \ingroup sscGroup
 *
 * The math in SSC should preferrably be done in terms of Eigen, http://eigen.tuxfamily.org/dox/
 *
 * The core classes Vector3D and Transform3D have been reduced to typedefs of
 * Eigen structures.
 *
 */

/**
 * \defgroup sscData SSC Data
 * \brief A model of the patient.
 * \ingroup sscGroup
 *
 * Data sets of various modalities, fiducials and labels are described in relation to each other.
 *
 * \image html sscArchitecture_data.png "Basic relation between the Data interfaces and classes."
 *
 */

/**
 * \defgroup sscTool SSC Tool
 * \brief Tool related interfaces and data classes.
 * \ingroup sscGroup
 *
 * \image html sscArchitecture_tracking.png "Basic relation between the Tool interfaces."
 *
 *
 */

/**
 * \defgroup sscVideo SSC Video
 * \brief Video related interfaces and data classes.
 * \ingroup sscGroup
 *
 */

/**
 * \defgroup sscRep SSC Rep
 * \brief Representations that visualize Data entities in a View.
 * \ingroup sscGroup
 *
 * Visualization of underlying structures assembled in Representations that are
 * easily configurable and insertible into views. View widgets that can be added
 * directly to a Qt layout.
 *
 * The representations are the core part of ssc. Each Rep contains one kind of
 * visualization. The point is to hide the vtk/OpenGL complexity and create a
 * toolkit where Data, Tools, Reps and Views can be assembled.
 *
 */



/** \defgroup sscProxy SSC Proxy
 *  \brief Helper classes for Rep subclasses.
 *
 *  \ingroup sscRep
 *
 *  SSC Proxy is a subgroup of \ref sscRep
 *
 *  In many cases, several reps share functionality. Instead of creating a complex
 *  inheritance hierarchy, this functionality is collected into reusable classes
 *  called proxies. A Rep will typically consist of proxies that provide reusable
 *  behaviour, and internal logic that is unique to that Rep. See below for examples.
 */

/** \defgroup sscRep2D SSC 2D Rep
 *  \brief All Rep subclasses that display in 2D views.
 *
 *  \ingroup sscRep
 *
 *  SSC 2D Rep is a subgroup of \ref sscRep
 *
 */

/** \defgroup sscRep3D SSC 3D Rep
 *  \brief All Rep subclasses that display in 3D views.
 *
 *  \ingroup sscRep
 *
 *  SSC 3D Rep is a subgroup of \ref sscRep
 *
 */

/** \defgroup sscRepVideo SSC Video Rep
 *  \brief All Rep subclasses that display in Video views.
 *
 *  \ingroup sscRep
 *
 *  SSC Video Rep is a subgroup of \ref sscRep
 *
 */

/** \defgroup sscNotUsed SSC Not used
 *  \brief All classes that seem to be unused in real code.
 *  \ingroup sscGroup
 *
 * Everything in this group are candidates for removal.
 *
 */


} // namespace ssc


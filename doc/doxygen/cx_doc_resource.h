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

namespace cx
{

/**
 * \defgroup cxResource Resource Layer
 * \brief A collection of common utility classes available to everyone.
 *
 *  - This is classes that can be used by the entire system. Utility classes and
 *    functions of various kinds.
 *  - A geometrical math library, part of SSC, using Eigen as the engine
 *    (ssc::Vector3D, ssc::Transform3D etc).
 *  - Interfaces for common objects: ssc::Data, ssc::Image, ssc::Mesh,
 *    ssc::VideoSource, ssc::Tool, ...
 *  - Visualization components (ssc::Rep)
 *  - A thread-safe logging service (ssc::MessageManager). Singleton.
 *  - Persistent state, stored in ini files and xml files. Interface classes
 *    to these (cx::Settings, cx::DataLocations, ssc::XmlOptionFile).
 *
 */

/**
 * \defgroup cxResourceAlgorithms Algorithm Resource
 * \ingroup cxResource
 * \brief In-house algorithms and adapters around external ones.
 *
 */

/**
 * \defgroup cxResourceSettings Settings Resource
 * \ingroup cxResource
 * \brief Settings and file system locations.
 *
 */

/**
 * \defgroup cxResourceUtilities Utilities Resource
 * \ingroup cxResource
 * \brief Reusable stateless building blocks
 *
 */

} // namespace ssc


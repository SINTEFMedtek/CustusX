#ifndef SSC_H_
#define SSC_H_

namespace ssc
{

/**
 * \mainpage
 * \brief An IGT library used by Sonowand and Sintef Medical Technology.
 *
 *
 * The <a href="modules.html">module list</a> is a recommended starting point.
 *
 * ssc is short for Sonowand / Sintef Medical Technology Cooperation.
 *
 * The library contains a common set of interfaces that enables the
 * two companies to share software components.
 */


/**\namespace ssc
 * \brief An IGT library used by Sonowand and Sintef Medical Technology.
 *
 * ssc is short for Sonowand / Sintef Medical Technology Cooperation.
 *
 * The library contains a common set of interfaces that enables the
 * two companies to share software components.
 *
 * The library is based on vtk and Qt, and uses code standards
 * defined by Sonowand.
 *
 * Definition of all coordinate systems: TBD
 */


/**
 * \defgroup sscUtility SSC Utility
 * \brief Defines and miscellaneous utilities for the other parts of SSC.
 *
 */


/**
 * \defgroup sscData SSC Data
 * \brief A model of the patient.
 *
 * Data sets of various modalities, fiducials and labels are described in relation to each other.
 *
 *
 */


/**
 * \defgroup sscRep SSC Rep
 * \brief Representations that visualize Data entities in a View.
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




} // namespace ssc

#endif /*SSC_H_*/

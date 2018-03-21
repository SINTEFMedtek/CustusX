/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#ifndef CXLOGGER_H_
#define CXLOGGER_H_

#include "cxResourceExport.h"
#include "cxDefinitions.h"
#include "cxTypeConversions.h"
#include "boost/shared_ptr.hpp"

/**
* \file
*
* The file cxLogger contains logging functions and macros.
*
 * \addtogroup cx_resource_core_logger
*
* @{
*/

namespace cx
{

/** Helper class for logging to a cx::Reporter
 *
 * Based on the Qt logger classes QDebug and QMessageLogger
 */
class cxResource_EXPORT MessageLogger
{
public:
	MessageLogger(const char *file, int line, const char *function, const QString& channel, MESSAGE_LEVEL severity);
	~MessageLogger();

	MessageLogger logger() const;
	MessageLogger logger(QString text) const;

	template <class T>
	MessageLogger& operator<<(T value)
	{
		this->getStream() << value;
		return *this;
	}

private:
	boost::shared_ptr<class MessageLoggerInternalData> mInternalData;
	std::stringstream& getStream();
};

//template <class T>
//cxResource_EXPORT MessageLogger& operator<<(MessageLogger& logger, const T& value)
//{
//	logger << value;
//	return logger;
//}

cxResource_EXPORT void reportDebug(QString msg);
cxResource_EXPORT void report(QString msg);
cxResource_EXPORT void reportWarning(QString msg);
cxResource_EXPORT void reportError(QString msg);
cxResource_EXPORT void reportSuccess(QString msg);
cxResource_EXPORT void reportVolatile(QString msg);

} //namespace cx


#define CX_LOG_SEVERITY_INTERNAL(severity) \
	cx::MessageLogger(__FILE__, __LINE__, Q_FUNC_INFO, "console", severity)

#define CX_LOG_CHANNEL_SEVERITY_INTERNAL(channel, severity) \
	cx::MessageLogger(__FILE__, __LINE__, Q_FUNC_INFO, channel, severity)


/** Macro for creating debug marks in the code.
  *	Usage: One of the following:
  *		CX_LOG_DEBUG_CHECKPOINT();
  *		CX_LOG_DEBUG_CHECKPOINT() << "Message " << "to output";
  */
#define CX_LOG_DEBUG_CHECKPOINT() CX_LOG_CHANNEL_SEVERITY_INTERNAL("checkpoints", cx::mlDEBUG).logger() << "CHECKPOINT: " << __FILE__ << " at " << __LINE__ << " in " << Q_FUNC_INFO << " : "

/** Log to the default channel
 * Usage: One of the following:
 *          CX_LOG_INFO("Message to output");
 *          CX_LOG_INFO() << "Message " << "to output";
 */
#define CX_LOG_DEBUG   CX_LOG_SEVERITY_INTERNAL(cx::mlDEBUG).logger
#define CX_LOG_INFO    CX_LOG_SEVERITY_INTERNAL(cx::mlINFO).logger
#define CX_LOG_SUCCESS CX_LOG_SEVERITY_INTERNAL(cx::mlSUCCESS).logger
#define CX_LOG_WARNING CX_LOG_SEVERITY_INTERNAL(cx::mlWARNING).logger
#define CX_LOG_ERROR   CX_LOG_SEVERITY_INTERNAL(cx::mlERROR).logger
#define CX_LOG_VOLATILE   CX_LOG_SEVERITY_INTERNAL(cx::mlVOLATILE).logger

/** Log to a named channel
 * Usage:
 *          CX_LOG_CHANNEL_INFO("channel_name") << "Message " << "to output";
 * Will be written to the file channel_name.txt
 */
#define CX_LOG_CHANNEL_DEBUG(channel)   CX_LOG_CHANNEL_SEVERITY_INTERNAL(channel, cx::mlDEBUG).logger()
#define CX_LOG_CHANNEL_INFO(channel)    CX_LOG_CHANNEL_SEVERITY_INTERNAL(channel, cx::mlINFO).logger()
#define CX_LOG_CHANNEL_SUCCESS(channel) CX_LOG_CHANNEL_SEVERITY_INTERNAL(channel, cx::mlSUCCESS).logger()
#define CX_LOG_CHANNEL_WARNING(channel) CX_LOG_CHANNEL_SEVERITY_INTERNAL(channel, cx::mlWARNING).logger()
#define CX_LOG_CHANNEL_ERROR(channel)   CX_LOG_CHANNEL_SEVERITY_INTERNAL(channel, cx::mlERROR).logger()
#define CX_LOG_CHANNEL_VOLATILE(channel) CX_LOG_CHANNEL_SEVERITY_INTERNAL(channel, cx::mlVOLATILE).logger()

/**
  */
#define CX_ASSERT(statement)			\
{										\
	if (!(statement))					\
	{									\
		CX_LOG_ERROR("Assert failure!");\
	}									\
}

/**
 * @}
 */

#endif

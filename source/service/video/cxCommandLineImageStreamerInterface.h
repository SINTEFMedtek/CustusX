// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.
#ifndef CXCOMMANDLINEIMAGESTREAMERINTERFACE_H
#define CXCOMMANDLINEIMAGESTREAMERINTERFACE_H

#include <map>
#include <QString>
#include "cxImageStreamerInterface.h"

namespace cx
{

typedef boost::shared_ptr<class CommandlineImageStreamerInterface> CommandlineImageStreamerInterfacePtr;
/**
 * \brief Interface to Image streamer with command line based interface
 *
 * \ingroup cx_service_video
 *
 * \date May 20, 2014
 * \author Ole Vegard Solberg, SINTEF
 */
class CommandlineImageStreamerInterface : public ImageStreamerInterface
{
public:
	CommandlineImageStreamerInterface();
	virtual StreamerPtr createStreamer();

	void setArguments(std::map<QString, QString> arguments) {mArguments = arguments;}

	private:
		std::map<QString, QString> mArguments;
};

} //end namespace cx
#endif // CXCOMMANDLINEIMAGESTREAMERINTERFACE_H

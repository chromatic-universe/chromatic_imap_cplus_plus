#ifndef __COMMON_H__
#define __COMMON_H__

#include <dds/DdsDcpsPublicationC.h>
#include <dds/DdsDcpsInfrastructureC.h>
#include <dds/DCPS/Service_Participant.h>
#include <dds/DCPS/Marked_Default_Qos.h>
#include <dds/DCPS/WaitSet.h>
#include <dds/DCPS/transport/framework/TheTransportFactory.h>
#include <map>
#include <string>
#include <exception>
#include <vcclr.h>

#include "StringConvert.h"
#include "Reader.h"
#include "Writer.h"
#include "DataReaderListenerImplBase.h"
#include "DDSBase.h"
#include "EntityMap.h"
#include "NetException.h"

EXPORT_COMMON void GetArguments(int &argc, char **&argv);

#endif
#pragma warning( disable: 4786 4800 4365 )

#ifndef OEMO_PROTOCOL_THREAD
#define OEMO_PROTOCOL_THREAD


#include "ace/Svc_Handler.h"
#include "ace/Date_Time.h"
#include "ace/Event.h"
#include "ace/High_Res_Timer.h"

#include "OemoImapHandler.h"
#include <iostream>

namespace OemoImapProtocolImpl
{
	
	//thread
	unsigned long __stdcall OemoProtocolThreadProc( VoidPtr ptrVoid );
}

#endif

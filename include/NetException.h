#ifndef __NETEXCEPTION_H__
#define __NETEXCEPTION_H__

#define NET_RETHROW_EXCEPTION(x) \
	try { \
		x; \
	} \
	catch(std::exception &e) { \
		throw gcnew DDSNetException(gcnew System::String(e.what())); \
	}\



#endif
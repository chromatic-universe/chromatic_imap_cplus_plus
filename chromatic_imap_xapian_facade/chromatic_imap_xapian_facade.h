/*
 * chromatic_imap_xapian_facade.h
 *
 *  Created on: Jun 28, 2013
 *      Author: wiljoh
 */

#ifndef CHROMATIC_IMAP_XAPIAN_FACADE_H_
#define CHROMATIC_IMAP_XAPIAN_FACADE_H_

#include <tuple>
#include <stdexcept>
#include <exception>
#include <system_error>
#include <new>
#include <ios>
#include <future>
#include <typeinfo>
#include <iostream>
#include <utility>
#include <string>
#include <memory>
#include <type_traits>
#include <limits>
#include <algorithm>
#include <chrono>
#include <ratio>
#include <list>
#include <vector>
#include <map>
#include <set>
#include <functional>
#include <sstream>
#include <thread>

//c runtime std namepsace
#include <cstdio>
#include <cerrno>
#include <cctype>
#include <csignal>
#include <cstdlib>
#include <cstddef>
#include <cstring>
#include <ctime>
#include <climits>
#include <cassert>
//c runtime
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

//adaptive communication environment
#include <ace/Task.h>
#include <ace/Atomic_Op.h>
#include <ace/Acceptor.h>
#include <ace/Svc_Handler.h>
#include <ace/Date_Time.h>
#include <ace/Mutex.h>
#include <ace/OS.h>
#include <ace/Get_Opt.h>
#include <ace/streams.h>
#include <ace/OS_NS_errno.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/Trace.h>
#include <ace/Timer_Queue_T.h>

//xapian
#include <xapian.h>

//hand rolled
//generic
#include <generic.h>

namespace chromatic_imap_xapian_facade
{
	//
}
namespace cixf = chromatic_imap_xapian_facade;

#endif //CHROMATIC_IMAP_XAPIAN_FACADE_H_

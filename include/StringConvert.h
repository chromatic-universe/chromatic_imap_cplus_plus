#ifndef __STRINGCONVERT_H__
#define __STRINGCONVERT_H__

#include "Def.h"
#include <vcclr.h>
#include <string>
#include <tao/String_Manager_T.h>

EXPORT_COMMON char *ConvertToASCII(gcroot<System::String^> netString);
EXPORT_COMMON std::string ConvertToString(gcroot<System::String^> netString);
EXPORT_COMMON TAO::String_Manager Convert(gcroot<System::String^> netString);
EXPORT_COMMON gcroot<System::String^> Convert(TAO::String_Manager corbaString);

#endif

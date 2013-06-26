
//============================================================================
// Name        : ace_imap.cpp
// Author      : Wlliam K. Johnson
// Version     :
// Copyright   : copyright Willliam K. Johnson 2013
//============================================================================

//suppress from adaptive communication environment
#pragma clang diagnostic ignored "-Wself-assign"
#pragma clang diagnostic ignored "-Wconstant-logical-operand"

#include "chromatic_imap.h"

int ACE_TMAIN ( int argc , ACE_TCHAR* argv[] )
{
  return 0L;
}

#if defined (ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION)
template class ACE_Singleton<Process, ACE_Mutex>;
#elif defined (ACE_HAS_TEMPLATE_INSTANTIATION_PRAGMA)
#pragma instantiate ACE_Singleton<Process, ACE_Mutex>
#endif /* ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION */





/**
 * $Id: HA_Status_Static.h 14 2007-02-01 15:49:12Z mitza $
 *
 * Home Automation Status server. Sample code from The ACE Programmer's Guide,
 * copyright 2003 Addison-Wesley. All Rights Reserved.
 */

#ifndef __CHROMATIC_IMAP_DAEMON_H
#define __CHROMATIC_IMAP_DAEMON_H


#include "ace/OS.h"
#include "ace/Acceptor.h"
#include "ace/INET_Addr.h"
#include "ace/SOCK_Stream.h"
#include "ace/SOCK_Acceptor.h"
#include "ace/Service_Object.h"
#include "ace/Svc_Handler.h"
#include "ace/Service_Config.h"

#include "chromatic_imap.h"


class chromatic_imap_daemon : public ACE_Service_Object
{ 

  private:
    
    ACE_Acceptor<chromatic_imap_protocol_impl::chromatic_handler , ACE_SOCK_ACCEPTOR> acceptor_;
    ACE_INET_Addr listen_addr_;
    
  public:
    
      virtual int init ( int argc, ACE_TCHAR *argv[] );
      virtual int fini ( void );
      virtual int info ( ACE_TCHAR **str, size_t len ) const;
};

#endif //CHROMATIC_IMAP_DAEMON_H

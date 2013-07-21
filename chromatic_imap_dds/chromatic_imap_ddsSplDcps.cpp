#include "chromatic_imap_ddsSplDcps.h"
#include "ccpp_chromatic_imap_dds.h"

const char *
__chromatic_imap_dds_chromatic_imap_command__name(void)
{
    return (const char*)"chromatic_imap_dds::chromatic_imap_command";
}

const char *
__chromatic_imap_dds_chromatic_imap_command__keys(void)
{
    return (const char*)"session_idx";
}

#include <v_kernel.h>
#include <v_topic.h>
#include <os_stdlib.h>
#include <string.h>
#include <os_report.h>

c_bool
__chromatic_imap_dds_chromatic_imap_command__copyIn(
    c_base base,
    struct ::chromatic_imap_dds::chromatic_imap_command *from,
    struct _chromatic_imap_dds_chromatic_imap_command *to)
{
    c_bool result = OS_C_TRUE;
    (void) base;

    to->session_idx = (c_long)from->session_idx;
    to->locale_idx = (c_long)from->locale_idx;
#ifdef OSPL_BOUNDS_CHECK
    if(from->moniker){
        to->moniker = c_stringNew(base, from->moniker);
    } else {
        OS_REPORT (OS_ERROR, "copyIn", 0,"Member 'chromatic_imap_dds::chromatic_imap_command.moniker' of type 'c_string' is NULL.");
        result = OS_C_FALSE;
    }
#else
    to->moniker = c_stringNew(base, from->moniker);
#endif
#ifdef OSPL_BOUNDS_CHECK
    if(from->command){
        to->command = c_stringNew(base, from->command);
    } else {
        OS_REPORT (OS_ERROR, "copyIn", 0,"Member 'chromatic_imap_dds::chromatic_imap_command.command' of type 'c_string' is NULL.");
        result = OS_C_FALSE;
    }
#else
    to->command = c_stringNew(base, from->command);
#endif
#ifdef OSPL_BOUNDS_CHECK
    if(from->payload){
        to->payload = c_stringNew(base, from->payload);
    } else {
        OS_REPORT (OS_ERROR, "copyIn", 0,"Member 'chromatic_imap_dds::chromatic_imap_command.payload' of type 'c_string' is NULL.");
        result = OS_C_FALSE;
    }
#else
    to->payload = c_stringNew(base, from->payload);
#endif
    return result;
}

void
__chromatic_imap_dds_chromatic_imap_command__copyOut(
    void *_from,
    void *_to)
{
    struct _chromatic_imap_dds_chromatic_imap_command *from = (struct _chromatic_imap_dds_chromatic_imap_command *)_from;
    struct ::chromatic_imap_dds::chromatic_imap_command *to = (struct ::chromatic_imap_dds::chromatic_imap_command *)_to;
    to->session_idx = (::DDS::Long)from->session_idx;
    to->locale_idx = (::DDS::Long)from->locale_idx;
    to->moniker = CORBA::string_dup(from->moniker);
    to->command = CORBA::string_dup(from->command);
    to->payload = CORBA::string_dup(from->payload);
}


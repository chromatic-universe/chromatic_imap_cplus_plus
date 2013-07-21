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

const char *
__chromatic_imap_dds_chromatic_name_service__name(void)
{
    return (const char*)"chromatic_imap_dds::chromatic_name_service";
}

const char *
__chromatic_imap_dds_chromatic_name_service__keys(void)
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
        if(((unsigned int)strlen(from->command)) <= 32){
            to->command = c_stringNew(base, from->command);
        } else {
            OS_REPORT (OS_ERROR, "copyIn", 0,"Member 'chromatic_imap_dds::chromatic_imap_command.command' of type 'C_STRING<32>' is out of range.");
            result = OS_C_FALSE;
        }
    } else {
        OS_REPORT (OS_ERROR, "copyIn", 0,"Member 'chromatic_imap_dds::chromatic_imap_command.command' of type 'C_STRING<32>' is NULL.");
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

c_bool
__chromatic_imap_dds_chromatic_name_service__copyIn(
    c_base base,
    struct ::chromatic_imap_dds::chromatic_name_service *from,
    struct _chromatic_imap_dds_chromatic_name_service *to)
{
    c_bool result = OS_C_TRUE;
    (void) base;

    to->session_idx = (c_long)from->session_idx;
#ifdef OSPL_BOUNDS_CHECK
    if(from->user_name){
        if(((unsigned int)strlen(from->user_name)) <= 32){
            to->user_name = c_stringNew(base, from->user_name);
        } else {
            OS_REPORT (OS_ERROR, "copyIn", 0,"Member 'chromatic_imap_dds::chromatic_name_service.user_name' of type 'C_STRING<32>' is out of range.");
            result = OS_C_FALSE;
        }
    } else {
        OS_REPORT (OS_ERROR, "copyIn", 0,"Member 'chromatic_imap_dds::chromatic_name_service.user_name' of type 'C_STRING<32>' is NULL.");
        result = OS_C_FALSE;
    }
#else
    to->user_name = c_stringNew(base, from->user_name);
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

void
__chromatic_imap_dds_chromatic_name_service__copyOut(
    void *_from,
    void *_to)
{
    struct _chromatic_imap_dds_chromatic_name_service *from = (struct _chromatic_imap_dds_chromatic_name_service *)_from;
    struct ::chromatic_imap_dds::chromatic_name_service *to = (struct ::chromatic_imap_dds::chromatic_name_service *)_to;
    to->session_idx = (::DDS::Long)from->session_idx;
    to->user_name = CORBA::string_dup(from->user_name);
}


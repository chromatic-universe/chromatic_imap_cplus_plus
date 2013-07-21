#ifndef CHROMATIC_IMAP_DDSSPLTYPES_H
#define CHROMATIC_IMAP_DDSSPLTYPES_H

#include "ccpp_chromatic_imap_dds.h"

#include <c_base.h>
#include <c_misc.h>
#include <c_sync.h>
#include <c_collection.h>
#include <c_field.h>

extern c_metaObject __chromatic_imap_dds_chromatic_imap_dds__load (c_base base);

extern c_metaObject __chromatic_imap_dds_command_type__load (c_base base);
typedef c_string _chromatic_imap_dds_command_type;

extern c_metaObject __chromatic_imap_dds_chromatic_imap_command__load (c_base base);
extern const char * __chromatic_imap_dds_chromatic_imap_command__keys (void);
extern const char * __chromatic_imap_dds_chromatic_imap_command__name (void);
struct _chromatic_imap_dds_chromatic_imap_command ;
extern  c_bool __chromatic_imap_dds_chromatic_imap_command__copyIn(c_base base, struct chromatic_imap_dds::chromatic_imap_command *from, struct _chromatic_imap_dds_chromatic_imap_command *to);
extern  void __chromatic_imap_dds_chromatic_imap_command__copyOut(void *_from, void *_to);
struct _chromatic_imap_dds_chromatic_imap_command {
    c_long session_idx;
    c_long locale_idx;
    c_string moniker;
    c_string command;
    c_string payload;
};

#endif

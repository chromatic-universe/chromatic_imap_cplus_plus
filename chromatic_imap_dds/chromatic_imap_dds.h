//******************************************************************
// 
//  Generated by IDL to C++ Translator
//  
//  File name: chromatic_imap_dds.h
//  Source: chromatic_imap_dds.idl
//  Generated: Sun Jul 21 00:54:47 2013
//  OpenSplice V6.3.0OSS
//  
//******************************************************************
#ifndef _CHROMATIC_IMAP_DDS_H_
#define _CHROMATIC_IMAP_DDS_H_

#include "sacpp_mapping.h"
#include "sacpp_DDS_DCPS.h"

namespace chromatic_imap_dds
{
   struct chromatic_imap_command;
   struct chromatic_name_service;
   const DDS::Long max_command_len = (DDS::Long) 32L;
   typedef DDS::Char* command_type;
   typedef DDS::String_var command_type_var;
   typedef DDS::String_out command_type_out;
   typedef DDS::Char* user_id;
   typedef DDS::String_var user_id_var;
   typedef DDS::String_out user_id_out;

   struct chromatic_imap_command
   {
      DDS::Long session_idx;
      DDS::Long locale_idx;
      DDS::String_mgr moniker;
      DDS::String_mgr command;
      DDS::String_mgr payload;
   };

   typedef DDS_DCPSStruct_var < chromatic_imap_command> chromatic_imap_command_var;
   typedef DDS_DCPSStruct_out < chromatic_imap_command> chromatic_imap_command_out;

   struct chromatic_name_service
   {
      DDS::Long session_idx;
      DDS::String_mgr user_name;
   };

   typedef DDS_DCPSStruct_var < chromatic_name_service> chromatic_name_service_var;
   typedef DDS_DCPSStruct_out < chromatic_name_service> chromatic_name_service_out;
}




#endif 

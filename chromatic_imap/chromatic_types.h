/*
 * chromatic_enumeration.h
 *
 *  Created on: Jun 17, 2013
 *      Author: wiljoh
 */

#ifndef CHROMATIC_TYPES_H_
#define CHROMATIC_TYPES_H_

namespace chromatic_imap_protocol_impl
{
				class chromatic_thread_pool;
				class chromatic_counter_guard;
				class chromatic_protocol_data;				;
				class chromatic_mailbox;
				class chromatic_imap_message;
				class chromatic_imap_mgr;
				class chromatic_handler;
				class chromatic_imap_search_criteria;
				class chromatic_imap_search_resolver;
				class chromatic_acceptor;

				//enumerations
				enum class concurrency_t : unsigned long
				{
					single_threaded_,
					thread_per_connection_,
					thread_pool_
				};

				enum class chromatic_protocol : unsigned long
				{
					oprOperationNotSupported = 0 ,
					oprRequiresSpecification
				};

				enum class chromatic_imap_dispatch : unsigned long
				{
					cidLogin = 0 ,
					cidLogout ,
					cidAuthenticate ,
					cidCreate ,
					cidSelect ,
					cidList ,
					cidDelete,
					cidRename ,
					cidLsub ,
					cidSubscribe ,
					cidUnsubscribe ,
					cidExamine ,
					cidNoop ,
					cidCheck ,
					cidCapability ,
					cidSearch ,
					cidCopy ,
					cidFetch ,
					cidStore ,
					cidUid ,
					cidClose ,
					cidExpunge ,
					cidAppend ,
					cidStarttls

				};

				enum class chromatic_packet_part : unsigned long
				{
					ppToken = 0 ,
					ppCommand ,
					ppPayload ,
					ppUid
				};

				enum class chromatic_packet_error : unsigned long
				{
					peNoToken = 0 ,
					peTokenTooLong ,
					peNoCommand ,
					peCommandTooLong ,
					peParametersExpectedGotNone ,
					peNoParametersExpectedGotSome ,
					peTooManyParameters ,
					peTooFewParameters ,
					peInvalidPayload ,
					peInvalidSyntax ,
					peUnrecognizedCommand ,
					peSelectState ,
					peFailure ,
					peInvalidObject ,
					peInvalidState ,
					peInvalidSpecification ,
					peNoSpecification ,
					peAttributeNotSupported ,
					peNotAuthenticated ,
					peAlreadyAuthenticated ,
					peNotEnoughPrivileges ,
					peNoPacketError
				};

				enum class chromatic_state : unsigned long
				{
					oesNonAuthenticated = 0 ,
					oesConnected ,
					oesAuthenticated ,
					oesSelected ,
					oesAdmin ,
					oesWait ,
					oesLogout
				};

				enum class chromatic_command : unsigned long
				{
					oecLogin = 0 ,
					oecLogout ,
					oecAuthenticate ,
					oecCreate ,
					oecSelect ,
					oecList ,
					oecDelete,
					oecRename ,
					oecLsub ,
					oecStatus ,
					oecSubscribe ,
					oecUnsubscribe ,
					oecExamine ,
					oecNoop ,
					oecCheck ,
					oecCapability ,
					oecSearch ,
					oecCopy ,
					oecFetch ,
					oecStore ,
					oecUid ,
					oecClose ,
					oecExpunge ,
					oecAppend ,
					oecStarttls ,
					oecNoCommand ,
					oecError
				};

				enum class chromatic_set_param : unsigned long
				{
					ospPrompt = 0
				};

				enum class chromatic_set_op : unsigned long
				{
					osopAdd ,
					osopDelete ,
					osopClear
				};

				enum class chromatic_imap_mgr_error_level : unsigned long
				{
					lmlCritical = 0 ,
					lmlLogic ,
					lmlInformation ,
					lmlDebug ,
					lmlNoError
				};

				enum class chromatic_mailbox_attribute : unsigned long
				{
					omaNoinferiors ,
					omaNoselect ,
					omaMarked ,
					omaUnmarked
				};

				enum class chromatic_mailbox_result : unsigned long
				{
					omrNotFound ,
					omrAlreadyExists ,
					omrSuccess ,
					omrAccessDenied
				};

				enum class chromatic_mail_message_flag : unsigned long
				{
					omfAnswered ,
					omfDeleted ,
					omfDraft ,
					omfFlagged ,
					omfRecent ,
					omfSeen
				};

				enum class chromatic_flags_method : unsigned long
				{
					ofmAdd ,
					ofmRemove ,
					ofmReplace
				};

				enum class chromatic_logic_operations : unsigned long
				{
					olpAnd ,
					olpNot ,
					olpOr
				};

				enum class chromatic_imap_search_error_level : unsigned long
				{
					islCritical = 0 ,
					islLogic ,
					islInformation ,
					islDebug ,
					islNoError
				};

				enum class chromatic_search_keys : unsigned long
				{
					oskAll ,
					oskText ,
					oskUnsupportedKey
				};

				//types
				typedef chromatic_protocol_data*  chromatic_protocol_data_ptr;
				typedef chromatic_thread_pool *   chromatic_thread_pool_ptr;
				typedef chromatic_imap_mgr*	chromatic_imap_mgr_ptr;
				typedef chromatic_acceptor* chromatic_imap_acceptor_ptr;
				typedef chromatic_handler*	chromatic_handler_ptr;
				typedef chromatic_imap_search_criteria* chromatic_imap_search_criteria_ptr;
				typedef chromatic_imap_search_resolver* chromatic_imap_search_resolver_ptr;
				typedef std::set<chromatic_protocol> protocol_set;
				typedef std::map<chromatic_imap_dispatch , protocol_set> protocol_map;
				typedef unsigned char* byte_ptr;
				typedef std::vector<int> v_int;
				typedef std::multimap<chromatic_packet_part , std::string>  map_of_parts;
				typedef std::vector<std::string> string_container;
				typedef std::map<std::string , bool> registry_dictionaryictionary;
				typedef std::multimap<std::string , std::string> object_dictionary;
				typedef std::map<std::string , std::string> string_dictionary;
				typedef std::map<chromatic_set_param , bool> set_map;
				typedef std::set<chromatic_state> current_state;
				typedef std::vector<std::string> imap_strings;
				typedef std::vector<std::string> vtor_string;
				typedef std::multimap<std::string , std::string> map_string;
				typedef void* void_ptr;
				typedef std::map<std::string,std::string> reg_params;
				typedef std::set<chromatic_mailbox_attribute> chromatic_mailbox_attributes;
				typedef std::vector<chromatic_mailbox> chromatic_mailbox_list;
				typedef std::vector<std::string> imap_strings;
				typedef std::set<chromatic_mail_message_flag> chromatic_mail_message_flags;
				typedef std::map<std::string,chromatic_imap_message> imap_server_messages;
				typedef imap_server_messages* imap_server_messages_ptr;
				typedef std::vector<int> v_int;
				typedef std::pair<std::string,std::string> chromatic_imap_fetch_request;
				typedef std::vector<chromatic_imap_fetch_request> chromatic_imap_fetch_requests;
				typedef std::map<unsigned long,std::string> imap_fetch_strings;
				typedef std::map<std::string,unsigned long> imap_url_to_uid;
				typedef std::map<std::string,std::string> imap_header_fields;
				typedef std::set<std::string> imap_header_set;
				typedef chromatic_imap_mgr* oemo_imap_mgr_ptr;
				typedef chromatic_imap_search_criteria* chromatic_imap_search_criteria_ptr;
				//atomic key with operator
				//	i.e 'not seen'
				typedef std::multimap<chromatic_logic_operations , std::string> chromatic_atomic_search_key;
				typedef std::vector<chromatic_atomic_search_key> chromatic_atomic_searches;
				//	=>=> parenthetical search key
				//		i.e. 'not (seen from "snaps")
				typedef std::pair<chromatic_logic_operations , std::string> chromatic_parenthetical_search_key;
				typedef std::vector<chromatic_parenthetical_search_key> chromatic_searches;
				typedef std::set<unsigned long> chromatic_message_numbers;
				typedef std::set<chromatic_search_keys> search_keys;

				//constant
				const char crlf[] = "\015\012";
				const char cr[] = "\015";
				const char lf[] = "\012";
				const unsigned short timer_id = 0;
				const unsigned long cmd_buffer_len = 65536L;
				const unsigned long max_client_timeout = 1800;//seconds


}

#endif //CHROMATIC_TYPES_H_

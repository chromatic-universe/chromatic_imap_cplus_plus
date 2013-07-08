/*
 * chromatic_imap_envelope.h
 *
 *  Created on: Jul 7, 2013
 *      Author: wiljoh
 */

#ifndef CHROMATIC_IMAP_ENVELOPE_H_
#define CHROMATIC_IMAP_ENVELOPE_H_


#pragma GCC diagnostic ignored "-Wswitch"

#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <cassert>
#include <utility>

#include <boost/algorithm/string.hpp>

#include <stdio.h>
#include <CkByteData.h>
#include <CkString.h>
#include <CkMime.h>
#include <CkEmail.h>

#include <unistd.h>


namespace chromatic_imap_protocol_impl
{

			typedef CkMime ck_mime;
			typedef CkEmail ck_email;
			typedef ck_email* ck_email_ptr;
			typedef ck_mime* ck_mime_ptr;
			typedef CkString ck_string;
			typedef std::ostringstream* ostr_ptr;
			typedef std::pair<std::string,std::string> addr_pr;
			typedef std::vector<addr_pr> addr_container;



			const std::string constMimeUnlockCode = "SFCNICOMSMIME_sK7NCgi98C5p";
			const std::string constMailUnlockCode = "SFCNICOMMAILQ_5NklwCi28R6H";
			const std::string space(" ");
			const std::string right_paren( ")" );
			const std::string left_paren( "(" );
			const std::string nil( "NIL" );
			const std::string quote( "\"" );
			const std::string empty( "" );
			const std::string colon( ":" );
			const std::string ampersand( "@" );
			const std::string left_angle( "<" );
			const std::string right_angle( ">" );
			const std::string crlf( "\n" );

			enum class addr_type : unsigned long
			{
				atFrom ,
				atTo ,
				atReplyTo ,
				atSender ,
				atCC ,
				atBCC ,
				atGroup ,
				atNoMoreAddrs
			};

			class envelope_parser
			{
				public:

					//ctor
					envelope_parser( ostr_ptr ostr , const std::string& file_name  ) :  m_ptr_ostr ( ostr ) ,
																						m_mime_ptr { new ( ck_mime ) } ,
																						m_email_ptr { new ( ck_email ) } ,
																						m_filename{ file_name }

					{
						m_ac.clear();

						assert( ostr );
						assert( m_mime_ptr->UnlockComponent( constMimeUnlockCode.c_str() ) );

						bool b_ret = m_mime_ptr->LoadMimeFile( m_filename.c_str() );
						m_email_ptr->SetFromMimeObject( *m_mime_ptr.get() );

						assert( b_ret );


						str_env_field_to_stream( "Date" );
						str_env_field_to_stream( "Subject" );
						fetch_addrs( addr_type::atFrom );
						str_env_field_to_stream( "In-Reply-To" );
						str_env_field_to_stream( "Message-ID" );
					 }

					 //dtor
					 ~envelope_parser()
					{}

				protected:

					//attributes
					ostr_ptr m_ptr_ostr;
					std::unique_ptr<ck_mime> m_mime_ptr;
					std::unique_ptr<ck_email> m_email_ptr;
					std::string m_filename;
					addr_container m_ac;

					//helpers
					bool check_for_group_addr( const std::string& addr , std::string& group_moniker )
					{
						bool b_ret( false );

						std::string::size_type sz = addr.find( colon );
						if( sz != std::string::npos )
						{
							group_moniker = addr.substr( 0 , sz );
							boost::algorithm::trim( group_moniker );
							b_ret = true;
						}

						return b_ret;
					}

					void process_group_addrs( const std::string& addrs )
					{
						//todo
					}

					std::string addr_atom( const std::string& line )
					{
						std::ostringstream strstr;

						line.empty() == true ? strstr << nil
													  << space
											 : strstr << quote
													  << line
													  << quote
													  << space;

						return strstr.str();
					}

					void single_sender_addr_line( const std::string& sender )
					{
						std::string::size_type sz = sender.find( left_angle );
						if( sz != std::string::npos )
						{
							std::string addr{};
							std::string::size_type sz2 = sender.find( right_angle , sz );
							if( sz2 != std::string::npos ) { addr = sender.substr( sz , sz2 ); }
							std::string moniker( sender.substr( 0 , sz ) );
							boost::algorithm::trim( moniker );
							std::ostringstream ostr;

							 moniker.find( quote , 0 ) != std::string::npos ? ostr << moniker : ostr << quote << moniker << quote
							 << space
							 << nil
							 << space
							 << nil
							 << space
							 << quote
							 << addr
							 << quote;

							single_addr_line( ostr.str() );
						}
					}

					void single_addr_line( const std::string& moniker )
					{

						*m_ptr_ostr << left_paren
									<< space
									<< left_paren
									<< moniker
									<< right_paren
									<< space
									<< right_paren
									<< crlf;
					}

					void multiple_addr_lines( const std::string& moniker )
					{
						*m_ptr_ostr << left_paren
									<< moniker
									<< right_paren
									<< crlf;
					}

					void nil_addr_line()
					{
						*m_ptr_ostr << nil << crlf;
					}

					//services

					//---------------------------------------------------------------------------------------
					void process_multiple_addrs( addr_container& adrc )
					{
						unsigned long dw { adrc.size() };
						std::ostringstream line_strm;


						if( dw > 1 )  *m_ptr_ostr << left_paren << space;
						for ( unsigned long i = 0; i < dw; i++ )
						{

							//name
							line_strm << addr_atom( adrc[i].first );

							//smtp route
							line_strm << addr_atom( "" );

							//friendly name
							std::string friendly( friendly_name( adrc[i].second ) );
							line_strm << addr_atom( friendly );

							//address
							line_strm << addr_atom( adrc[i].second );

							std::string str = line_strm.str();
							line_strm.str( empty );

							boost::algorithm::trim( str );
							dw == 1 ? single_addr_line( str  ) : multiple_addr_lines( str );

						}
						if( dw > 1 ) *m_ptr_ostr << space << right_paren << crlf;
					}

					std::string friendly_name( const std::string& moniker )
					{

						std::string str( moniker );
						std::string::size_type sz = str.find( ampersand );
						if( sz != std::string::npos ) { str = str.substr( 0 , sz ); }

						return str;
					}

					//---------------------------------------------------------------------------------------
					inline void fetch_addrs( addr_type at )
					{
						std::string cached_addr{};
						std::string cached_from{};
						std::ostringstream line_strm;

						while( at != addr_type::atNoMoreAddrs )
						{
							switch ( at )
							{
								case addr_type::atFrom :
								{
									CkString cks;


									//personal
									m_email_ptr->get_FromName( cks );
									std::string str( cks.getString() );
									line_strm << addr_atom( str );

									//smtp route
									str = empty;
									line_strm << addr_atom( str );

									m_email_ptr->get_FromAddress( cks );
									str = cks.getString();
									//friendly name
									std::string friendly( friendly_name( str ) );
									line_strm << addr_atom( friendly );
									//address
									line_strm << addr_atom( str );

									str = line_strm.str();

									boost::algorithm::trim( str );
									cached_addr = str;
									m_email_ptr->get_From( cks );
									cached_from = cks.getString();

									single_addr_line( str );

									at = addr_type::atSender;

									continue;
								}
								case addr_type::atSender :
								{
									CkString cks;
									bool b_ret = m_email_ptr->GetHeaderField( "Sender" , cks );
									//no sender , use from
									if( b_ret == false ) { single_addr_line( cached_addr ); }
									else single_sender_addr_line( cks.getString() );

									at = addr_type::atReplyTo ;

									continue;
								}
								case addr_type::atReplyTo :
								{

									CkString cks;
									bool b_ret = m_email_ptr->GetHeaderField( "Reply-To" , cks );


									//no reply to , use from
									if( b_ret == false ) { single_addr_line( cached_addr ); }
									else single_sender_addr_line( cks.getString() );

									at = addr_type::atTo ;

									continue;
								}
								case addr_type::atTo :
								case addr_type::atCC :
								case addr_type::atBCC :
								{
									line_strm.str( empty );
									unsigned long dw = m_email_ptr->get_NumTo();
									addr_pr apr;
									addr_container actr;

									//to
									for ( unsigned long i = 0; i <= dw - 1; i++ )
									{
										apr.first = m_email_ptr->getToName( i );
										apr.second = m_email_ptr->getToAddr( i );

										actr.push_back( apr );

										process_multiple_addrs( actr );
									}
									//cc
									actr.clear();
									dw = m_email_ptr->get_NumCC();
									if( dw == 0 ) {  nil_addr_line(); }
									else
									{
										for ( unsigned long i = 0; i <= dw - 1; i++ )
										{
											apr.first = m_email_ptr->getCcAddr( i );
											apr.second = m_email_ptr->getCcName( i ) ;

											actr.push_back( apr );

											process_multiple_addrs( actr );
										}
									}
									//bcc
									actr.clear();
									dw = m_email_ptr->get_NumBcc();
									if( dw == 0 ) {  nil_addr_line(); }
									else
									{
										for ( unsigned long i = 0; i <= dw - 1; i++ )
										{
											apr.first = m_email_ptr->getBccAddr( i );
											apr.second = m_email_ptr->getBccName( i ) ;

											actr.push_back( apr );

											process_multiple_addrs( actr );
										}
									}
									at = addr_type::atNoMoreAddrs;


								}
							}
						}
					}

					//---------------------------------------------------------------------------------------
					inline void str_env_field_to_stream( const std::string& field_moniker )
					{
						std::unique_ptr<ck_string> cks( new ck_string );
						bool b_ret = m_mime_ptr->GetHeaderField( field_moniker.c_str(), *cks.get() );

						std::string str( cks->getString() );

						 //if the Date, Subject, In-Reply-To, and Message-ID header lines
						 //are absent in the [RFC-2822] header, the corresponding member
						 //of the envelope is NIL; if these header lines are present but
						 //empty the corresponding member of the envelope is the empty
						 //string.[3501]
						 if( ( b_ret == true ) && ( str.empty() == true ) ) { *m_ptr_ostr << quote << quote; }
						 else if( str.empty() ) { *m_ptr_ostr << nil; }
						 else { *m_ptr_ostr << "\"" << str << "\""; }
						 *m_ptr_ostr << crlf;
					}

				public:

					//
			};
}


#endif//CHROMATIC_IMAP_ENVELOPE_H_

//chromatic_imap_envelope.cpp
// created on: Jul 23, 2013
// Author: wiljoh


#include "chromatic_imap_envelope.h"


using namespace chromatic_imap_protocol_impl;

using std::string;
using std::ostringstream;
using std::unique_ptr;

//-------------------------------------------------------------------------------------------------------------
envelope_parser::envelope_parser( ostr_ptr ostr ,
								  const string& file_name ) :  m_ptr_ostr ( ostr ) ,
															   m_mime_ptr { new ( ck_mime ) } ,
															   m_email_ptr { new ( ck_email ) } ,
															   m_filename{ file_name }
{
			m_ac.clear();

			assert( ostr );
			assert( m_mime_ptr->UnlockComponent( constMimeUnlockCode.c_str() ) );

			parse();

}

//-------------------------------------------------------------------------------------------------------------
envelope_parser::~envelope_parser()
{
	//
}

//-------------------------------------------------------------------------------------------------------------
bool envelope_parser::check_for_group_addr( const string& addr , string& group_moniker )
{
	bool b_ret( false );

	string::size_type sz = addr.find( colon );
	if( sz != string::npos )
	{
		group_moniker = addr.substr( 0 , sz );
		boost::algorithm::trim( group_moniker );
		b_ret = true;
	}

	return b_ret;
}

//---------------------------------------------------------------------------------------
void envelope_parser::process_group_addrs( const string& addrs )
{
	//todo
}

//---------------------------------------------------------------------------------------
string envelope_parser::addr_atom( const string& line )
{
	ostringstream strstr;

	//nil if empty
	line.empty() == true ? strstr << nil
								  << space
						 : strstr << quote
								  << line
								  << quote
								  << space;


	return strstr.str();
}

//---------------------------------------------------------------------------------------
void envelope_parser::single_sender_addr_line( const string& sender )
{
		//left delimiter
		string::size_type sz = sender.find( left_angle );
		if( sz != string::npos )
		{
			string addr{};
			//right delimiter
			string::size_type sz2 = sender.find( right_angle , sz );
			//address atom
			if( sz2 != string::npos ) { addr = sender.substr( sz , sz2 ); }
			//name atom
			string moniker( sender.substr( 0 , sz ) );
			//trim moniker
			boost::algorithm::trim( moniker );
			std::ostringstream ostr;
			//name may be unquoted
			moniker.find( quote , 0 ) != string::npos ? ostr << moniker : ostr << quote << moniker << quote
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

//---------------------------------------------------------------------------------------
void envelope_parser::single_addr_line( const std::string& moniker )
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

//---------------------------------------------------------------------------------------
void envelope_parser::multiple_addr_lines( const string& moniker )
{
	*m_ptr_ostr << left_paren
				<< moniker
				<< right_paren
				<< crlf;
}

//---------------------------------------------------------------------------------------
string envelope_parser::friendly_name( const string& moniker )
{

	string str( moniker );
	string::size_type sz = str.find( ampersand );
	//first atom of address
	if( sz != string::npos ) { str = str.substr( 0 , sz ); }

	return str;
}

//---------------------------------------------------------------------------------------
void envelope_parser::process_multiple_addrs( addr_container& adrc )
{
	unsigned long dw { adrc.size() };
	ostringstream line_strm;

	//open singleton address
	if( dw > 1 )  *m_ptr_ostr << left_paren << space;
	for ( unsigned long i = 0; i < dw; i++ )
	{

		//name
		line_strm << addr_atom( adrc[i].first );

		//smtp route
		line_strm << addr_atom( empty );

		//friendly name
		std::string friendly( friendly_name( adrc[i].second ) );
		line_strm << addr_atom( friendly );

		//address
		line_strm << addr_atom( adrc[i].second );

		std::string str = line_strm.str();
		line_strm.str( empty );

		boost::algorithm::trim( str );
		// ? open : open-close
		dw == 1 ? single_addr_line( str  ) : multiple_addr_lines( str );

	}
	//close singleton address
	if( dw > 1 ) *m_ptr_ostr << space << right_paren << crlf;
}

//---------------------------------------------------------------------------------------
void envelope_parser::fetch_addrs( addr_type at )
{
	//cached line
	string cached_addr{};
	//cached address
	string cached_from{};
	//addr line
	ostringstream line_strm;

	////

	while( at != addr_type::atNoMoreAddrs )
	{
		switch ( at )
		{
			case addr_type::atFrom :
			{
				CkString cks;

				//personal
				m_email_ptr->get_FromName( cks );
				string str( cks.getString() );
				line_strm << addr_atom( str );

				//smtp route
				str = empty;
				line_strm << addr_atom( str );

				m_email_ptr->get_FromAddress( cks );
				str = cks.getString();
				//friendly name
				string friendly( friendly_name( str ) );
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
				//treatment often ambigous by client and server
				bool b_ret = m_email_ptr->GetHeaderField( sender.c_str() , cks );
				//no sender , use from
				if( b_ret == false ) { single_addr_line( cached_addr ); }
				else single_sender_addr_line( cks.getString() );

				at = addr_type::atReplyTo ;

				continue;
			}
			case addr_type::atReplyTo :
			{

				CkString cks;
				bool b_ret = m_email_ptr->GetHeaderField( reply_to.c_str() , cks );


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
void envelope_parser::str_env_field_to_stream( const string& field_moniker )
{
	unique_ptr<ck_string> cks( new ck_string );
	bool b_ret = m_mime_ptr->GetHeaderField( field_moniker.c_str(), *cks.get() );

	string str( cks->getString() );

	 //if the Date, Subject, In-Reply-To, and Message-ID header lines
	 //are absent in the [RFC-2822] header, the corresponding member
	 //of the envelope is NIL; if these header lines are present but
	 //empty the corresponding member of the envelope is the empty
	 //string.[3501]
	 if( ( b_ret == true ) && ( str.empty() == true ) ) { *m_ptr_ostr << quote << quote; }
	 else if( str.empty() ) { *m_ptr_ostr << nil; }
	 else { *m_ptr_ostr << quote << str << quote; }
	 *m_ptr_ostr << crlf;
}

//---------------------------------------------------------------------------------------
void envelope_parser::parse()
{
	bool b_ret = m_mime_ptr->LoadMimeFile( m_filename.c_str() );
	m_email_ptr->SetFromMimeObject( *m_mime_ptr.get() );

	assert( b_ret );

	//field
	str_env_field_to_stream( date );
	//field
	str_env_field_to_stream( subject );
	//addresses
	fetch_addrs( addr_type::atFrom );
	//field
	str_env_field_to_stream( in_reply_to );
	//field
	str_env_field_to_stream( message_id );
}






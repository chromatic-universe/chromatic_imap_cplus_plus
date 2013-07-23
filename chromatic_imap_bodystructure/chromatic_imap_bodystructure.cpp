//============================================================================
// Name        : chromatic_imap_bodystructure.cpp
// Author      : 
// Version     :
// Copyright   :
// Description :
//============================================================================

#include "chromatic_imap_bodystructure.h"

using namespace chromatic_imap_protocol_impl;
namespace cipi = chromatic_imap_protocol_impl;

using std::cout;
using std::unique_ptr;
using std::string;
using std::cerr;
using std::ostringstream;
using std::vector;
using std::pair;


int main()
{
	unique_ptr<ostringstream> ostr( new ( ostringstream ) );

	//*ostr << "ENVELOPE" << "(";
	//unique_ptr<envelope_parser> ep( new  envelope_parser( ostr.get() , "Senior.eml" ) ) ;
	//*ostr << ")";
	//cout << ostr->str();

	unique_ptr<bodystructure_parser> bsp( new  bodystructure_parser( ostr.get() , "Senior.eml" ) ) ;
	bsp->mime_traverse();

	cout << ostr->str();

	return 0;

}

//---------------------------------------------------------------------------------------
bool bodystructure_parser::_S( const string& moniker , const string& token , string::size_type& sz )
{
	 bool b_ret( false );
	 sz = moniker.find( token );
	 sz != string::npos  ? b_ret = true : b_ret = false;

	 return b_ret;
}

//---------------------------------------------------------------------------------------
mime_pr bodystructure_parser::splice( const string& category , const string& token )
 {
	 mime_pr pr{};
	 string::size_type sz;

	 if( _S( category , token , sz ) == true ){ pr.first = category.substr( 0 , sz );
												pr.second = category.substr( ++sz );
												ostringstream ostr;
												ostr << quote << pr.first << quote; pr.first = ostr.str(); ostr.str( "" );
												ostr << quote << pr.second << quote; pr.second = ostr.str();
											   }

	 return ( pr );
 }

//---------------------------------------------------------------------------------------
void bodystructure_parser::mime_traverse( ck_mime_ptr ptr_mime , int level )
 {
	  CkString strContentType;
	  ptr_mime->get_ContentType( strContentType );

	  mime_pr pr( splice( strContentType.getString() , forward_slash ) );
	  mime_pr cr( splice( ptr_mime->charset() , dash ) );
	  *m_ptr_ostr << content_type
				  << colon
				  << pr.first << space << pr.second
				  << crlf
				  << charset
				  << colon
				  << cr.first << space << cr.second
				  << crlf;

	  int num_parts = ptr_mime->get_NumParts();
	  for ( int i = 0; i < num_parts; i++ )
	  {
		cout << i << crlf;
		unique_ptr<CkMime> mime_part( ptr_mime->GetPart( i ) );
		mime_traverse( mime_part.get() , level + 1 );
	  }
 }

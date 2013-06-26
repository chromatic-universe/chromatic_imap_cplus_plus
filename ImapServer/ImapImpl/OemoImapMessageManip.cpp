#include "OemoImapMessageManip.h"

using std::string;
using std::ostringstream;
using std::generate_n;
using std::back_inserter;
using std::back_insert_iterator;
using std::vector;
using std::max_element;
using std::min_element;
using std::sort;
using std::unique;
using std::less;
using std::set_intersection;
using std::insert_iterator;
using std::set_symmetric_difference;
using std::cout;
using std::endl;
using std::greater;
using std::remove;
using std::set;

using namespace OemoImapMessageManip;

//--------------------------------------------------------------------------------------------
vInt OemoImapMessageManip::ExpandMessageSequence( const std::string& sequence_in ,
                                                  int dwLimit )
{
   vStr SeqStrings;
   vInt SeqSubAtoms;

   string s ( sequence_in );
   

  safe_tokenize_t( SeqStrings , (char*) sequence_in.c_str() , "," );

  for( vStr::iterator idx = SeqStrings.begin();
                idx != SeqStrings.end();
                        idx++ )
                        {
                          ProcessAtoms( *idx , SeqSubAtoms , dwLimit );
                        }

   return ( SeqSubAtoms );
}

//--------------------------------------------------------------------------------------------
int OemoImapMessageManip::GetMessageCount( const string& sequence_in , int dwLimit )
{
   vStr SeqStrings;
   vInt SeqSubAtoms;

   safe_tokenize_t( SeqStrings , (char*) sequence_in.c_str() , "," );

   for( vStr::iterator idx = SeqStrings.begin();
                idx != SeqStrings.end();
                        idx++ )
                        {
                          ProcessAtoms( *idx , SeqSubAtoms , dwLimit  );
                        }

   return ( SeqSubAtoms.size() );

}

//--------------------------------------------------------------------------------------------
bool OemoImapMessageManip::IsMessageSet( const std::string& atom , int dwLimit )
{
		ostringstream ostr;
        ostr << atom;				

		bool bRet( false );

		set<int> sint;
		sint.insert( ':' );
		sint.insert( '*' );

		string str( atom );
		string::iterator iter = str.begin();
		{
			string::size_type pos = 0;
			string::size_type mark = 0;

			//only colon and digits
			while( iter != str.end() )
			{
				char c = *iter;
				if( isalpha( c ) )
				{
					return ( false );
				}
				else if( isdigit( c ) )
				{
					//
				}
				else if( sint.find( c ) == sint.end() )
				{
					return ( false );
				}
				else if( c == ':' )
				{
					mark = pos;											
				}
				else if( c == '*' )
				{
					//asterisk cannot precede colon
					if( mark > pos  )
					{
						return ( false );
					}
				}
				
				pos++;
				iter++;
			}
		}
        string::size_type idx = atom.find( ":" );
        if( idx == string::npos )
        {

            //single sequence number 			
			if( atoi( (char*) atom.c_str() ) != 0 )
			{				
				bRet = true;
			}
        }
        else
        {
                //range
                string left_sequence;
                string right_sequence;
                vector<string> v;

                left_sequence = atom.substr( 0 , idx );
                idx++;
                right_sequence = atom.substr( idx );

                int dwLeft( 0L );
                int dwRight( 0L );

                //double ended range boundary
                if( right_sequence.compare( "*" ) != 0 )
                {
                   //range can only be ascending
                   dwLeft =  atoi( (char*) left_sequence.c_str() );
                   dwRight = atoi( (char*) right_sequence.c_str() );

				   if( ( dwLeft != 0 ) && ( dwRight != 0 )  )
				   {
					   if( dwLeft <= dwRight )
					   {
							bRet = true;
					   }
				   }

                }
                else
                {
                    //bounded on the left only
                    dwLeft =  atoi( (char*) left_sequence.c_str() );
					if( dwLeft != 0 ) 
				    {
						if( dwLimit == 0 )
						{
							//do nothing , we don't know the limit of the message context
	                       
						}
						else
						{
							dwRight = dwLimit;

						   if( dwLeft <= dwRight )
						   {
							  bRet = true;
						   }

						}
					}

                }
         }

		return ( bRet );
}


//--------------------------------------------------------------------------------------------
void OemoImapMessageManip::ProcessAtoms( const string& atom  , vInt& subatoms , int dwLimit )
{
        ostringstream ostr;
        ostr << atom;

        string::size_type idx = atom.find( ":" );
        if( idx == string::npos )
        {
           //single sequence number
           subatoms.push_back( atoi( (char*) atom.c_str() ) );
        }
        else
        {
                //range
                string left_sequence;
                string right_sequence;
                vector<string> v;

                left_sequence = atom.substr( 0 , idx );
                idx++;
                right_sequence = atom.substr( idx );

                int dwLeft =  0L;
                int dwRight = 0L;

                //double ended range boundary
                if( right_sequence.compare( "*" ) != 0 )
                {
                   //range can only be ascending
                   dwLeft =  atoi( (char*) left_sequence.c_str() );
                   dwRight = atoi( (char*) right_sequence.c_str() );

                   if( dwLeft <= dwRight )
                   {
                        if( dwRight > dwLimit ) dwRight = dwLimit;

                        //enumerate range
                        generate_n( back_inserter( subatoms ) ,
                                    ++dwRight - dwLeft ,
                                    PopulateRange( dwLeft ) );

                   }

                }
                else
                {
                    //bounded on the left only
                    dwLeft =  atoi( (char*) left_sequence.c_str() );

                    if( dwLimit == 0 )
                    {
                        //do nothing , we don't know the limit of the message context
                        return;
                    }
                    else
                    {
                        dwRight = dwLimit;

                       if( dwLeft <= dwRight )
                       {
                            //enumerate range
                            generate_n( back_inserter( subatoms ) ,
                                        ++dwRight - dwLeft ,
                                        PopulateRange( dwLeft ) );
                       }

                    }

                }
         }
}

//--------------------------------------------------------------------------------------------
int  OemoImapMessageManip::MaxAtom( vInt subatoms )
{
      return( *max_element( subatoms.begin() , subatoms.end() ) );
}

//--------------------------------------------------------------------------------------------
int  OemoImapMessageManip::MinAtom( vInt subatoms )
{
      return( *min_element( subatoms.begin() , subatoms.end() ) );
}

//--------------------------------------------------------------------------------------------
vInt OemoImapMessageManip::AtomsIntersection( vInt& first_atoms , vInt& second_atoms )
{


	  
      unsigned long dw = 0L;
      if( first_atoms.size() > second_atoms.size() )
       dw = first_atoms.size();
      else
       dw = second_atoms.size();

	  //although not clearly documented , the  target vector must be initialized to a size greater than zero
      vInt v( dw );      

      vInt::iterator iter;
      vInt::iterator sub_iter;	 

	  //ranges must be sorted and unique
      sort( first_atoms.begin() , first_atoms.end() );
      sort( second_atoms.begin() , second_atoms.end() );
      unique( first_atoms.begin() , first_atoms.end() );
      unique( second_atoms.begin() , second_atoms.end() );

	  //set intersection , default sort order
      iter = set_intersection( first_atoms.begin() ,
							   first_atoms.end() ,
							   second_atoms.begin() ,
							   second_atoms.end() ,
							   v.begin() ,
							   less<int>() );
	  //since zero is not a valid message number , we assume all zeros are initialziations and remove them
	  sub_iter = remove( v.begin() , v.end() , 0 );
	  v.erase( sub_iter , v.end() );  


      return ( v );

}

//--------------------------------------------------------------------------------------------
string OemoImapMessageManip::ExtractParentheticalAtoms( const string& payload )
{
	string str( payload );

	string::size_type idx = str.find_first_of( "(" );
	if( idx != string::npos )
	{
		idx++;
		string::size_type jdx = idx;
		idx = str.find_last_of( ")" );
		if( idx != string::npos )
		{
			str = str.substr( jdx , idx - jdx );
		}

	}	

	return ( str );

}


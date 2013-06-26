//--------------------------------------------------------------------------------
//						Header for Imap C-Client c++ wrapper			  
//								  Inter-Tel , Inc.								  
//								?GNU Public License?							  
//						William K. Johnson - initial version 21 Nov 2002
//							(C) copyright Intertel-Tel , Inc,
//--------------------------------------------------------------------------------

#ifndef _GENERIC_H
#define _GENERIC_H


//generic interfaces

//lists , deques , vectors 
template <typename A , typename B>
inline int add_element_t(A const& collection , B const& element)
{
	collection.push_back(element);	
		
	return (collection.size());		
}

//lists , deques , vectors , strings
template <typename A >
inline int get_element_count_t( A& collection ) 
{
	return (collection.size());
}


//all containers
template <typename A>
inline void clear_elements_t( A& collection ) 
{
	collection.clear();
}

//lists , deques , vectors
template <typename A>
inline int safe_tokenize_t( A& collection , const char* szStr,
						                   const char* delimiters = " " )
{
	std::string  str(szStr);

    // Skip delimiters at beginning , i.e leading whitespace
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);

    // Find first "non-delimiter".
    string::size_type pos  = str.find_first_of(delimiters, lastPos);

    while (string::npos != pos || string::npos != lastPos)
    {
		string fstr;

		fstr = str.substr(lastPos, pos - lastPos);

        // Found a token, add it to the vector.
        collection.push_back( fstr );

        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);

        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }

	return (collection.size()); 
};

//lists , deques , vectors
template <typename A , typename B>
inline void  get_element_at_t( A const& collection , B const& element , const int idx )
{
	try
	{
		element = collection.at(idx);	

	}
	catch(std::out_of_range& e)
	{
		//
	}	
}

//return map value for key , maps
template <typename A , typename B>
inline A get_map_element_at_t( A const& key , B const& map )
{

	A value;
	typename B::const_iterator pos;

	for( pos = map.begin(); pos != map.end(); ++pos )
	{
		if( pos->first == key )
		{
			value = pos->second;

			break;
		}
	}

	return ( value );

}

//stream key pairs of map/multimap
template <typename A , typename B>
inline void stream_map_elements_t( A const& map , B& stream , 
								   const std::string& sep = " " ,
								   const std::string& terminator = "\n")
{
	
	typename A::const_iterator pos;

	for( pos = map.begin(); pos != map.end(); ++pos )
	{
		stream << pos->first << sep << pos->second << terminator ;
	}

	stream << ends;
}

template <typename A >
inline A return_type_spec()
{
	
	return ( A );
	
}

#endif //_GENERIC_H
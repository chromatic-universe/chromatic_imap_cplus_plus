#ifndef OEMO_IMAP_MESSAGE_MANIP
#define OEMO_IMAP_MESSAGE_MANIP

#pragma warning( disable: 4786 4800 4365 )

#include <windows.h>

#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include <functional>
#include <set>
#include "generic.h"


namespace OemoImapMessageManip
{
  typedef std::vector<std::string> vStr;
  typedef std::vector<int> vInt;

  //functors
  class PopulateRange
  {
       private:

          int m_value;

       public:

          //ctor
          PopulateRange( int initValue )
                  : m_value( initValue )
                  {
                          //
                  }

          //functor call
          int operator() ()
          {
             return ( m_value++ );
          }

  };

  vInt ExpandMessageSequence( const std::string& sequence_in ,
                              int   dwLimit );
  int  GetMessageCount( const std::string& sequence_in , int dwLimit );
  void ProcessAtoms( const std::string& atom  , vInt& subatoms , int dwLimit );
  int  MaxAtom( vInt subatoms );
  int  MinAtom( vInt subatoms );
  vInt AtomsIntersection( vInt& first_atoms , vInt& second_atoms );
  std::string ExtractParentheticalAtoms( const std::string& payload );
  bool IsMessageSet( const std::string& atom , int dwLimit );


}

#endif//OEMO_IMAP_MESSAGE_MANIP


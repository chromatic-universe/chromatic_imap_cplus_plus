/*
 * chromatic_imap_fs.h
 *
 *  Created on: Jun 27, 2013
 *      Author: wiljoh
 */

#ifndef CHROMATIC_IMAP_FS_H_
#define CHROMATIC_IMAP_FS_H_

#include <fusekit/daemon.h>
#include <fusekit/stream_object_file.h>

#include "chromatic_imap.h"

namespace chromatic_imap_fs
{
			class cif_append_file : public fusekit::iostream_object_file< cif_append_file >::type
			{
				public:

					//ctor
					cif_append_file(): fusekit::iostream_object_file< cif_append_file >::type{ *this }
									   , m_buf{ "this is the chromatic_imap_fs daemon speaking..." } {}

					//dtor
					virtual ~cif_append_file() {}

					//stream
					friend std::ostream& operator<<( std::ostream& os, const cif_append_file& f );
					friend std::istream& operator>>( std::istream& is, cif_append_file& f );


				protected:

					std::string m_buf;

				public:

				  int open( fuse_file_info& fi )
				  {

					if( write_flag_set( fi.flags ) && ( fi.flags & O_APPEND ) == 0 )
					{
					  m_buf = "";
					}
					return fusekit::iostream_object_file< cif_append_file >::type::open( fi );
				  }

			};

			inline std::ostream& operator<<( std::ostream& os, const cif_append_file& f )
			{
			  return os << f.m_buf;
			}

			inline std::istream& operator>>( std::istream& is, cif_append_file& f )
			{
			  std::string line;
			  is >> line;

			  if( !f.m_buf.empty() )
			  {
				f.m_buf += "\n";
			  }

			  f.m_buf += line;

			  return is;
			}
}



#endif //CHROMATIC_IMAP_FS_H_

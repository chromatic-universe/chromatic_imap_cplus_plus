
#include <fusekit/daemon.h>
#include <fusekit/stream_object_file.h>

#include "chromatic_imap_fs.h"

using namespace chromatic_imap_fs;


/// demonstrates how to derive from fusekit::iostream_object_file
///
/// the file simply holds a string as buffer and is appendable, that
/// is it checks the open mode and only resets content if O_APPEND is not set.
/// note that operators << and >> have been overloaded to make this
/// class compatible with fusekit::iostream_object_file.


/// example program which demonstrates how to make file 'appendable'.
int main( int argc, char* argv[] )
{
  fusekit::daemon<>::instance().root().add_file(
                                                "append",
                                                new cif_append_file);
  return fusekit::daemon<>::instance().run(argc,argv);
}

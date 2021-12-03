/** Version check.
 *
 * Copyright (c) 2000-2019, Jeroen T. Vermeulen.
 *
 * See COPYING for copyright license.  If you did not receive a file called
 * COPYING with this source code, please notify the distributor of this mistake,
 * or contact the author.
 */
#include "pqxx/compiler-internal.hxx"

#include "pqxx/version"

namespace pqxx::internal
{
// One, single definition of this function.  If a call fails to link, then the
// libpqxx binary was built against a different libpqxx version than the code
// which is being linked against it.
template<> PQXX_LIBEXPORT
int check_library_version<PQXX_VERSION_MAJOR, PQXX_VERSION_MINOR>() noexcept
{
  return 0;
}
} // namespace pqxx::internal

#include <iostream>

#include "test_helpers.hxx"

using namespace pqxx;


// Simple test program for libpqxx.  Open a connection to database, start a
// transaction, and perform a query inside it.
namespace
{
void test_021()
{
  connection conn;

  const std::string HostName = (conn.hostname() ? conn.hostname() : "<local>");
  conn.process_notice(std::string{} +
	"database=" + conn.dbname() + ", "
	"username=" + conn.username() + ", "
	"hostname=" + HostName + ", "
	"port=" + to_string(conn.port()) + ", "
	"backendpid=" + to_string(conn.backendpid()) + "\n");

  work tx{conn, "test_021"};

  // By now our connection should really have been created
  conn.process_notice("Printing details on actual connection\n");
  conn.process_notice(std::string{} +
	"database=" + conn.dbname() + ", "
	"username=" + conn.username() + ", "
	"hostname=" + HostName + ", "
	"port=" + to_string(conn.port()) + ", "
	"backendpid=" + to_string(conn.backendpid()) + "\n");

  std::string P;
  from_string(conn.port(), P);
  PQXX_CHECK_EQUAL(
	P,
	to_string(conn.port()),
	"Port string conversion is broken.");
  PQXX_CHECK_EQUAL(to_string(P), P, "Port string conversion is broken.");

  result R( tx.exec("SELECT * FROM pg_tables") );

  tx.process_notice(to_string(R.size()) + " "
		   "result row in transaction " +
		   tx.name() +
		   "\n");
  tx.commit();
}


PQXX_REGISTER_TEST(test_021);
} // namespace

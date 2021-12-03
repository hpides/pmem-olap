#include "../test_helpers.hxx"

namespace
{
void test_move_constructor()
{
  pqxx::connection c1;
  PQXX_CHECK(c1.is_open(), "New connection is not open.");

  pqxx::connection c2{std::move(c1)};

  PQXX_CHECK(not c1.is_open(), "Moving did not close original connection.");
  PQXX_CHECK(c2.is_open(), "Moved constructor is not open.");

  pqxx::work tx{c2};
  PQXX_CHECK_EQUAL(tx.exec1("SELECT 5")[0].as<int>(), 5, "Weird result!");

  PQXX_CHECK_THROWS(
	pqxx::connection c3{std::move(c2)},
	pqxx::usage_error,
	"Moving a connection with a transaction open should be an error.");
}


void test_move_assign()
{
  pqxx::connection c1;
  pqxx::connection c2;

  c2.close();

  c2 = std::move(c1);

  PQXX_CHECK(not c1.is_open(), "Connection still open after being moved out.");
  PQXX_CHECK(c2.is_open(), "Moved constructor is not open.");

  {
    pqxx::work tx1{c2};
    PQXX_CHECK_EQUAL(tx1.exec1("SELECT 8")[0].as<int>(), 8, "What!?");

    pqxx::connection c3;
    PQXX_CHECK_THROWS(
	c3 = std::move(c2),
	pqxx::usage_error,
	"Moving a connection with a transaction open should be an error.");

    PQXX_CHECK_THROWS(
	c2 = std::move(c3),
	pqxx::usage_error,
	"Moving a connection onto one with a transaction open should be "
	"an error.");
  }

  // After failed move attempts, the connection is still usable.
  pqxx::work tx2{c2};
  PQXX_CHECK_EQUAL(tx2.exec1("SELECT 6")[0].as<int>(), 6, "Huh!?");
}


PQXX_REGISTER_TEST(test_move_constructor);
PQXX_REGISTER_TEST(test_move_assign);
} // namespace

#include "../test_helpers.hxx"


namespace
{
void test_nontransaction_continues_after_error()
{
  pqxx::connection c;
  pqxx::nontransaction tx{c};

  PQXX_CHECK_EQUAL(
	tx.exec1("SELECT 9")[0].as<int>(),
	9,
	"Simple query went wrong.");
  PQXX_CHECK_THROWS(
	tx.exec("SELECT 1/0"),
	pqxx::sql_error,
	"Expected error did not happen.");

  PQXX_CHECK_EQUAL(
	tx.exec1("SELECT 5")[0].as<int>(),
	5,
	"Wrong result after error.");
}


const std::string table = "pqxx_test_transaction";


void delete_temp_table(pqxx::transaction_base &tx)
{
  tx.exec0("DROP TABLE IF EXISTS " + table);
}


void create_temp_table(pqxx::transaction_base &tx)
{
  tx.exec0("CREATE TEMP TABLE " + table + " (x integer)");
}


void insert_temp_table(pqxx::transaction_base &tx, int value)
{
  tx.exec0(
    "INSERT INTO " + table + " (x) VALUES (" + pqxx::to_string(value) + ")");
}

int count_temp_table(pqxx::transaction_base &tx)
{
  return tx.exec1("SELECT count(*) FROM " + table)[0].as<int>();
}


template<typename TX>
void test_db_transaction_rolls_back()
{
  pqxx::connection c;
  pqxx::nontransaction tx1{c};
  delete_temp_table(tx1);
  create_temp_table(tx1);
  tx1.commit();

  TX tx2{c};
  insert_temp_table(tx2, 32);
  tx2.abort();

  pqxx::nontransaction tx3{c};
  PQXX_CHECK_EQUAL(
	count_temp_table(tx3),
	0,
	"Abort on " + tx3.classname() + " did not roll back.");
  delete_temp_table(tx3);
}


void test_nontransaction_autocommits()
{
  pqxx::connection c;

  pqxx::nontransaction tx1{c};
  delete_temp_table(tx1);
  create_temp_table(tx1);
  tx1.commit();

  pqxx::nontransaction tx2{c};
  insert_temp_table(tx2, 4);
  tx2.abort();

  pqxx::nontransaction tx3{c};
  PQXX_CHECK_EQUAL(
	count_temp_table(tx3),
	1,
	"Did not keep effect of aborted nontransaction.");
  delete_temp_table(tx3);
}


template<typename TX> void test_double_close()
{
  pqxx::connection c;

  TX tx1{c};
  tx1.exec1("SELECT 1");
  tx1.commit();
  tx1.commit();

  TX tx2{c};
  tx2.exec1("SELECT 2");
  tx2.abort();
  tx2.abort();

  TX tx3{c};
  tx3.exec1("SELECT 3");
  tx3.commit();
  PQXX_CHECK_THROWS(
	tx3.abort(),
	pqxx::usage_error,
	"Abort after commit not caught.");;

  TX tx4{c};
  tx4.exec1("SELECT 4");
  tx4.abort();
  PQXX_CHECK_THROWS(
	tx4.commit(),
	pqxx::usage_error,
	"Commit after abort not caught.");
}


void test_transaction()
{
  test_nontransaction_continues_after_error();
  test_nontransaction_autocommits();
  test_double_close<pqxx::transaction<>>();
  test_double_close<pqxx::read_transaction>();
  test_double_close<pqxx::nontransaction>();
  test_double_close<pqxx::robusttransaction<>>();
}


PQXX_REGISTER_TEST(test_transaction);
} // namespace

#include "../test_helpers.hxx"

namespace
{
void test_pipeline()
{
  pqxx::connection conn;
  pqxx::work tx{conn};

  // A pipeline grabs transaction focus, blocking regular queries and such.
  pqxx::pipeline pipe(tx, "test_pipeline_detach");
  PQXX_CHECK_THROWS(
	tx.exec("SELECT 1"),
	std::logic_error,
	"Pipeline does not block regular queries");

  // Flushing a pipeline relinquishes transaction focus.
  pipe.flush();
  auto r = tx.exec("SELECT 2");
  PQXX_CHECK_EQUAL(r.size(), 1u, "Wrong query result after flushing pipeline.");
  PQXX_CHECK_EQUAL(
    r[0][0].as<int>(),
    2,
    "Query returns wrong data after flushing pipeline.");

  // Inserting a query makes the pipeline grab transaction focus back.
  pqxx::pipeline::query_id q = pipe.insert("SELECT 2");
  PQXX_CHECK_THROWS(
	tx.exec("SELECT 3"),
	std::logic_error,
	"Pipeline does not block regular queries");

  // Invoking complete() also detaches the pipeline from the transaction.
  pipe.complete();
  r = tx.exec("SELECT 4");
  PQXX_CHECK_EQUAL(r.size(), 1u, "Wrong query result after complete().");
  PQXX_CHECK_EQUAL(
    r[0][0].as<int>(),
    4,
    "Query returns wrong data after complete().");

  // The complete() also received any pending query results from the backend.
  r = pipe.retrieve(q);
  PQXX_CHECK_EQUAL(r.size(), 1u, "Wrong result from pipeline.");
  PQXX_CHECK_EQUAL(
    r[0][0].as<int>(),
    2,
    "Pipeline returned wrong data.");

  // We can cancel while the pipe is empty, and things will still work.
  pipe.cancel();

  // Issue a query and cancel it.
  pipe.retain(0);
  pipe.insert("pg_sleep(10)");
  pipe.cancel();
  // TODO: Measure time to ensure we don't really wait.
}
} // namespace

PQXX_REGISTER_TEST(test_pipeline);

/** Implementation of the pqxx::notification_receiever class.
 *
 * pqxx::notification_receiver processes notifications.
 *
 * Copyright (c) 2000-2019, Jeroen T. Vermeulen.
 *
 * See COPYING for copyright license.  If you did not receive a file called
 * COPYING with this source code, please notify the distributor of this mistake,
 * or contact the author.
 */
#include "pqxx/compiler-internal.hxx"

#include <string>

#include "pqxx/internal/gates/connection-notification_receiver.hxx"

#include "pqxx/notification"


pqxx::notification_receiver::notification_receiver(
	connection &c,
	const std::string &channel_name) :
  m_conn{c},
  m_channel{channel_name}
{
  pqxx::internal::gate::connection_notification_receiver{c}.add_receiver(this);
}


pqxx::notification_receiver::~notification_receiver()
{
  pqxx::internal::gate::connection_notification_receiver{
	this->conn()
	}.remove_receiver(this);
}

#include "ConnectorHandler.h"

namespace dtests {
namespace proton {
namespace reactor {
	
using namespace dtests::common;
using namespace dtests::common::log;

ConnectorHandler::ConnectorHandler(const string &url)
	: super(url)
{
	logger(debug) << "Initializing the connector handler";
}

ConnectorHandler::~ConnectorHandler()
{
}

void ConnectorHandler::on_start(event &e) {
	logger(debug) << "Starting messaging handler";
	e.container().connect(broker_url);
}


void ConnectorHandler::on_connection_opened(event& e) {
	logger(debug) << "Connected to " << broker_url.host_port();

	connection &conn = e.connection();
	
	logger(debug) << "Closing connection";
	conn.close();
	logger(debug) << "Connection closed";
}

} /* namespace reactor */
} /* namespace proton */
} /* namespace dtests */

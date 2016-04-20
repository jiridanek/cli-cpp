#include "ConnectorHandler.h"

namespace dtests {
namespace proton {
namespace reactor {

using namespace dtests::common;
using namespace dtests::common::log;

ConnectorHandler::ConnectorHandler(const string &url, int timeout)
    : super(url, timeout),
      objectControl(CONNECTION)
{
    logger(debug) << "Initializing the connector handler";

}

ConnectorHandler::~ConnectorHandler()
{
    closeObjects();
}

void ConnectorHandler::setCount(int count)
{
    this->count = count;
}

int ConnectorHandler::getCount() const
{
    return count;
}

void ConnectorHandler::on_start(event &e)
{
    logger(debug) << "Starting messaging handler";

    logger(trace) << "Creating connection";

    conn = e.container().connect(broker_url);
    
    if ((objectControl & SESSION)) {
        logger(trace) << "Creating the session as requested";
        sessionObj = conn.default_session();
    }
    
    if ((objectControl & SENDER)) {
        logger(trace) << "Opening the sender as requested";
        senderObj = conn.open_sender(broker_url.host_port());
    }
    
    if ((objectControl & RECEIVER)) {
        logger(trace) << "Opening the receiver as requested";
        receiverObj = conn.open_receiver(broker_url.host_port());
    }
    
    logger(debug) << "Setting up timeout";
    task t = e.container().schedule(1000);
    timeoutTask = &t;
}

void ConnectorHandler::on_connection_open(event& e)
{
    logger(debug) << "Connected to " << broker_url.host_port();
    
    if ((objectControl & SESSION)) {
        logger(trace) << "Opening the session as requested";
        sessionObj.open();
    }
}


void ConnectorHandler::on_connection_close(event& e)
{
    logger(debug) << "Closing the connection to " << broker_url.host_port();
    
    super::disableTimer();
}

void ConnectorHandler::on_connection_error(event &e)
{
    logger(error) << "Failed to connect to " << broker_url.host_port();
}

void ConnectorHandler::on_connection_local_open(event& e)
{
    logger(debug) << "Local connection opened to " << broker_url.host_port();
}

void ConnectorHandler::on_connection_remote_open(event& e)
{
    logger(debug) << "Remote connection opened to " << broker_url.host_port();
}


void ConnectorHandler::on_transport_error(event &e) {
    logger(error) << "The connection with " << broker_url.host_port() << 
            " was interrupted";
    closeObjects();
}


void ConnectorHandler::on_session_error(event &e) {
    logger(error) << "The remote peer at " << broker_url.host_port() << 
            " closed the session with an error condition";
    closeObjects();
} 


void ConnectorHandler::on_link_error(event &e) {
    logger(error) << "The remote peer at " << broker_url.host_port() << 
            " closed the link with an error condition";
    closeObjects();
}

void ConnectorHandler::setObjectControl(int control) {
    objectControl = control;
}

void ConnectorHandler::closeObjects() {
    if ((objectControl & RECEIVER)) {
        logger(trace) << "Closing the receiver";
        receiverObj.close();
    }
    
    if ((objectControl & SENDER)) {
        logger(trace) << "Closing the sender";
        senderObj.close();
    }
    
    if ((objectControl & SESSION)) {
        logger(trace) << "Closing the session (currently ignored)";
        
        // This is causing problems in the compilation, and, therefore is 
        // temporarily disabled. 
        // TODO: check if this is some problem due to the API not being stable
        // sessionObj.close();
    }
    
    conn.close();
}

void ConnectorHandler::on_timer(event &e)
{
   super::timerEvent(e);
   
}


} /* namespace reactor */
} /* namespace proton */
} /* namespace dtests */

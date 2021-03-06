/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ConnectingClient.cpp
 * Author: opiske
 * 
 * Created on October 14, 2015, 1:24 PM
 */

#include "ConnectingClient.h"

using proton::container;

namespace dtests {
namespace proton {
namespace reactor {

using dtests::common::UriParser;

ConnectingClient::ConnectingClient()
    : super()
{
}

ConnectingClient::~ConnectingClient()
{
}

void ConnectingClient::setMessageHandlerOptions(const OptionsSetter &setter,
        ConnectorHandler &handler) const
{
    setter.setNumber("count", &handler, &ConnectorHandler::setCount, 1);
}

int ConnectingClient::setConnectionOptions(const optparse::Values &options) const
{
    string value = options["obj-ctrl"];
    int connControl = 0;
    
    size_t pos = value.find("C");
    if (pos != string::npos) {
        connControl |= CONNECTION; 
    }
    
    pos = value.find("E");
    if (pos != string::npos) {
        connControl |= SESSION;
    }
    
    pos = value.find("S");
    if (pos != string::npos) {
        connControl |= SENDER;
    }
    
    pos = value.find("R");
    if (pos != string::npos) {
        connControl |= RECEIVER;
    }
    
    return connControl;
}

int ConnectingClient::run(int argc, char** argv) const
{
    const string usage = "usage: %prog [OPTION]... DIR [FILE]...";
    const string version = "1.0";
    const string desc = "C/C++ AMQ reactive API connecting client for Qpid Proton";

    ConnectorOptionsParser parser = ConnectorOptionsParser();
    UriParser uri_parser = UriParser();

    /**
     * WARNING: do not reassign the result of chainned calls to usage/version/etc
     *  because it causes the code to segfault. For example, this crashes:
     *
     *  ControlOptions parser = ControlOptions().usage(usage)
     */
    parser.usage(usage).version(version).description(desc);

    optparse::Values options = parser.parse_args(argc, argv);

    parser.validate(options);

    setLogLevel(options);

    // Temporary variable for address, will search for prefix
    string tempAddress = options["broker-url"];
    // Variable for final address
    string address;
    // Variable for recognition of topic
    bool is_topic = false;
    // Search for prefix
    std::size_t prefix_index = tempAddress.find("topic://");
    // If prefix is present
    if (prefix_index != std::string::npos) {
        // Delete prefix
        address = tempAddress.replace(prefix_index, 8, "");
        // Set that it will be topic
        is_topic = true;
    // If prefix is NOT present
    } else {
        // Use full address
        address = tempAddress;
    }

    uri_parser.parse(options["broker-url"]);

    std::vector<std::string> conn_urls;
    if (options.is_set("conn-urls")) {
        std::stringstream conn_urls_string(options["conn-urls"]);
        std::string segment;

        while(std::getline(conn_urls_string, segment, ',')) {
           conn_urls.push_back(segment);
        }
    }

    string user = "";
    if (options.is_set("user")) {
        user = options["user"];
    } else {
        user = uri_parser.getUser();
    }
    
    string password = "";
    if (options.is_set("password")) {
        password = options["password"];
    } else {
        password = uri_parser.getPassword();
    }
    
    string sasl_mechanisms = "";
    if (options.is_set("sasl-mechanisms")) {
        sasl_mechanisms = options["sasl-mechanisms"];
    } else if (user != "" && password != "") {
        sasl_mechanisms = "PLAIN";
    } else {
        sasl_mechanisms = "ANONYMOUS";
    }
    
    string conn_sasl_enabled = "true";
    if (options.is_set("conn-sasl-enabled")) {
        conn_sasl_enabled = options["conn-sasl-enabled"];
        std::transform(conn_sasl_enabled.begin(), conn_sasl_enabled.end(), conn_sasl_enabled.begin(), ::tolower);
    }

    string conn_ssl_certificate = "";
    if (options.is_set("conn-ssl-certificate")) {
        conn_ssl_certificate = options["conn-ssl-certificate"];
    }

    string conn_ssl_private_key = "";
    if (options.is_set("conn-ssl-private-key")) {
        conn_ssl_private_key = options["conn-ssl-private-key"];
    }

    string conn_ssl_password = "";
    if (options.is_set("conn-ssl-password")) {
        conn_ssl_password = options["conn-ssl-password"];
    }

    string conn_ssl_trust_store = "";
    if (options.is_set("conn-ssl-trust-store")) {
        conn_ssl_trust_store = options["conn-ssl-trust-store"];
    }

    bool conn_ssl_verify_peer = options.is_set("conn-ssl-verify-peer");

    bool conn_ssl_verify_peer_name = options.is_set("conn-ssl-verify-peer-name");

    bool conn_reconnect_custom = false;

    string conn_reconnect = "true";
    if (options.is_set("conn-reconnect")) {
        conn_reconnect = options["conn-reconnect"];
        std::transform(conn_reconnect.begin(), conn_reconnect.end(), conn_reconnect.begin(), ::tolower);
    }

    int32_t conn_reconnect_interval = 10;
    if (options.is_set("conn-reconnect-interval")) {
        conn_reconnect_interval = std::strtol(options["conn-reconnect-interval"].c_str(), NULL, 10);

        conn_reconnect_custom = true;
    }

    int32_t conn_reconnect_limit = 0;
    if (options.is_set("conn-reconnect-limit")) {
        conn_reconnect_limit = std::strtol(options["conn-reconnect-limit"].c_str(), NULL, 10);

        conn_reconnect_custom = true;
    }

    int32_t conn_reconnect_timeout = duration::FOREVER.milliseconds();
    if (options.is_set("conn-reconnect-timeout")) {
        conn_reconnect_timeout = std::strtol(options["conn-reconnect-timeout"].c_str(), NULL, 10);

        conn_reconnect_custom = true;
    }

    uint32_t conn_reconnect_first = 0;
    if (options.is_set("conn-reconnect-first")) {
        conn_reconnect_first = std::strtoul(options["conn-reconnect-first"].c_str(), NULL, 10);

        conn_reconnect_custom = true;
    }

    uint32_t conn_reconnect_increment = 100;
    if (options.is_set("conn-reconnect-increment")) {
        conn_reconnect_increment = std::strtoul(options["conn-reconnect-increment"].c_str(), NULL, 10);

        conn_reconnect_custom = true;
    }

    bool conn_reconnect_doubling = true;
    if (options.is_set("conn-reconnect-doubling")) {
        if (options["conn-reconnect-doubling"] == "false") {
            conn_reconnect_doubling = false;
        }

        conn_reconnect_custom = true;
    }

    uint32_t conn_heartbeat = 0;
    if (options.is_set("conn-heartbeat")) {
        conn_heartbeat = std::strtoul(options["conn-heartbeat"].c_str(), NULL, 10);
    }

    uint32_t max_frame_size = -1;
    if (options.is_set("conn-max-frame-size")) {
        max_frame_size = std::strtoul(options["conn-max-frame-size"].c_str(), NULL, 10);
    }

    OptionsSetter setter = OptionsSetter(options);
    
    int timeout = 0;
    if (options.is_set("timeout")) {
        timeout = static_cast<int> (options.get("timeout"));
    }

    bool conn_use_config_file = options.is_set("conn-use-config-file");

    ConnectorHandler handler = ConnectorHandler(
        address,
        conn_urls,
        is_topic,
        user,
        password,
        sasl_mechanisms,
        conn_sasl_enabled,
        conn_ssl_certificate,
        conn_ssl_private_key,
        conn_ssl_password,
        conn_ssl_trust_store,
        conn_ssl_verify_peer,
        conn_ssl_verify_peer_name,
        timeout,
        conn_reconnect,
        conn_reconnect_interval,
        conn_reconnect_limit,
        conn_reconnect_timeout,
        conn_reconnect_first,
        conn_reconnect_increment,
        conn_reconnect_doubling,
        conn_reconnect_custom,
        conn_heartbeat,
        max_frame_size,
        conn_use_config_file
    );
    setMessageHandlerOptions(setter, handler);
    
    int connControl = setConnectionOptions(options);
    handler.setObjectControl(connControl);

    try {    
        container(handler).run();

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "error: " << e.what() << std::endl;
    }

    return 1;
}

} /* namespace reactor */
} /* namespace proton */
} /* namespace dtests */


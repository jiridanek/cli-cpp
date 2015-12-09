/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ReactorDecoder.cpp
 * Author: opiske
 * 
 * Created on October 22, 2015, 12:03 PM
 */

#include "ReactorDecoder.h"

using namespace dtests::proton::reactor;
using namespace dtests::common;
using namespace dtests::common::log;

Logger ReactorDecoder::logger = LoggerWrapper::getLogger();

ReactorDecoder::ReactorDecoder(message &msg)
    : super(),
    m(msg)
{
}

ReactorDecoder::ReactorDecoder(const ReactorDecoder& orig)
{
}

ReactorDecoder::~ReactorDecoder()
{
}

/**
 * Writes a header property.
 * @param writer the writer to use
 * @param property the header property to write
 * @param reader The reader to use. The reader is a pointer to a member function 
 * that can access the header property data (ie.: the class getter).
 */
void ReactorDecoder::write(Writer *writer, HeaderProperty property, StringReader reader) const
{
    string value = (m.*reader)();

    writer->write(KeyValue(property.name, super::decodeValue(value)));
}

/**
 * Writes a header property.
 * @param writer the writer to use
 * @param property the header property to write
 * @param reader The reader to use. The reader is a pointer to a member function 
 * that can access the header property data (ie.: the class getter).
 */
void ReactorDecoder::write(Writer *writer, HeaderProperty property, DataReader reader) const
{
    const data &value = (m.*reader)();

    logger(debug) << "Decoding " << property.name << ": ";
    writer->write(KeyValue(property.name, this->decodeValue(value)));
}

/**
 * Writes a header property.
 * @param writer the writer to use
 * @param property the header property to write
 * @param reader The reader to use. The reader is a pointer to a member function 
 * that can access the header property data (ie.: the class getter).
 */
void ReactorDecoder::write(Writer *writer, HeaderProperty property, MessageIdReader reader) const
{
    const message_id value = (m.*reader)();

    logger(debug) << "Decoding " << property.name << ": ";

    amqp_string str;

    if (!value.empty()) {
        value.get(str);
    } else {
        logger(debug) << "Value for property " << property.name << " is empty";
    }

    writer->write(KeyValue(property.name, this->decodeValue(str)));
}

/**
 * Writes the TTL.
 * @param writer the writer to use
 */
void ReactorDecoder::writeTTL(Writer *writer) const
{
    pn_timestamp_t ttl = m.expiry_time().milliseconds;

    writer->write(KeyValue(MessageHeader::TTL.name,
            super::decodeValue(ttl)));
}

/**
 * Writes the content size for plain text messages
 * @param writer the writer to use
 */
void ReactorDecoder::writeContentSize(Writer *writer) const
{

    if (m.content_type() != ContentType::TEXT_PLAIN) {
        writer->write(KeyValue(MessageHeader::CONTENT_SIZE.name, "None"));

        return;
    } else {
        string body = decodeValue(m.body());
        string len = super::decodeValue(body.length());

        writer->write(KeyValue(MessageHeader::CONTENT_SIZE.name,
                len));

        return;
    }
}

void ReactorDecoder::decodeHeader(Writer *writer) const
{
    // write(writer, MessageHeader::REDELIVERED, &message::redelivered);
    write(writer, MessageHeader::REPLY_TO,
            static_cast<StringReader> (&message::reply_to));
    write(writer, MessageHeader::SUBJECT,
            static_cast<StringReader> (&message::subject));
    write(writer, MessageHeader::CONTENT_TYPE,
            static_cast<StringReader> (&message::content_type));
    write(writer, MessageHeader::USER_ID,
            static_cast<StringReader> (&message::user_id));
    write(writer, MessageHeader::ID,
            static_cast<MessageIdReader> (&message::id));
    write(writer, MessageHeader::CORRELATION_ID,
            static_cast<MessageIdReader> (&message::correlation_id));

    // Not implemented
    // write(writer, MessageHeader::PRIORITY, &message::priority);
    // write(writer, MessageHeader::DURABLE, &message::durable);
    writeTTL(writer);
    writeContentSize(writer);
}

void ReactorDecoder::decodeProperties(Writer *writer) const
{
    logger(debug) << "Decoding message properties";
    writer->startProperties();
    decodeValue(writer, m.properties());
    writer->endProperties();
}

void ReactorDecoder::decodeValue(Writer *writer, decoder &dec) const
{
    type_id type = dec.type();
    start s;
    
    switch (type) {
        case ARRAY:
        case DESCRIBED: 
            logger(debug) << "(o) Type id: " << type;
            break;

        case LIST: {
            logger(debug) << "(m) Type id: " << type;
        
            writer->startList();
            logger(debug) << "(m) Type id: " << type;
            dec >> s;
            
            logger(debug) << "(m) Size: " << s.size;
            for (size_t i = 0; i < s.size; i++) {
                decodeValue(writer, dec);
                writer->write(": ");
                
                if (i < s.size) {
                    writer->endField();
                }
            }
            
            dec >> finish();
            writer->endMap();        
        }
        case MAP:
        {
            writer->startMap();
            logger(debug) << "(m) Type id: " << type;
            dec >> s;
            
            logger(debug) << "(m) Size: " << s.size;
            for (size_t i = 0; i < s.size/2; i++) {
                decodeValue(writer, dec);
                writer->separate();
                decodeValue(writer, dec);
                
                if (i < ((s.size/2) - 1)) {
                    writer->endField();
                }
            }
            
            dec >> finish();
            writer->endMap();
        }
        default:
        {
            if (!dec.more()) {
                logger(debug) << "No more data to read";

                break;
            }
            std::ostringstream stream;
            
            value v;
            dec >> v;
            stream << v;

            writer->write(stream.str());            
        }
    }
}

string ReactorDecoder::decodeValue(const data &d) const
{
    std::ostringstream stream;
    if (d.empty()) {
        logger(debug) << "Empty data!";
    }

    try {
        stream << d;
    } catch (const std::exception& e) {
        logger(warning) << e.what();
    }

    return stream.str();
}


void ReactorDecoder::decodeValue(Writer *writer, const data &d) const
{
    try {
        logger(debug) << "Decoding a map";
        decoder dec = const_cast<data &> (d).decoder();
        decodeValue(writer, dec);
    } catch (const std::exception& e) {
        logger(warning) << e.what();
    }
}

string ReactorDecoder::decodeValue(const amqp_string &str) const
{
    std::ostringstream stream;

    stream << str.c_str();

    return stream.str();
}

void ReactorDecoder::decodeContent(Writer *writer) const
{
    if (m.body().type() == MAP) {
        logger(debug) << "Decoding a map message";
    }

    string content = decodeValue(m.body());


    writer->write(content);
}
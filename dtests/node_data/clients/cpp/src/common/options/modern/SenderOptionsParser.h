/*
 * ControlOptions.h
 *
 *  Created on: Apr 16, 2015
 *      Author: opiske
 */

#ifndef DTESTS_NODE_DATA_CLIENTS_LANG_CPP_COMMON_OPTIONS_MODERN_SENDEROPTIONSPARSER_H_
#define DTESTS_NODE_DATA_CLIENTS_LANG_CPP_COMMON_OPTIONS_MODERN_SENDEROPTIONSPARSER_H_

#include <optparse/OptionParser.h>

#include "options/modern/ModernOptionsParser.h"


/**
 * SenderOptionsParser offers a common interface for implementing command-line 
 * options parsers for both newer sender C++ clients
 */
class SenderOptionsParser: public ModernOptionsParser {
public:
	SenderOptionsParser();


	virtual ~SenderOptionsParser();

	virtual void validate(const Values &options) const;

private:
	typedef ModernOptionsParser super;
};

#endif /* DTESTS_NODE_DATA_CLIENTS_LANG_CPP_COMMON_OPTIONS_MODERN_SENDEROPTIONSPARSER_H_ */

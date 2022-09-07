/**
 * \file plogger.hpp
 * Implementations that is required by actrepo.
 *
 * Copyright 2012..2022 Cloud Avid Co. (www.cloudavid.com)
 * \author Hamid Jafarian (hamid.jafarian@cloudavid.com)
 *
 * plogger is a part of actrepo.
 *
 * actrepo is free software: you can redistribute it and/or modify it
 * under the terms of GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the license, or
 * (at your option) any later version.
 *
 * You should have received a copy of the GNU General Public License
 * along with actrepo. If not, see <http://www.gnu.org/licenses/>
 */
#pragma once

#include <plogger/plogger.hpp>
#include <stdarg.h>
#include <string>
using plogger::PLogger;
using plogger::Severity;

#include <plogger/translator.hpp>

/* Name of actrepo module */
#define ACTREPO_MODULE "actrepo"

namespace actrepo
{

enum ELogID
{
    L_CLIENT_CONNECTED = PLOGGER_FIRST_LOGID,
    L_CLIENT_DISCONNECTED,
    L_FIRE_CALLED,
    L_USER_COMMAND,
    L_ACTREPO_BAD_ACTION,
    L_ACTREPO_BAD_MODULE
};

/**
 * \class Translator
 * Putil logID translator.
 */
class Translator : public plogger::Translator
{
public:
    virtual string getLogFormat(plogger::LogID logID);
    virtual string getModule();
};

/**
 * Putil translator.
 */
extern Translator PutilTranslator;

/**
 * Would be used by PLOG Macro to aquire actrepo translator.
 */
Translator *PLoggerTranslator();

} // namespace actrepo

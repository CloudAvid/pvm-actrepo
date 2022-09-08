#include "plogger.hpp"

namespace actrepo
{
Translator PutilTranslator;

/* Implementation of Translator Class.
 */
string Translator::getLogFormat(plogger::LogID logID)
{
    switch (logID) {
    case L_CLIENT_CONNECTED:
        return "Client connected, address: %s, port: %d.";
    case L_CLIENT_DISCONNECTED:
        return "Client disconnected, address: %s, port: %d.";
    case L_FIRE_CALLED:
        return "Fire called for: %s.";
    case L_USER_COMMAND:
        return "User command is: %d:%d";
    case L_ACTREPO_BAD_ACTION:
        return "Bad action id.";
    case L_ACTREPO_BAD_MODULE:
        return "Bad module id.";
    default:
        return PLOGGER_NONE;
    };
    return PLOGGER_NONE;
}

string Translator::getModule()
{
    return ACTREPO_MODULE;
}

Translator *PLoggerTranslator()
{
    return &PutilTranslator;
}

} // namespace actrepo

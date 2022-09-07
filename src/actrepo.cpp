#include "actrepo.hpp"

namespace actrepo
{
/* Implementation of ActionList Class.
 */
string ActionList::run(XParam::XInt cmdID, ActionSource::Type st, const XParam::XmlNode *rnode,
                       void *data)
{
    PLogger::threadInfo(getModule(), getActionName(cmdID));
    PLogger::threadInfo(plogger::ThreadInfo::TI_ACTION, getActionName(cmdID));
    PLogger::setMode(plogger::ThreadRecorder::TRM_REAL);
    PLogger::setBroadcast(true);
    CALL_FUNCTION;
    try {
        string ret = Actions.at(cmdID)(st, rnode, data);
        EXIT_FUNCTION_RETURN(ret);
    } catch (std::out_of_range &oor) {
        EXIT_FUNCTION_THROW(L_ACTREPO_BAD_ACTION);
    } catch (std::exception &e) {
        EXIT_FUNCTION_THROW_EXCEPTION(Exception(e.what(), TracePoint("action-list")));
    }
    EXIT_FUNCTION;
}

void ActionList::push_action(FT_action act)
{
    CALL_FUNCTION;
    Actions.push_back(act);
    EXIT_FUNCTION;
}

string ActionList::getModule()
{
    CALL_FUNCTION;
    EXIT_FUNCTION_RETURN(PLOGGER_NONE);
}

string ActionList::getActionName(XParam::XInt cmdID)
{
    CALL_FUNCTION;
    EXIT_FUNCTION_RETURN(PLOGGER_NONE);
}

/* Implementation of ActionRepository Class.
 */
LogSystem ActionRepository::log("actrepo");
vector<ActionList *> ActionRepository::SSysActions;

void ActionRepository::init(XParam::XInt ssysNO)
{
    CALL_FUNCTION;
    SSysActions.insert(SSysActions.begin(), ssysNO, (ActionList *) NULL);
    EXIT_FUNCTION;
}

void ActionRepository::regActList(XParam::XInt SysID, ActionList *actlist)
{
    CALL_FUNCTION;
    try {
        SSysActions.at(SysID) = actlist;
    } catch (std::out_of_range &oor) {
        EXIT_FUNCTION_THROW(L_ACTREPO_BAD_MODULE);
    }
    EXIT_FUNCTION;
}

void ActionRepository::unregActList(XParam::XInt SysID)
{
    CALL_FUNCTION;
    try {
        SSysActions.at(SysID) = NULL;
    } catch (std::out_of_range &oor) {
        EXIT_FUNCTION_THROW(L_ACTREPO_BAD_MODULE);
    }
    EXIT_FUNCTION;
}

string ActionRepository::runCmd(XParam::XInt sid, XParam::XInt cid, ActionSource::Type st,
                                const XParam::XmlNode *rnode, void *data)
{
    CALL_FUNCTION;
    try {
        if (SSysActions.at(sid) != NULL) {
            EXIT_FUNCTION_RETURN(SSysActions[sid]->run(cid, st, rnode, data));
        } else
            EXIT_FUNCTION_THROW(L_ACTREPO_BAD_MODULE);
    } catch (std::out_of_range &oor) {
        EXIT_FUNCTION_THROW(L_ACTREPO_BAD_MODULE);
    } catch (std::exception &e) {
        EXIT_FUNCTION_THROW_EXCEPTION(Exception(e.what(), TracePoint("action_list")));
    }
    EXIT_FUNCTION;
}

} // namespace actrepo

/**
 * \file actrepo.hpp
 * Action repository manages action list of sub-systems.
 *
 * Commands are defined by an "id" and a "system id - sid" which respond to
 * program's subsystems. Each subsystem at load time, should register
 * his commands in the action repository.
 * Other program parts may call this actions base on user requests to
 * communicate with sub-systems.
 *
 * Copyright 2011,2022 Cloud Avid Co. (www.cloudavid.com)
 * \author Hamid Jafarian (hamid.jafarian@cloudavid.com)
 *
 * actrepo is part of pvm-actrepo.
 *
 * pvm-acrepo is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * pvm-acrepo is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with pvm-actrepp.  If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

#include "config.h"
#include "plogger.hpp"

#include <putil/cmd.hpp>

using namespace putil;

namespace actrepo
{

/**
 * \class ActionSource
 * @brief Defines the caller of action.
 *
 */
class ActionSource
{
public:
    enum Type
    {
        FIRELOOP = 0x01, /**<fireloop - user communication interface*/
        SBALLNM = 0x02,  /**<Sball Node Manager subsystem */
        SBALLTM = 0x04,  /**<Sball Task Manager subsystem */
    };
};

/**
 * \class ActionList
 * @brief Defines list of actions that corresponds to user commands in sub-systems.
 *
 * @note Each sub-system should define a public inherited class(Child) from
 * "ActionList" and also defines his actions as static member functions.
 * @note Then in his constructor, all of this actions should be inserted in "Actions"
 * vector base on their "Command IDs - CmdID".
 * For Example:
 *
 * @code
 * class SubSysActionList : buplic ActionList
 * {
 * public:
 * 	SubSysActionList()
 * 	{
 * 		push_action(&id0Action); // CmdID = 0
 *		// and push other Actions
 * 	}
 * 	static string id0Action(const XParam::XmlNode *rnode)
 * 						throw (Exception)
 * 	{
 * 		.... may throw Exception
 * 		return "result"
 * 	}
 * };
 * @endcode
 */
class ActionList
{
public:
    /**
     * @typedef FT_action
     * defines cmd prototype that would be fired in response to user
     * commands.
     * @param st Type of action source.
     * @param rnode pointer to root node of parsed xml-formatted command.
     * @param data associated data with rnode.
     */
    typedef string (*FT_action)(ActionSource::Type st, const XParam::XmlNode *rnode, void *data);

    /**
     * @brief Run requested user action on command id.
     * @param cmdID command id of requested action.
     * @param rnode pointer to root node of parsed xml-formatted command.
     * @param data associated data with rnode.
     */
    string run(XParam::XInt cmdID, ActionSource::Type st, const XParam::XmlNode *rnode, void *data);

protected:
    /**
     * @brief Add new action at the end of Actions vector.
     * @param act new action.
     */
    void push_action(FT_action act);
    /**
     * @brief Returns module name of owner of this list.
     * @return module name of owner of this list.
     *
     * @warning All modules MUST develop this function to return their names.
     */
    virtual string getModule();

    /**
     * @brief Returns name of specified action.
     * @param cmdID command id of action.
     * @brief name of specified action.
     *
     * @warning All modules should develope this function to return action names in human readable
     * format.
     */
    virtual string getActionName(XParam::XInt cmdID);

protected:
    /**
     * @brief List of Commands.
     */
    vector<FT_action> Actions;
};

/**
 * \class ActionRepository.
 * @brief Manages action lists of aub-systems.
 */
class ActionRepository
{
public:
    /**
     * @brief Initialize Action Repositories environments.
     * @param ssysNO Maximum number of sub-systems.
     *
     * @note Would be called before any subsystem action list registeration.
     */
    static void init(XParam::XInt ssysNO);

    /**
     * @brief Register an action list for specified sub-system.
     * @param SysID Subsystem ID
     * @param actlist Action list
     */
    static void regActList(XParam::XInt SysID, ActionList *actlist);

    /**
     * @brief Unregister an action list for specified sub-system.
     * @param SysID Subsystem ID
     */
    static void unregActList(XParam::XInt SysID);

    /**
     * @brief Run the Specified command(cid) in defined sub-system(sid).
     * @param sid sub-system id
     * @param cid command id
     * @param st Action source.
     * @param data associated data with rnode.
     */
    static string runCmd(XParam::XInt sid, XParam::XInt cid, ActionSource::Type st,
                         const XParam::XmlNode *rnode, void *data);

private:
    /**
     * @brief looger system.
     */
    static LogSystem log;

    /**
     * @brief List of Actions List of SubSystems.
     */
    static vector<ActionList *> SSysActions;
};

} // namespace actrepo


/**
 * \file fireloop.hpp
 * Defines and manages process loop (as fire loop) of user commands.
 *
 * Based on user commands that are defined by an "command id - cid" and a
 * "system id - sid", fireloop would call respond action from
 * action repository.
 * Subsystems would register their actions in action repository at start-up.
 *
 * Copyright 2011-2022 Cloud Avid Co. (www.cloudavid.com)
 * \author Hamid Jafarian (hamid.jafarian@cloudavid.com)
 * \author Hamed Haji Husseini (hajihussaini@cloudavid.com)
 *
 * fireloop is part of pvm-actrepo.
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
 * along with pvm-actrepo.  If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

#include "actrepo.hpp"

#include <ipc/socket-client.hpp>
#include <ipc/socket-server.hpp>
#include <putil/epoll.hpp>

using namespace ipc::net;

namespace actrepo
{
/**
 * \struct Session.
 * @brief Defines a session between user and pvm.
 */
struct Session {
    /**
     * @brief Session constructor.
     * @param token initilize session's token.
     */
    Session(const std::string token);

    /**
     * @brief Session constructor.
     * @param sfd sesstion FD.
     * @param _socketAddress socket address.
     */
    Session(int sfd, struct sockaddr *_socketAddress);

    /**
     * @brief Responses to client and then closes the client connection
     */
    bool response_close;

    /**
     * @brief Socket file descriptor of opened session by user.
     */
    int socket_fd;

    /* Connection information */
    /**
     * @brief socket address.
     */
    struct sockaddr *socketAddress;

    /**
     * @brief IP.
     */
    string ip;

    /**
     * @brief Port.
     */
    int port;

    /**
     * @brief Length of Command.
     */
    unsigned int length;

    /**
     * @brief XML Formatted command sent by user over session.
     */
    const char *xml_cmd;

    /**
     * @brief XML Formatted command sent by user over session.
     */
    string _xml_cmd;

    /**
     * @brief Session's token.
     */
    std::string token;
};

/**
 * \class ResponseStatus.
 * @brief Responses status.
 */
class ResponseStatus
{
public:
    enum
    {
        SUCCESS,
        WARNING,
        FAILED,
        MAX
    };

    static const string typeString[MAX];
};

/**
 * \class Response
 */
class Response : public XMixParam
{
public:
    /**
     * @brief Default constructor.
     */
    Response();

    int get_status();
    void set_status(const int _status);
    string get_description();
    void set_description(const string _description);

private:
    /**
     * @brief Status of response.
     */
    XEnumParam<ResponseStatus> status;

    /**
     * @brief Description of response.
     */
    XTextParam description;
};

/**
 * \class FireLoop
 * @brief Manages process loop of user commands.
 */
class FireLoop
{
public:
    /**
     * @brief Initialize FireLoop environments.
     *
     * @note Would be called before any subsystem action list registeration.
     */
    static void init();
    /**
     * Get/Set the IP address
     * \param IP address to listen on
     */
    static string get_ip();
    static void set_ip(const string _ip);
    /**
     * Get/Set the port number
     * \param Port number to listen on
     */
    static int get_port();
    static void set_port(const int _port);
    /**
     * Get/Set the unix socket path.
     * Fireloop will listen on, and communicate with user through him.
     */
    static string get_unixSocket();
    static void set_unixSocket(string path);
    /**
     * Set Size of Communication buffer with user.
     */
    static void set_bufSize(unsigned int size);
    /**
     * @brief Listen to the unix socket and read user XML-formatted command.
     *
     * @note This method loops over user commands and calls fire() for
     * calling apprpriate action based on user command.
     */
    static void loop();

private:
    /**
     * @brief Accepts new connection.
     * @param epollData polling data associated with socket.
     * @param data Session's necessary data.
     */
    static void acceptSocket(EPoll::Data *epollData, void *data);

    /**
     * @brief Call appropriate action base on user command.
     * @param session user 's session information.
     *
     * @note Each fire would be run in a seprate thread.
     * fire() is thread function.
     * @note Threads would be created by this * function to response to user requests.
     */
    static void *fire(void *session);

    /**
     * @brief Sends failure message.
     * @param session User's session.
     * @param message Message.
     */
    static void fire_failed(Session *session, const string message);

    /**
     * @brief Handles proxy operations.
     * @param epollData polling data associated with socket.
     * @param data Session's necessary data.
     */
    static void processSocket(EPoll::Data *epollData, void *data);

    /**
     * @brief Send appropraite message to user when action excution
     * be successfull (run without any exception).
     * @param [in] des Description returned by action execution.
     */
    static void answer_ok(const Session *session, const string &des);

    /**
     * @brief Send appropriate message to user when action execution failed,
     * (exception throwed).
     * @param [in] e Exception throwed by action.
     */
    static void answer_failed(const Session *session, const Exception &e);

    /**
     * @brief Sends message to peer.
     * @param session User's session.
     * @param message Message.
     */
    static void writeResponse(const Session *session, const string response);

private:
    /**
     * @brief Fireloop logging system.
     */
    static LogSystem log;

    /**
     * @brief IP address to listen on for incoming users'commands
     */
    static string ip;

    /**
     * @brief Port number to listen on for incoming users'commands
     */
    static int port;

    /**
     * @brief The unix socket path to listen on for user communication.
     */
    static string unixSocketPath;

    /**
     * @brief Size of communication buffer.
     */
    static unsigned int bufSize;

    /**
     * @brief Server TCP socket.
     */
    static Server tcpSocket;

    /**
     * @brief Server UNIX socket.
     */
    static Server unixSocket;
};

} // namespace actrepo

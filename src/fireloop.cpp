#include "fireloop.hpp"

namespace actrepo
{

LogSystem FireLoop::log("fireloop");
string FireLoop::ip = "0.0.0.0";
int FireLoop::port = 7090;

string FireLoop::unixSocketPath;
unsigned int FireLoop::bufSize;

Server FireLoop::tcpSocket(SockDom::IPV4, SockType::TCP);

Server FireLoop::unixSocket(SockDom::UNIX, SockType::TCP);

const string ResponseStatus::typeString[ResponseStatus::MAX] = {
    "success", /* SUCCESS */
    "warning", /* WARNING */
    "failed"   /* FAILED */
};

/* Implementation of "Response" class */

Response::Response() :
    XMixParam("response"),
    status("status", ResponseStatus::SUCCESS),
    description("description")
{
    addParam(&status);
    addParam(&description);
}

int Response::get_status()
{
    return status.get_value();
}

void Response::set_status(const int _status)
{
    status = _status;
}

string Response::get_description()
{
    return description.value();
}

void Response::set_description(const string _description)
{
    description = _description;
}

/* Implementation of "Session" structure */

Session::Session(const std::string token) : token(token)
{
}
Session::Session(int sfd, struct sockaddr *_socketAddress) :
    socket_fd(sfd),
    socketAddress(_socketAddress),
    length(0)
{
    char _ip[INET_ADDRSTRLEN];
    void *address;

    if (socketAddress->sa_family == AF_INET) {
        address = &((struct sockaddr_in *) socketAddress)->sin_addr;
        port = ntohs(((struct sockaddr_in *) socketAddress)->sin_port);
    } else {
        address = &((struct sockaddr_in6 *) socketAddress)->sin6_addr;
        port = ntohs(((struct sockaddr_in6 *) socketAddress)->sin6_port);
    }
    /*
     * TODO Change the first arguemt to commented arguemtn after
     * deleting Unix socket
     * */
    inet_ntop(AF_INET, // socketAddress->sa_family,
              address, _ip, INET_ADDRSTRLEN);
    ip.assign(_ip);
}

/* Implementation of "FireLoop" class */

void FireLoop::init()
{
}

string FireLoop::get_ip()
{
    return ip;
}

void FireLoop::set_ip(const string _ip)
{
    ip = _ip;
}

int FireLoop::get_port()
{
    return port;
}

void FireLoop::set_port(const int _port)
{
    port = _port;
}

string FireLoop::get_unixSocket()
{
    return unixSocketPath;
}

void FireLoop::set_unixSocket(string path)
{
    unixSocketPath = path;
}

void FireLoop::set_bufSize(unsigned int size)
{
    bufSize = size;
}

void FireLoop::loop()
{
    EPoll epoll;
    const gid_t PVM_GROUP_ID = 3000;
    tcpSocket.setAddr(std::make_pair(ip, port));
    unixSocket.setAddr(unixSocketPath);

    try {
        tcpSocket.bind();
    } catch (Exception &e) {
        EXIT_FUNCTION_THROW_EXCEPTION(e);
    }

    try {
        unixSocket.bind();
    } catch (Exception &e) {
        tcpSocket.close();
        EXIT_FUNCTION_THROW_EXCEPTION(e);
    }

    chown(unixSocket.get_unixAddr().c_str(), 0, PVM_GROUP_ID);
    ::chmod(unixSocket.get_unixAddr().c_str(), 0664);

    try {
        tcpSocket.listen();
        unixSocket.listen();

        epoll.create();

        epoll.add(tcpSocket.get_fd(), EPoll::INPUT, acceptSocket, NULL);

        epoll.add(unixSocket.get_fd(), EPoll::INPUT, acceptSocket, NULL);

        while (true) {
            epoll.wait();
        }

    } catch (Exception &e) {
        tcpSocket.close();
        unixSocket.close();
        EXIT_FUNCTION_THROW_EXCEPTION(e);
    }
}

void FireLoop::acceptSocket(EPoll::Data *epollData, void *data)
{
    int socketDescriptor;
    pthread_attr_t threadAttribute;
    pthread_t threadID;
    Session *session;
    socklen_t socketLength;
    struct sockaddr clientAddress;

    if (! (epollData->events & EPoll::INPUT))
        return;

    if (pthread_attr_init(&threadAttribute) != 0) {
        log << LogLevel::ERROR << string("pthread_attr_init: ") + strerror(errno);

        return;
    }
    pthread_attr_setdetachstate(&threadAttribute, PTHREAD_CREATE_DETACHED);
    socketLength = sizeof(clientAddress);
    socketDescriptor = accept4(epollData->fileDescriptor, &clientAddress, &socketLength,
                               SOCK_CLOEXEC);
    if (socketDescriptor == -1) {
        log << LogLevel::ERROR << string("Failed to accpet connection - ") + strerror(errno);

        return;
    }
    try {
        session = new Session(socketDescriptor, &clientAddress);
        if (epollData->fileDescriptor == unixSocket.get_fd())
            session->response_close = false;
        else if (epollData->fileDescriptor == tcpSocket.get_fd())
            session->response_close = false;
        if (pthread_create(&threadID, &threadAttribute, fire, session)) {
            log << LogLevel::ERROR << string("Failed to create thread - ") + strerror(errno);
            close(socketDescriptor);
            delete session;
        }
    } catch (std::bad_alloc &exception) {
        log << LogLevel::ERROR << "Can't allocate session !";
        close(socketDescriptor);
    }
}

void *FireLoop::fire(void *_session)
{
    string response;
    string xmlCommand;
    Cmd command;
    EPoll epoll;
    Session *session = static_cast<Session *>(_session);
    XParam::XmlParser parser;

    PLogger::threadInfo(ACTREPO_MODULE, "fire");
    PLogger::setMode(plogger::ThreadRecorder::TRM_REAL);
    PLOG(Severity::VERBOSE, ELogID::L_CLIENT_CONNECTED, session->ip.c_str(), session->port);
    epoll.create();
    try {
        epoll.add(session->socket_fd, EPoll::INPUT, processSocket, _session);
        do {
            if (! epoll.wait())
                break;
        } while (! session->response_close);
    } catch (Exception &e) {
        answer_failed(session, e);
        PLOG(Severity::DEBUG, plogger::ELogID::L_INTERNAL_ERROR, e.xml().c_str());
        goto finalize;
    }
#ifdef __DEBUG__
    PLOG(Severity::VERBOSE, ELogID::L_FIRE_CALLED, session->_xml_cmd);
#endif
    try {
        command.loadXmlStr(session->_xml_cmd, &parser);
        PLOG(Severity::VERBOSE, ELogID::L_USER_COMMAND, command.get_sysID(), command.get_cmdID());
        PLogger::threadInfo(plogger::ThreadInfo::TI_TOKEN, command.get_token());
        session->token = command.get_token();
        response = ActionRepository::runCmd(command.get_sysID(), command.get_cmdID(),
                                            ActionSource::FIRELOOP,
                                            parser.get_document()->get_root_node(), session);
        answer_ok(session, response);
    } catch (Exception &exception) {
        answer_failed(session, exception);
        PLOG(Severity::DEBUG, plogger::ELogID::L_INTERNAL_ERROR, exception.xml().c_str());
    } catch (std::exception &exception) {
        Exception _exception(exception.what(), TracePoint("fireloop"));
        answer_failed(session, _exception);
        PLOG(Severity::DEBUG, plogger::ELogID::L_INTERNAL_ERROR, _exception.xml().c_str());
    }

finalize:
    //	epollData->remove = true;
    shutdown(session->socket_fd, SHUT_RDWR);
    close(session->socket_fd);
    PLOG(Severity::VERBOSE, ELogID::L_CLIENT_DISCONNECTED, session->ip.c_str(), session->port);
    delete session;

    PLogger::threadExit();
    pthread_exit(NULL);
}

void FireLoop::processSocket(EPoll::Data *epollData, void *data)
{
    char *buffer;
    char *endPointer;
    int bufferSize = 1024;
    int bytesRead;
    size_t position;
    string message;
    Session *session = static_cast<Session *>(data);

    if (! (epollData->events & EPoll::INPUT))
        return;

    try {
        buffer = new char[bufferSize + 1];
    } catch (std::bad_alloc &exception) {
        log << LogLevel::ERROR << "Can't allocate buffer!";
        fire_failed(session, "Can't allocate buffer!");

        return;
    }
    if ((bytesRead = read(epollData->fileDescriptor, buffer, bufferSize)) > 0) {
        /* An error occurred */
        if (bytesRead == -1) {
            log << LogLevel::ERROR << "Failed to read data: " + string(strerror(errno));
            epollData->remove = true;
            delete buffer;

            return;
        }
        /* The device is disconnected */
        if (bytesRead == 0) {
            epollData->remove = true;
            delete buffer;

            return;
        }
        buffer[bytesRead] = '\0';
        message.assign(buffer);
        if (! session->length) {
            position = message.find(':');
            if ((position != string::npos) && (position < 7)) {
                session->length = strtoul(message.substr(0, position).c_str(), &endPointer, 10);
                message.erase(0, position + 1);
            } else {
                session->response_close = true;
                log << LogLevel::ERROR
                    << "Failed to get the length of "
                       "the command or the length is too big";
                delete buffer;

                return;
            }
        }
        session->_xml_cmd += message;
        session->length -= message.length();
    }
    if ((signed int) session->length <= 0)
        session->response_close = true;
    delete buffer;
}

void FireLoop::fire_failed(Session *session, const string message)
{
    Exception exception(message, TracePoint("fireloop"));

    answer_failed(session, exception);
    close(session->socket_fd);
    delete session;

    pthread_exit(NULL);
}

void FireLoop::answer_ok(const Session *session, const string &des)
{
    Response response;

    response.set_description(des);
    writeResponse(session, response.xml());
}

void FireLoop::answer_failed(const Session *session, const Exception &e)
{
    Response response;

    if (e.is_nok())
        response.set_status(ResponseStatus::WARNING);
    else if (e.is_failed())
        response.set_status(ResponseStatus::FAILED);
    response.set_description(e.xml() + ((e.is_nok()) ? e.get_nokDesc() : ""));
    writeResponse(session, response.xml());
}

void FireLoop::writeResponse(const Session *session, const string response)
{
    char buffer[10];
    int bytesWritten;
    size_t length = response.length();
    string _response;

    snprintf(buffer, sizeof(buffer), "%ld:", length);
    _response = buffer + response;
    length = _response.length();
    while (length > 0) {
        bytesWritten = write(session->socket_fd, _response.c_str() + (_response.length() - length),
                             length);
        if (bytesWritten == -1)
            break;

        length -= bytesWritten;
    }
}

} // namespace actrepo

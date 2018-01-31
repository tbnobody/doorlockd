#include <errno.h>
#define LDAP_DEPRECATED 1
#include <ldap.h>

#include "logic.h"
#include "util.h"

Logic::Logic(const std::string &ldapUri,
             const std::string &bindDN,
             const std::string &webPrefix,
             const std::string &serDev,
             const unsigned int baudrate,
             const std::string &logfile_scripts) :
    _logger(Logger::get()),
    _door(serDev, baudrate, logfile_scripts),
    _ldapUri(ldapUri),
    _bindDN(bindDN),
    _webPrefix(webPrefix)
{
    _door.setDoorCallback(std::bind(&Logic::_doorCallback,
                                    this,
                                    std::placeholders::_1));
}

Logic::~Logic()
{
    _run = false;
}

Response Logic::processDoor(const DoorCommand &doorCommand)
{
    Response response;

    switch (doorCommand) {
        case DoorCommand::Lock:
            response = _lock();
            break;
        case DoorCommand::Unlock:
            response = _unlock();
            break;
        default:
            response.code = Response::Code::UnknownCommand;
            response.message = "Unknown DoorCommand";
            break;
    }

    return response;
}

Response Logic::request(const Request &request)
{
    std::unique_lock<std::mutex> l(_mutex);
    Response response;

    DoorCommand doorCommand;

    switch (request.command) {
        case Request::Command::Lock:
            doorCommand = DoorCommand::Lock;
            break;
        case Request::Command::Unlock:
            doorCommand = DoorCommand::Unlock;
            break;
        default:
            response.code = Response::Code::UnknownCommand;
            response.message = "Unknown Command";
            goto out;
    }

    response = _checkLDAP(request.user, request.password);
    if (!response) {
        goto out;
    }
    _logger(LogLevel::info, "   -> LDAP check successful");

    response = processDoor(doorCommand);
    _logger(LogLevel::info, "   -> Door Command successful");

out:
    return response;
}

Response Logic::_lock()
{
    Response response;
    if (_door.state() == Door::State::Locked)
    {
        response.code = Response::Code::AlreadyLocked;
        response.message = "Unable to lock: already closed";
    } else {
        response.code = Response::Code::Success;
        response.message = "Success";
    }

    _door.lock();

    return response;
}

Response Logic::_unlock()
{
    Response response;

    const auto oldState = _door.state();
    _door.unlock();

    if (oldState == Door::State::Unlocked)
    {
        response.code = Response::Code::AlreadyUnlocked;
        response.message = "Unable to unlock: already unlocked";
        _logger(response.message, LogLevel::info);
    } else {
        response.code = Response::Code::Success;
        response.message = "Success";
    }

    return response;
}

Response Logic::_checkLDAP(const std::string &user,
                           const std::string &password)
{
    constexpr int BUFFERSIZE = 1024;
    char buffer[BUFFERSIZE];
    Response retval;

    int rc = -1;
    LDAP* ld = nullptr;
    unsigned long version = LDAP_VERSION3;

    _logger(LogLevel::info, "      Trying to authenticate as user \"%s\"", user.c_str());
    snprintf(buffer, BUFFERSIZE, _bindDN.c_str(), user.c_str());

    rc = ldap_initialize(&ld, _ldapUri.c_str());
    if(rc != LDAP_SUCCESS)
    {
        retval.message = (std::string)"LDAP initialize error: "
                + ldap_err2string(rc);
        retval.code = Response::Code::LDAPInit;
        goto out2;
    }

    rc = ldap_set_option(ld,
                         LDAP_OPT_PROTOCOL_VERSION,
                         (void*)&version);
    if (rc != LDAP_SUCCESS)
    {
        retval.code = Response::Code::LDAPInit;
        retval.message = "LDAP set version failed";
        goto out;
    }

    rc = ldap_simple_bind_s(ld, buffer, password.c_str());
    if (rc != LDAP_SUCCESS)
    {
        retval = Response::Code::InvalidCredentials;
        retval.message = "Credential check for user \"" + user
                       + "\" failed: " + ldap_err2string(rc);
        goto out;
    }

    retval.code = Response::Code::Success;
    retval.message = "";

out:
    ldap_unbind(ld);
    ld = nullptr;
out2:
    return retval;
}

Clientmessage Logic::getClientMessage()
{
    std::lock_guard<std::mutex> l(_mutex);
    Clientmessage retval(_webPrefix,
                         _door.state() == Door::State::Unlocked,
                         _doormessage);

    // Reset doormessage
    _doormessage = Doormessage();

    return retval;
}

void Logic::_doorCallback(Doormessage doormessage)
{
    std::lock_guard<std::mutex> l(_mutex);
    _doormessage = doormessage;
}

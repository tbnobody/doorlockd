#include "clientmessage.h"
#include "util.h"
#include "response.h"

const std::string Clientmessage::_unlockButtonKey = "unlockButton";
const std::string Clientmessage::_lockButtonKey = "lockButton";
const std::string Clientmessage::_emergencyUnlockKey = "emergencyUnlock";
const std::string Clientmessage::_isOpenKey = "isOpen";

Clientmessage::Clientmessage(bool isOpen,
                             Doormessage doormessage) :
    _isOpen(isOpen),
    _doormessage(doormessage)
{
}

Clientmessage::Clientmessage() :
    _isOpen(false),
    _doormessage()
{
}

Clientmessage &Clientmessage::operator=(const Clientmessage &rhs)
{
    // Protect against self assignement
    if (this == &rhs) {
        return *this;
    }

    this->_isOpen = rhs._isOpen;
    this->_doormessage = rhs._doormessage;

    return *this;
}

std::string Clientmessage::toJson() const
{
    Json::StyledWriter writer;
    Json::Value message;

    message[_unlockButtonKey] = _doormessage.isUnlockButton;
    message[_lockButtonKey] = _doormessage.isLockButton;
    message[_emergencyUnlockKey] = _doormessage.isEmergencyUnlock;
    message[_isOpenKey] = _isOpen;

    return writer.write(message);
}

const Doormessage& Clientmessage::doormessage() const
{
    return _doormessage;
}

Clientmessage Clientmessage::fromJson(const Json::Value &root)
{
    bool isOpen;
    Doormessage doormessage;

    try {
        doormessage.isLockButton = getJsonOrFail<bool>(root, _lockButtonKey);
        doormessage.isUnlockButton = getJsonOrFail<bool>(root, _unlockButtonKey);
        doormessage.isEmergencyUnlock = getJsonOrFail<bool>(root, _emergencyUnlockKey);
        isOpen = getJsonOrFail<bool>(root, _isOpenKey);
    }
    catch (const std::exception &ex) {
        throw Response(Response::Code::JsonError, ex.what());
    }

    return Clientmessage(isOpen, doormessage);
}

Clientmessage Clientmessage::fromString(const std::string &string)
{
    Json::Reader reader;
    Json::Value root;

    if (reader.parse(string, root) == false)
        throw Response(Response::Code::NotJson,
                       "No valid JSON");

    return fromJson(root);
}

bool Clientmessage::isOpen() const
{
    return _isOpen;
}

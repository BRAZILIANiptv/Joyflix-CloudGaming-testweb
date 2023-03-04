#include "Signalling_Handler.h"
#include <functional>
#include <nlohmann/json.hpp>
#include "Webrtc/Types.h"

using namespace nlohmann;

Signalling_Handler::Signalling_Handler(
    int port,
    DelegateOnGotDescription onGotDescriptionCallback,
    DelegateOnICECandidate onIceCandidateCallback)
    : Port(port)
    , onGotDescriptionDelegate(onGotDescriptionCallback)
    , onICECandidateDelegate(onIceCandidateCallback)
    , signalling(std::bind(&Signalling_Handler::OnMessage, this, std::placeholders::_1))
{

}

Signalling_Handler::~Signalling_Handler()
{
    if (signalling_started)
    {
        StopSignalling();
    }
}

void Signalling_Handler::RegisterOnConnectCallback(Signalling::DelegateOnConnect callback) {
    signalling.RegisterOnConnectCallback(callback);
}
void Signalling_Handler::RegisterOnDisconnectCallback(Signalling::DelegateOnDisconnect callback) {
    signalling.RegisterOnDisconnectCallback(callback);
}

void Signalling_Handler::StartSignalling()
{
    signalling.StartServer(Port);
    signalling_started = true;
}

void Signalling_Handler::StopSignalling()
{
    signalling_started = false;
    signalling.StopServer();
}

void Signalling_Handler::OnMessage(std::string msg)
{
    json data = json::parse(msg);
    std::string type = data["type"].get<std::string>();

    if (type == "offer")
    {
        std::string sdp = data["data"]["sdp"].get<std::string>();
        onGotDescriptionDelegate(webrtc::RTCSdpType::Offer, sdp);
        return;
    }
    if (type == "candidate")
    {
        std::string candidate = data["data"]["candidate"].get<std::string>();
        std::string sdpMLineIndex = data["data"]["sdpMLineIndex"].get<std::string>();
        int sdpMid = data["data"]["sdpMid"].get<int>();
        if (onICECandidateDelegate)
            onICECandidateDelegate(candidate, sdpMLineIndex, sdpMid);
        return;
    }
}

void Signalling_Handler::SendSDP(webrtc::RTCSdpType type, std::string desc)
{
    switch (type)
    {
    case webrtc::RTCSdpType::Answer:
        json data = json::parse(R"(
                {
                  "data": {
                    "connectionId": "server",
                    "sdp": ""
                  },
                  "from": "f67a7cf5-9950-4ffc-b97c-3370df518964",
                  "type": "answer"
                }
            )");
        data["data"]["sdp"] = desc;
        signalling.SendMessage(data.dump());
        break;
    }
}

void Signalling_Handler::SendICE(char* candidate, char* sdpMid, int sdpMlineIndex)
{
    json data = json::parse(R"(
                {
                  "type": "candidate",
                  "from": "9dc9690c-183f-404a-bb7e-3e57f5a59566",
                  "data": {
                    "candidate": "",
                    "sdpMid": "",
                    "sdpMLineIndex": 0,
                    "connectionId": "9dc9690c-183f-404a-bb7e-3e57f5a59566"
                  }
                }
            )");
    data["data"]["candidate"] = candidate;
    data["data"]["sdpMid"] = sdpMid;
    data["data"]["sdpMLineIndex"] = sdpMlineIndex;
    printf("SendICE: \n%s\n", data.dump().c_str());

    signalling.SendMessage(data.dump());
}


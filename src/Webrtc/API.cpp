#include "API.h"
#include "Context.h"
#include "WebRTCPlugin.h"
#include "Logger.h"

using namespace webrtc;

WebRTCPlugin* plugin;
Context* ctx;

API::API() {
    plugin = new WebRTCPlugin;
    DebugLog("Plugin Created!");
}

API::~API() {
    DebugLog("Plugin Destroyed!");
    delete plugin;
}

void* API::ContextCreate() {
    DebugLog("Context Created!");
    ctx = new Context;
    return ctx;
}

void API::ContextDestroy() {
    delete ctx;
    DebugLog("Context Destroyed!");
}

void OnSuccess(PeerConnectionObject* pco, RTCSdpType type, const char* sdp) {
    switch (type) {
        case RTCSdpType::Offer: {
            RTCSessionDescription desc = {};
            desc.sdp = sdp;
            desc.type = type;
            std::string error;
            auto errorType =
                    plugin->PeerConnectionSetLocalDescription(ctx, pco, &desc, error);

            if (errorType != webrtc::RTCErrorType::NONE) {
                std::cout << "error: " << error << std::endl;
            }

            std::cout << "Offer: \n" << sdp << std::endl;
        } break;

        case RTCSdpType::Answer: {
            RTCSessionDescription desc = {};
            desc.sdp = sdp;
            desc.type = type;
            std::string error;

            plugin->PeerConnectionSetLocalDescription(ctx, pco, &desc, error);
            LogPrint("Answer SDP: \n%s", sdp);
        } break;
        default:
            // TODO:
            break;
    }
}

void OnIceCandidate(PeerConnectionObject* pco,
                    const char* candidate,
                    const char* sdpMid,
                    const int sdpMlineIndex) {
    LogPrint(
            "OnIceCandidate:\n candidate: %s\n, sdpMid: %s\n, sdpMlineIndex: %d\n",
            candidate, sdpMid, sdpMlineIndex);
}

void CreateOffer(PeerConnectionObject* pco) {
    const RTCOfferAnswerOptions options{false, true};
    plugin->PeerConnectionCreateOffer(pco, &options);
}

void API::StartWebRTCServer() {
    PeerConnectionObject* pco = plugin->ContextCreatePeerConnection(ctx);
    plugin->PeerConnectionRegisterCallbackCreateSD(pco, OnSuccess, nullptr);
    plugin->PeerConnectionRegisterOnIceCandidate(pco, OnIceCandidate);
    plugin->AddTracks(ctx);
    CreateOffer(pco);
}


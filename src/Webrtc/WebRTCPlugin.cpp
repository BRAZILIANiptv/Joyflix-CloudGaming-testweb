//
// Created by owais on 25/8/22.
//
#pragma clang diagnostic ignored "-Wunused-variable"

#include "WebRTCPlugin.h"
#include "CSDO.h"
#include "Context.h"
#include "Logger.h"
#include "api/jsep.h"

namespace webrtc {


    MediaStreamInterface *WebRTCPlugin::ContextCreateMediaStream(Context *context, const char *streamId) {
        rtc::scoped_refptr<MediaStreamInterface> stream = context->CreateMediaStream(streamId);
        context->AddRefPtr(stream);
        return stream.get();
    }

    void WebRTCPlugin::ContextRegisterMediaStreamObserver(Context *context, MediaStreamInterface *stream) {

    }

    void WebRTCPlugin::ContextUnRegisterMediaStreamObserver(Context *context, MediaStreamInterface *stream) {

    }

    MediaStreamTrackInterface *WebRTCPlugin::ContextCreateVideoTrack(Context *context, const char *label,
                                                                     webrtc::VideoTrackSourceInterface *source) {
        return nullptr;
    }

    void WebRTCPlugin::ContextStopMediaStreamTrack(Context *context, ::webrtc::MediaStreamTrackInterface *track) {

    }

    webrtc::VideoTrackSourceInterface *WebRTCPlugin::ContextCreateVideoTrackSource(Context *context) {
        return nullptr;
    }

    webrtc::AudioSourceInterface *WebRTCPlugin::ContextCreateAudioTrackSource(Context *context) {
        return nullptr;
    }

    webrtc::MediaStreamTrackInterface *
    WebRTCPlugin::ContextCreateAudioTrack(Context *context, const char *label, webrtc::AudioSourceInterface *source) {
        return nullptr;
    }

    void WebRTCPlugin::ContextAddRefPtr(Context *context, rtc::RefCountInterface *ptr) {

    }

    void WebRTCPlugin::ContextDeleteRefPtr(Context *context, rtc::RefCountInterface *ptr) {

    }

    bool WebRTCPlugin::MediaStreamAddTrack(MediaStreamInterface *stream, MediaStreamTrackInterface *track) {
        return false;
    }

    bool WebRTCPlugin::MediaStreamRemoveTrack(MediaStreamInterface *stream, MediaStreamTrackInterface *track) {
        return false;
    }

    char *WebRTCPlugin::MediaStreamGetID(MediaStreamInterface *stream) {
        return nullptr;
    }

    Context *WebRTCPlugin::ContextCreate(int uid) {
        return nullptr;
    }

    void WebRTCPlugin::ContextDestroy(int uid) {

    }

    PeerConnectionObject *WebRTCPlugin::ContextCreatePeerConnection(
            Context *context) {
        PeerConnectionInterface::RTCConfiguration config = {};
        config.sdp_semantics = SdpSemantics::kUnifiedPlan;
        config.enable_implicit_rollback = true;
        webrtc::PeerConnectionInterface::IceServer server;
        server.uri = GetPeerConnectionString();
        config.servers.push_back(server);
        return context->CreatePeerConnection(config);
    }

    void WebRTCPlugin::PeerConnectionRegisterOnIceCandidate(
            PeerConnectionObject *obj,
            DelegateIceCandidate callback) {
        obj->RegisterIceCandidate(callback);
    }

    CSDO *WebRTCPlugin::PeerConnectionCreateOffer(
            Context *context, PeerConnectionObject *obj, const RTCOfferAnswerOptions *options) {
        auto observer = CSDO::Create(obj);
        obj->CreateOffer(*options, observer.get());
        return observer.get();
    }

    CSDO *WebRTCPlugin::PeerConnectionCreateAnswer(
            Context *context, PeerConnectionObject *obj, const RTCOfferAnswerOptions *options) {
        auto observer = CSDO::Create(obj);
        obj->CreateAnswer(*options, observer.get());
        return observer.get();
    }

    RTCErrorType WebRTCPlugin::PeerConnectionSetLocalDescription(
            Context *context,
            PeerConnectionObject *obj,
            const RTCSessionDescription *desc,
            std::string &error) {
        //Not Implemented
    }

    RTCErrorType WebRTCPlugin::PeerConnectionSetRemoteDescription(
            Context *context,
            PeerConnectionObject *obj,
            const RTCSessionDescription *desc,
            std::string &error) {
        //Not Implemented
    }

    PeerConnectionInterface::SignalingState
    WebRTCPlugin::PeerConnectionSignalingState(PeerConnectionObject *obj) {
        return obj->connection->signaling_state();
    }

    RTCErrorType
    WebRTCPlugin::PeerConnectionAddIceCandidate(PeerConnectionObject *obj, char *candidate, char *sdpMLineIndex,
                                                int sdpMid) {
        SdpParseError error;
        std::unique_ptr<webrtc::IceCandidateInterface> _candidate(
                CreateIceCandidate(sdpMLineIndex, sdpMid, candidate, &error));
        if (!_candidate.get()) {
            DebugError("Can't parse received candidate message. SdpParseError was: %s", error.description.c_str());
            return RTCErrorType::INTERNAL_ERROR;
        }
        if (!obj->connection->AddIceCandidate(_candidate.get())) {
            DebugError("Failed to apply the received candidate: %s", error.description.c_str());
        }
    }

    PeerConnectionObject *WebRTCPlugin::ContextCreatePeerConnectionWithConfig(Context *context, const char *conf) {
        PeerConnectionInterface::RTCConfiguration config;
        if (!Convert(conf, config))
            return nullptr;

        config.sdp_semantics = SdpSemantics::kUnifiedPlan;
        config.enable_implicit_rollback = true;
        return context->CreatePeerConnection(config);
    }

    void WebRTCPlugin::ContextDeletePeerConnection(Context *context, PeerConnectionObject *obj) {

    }

    void WebRTCPlugin::PeerConnectionClose(PeerConnectionObject *obj) {

    }

    void WebRTCPlugin::PeerConnectionRestartIce(PeerConnectionObject *obj) {

    }

    RTCErrorType WebRTCPlugin::PeerConnectionAddTrack(PeerConnectionObject *obj, MediaStreamTrackInterface *track,
                                                      const char *streamId, RtpSenderInterface **sender) {
        return RTCErrorType::INVALID_PARAMETER;
    }

    char *ConvertString(const std::string str) {
        const size_t size = str.size();
        char *ret = static_cast<char *>(CoTaskMemAlloc(size + sizeof(char)));
        str.copy(ret, size);
        ret[size] = '\0';
        return ret;
    }

}  // namespace webrtc
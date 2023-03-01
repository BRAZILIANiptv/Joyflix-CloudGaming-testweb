#pragma once

#include <iostream>

#include "pch.h"

namespace webrtc
{

class API;
class Context;
class PeerConnectionObject;
class CSDO;
class UnityVideoRenderer;
class AudioTrackSinkAdapter;
enum class RTCSdpType;
enum class RTCPeerConnectionEventType;
struct MediaStreamEvent;

using DelegateDebugLog = void (*)(const char*);
using DelegateSetResolution = void (*)(int32_t*, int32_t*);
using DelegateMediaStreamOnAddTrack = void (*)(MediaStreamInterface*, MediaStreamTrackInterface*);
using DelegateMediaStreamOnRemoveTrack = void (*)(MediaStreamInterface*, MediaStreamTrackInterface*);
using DelegateVideoFrameResize = void (*)(UnityVideoRenderer* renderer, int width, int height);

enum class RTCSdpType
{
    Offer,
    PrAnswer,
    Answer,
    Rollback
};

struct RTCSessionDescription
{
    RTCSdpType type;
    const char* sdp;
};

enum class RTCPeerConnectionState
{
    New,
    Connecting,
    Connected,
    Disconnected,
    Failed,
    Closed
};

enum class RTCIceConnectionState
{
    New,
    Checking,
    Connected,
    Completed,
    Failed,
    Disconnected,
    Closed,
    Max
};

enum class RTCSignalingState
{
    Stable,
    HaveLocalOffer,
    HaveRemoteOffer,
    HaveLocalPranswer,
    HaveRemotePranswer,
    Closed
};

enum class RTCPeerConnectionEventType
{
    ConnectionStateChange,
    DataChannel,
    IceCandidate,
    IceConnectionStateChange,
    Track
};

enum class SdpSemanticsType
{
    UnifiedPlan
};

enum class RTCIceCredentialType
{
    Password,
    OAuth
};

enum class TrackKind
{
    Audio,
    Video
};

struct RTCOfferAnswerOptions
{
    bool iceRestart;
    bool voiceActivityDetection;
};

struct RTCIceServer
{
    char* credential;
    char* credentialType;
    char** urls;
    int urlsLength;
    char* username;
};

struct RTCConfiguration
{
    RTCIceServer* iceServers;
    int iceServersLength;
    char* iceServerPolicy;
};

struct RTCIceCandidate
{
    char* candidate;
    char* sdpMid;
    int sdpMLineIndex;
};

struct RTCIceCandidateInit
{
    char* candidate;
    char* sdpMid;
    int32_t sdpMLineIndex;
};

char* ConvertString(const std::string str);

struct Candidate
{
    char* candidate;
    int32_t component;
    char* foundation;
    char* ip;
    uint16_t port;
    uint32_t priority;
    char* address;
    char* protocol;
    char* relatedAddress;
    uint16_t relatedPort;
    char* tcpType;
    char* type;
    char* usernameFragment;

    Candidate& operator=(const cricket::Candidate& obj)
    {
        candidate = ConvertString(obj.ToString());
        component = obj.component();
        foundation = ConvertString(obj.foundation());
        ip = ConvertString(obj.address().ipaddr().ToString());
        port = obj.address().port();
        priority = obj.priority();
        address = ConvertString(obj.address().ToString());
        protocol = ConvertString(obj.protocol());
        relatedAddress = ConvertString(obj.related_address().ToString());
        relatedPort = obj.related_address().port();
        tcpType = ConvertString(obj.tcptype());
        type = ConvertString(obj.type());
        usernameFragment = ConvertString(obj.username());
        return *this;
    }
};

template<typename T>
struct MarshallArray
{
    size_t length;
    T* values;

    T& operator[](size_t i) const { return values[i]; }

    template<typename U>
    MarshallArray& operator=(const std::vector<U>& src)
    {
        length = static_cast<int32_t>(src.size());
        values = static_cast<T*>(CoTaskMemAlloc(sizeof(T) * src.size()));

        for (size_t i = 0; i < src.size(); i++)
        {
            values[i] = src[i];
        }
        return *this;
    }
};

template<typename T>
struct Optional
{
    bool hasValue;
    T value;

    template<typename U>
    Optional& operator=(const absl::optional<U>& src)
    {
        hasValue = src.has_value();
        if (hasValue)
        {
            value = static_cast<T>(src.value());
        }
        else
        {
            value = T();
        }
        return *this;
    }

#if defined(__clang__) || defined(__GNUC__)
    __attribute__((optnone))
#endif
    explicit
    operator const absl::optional<T>() const
    {
        absl::optional<T> dst = absl::nullopt;
        if (hasValue)
            dst = value;
        return dst;
    }

    const T& value_or(const T& v) const { return hasValue ? value : v; }
};

template<typename T>
absl::optional<T> ConvertOptional(const Optional<T>& value)
{
    absl::optional<T> dst = absl::nullopt;
    if (value.hasValue)
    {
        dst = value.value;
    }
    return dst;
}

struct RTCRtpEncodingParameters
{
    bool active;
    Optional<uint64_t> maxBitrate;
    Optional<uint64_t> minBitrate;
    Optional<uint32_t> maxFramerate;
    Optional<double> scaleResolutionDownBy;
    char* rid;

    RTCRtpEncodingParameters& operator=(const RtpEncodingParameters& obj)
    {
        active = obj.active;
        maxBitrate = obj.max_bitrate_bps;
        minBitrate = obj.min_bitrate_bps;
        maxFramerate = obj.max_framerate;
        scaleResolutionDownBy = obj.scale_resolution_down_by;
        rid = ConvertString(obj.rid);
        return *this;
    }

    operator RtpEncodingParameters() const
    {
        RtpEncodingParameters dst = {};
        dst.active = active;
        dst.max_bitrate_bps = static_cast<absl::optional<int>>(ConvertOptional(maxBitrate));
        dst.min_bitrate_bps = static_cast<absl::optional<int>>(ConvertOptional(minBitrate));
        dst.max_framerate = static_cast<absl::optional<double>>(ConvertOptional(maxFramerate));
        dst.scale_resolution_down_by = ConvertOptional(scaleResolutionDownBy);
        if (rid != nullptr)
            dst.rid = std::string(rid);
        return dst;
    }
};

struct RTCRtpTransceiverInit
{
    RtpTransceiverDirection direction;
    MarshallArray<RTCRtpEncodingParameters> sendEncodings;
    MarshallArray<MediaStreamInterface*> streams;

    operator RtpTransceiverInit() const
    {
        RtpTransceiverInit dst = {};
        dst.direction = direction;
        dst.send_encodings.resize(sendEncodings.length);
        for (size_t i = 0; i < dst.send_encodings.size(); i++)
        {
            dst.send_encodings[i] = sendEncodings[i];
        }
        dst.stream_ids.resize(streams.length);
        for (size_t i = 0; i < dst.stream_ids.size(); i++)
        {
            dst.stream_ids[i] = streams[i]->id();
        }
        return dst;
    }
};

// Callback Delegates
using DelegateCreateSDSuccess = void (*)(API*, PeerConnectionObject*, RTCSdpType, const char*);

using DelegateCreateSDFailure = void (*)(PeerConnectionObject*, webrtc::RTCErrorType, const char*);
using DelegateLocalSdpReady = void (*)(PeerConnectionObject*, const char*, const char*);
using DelegateIceCandidate = void (*)(API*, PeerConnectionObject*, const char*, const char*, const int);
using DelegateOnIceConnectionChange =
    void (*)(PeerConnectionObject*, webrtc::PeerConnectionInterface::IceConnectionState);
using DelegateOnIceGatheringChange =
    void (*)(PeerConnectionObject*, webrtc::PeerConnectionInterface::IceGatheringState);
using DelegateOnConnectionStateChange =
    void (*)(PeerConnectionObject*, webrtc::PeerConnectionInterface::PeerConnectionState);
using DelegateOnDataChannel = void (*)(PeerConnectionObject*, webrtc::DataChannelInterface*);
using DelegateOnRenegotiationNeeded = void (*)(PeerConnectionObject*);
using DelegateOnTrack = void (*)(PeerConnectionObject*, webrtc::RtpTransceiverInterface*);
using DelegateOnRemoveTrack = void (*)(PeerConnectionObject*, webrtc::RtpReceiverInterface*);

////////////////////////////////////
class WebRTCPlugin
{
private:
    const std::string stun_server_url = "stun:stun.l.google.com:19302";

public:
    MediaStreamInterface* ContextCreateMediaStream(Context* context, const char* streamId);
    void ContextRegisterMediaStreamObserver(Context* context, MediaStreamInterface* stream);
    void ContextUnRegisterMediaStreamObserver(Context* context, MediaStreamInterface* stream);
    MediaStreamTrackInterface*
    ContextCreateVideoTrack(Context* context, const char* label, webrtc::VideoTrackSourceInterface* source);
    void ContextStopMediaStreamTrack(Context* context, ::webrtc::MediaStreamTrackInterface* track);

    webrtc::VideoTrackSourceInterface* ContextCreateVideoTrackSource(Context* context);

    webrtc::AudioSourceInterface* ContextCreateAudioTrackSource(Context* context);
    webrtc::MediaStreamTrackInterface*
    ContextCreateAudioTrack(Context* context, const char* label, webrtc::AudioSourceInterface* source);

    void ContextAddRefPtr(Context* context, rtc::RefCountInterface* ptr);
    void ContextDeleteRefPtr(Context* context, rtc::RefCountInterface* ptr);

    bool MediaStreamAddTrack(MediaStreamInterface* stream, MediaStreamTrackInterface* track);
    bool MediaStreamRemoveTrack(MediaStreamInterface* stream, MediaStreamTrackInterface* track);
    char* MediaStreamGetID(MediaStreamInterface* stream);

    Context* ContextCreate(int uid);
    void ContextDestroy(int uid);

    PeerConnectionObject* ContextCreatePeerConnection(Context* context);
    PeerConnectionObject* ContextCreatePeerConnectionWithConfig(Context* context, const char* conf);
    void ContextDeletePeerConnection(Context* context, PeerConnectionObject* obj);
    void PeerConnectionClose(PeerConnectionObject* obj);
    void PeerConnectionRestartIce(PeerConnectionObject* obj);
    RTCErrorType PeerConnectionAddTrack(
        PeerConnectionObject* obj, MediaStreamTrackInterface* track, const char* streamId, RtpSenderInterface** sender);

    RtpTransceiverInterface* PeerConnectionAddTransceiver(PeerConnectionObject* obj, MediaStreamTrackInterface* track);

    RtpTransceiverInterface* PeerConnectionAddTransceiverWithInit(
        PeerConnectionObject* obj, MediaStreamTrackInterface* track, const RTCRtpTransceiverInit* init);

    RtpTransceiverInterface*
    PeerConnectionAddTransceiverWithType(PeerConnectionObject* obj, cricket::MediaType type);

    void PeerConnectionRegisterOnIceCandidate(PeerConnectionObject* obj, DelegateIceCandidate callback);

    CSDO* PeerConnectionCreateOffer(Context* context, PeerConnectionObject* obj, const RTCOfferAnswerOptions* options);

    CSDO* PeerConnectionCreateAnswer(Context* context, PeerConnectionObject* obj, const RTCOfferAnswerOptions* options);

    webrtc::RTCErrorType PeerConnectionSetLocalDescription(
        Context* context, PeerConnectionObject* obj, const RTCSessionDescription* desc, std::string& error);

    webrtc::RTCErrorType PeerConnectionSetRemoteDescription(
        Context* context, PeerConnectionObject* obj, const RTCSessionDescription* desc, std::string& error);

    webrtc::PeerConnectionInterface::SignalingState PeerConnectionSignalingState(PeerConnectionObject* obj);

    webrtc::RTCErrorType
    PeerConnectionAddIceCandidate(PeerConnectionObject* obj, char* candidate, char* sdpMLineIndex, int sdpMid);

    std::string GetEnvVarOrDefault(const char* env_var_name, const char* default_value)
    {
        std::string value;
        const char* env_var = getenv(env_var_name);
        if (env_var)
            value = env_var;

        if (value.empty())
            value = default_value;

        return value;
    }

    std::string GetPeerConnectionString()
    {
        return stun_server_url;
        return GetEnvVarOrDefault("WEBRTC_CONNECT", "stun:stun.l.google.com:19302");
    }
};

extern bool Convert(const std::string& str, webrtc::PeerConnectionInterface::RTCConfiguration& config);
}

#include "PeerConnectionObject.h"

#include <iostream>

#include "Context.h"

namespace webrtc {
PeerConnectionObject::PeerConnectionObject() {}
void PeerConnectionObject::OnSuccess(
    webrtc::SessionDescriptionInterface* desc) {
  std::string out;
  desc->ToString(&out);
  const auto type = ConvertSdpType(desc->GetType());
  if (onCreateSDSuccess != nullptr) {
    onCreateSDSuccess(this, type, out.c_str());
  }
}
void PeerConnectionObject::OnFailure(webrtc::RTCError error) {
  //        //::TODO
  //        //RTCError _error = { RTCErrorDetailType::IdpTimeout };
  //        if (onCreateSDFailure != nullptr)
  //        {
  //            onCreateSDFailure(this, error.type(), error.message());
  //        }
}

void PeerConnectionObject::OnDataChannel(
    rtc::scoped_refptr<webrtc::DataChannelInterface> channel) {
  ////        context.AddDataChannel(channel, *this);
  //        if (onDataChannel != nullptr) {
  //            onDataChannel(this, channel);
  //        }
}

void PeerConnectionObject::OnIceCandidate(
    const webrtc::IceCandidateInterface* candidate) {
  std::string out;

  if (!candidate->ToString(&out)) {
    //            DebugError("Can't make string form of sdp.");
  }
  if (onIceCandidate != nullptr) {
    onIceCandidate(this, out.c_str(), candidate->sdp_mid().c_str(),
                   candidate->sdp_mline_index());
  }
}

void PeerConnectionObject::OnRenegotiationNeeded() {
  //        if (onRenegotiationNeeded != nullptr)
  //        {
  //            onRenegotiationNeeded(this);
  //        }
}

void PeerConnectionObject::OnTrack(
    rtc::scoped_refptr<webrtc::RtpTransceiverInterface> transceiver) {
  //        context.AddRefPtr(transceiver);
  //        context.AddRefPtr(transceiver->receiver());
  //        context.AddRefPtr(transceiver->receiver()->track());

  //        if (onTrack != nullptr)
  //        {
  //            onTrack(this, transceiver.get());
  //        }
}

void PeerConnectionObject::OnRemoveTrack(
    rtc::scoped_refptr<RtpReceiverInterface> receiver) {
  //        if (onRemoveTrack != nullptr)
  //        {
  //            onRemoveTrack(this, receiver.get());
  //        }
}

// Called any time the IceConnectionState changes.
void PeerConnectionObject::OnIceConnectionChange(
    webrtc::PeerConnectionInterface::IceConnectionState new_state) {
  //        DebugLog("OnIceConnectionChange %d", new_state);
  //        if (onIceConnectionChange != nullptr)
  //        {
  //            onIceConnectionChange(this, new_state);
  //        }
}

void PeerConnectionObject::OnConnectionChange(
    PeerConnectionInterface::PeerConnectionState new_state) {
  //        DebugLog("OnConnectionChange %d", new_state);
  //        if(onConnectionStateChange != nullptr)
  //        {
  //            onConnectionStateChange(this, new_state);
  //        }
}

// Called any time the IceGatheringState changes.
void PeerConnectionObject::OnIceGatheringChange(
    webrtc::PeerConnectionInterface::IceGatheringState new_state) {
  //        DebugLog("OnIceGatheringChange %d", new_state);
  //        if (onIceGatheringChange != nullptr)
  //        {
  //            onIceGatheringChange(this, new_state);
  //        }
}

void PeerConnectionObject::OnSignalingChange(
    webrtc::PeerConnectionInterface::SignalingState new_state) {
  //        DebugLog("OnSignalingChange %d", new_state);
}

void PeerConnectionObject::OnAddStream(
    rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) {
  //        DebugLog("OnAddStream");
}

void PeerConnectionObject::OnRemoveStream(
    rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) {
  //        DebugLog("OnRemoveStream");
}

void PeerConnectionObject::CreateOffer(const RTCOfferAnswerOptions& options) {
  webrtc::PeerConnectionInterface::RTCOfferAnswerOptions _options;
  _options.ice_restart = options.iceRestart;
  _options.voice_activity_detection = options.voiceActivityDetection;
  connection->CreateOffer(this, _options);
}

void PeerConnectionObject::CreateAnswer(const RTCOfferAnswerOptions& options) {
  webrtc::PeerConnectionInterface::RTCOfferAnswerOptions _options;
  _options.ice_restart = options.iceRestart;
  _options.voice_activity_detection = options.voiceActivityDetection;
  connection->CreateAnswer(this, _options);
}

RTCErrorType PeerConnectionObject::SetLocalDescription(
    const RTCSessionDescription& desc,
    webrtc::SetSessionDescriptionObserver* observer,
    std::string& error) {
  SdpParseError error_;
  std::unique_ptr<SessionDescriptionInterface> _desc =
      CreateSessionDescription(ConvertSdpType(desc.type), desc.sdp, &error_);
  if (!_desc.get()) {
    error = error_.description;
    return RTCErrorType::SYNTAX_ERROR;
  }
  connection->SetLocalDescription(observer, _desc.release());
  return RTCErrorType::NONE;
}

RTCErrorType PeerConnectionObject::SetRemoteDescription(
    const RTCSessionDescription& desc,
    webrtc::SetSessionDescriptionObserver* observer,
    std::string& error) {
  SdpParseError error_;
  std::unique_ptr<SessionDescriptionInterface> _desc =
      CreateSessionDescription(ConvertSdpType(desc.type), desc.sdp, &error_);
  if (!_desc.get()) {
    error = error_.description;
    return RTCErrorType::SYNTAX_ERROR;
  }
  connection->SetRemoteDescription(observer, _desc.release());
  return RTCErrorType::NONE;
}

void PeerConnectionObject::AddTracks(
    webrtc::PeerConnectionFactoryInterface* peer_connection_factory_) const {
  if (!connection->GetSenders().empty()) {
    return;  // Already added tracks.
  }
  rtc::scoped_refptr<webrtc::AudioTrackInterface> audio_track(
      peer_connection_factory_->CreateAudioTrack(
          kAudioLabel,
          peer_connection_factory_->CreateAudioSource(cricket::AudioOptions())
              .get()));
  auto result_or_error = connection->AddTrack(audio_track, {kStreamId});
  if (!result_or_error.ok()) {
    RTC_LOG(LS_ERROR) << "Failed to add audio track to PeerConnection: "
                      << result_or_error.error().message();
  }
}
}  // namespace webrtc
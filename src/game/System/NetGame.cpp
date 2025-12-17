#include "NetGame.hpp"
#include "app/Layout.hpp"
#include "app/Settings.hpp"
#include "app/System/Network.hpp"
#include "app/System/Time.hpp"
#include "game/Utility/Message.hpp"
#include "protocol/ShowMessageNotify.pb.h"
#include "protocol/PacketOpcodes.hpp"

using namespace game::sys;
using namespace mathfu;
using namespace std::string_view_literals;

static void OnConnectionFailed(const app::InboundPacket& packet)
{
    SDL_Log("NetGame::OnConnectionFailed: Failed to connect to the server");
}

static void OnDisconnectionNotify(const app::InboundPacket& packet)
{
    // FIXME: memory leak
    auto& language = app::Settings::GetLanguage();
    game::util::Message* message = new game::util::Message(language["connection_lost"].get_or("Lost connection of the server"sv));
    message->SetPosition(0.f, 0.f, MiddleCenter);
    message->SetBox(app::res::ANNOUNCEMENT_MOX);
    message->SetClickable([message]() { message->SetInvalid(); });
}

static void OnConnectionSuccessed(const app::InboundPacket& packet)
{
    SDL_Log("NetGame::OnConnectionSuccessed: successed to connect to the server");
}

static void OnShowMessageNotify(const app::InboundPacket& packet)
{
    proto::ShowMessageNotify payload;
    std::string_view data = packet.GetPayload();
    payload.ParseFromArray(data.data(), data.size());

    // FIXME: memory leak
    game::util::Message* message = new game::util::Message(payload.text());
    message->SetPosition(0.f, 0.f, MiddleCenter);
    message->SetBox(app::res::ANNOUNCEMENT_MOX);
    if (payload.box_stlye() == proto::MESSAGE_BOX_CLICKABLE) {
        message->SetClickable([message]() { message->SetInvalid(); });
    }
    if (payload.has_duration_seconds()) {
        message->SetLogic(
            [message,
                end_time = app::sys::Time::GetInstance().Now() + std::chrono::seconds { payload.duration_seconds() }]() {
                if (app::sys::Time::GetInstance().Now() < end_time) return;
                message->SetInvalid();
            });
    }
}

NetGame::NetGame()
{
    auto& network = app::sys::Network::GetInstance();
    auto& language = app::Settings::GetLanguage();
    game::util::Message* message = new game::util::Message(language["connection_started"].get_or("Connecting to the server"sv));
    message->SetPosition(0.f, 0.f, MiddleCenter);
    message->SetBox(app::res::ANNOUNCEMENT_MOX, 0, 0, game::util::Message::msc_StandardBoxScale);

    // just let it leak when failed
    network.Subscribe(ID_CONNECTION_ATTEMPT_FAILED,
        [message](const app::InboundPacket& packet) mutable {
            auto& language = app::Settings::GetLanguage();
            message->SetString(language["connection_failed"].get_or("Failed to connect to the server"sv));
            message->SetBox(app::res::ANNOUNCEMENT_MOX);
            OnConnectionFailed(packet); });
    network.Subscribe(ID_CONNECTION_REQUEST_ACCEPTED,
        [message](const app::InboundPacket& packet) mutable { message->SetInvalid(); OnConnectionSuccessed(packet); });
    network.Subscribe(ID_DISCONNECTION_NOTIFICATION, OnDisconnectionNotify);
    network.Subscribe(ID_CONNECTION_LOST, OnDisconnectionNotify);
    network.Subscribe(proto::ID_SHOW_MESSAGE_NOTIFY, OnShowMessageNotify);
    // network.Subscribe(proto::ID_MATCHMAKING_RSP, std::bind(&NetGame::OnMatchmakingRsp, this, std::placeholders::_1));
    // network.Subscribe(proto::ID_MATCH_FINISHED_NOTIFY, std::bind(&NetGame::OnMatchFinishedNotify, this, std::placeholders::_1));
}

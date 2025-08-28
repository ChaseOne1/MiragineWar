#include "MainMenuScene.hpp"
#include "app/Resources.hpp"
#include "app/Layout.hpp"
#include "app/Component/Render/Texture.hpp"
#include "app/Settings.hpp"
#include "app/System/Network.hpp"
#include "app/System/Time.hpp"
#include "app/resources/AllInOneIndex.hpp"
#include "app/Component/Render/ZIndex.hpp"
#include "game/System/Scene.hpp"
#include "game/Component/UIElement.hpp"
#include "game/Component/Transform.hpp"
#include "game/Component/Movement.hpp"
#include "game/Camera.hpp"
#include "game/Utility/Message.hpp"
#include "protocol/PacketOpcodes.hpp"
#include "utility/Random.hpp"
#include "protocol/MatchmakingReq.pb.h"
#include "protocol/MatchmakingRsp.pb.h"

using namespace game::logic;
using namespace entt;
using namespace mathfu;
using namespace game::util;
using namespace std::string_view_literals;
using namespace std::chrono_literals;

MainMenuScene::MainMenuScene()
{
    SetupWorldScene();
    SetupTitle();
    SetupSoldiers();
    app::sys::Network::GetInstance().Subcribe(ID_CONNECTION_REQUEST_ACCEPTED,
        std::bind(&MainMenuScene::SetupMenu, this, std::placeholders::_1));
}

void MainMenuScene::SetupWorldScene()
{
    game::sys::Scene::GetInstance();
}

void MainMenuScene::SetupTitle()
{
    registry& registry = utility::Registry::GetInstance().GetRegistry();
    app::Resources& resources = app::Resources::GetInstance();
    const app::Layout& layouts = app::Layout::GetInstance();

    registry.emplace<game::comp::UIElement>(m_Title);

    const std::shared_ptr<SDL_Texture>& title_img = resources.Require<SDL_Texture>(app::res::TITLE_IMG, app::idx::MISC_IDX);
    registry.emplace<app::comp::Texture>(m_Title, title_img, std::nullopt);
    registry.emplace<app::comp::ZIndex>(m_Title, app::comp::ZIndex::ZINDEX_UIELEMENT);

    const std::shared_ptr<toml::table>& boot_layouts = resources.Require<toml::table>(app::res::BOOT_LO, app::idx::MISC_IDX);
    const SDL_FRect& layout = layouts.Transform(*boot_layouts->at("title").as_table());
    registry.emplace<game::comp::Transform>(m_Title, vec2 { layout.x + layout.w / 2.f, layout.y + layout.h / 2.f },
        vec2 { layout.w / 2.f, layout.h / 2.f });
}

void MainMenuScene::SetupSoldiers()
{
    using namespace app::res;
    registry& registry = utility::Registry::GetInstance().GetRegistry();
    app::Resources& resources = app::Resources::GetInstance();
    const toml::table& settings = *app::Settings::GetInstance().GetSettings()["MainMenuScene"].as_table();
    constexpr std::array<app::GUID_t, 16u> soldiers_guid = {
        RECRUIT_ASF, VETERAN_ASF, ZOMBIE_ASF, SAMURAI_ASF,
        SWORDSMAN_ASF, NINJA_ASF, SISTER_ASF, ARMORED_SWORDSMAN_ASF,
        MONK_ASF, VAMPIRE_ASF, KNIGHT_ASF, IMMORTAL_ASF,
        ARCANIST_ASF, GENERAL_ASF, DREADLORD_ASF, HIGHLORD_ASF
    };

    std::mt19937& gen = utility::Random::GetInstance().GetGenerator();
    std::uniform_int_distribution<std::size_t> guid_dist(0, 15);
    std::uniform_real_distribution<float> posn_dist(0.f, 1.f);
    const vec2& cam_center = registry.get<game::comp::Transform>(game::Camera::GetInstance().GetCameraEntity()).m_Position;
    const float distance_x_from = **settings.at_path("soldiers_distance_x.from").as_floating_point(),
                distance_x_to = **settings.at_path("soldiers_distance_x.to").as_floating_point(),
                distance_hy = **settings["soldiers_distance_hy"].as_floating_point();

    m_Soldiers.reserve(**settings["soldiers_num"].as_integer());
    for (int i = 0; i < m_Soldiers.capacity(); ++i) {
        const int8_t mirrored = i > m_Soldiers.capacity() - 1 >> 1 ? 1 : -1;

        auto& soldier = m_Soldiers.emplace_back(soldiers_guid[guid_dist(gen)],
            vec2 { cam_center.x + mirrored * (distance_x_from - posn_dist(gen) * (distance_x_from - distance_x_to)),
                cam_center.y - distance_hy + posn_dist(gen) * distance_hy * 2 },
            app::AnimSeqFrames::ANIM_RUN, mirrored < 0);

        // Movement
        soldier.SetMovement(vec2 { -mirrored * settings["soldiers_speed"].value_or(32.f), 0.f });

        // Logic
        const float posn_to = cam_center.x + mirrored * (distance_x_to + posn_dist(gen) * settings["soldiers_float_length"].value_or(50.f));
        // we don't capture components because deleting the movement component will invalidate those references
        soldier.SetLogic([&soldier, posn_to, mirrored]() mutable {
            auto& registry = utility::Registry::GetInstance().GetRegistry();
            entity soldier_ent = soldier.GetEntity();
            const vec2& posn = registry.get<game::comp::Transform>(soldier_ent).m_Position;
            if (-mirrored * posn.x > -mirrored * posn_to && registry.all_of<game::comp::Movement>(soldier_ent)) {
                registry.erase<game::comp::Movement>(soldier_ent);
                soldier.SetAnimation(app::AnimSeqFrames::ANIM_IDLE);
            }
        });
    }
}

void MainMenuScene::SetupMenu(const app::InboundPacket&)
{
    auto& language = app::Settings::GetLanguageSettings();
    game::util::Message* msg = new game::util::Message(language["start_game"].value_or("START"sv));
    msg->SetPosition(0.f, 0.f, MiddleCenter);
    msg->SetBox(app::res::BUTTON_MOX);
    msg->SetPointable([msg]() { msg->SetBox(app::res::BUTTON_POINTED_MOX); },
        [msg]() { msg->SetBox(app::res::BUTTON_MOX); });
    msg->SetClickable([msg]() { msg->SetBox(app::res::BUTTON_POINTED_CLICKED_MOX); },
        [msg, next_clickable_time = app::sys::Time::Now(), this]() mutable {
            msg->SetBox(app::res::BUTTON_POINTED_MOX);
            if (app::sys::Time::Now() < next_clickable_time) return;
            next_clickable_time = app::sys::Time::Now() + 3s;
            auto& network = app::sys::Network::GetInstance();

            proto::MatchmakingReq req;
            req.set_host_name(std::to_string(app::sys::Time::GetInstance().Now().time_since_epoch().count()));
            const std::string packet = app::OutboundPacket::Build(proto::ID_MATCHMAKING_REQ, req);
            network.GetRakPeerInterface()->Send(packet.data(), packet.size(),
                MEDIUM_PRIORITY, RELIABLE_ORDERED, 0, SLNet::UNASSIGNED_SYSTEM_ADDRESS, true);
            const utility::TopicsSubscriberID sid = network.GetNextSubscriberID();
            network.Subcribe(proto::ID_MATCHMAKING_RSP, [msg, sid, this](const app::InboundPacket& packet) {
                delete msg;
                proto::MatchmakingRsp rsp;
                rsp.ParseFromString(packet.GetPayload());
                auto& language = app::Settings::GetLanguageSettings();
                game::util::Message* opponent = new game::util::Message(language["matchmaking_prefix"].value_or("Challenge from:\n")
                    + std::string(rsp.opponent_name()) + language["matchmaking_suffix"].value_or("\nHeading to Miragine..."));
                opponent->SetPosition(0.f, 0.f, MiddleCenter);
                opponent->SetBox(app::res::ANNOUNCEMENT_MOX);
                opponent->SetAlignment(Message::Alignment::CENTER);

                opponent->SetLogic([opponent, sid, end_time = app::sys::Time::Now() + 3s] {
                    if (app::sys::Time::Now() < end_time) return;
                    app::sys::Network::UnSubscribe(proto::ID_MATCHMAKING_RSP, sid);
                    opponent->SetInvalid();
                });

                m_Soldiers.clear();
            });
        });
}

#include "MainMenuScene.hpp"

#include "../System/Scene.hpp"
#include "app/Resources.hpp"
#include "app/Layout.hpp"
#include "app/Component/Render/Texture.hpp"
#include "app/Settings.hpp"
#include "app/System/Network.hpp"
#include "app/System/Time.hpp"
#include "app/resources/AllInOneIndex.hpp"
#include "app/Component/Render/ZIndex.hpp"
#include "game/Component/UIElement.hpp"
#include "game/Component/Transform.hpp"
#include "game/Component/Movement.hpp"
#include "game/Camera.hpp"
#include "game/System/Scene.hpp"
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
    //SetupWorldScene();

    game::sys::Scene::GetInstance();

   // app::sys::Network::GetInstance().Subscribe(ID_CONNECTION_REQUEST_ACCEPTED,
   //     std::bind(&MainMenuScene::SetupMenu, this, std::placeholders::_1));

}

void MainMenuScene::SetupWorldScene()
{
}
#include "../vendor/minhook/include/MinHook.h"
#include "../vendor/AOBScan/AOBScan.hpp"

#include "../vendor/CppSDK/SDK/Basic.cpp"
#include "../vendor/CppSDK/SDK/CoreUObject_functions.cpp"
#include "../vendor/CppSDK/SDK/DeathItemCache_classes.hpp"

#include "../vendor/CppSDK/SDK/Engine_classes.hpp"
#include "../vendor/CppSDK/SDK/Engine_functions.cpp"

#include "../vendor/CppSDK/SDK/ShooterGame_classes.hpp"
#include "../vendor/CppSDK/SDK/ShooterGame_functions.cpp"

#include "ZeroGUI.h"

#include <basetsd.h>
#include <cfloat>
#include <consoleapi.h>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <format>
#include <libloaderapi.h>
#include <memoryapi.h>
#include <minwindef.h>
#include <stdio.h>
#include <string>
#include <synchapi.h>
#include <sys/stat.h>
#include <winbase.h>
#include <winuser.h>

#undef DrawText

namespace World {
    static SDK::UWorld* world_gworld = nullptr;
    static SDK::UShooterGameInstance* world_game_instance = nullptr;
    static SDK::AShooterGameState* world_game_state = nullptr;
    static SDK::ULocalPlayer* world_local_player = nullptr;
    static SDK::AShooterPlayerController* world_player_controller = nullptr;
    static SDK::AShooterCharacter* world_player_character = nullptr;

    static SDK::UNetConnection* world_net_connection = nullptr;
    static SDK::UNetDriver* world_net_driver = nullptr;
    static SDK::UNetConnection* world_server_connection = nullptr;

    static SDK::APlayerCameraManager* world_player_camera_manager = nullptr;
    static SDK::FVector world_player_camera_location = {};
    static SDK::TArray<class SDK::ULevel*> world_persistent_levels = {};
    static SDK::ULevel* world_persistent_level = nullptr;

    static SDK::TArray<class SDK::AActor*> world_actors = {};
} // namespace World

namespace Func {
    static __forceinline float get_dpi_scale() {
        HDC hdc = GetDC(NULL);
        float dpi_scale = (float)GetDeviceCaps(hdc, LOGPIXELSX) / 96.0f;
        ReleaseDC(NULL, hdc);

        return dpi_scale;
    }

    static __forceinline bool 更新世界();

    static __forceinline void 自瞄_获取最佳目标(SDK::AShooterCharacter*, SDK::UCanvas*);
    static __forceinline void 自瞄();
    static __forceinline void 绘制自瞄FOV(SDK::UCanvas*);

    static __forceinline void 绘制菜单(SDK::UCanvas*);
    static __forceinline void 绘制菜单栏_恐龙(SDK::UCanvas*);
    static __forceinline void 绘制菜单栏_建筑(SDK::UCanvas*);
    static __forceinline void 绘制菜单栏_自瞄(SDK::UCanvas*);
    static __forceinline void 绘制菜单栏_功能(SDK::UCanvas*);
    static __forceinline void 绘制菜单栏_玩家(SDK::UCanvas*);

    static __forceinline void 绘制服务器信息(SDK::UCanvas*, SDK::FLinearColor);

    static __forceinline void on_actors(SDK::UCanvas*);
    static __forceinline void 绘制其他(SDK::AActor*, SDK::UCanvas*);

    static __forceinline void 绘制玩家ALL(SDK::AShooterCharacter*, SDK::UCanvas*);
    static __forceinline void 绘制玩家信息(SDK::AShooterCharacter*, SDK::UCanvas*, SDK::FLinearColor);
    static __forceinline void 绘制玩家骨骼(SDK::AShooterCharacter* r, SDK::UCanvas*, SDK::FLinearColor);

    static __forceinline void 绘制恐龙ALL(SDK::APrimalDinoCharacter*, SDK::UCanvas*);
    static __forceinline void 绘制恐龙(SDK::APrimalDinoCharacter*, SDK::UCanvas*, SDK::FLinearColor color);

    static __forceinline void 绘制容器ALL(SDK::APrimalStructureItemContainer*, SDK::UCanvas*);
    static __forceinline void 绘制补给箱(SDK::APrimalStructureItemContainer_SupplyCrate*, SDK::UCanvas*);
    static __forceinline void 绘制物品缓存(SDK::ADeathItemCache_C*, SDK::UCanvas*);
    static __forceinline void 绘制炮塔(SDK::APrimalStructureTurret*, SDK::UCanvas*);

} // namespace Func

namespace Var {

    const float distance_div = 0.01;
    const float min_distance_slider = 1.0;
    const float max_distance_slider = 10000.0;

    static float dpi_scale = 1.25f;
    static float center_x = 0.0f;
    static float center_y = 0.0f;

    static auto menu_pos = SDK::FVector2D {0, 0};
    static bool toggle_menu = true;
    static bool toggle_menu_last_state = false;

    // esp
    static bool toggle_draw_server_info = false;

    // esp player

    static bool toggle_draw_others = false;

    static float draw_player_limit_distance = 1000.0;

    static bool toggle_draw_player_dead = false;
    static bool toggle_draw_player_offline = false;

    static bool toggle_draw_player_name = false;
    static bool toggle_draw_player_level = false;
    static bool toggle_draw_player_tribe = false;
    static bool toggle_draw_player_health = false;
    static bool toggle_draw_player_max_health = false;
    static bool toggle_draw_player_distance = false;
    static bool toggle_draw_player_item_count = false;

    static bool toggle_draw_tribe_player_info = false;
    static bool toggle_draw_enemy_player_info = false;

    static bool toggle_draw_tribe_player_bone = false;
    static bool toggle_draw_enemy_player_bone = false;

    // esp dino
    static float draw_dino_limit_distance = 1000.0;

    static bool toggle_draw_dino_dead = false;

    static bool toggle_draw_dino_name = false;
    static bool toggle_draw_dino_level = false;
    static bool toggle_draw_dino_sex = false;
    static bool toggle_draw_dino_health = false;
    static bool toggle_draw_dino_max_health = false;
    static bool toggle_draw_dino_owner = false;
    static bool toggle_draw_dino_distance = false;

    static bool toggle_draw_wild_dino = false;
    static bool toggle_draw_friendly_dino = false;
    static bool toggle_draw_enemy_dino = false;

    // esp structure
    static float draw_structure_limit_distance = 1000.0;

    static bool toggle_draw_structure_distance = false;

    static bool toggle_draw_structure_airdrop = false;

    static bool toggle_draw_structure_item_cache = false;
    static bool toggle_draw_structure_turret = false;

    // aim
    static bool toggle_aim = false;
    static bool toggle_aim_fov = false;
    static float aim_fov = 100.0f;
    static int aim_hotkey = VK_RBUTTON;
    static float aim_best_closest_distance = FLT_MAX;
    static SDK::AShooterCharacter* aim_best_closest_actor = nullptr;
    static SDK::AShooterCharacter* aim_locking_actor = nullptr;
    static bool aim_is_hotkey_down = false;
    static bool aim_is_mouse_patched = false;

    // feature
    static bool toggle_灵魂出窍 = false;
    static bool toggle_恐龙加速 = false;
    static bool toggle_瞬移 = false;
    static bool toggle_子弹追踪 = false;
    static bool toggle_原地上传 = false;

} // namespace Var

namespace Color {
    static SDK::FLinearColor 颜色_服务器信息 = {1.0f, 1.0f, 1.0f, 1.0f};
    static SDK::FLinearColor 颜色_野生恐龙 = {1.0f, 1.0f, 1.0f, 1.0f}; // 野生恐龙
    static SDK::FLinearColor 颜色_友好恐龙 = {0.0f, 1.0f, 1.0f, 1.0f}; // 友好恐龙
    static SDK::FLinearColor 颜色_敌对恐龙 = {1.0f, 0.0f, 0.0f, 1.0f}; // 敌对恐龙

    static SDK::FLinearColor 颜色_友好玩家 = {0.0f, 1.0f, 0.0f, 1.0f}; // 友好玩家
    static SDK::FLinearColor 颜色_敌对玩家 = {1.0f, 0.0f, 1.0f, 1.0f}; // 敌对玩家

    static SDK::FLinearColor 颜色_物品缓存 = {0.0f, 0.0f, 1.0f, 1.0f}; // 建筑
    static SDK::FLinearColor 颜色_炮塔 = {1.0f, 0.5f, 0.0f, 1.0f}; // 炮塔
    static SDK::FLinearColor 颜色_补给箱 = {1.0f, 1.0f, 0.0f, 1.0f}; // 补给箱

    static SDK::FLinearColor 颜色_自瞄FOV = {1.0f, 1.0f, 1.0f, 1.0f};
} // namespace Color

using TypePostRender = void(__fastcall*)(SDK::UGameViewportClient*, SDK::UCanvas* Canvas);

static TypePostRender original_post_render = nullptr;

void __fastcall hook_post_render(SDK::UGameViewportClient* _this, SDK::UCanvas* canvas) {
    original_post_render(_this, canvas);

    if (!canvas || !Func::更新世界()) {
        return;
    }

    Var::center_x = (canvas->SizeX / Var::dpi_scale) * 0.5f;
    Var::center_y = (canvas->SizeY / Var::dpi_scale) * 0.5f;

    if (GetAsyncKeyState(VK_END) & 1) {
        Var::toggle_menu = !Var::toggle_menu;
    }

    Func::绘制菜单(canvas);
    if (Var::toggle_menu != Var::toggle_menu_last_state) {
        if (Var::toggle_menu) {
            World::world_player_controller->SetIgnoreLookInput(true);
            World::world_player_controller->SetIgnoreMoveInput(true);
        } else {
            World::world_player_controller->SetIgnoreLookInput(false);
            World::world_player_controller->SetIgnoreMoveInput(false);
        }

        Var::toggle_menu_last_state = Var::toggle_menu;
    }

    Func::on_actors(canvas);

    if (Var::toggle_draw_server_info) {
        Func::绘制服务器信息(canvas, Color::颜色_服务器信息);
    }

    if (Var::toggle_aim_fov) {
        Func::绘制自瞄FOV(canvas);
    }
}

static __forceinline void Func::绘制菜单(SDK::UCanvas* canvas) {
    ZeroGUI::SetupCanvas(canvas);
    ZeroGUI::Input::Handle();

    if (ZeroGUI::Window(const_cast<wchar_t*>(L"Solo"), &Var::menu_pos, SDK::FVector2D {500.0f, 500.0f}, Var::toggle_menu)) {
        static int tab = 0;

        if (ZeroGUI::ButtonTab(const_cast<wchar_t*>(L"玩家"), SDK::FVector2D {100, 20}, tab == 0)) {
            tab = 0;
        }

        if (ZeroGUI::ButtonTab(const_cast<wchar_t*>(L"恐龙"), SDK::FVector2D {100, 20}, tab == 1)) {
            tab = 1;
        }

        if (ZeroGUI::ButtonTab(const_cast<wchar_t*>(L"建筑"), SDK::FVector2D {100, 20}, tab == 2)) {
            tab = 2;
        }

        if (ZeroGUI::ButtonTab(const_cast<wchar_t*>(L"自瞄"), SDK::FVector2D {100, 20}, tab == 3)) {
            tab = 3;
        }

        if (ZeroGUI::ButtonTab(const_cast<wchar_t*>(L"功能"), SDK::FVector2D {100, 20}, tab == 0)) {
            tab = 4;
        }

        ZeroGUI::NextColumn(130.0f);

        if (tab == 0) {
            Func::绘制菜单栏_玩家(canvas);
        }

        if (tab == 1) {
            Func::绘制菜单栏_恐龙(canvas);
        }

        if (tab == 2) {
            Func::绘制菜单栏_建筑(canvas);
        }

        if (tab == 3) {
            Func::绘制菜单栏_自瞄(canvas);
        }

        if (tab == 4) {
            Func::绘制菜单栏_功能(canvas);
        }
    }

    ZeroGUI::Render();
    ZeroGUI::Draw_Cursor(Var::toggle_menu);
}

static __forceinline void Func::绘制菜单栏_玩家(SDK::UCanvas* canvas) {
    ZeroGUI::SliderFloat(
        const_cast<wchar_t*>(L"绘制距离"),
        &Var::draw_player_limit_distance,
        Var::min_distance_slider,
        Var::max_distance_slider
    );

    ZeroGUI::Checkbox(const_cast<wchar_t*>(L"others"), &Var::toggle_draw_others);

    ZeroGUI::Checkbox(const_cast<wchar_t*>(L"显示服务器信息"), &Var::toggle_draw_server_info);
    ZeroGUI::SameLine();
    ZeroGUI::ColorPicker(const_cast<wchar_t*>(L"服务器信息颜色"), &Color::颜色_服务器信息);

    ZeroGUI::PushNextElementY(15.0);

    ZeroGUI::Checkbox(const_cast<wchar_t*>(L"显示离线"), &Var::toggle_draw_player_offline);
    ZeroGUI::SameLine();
    ZeroGUI::Checkbox(const_cast<wchar_t*>(L"显示死亡"), &Var::toggle_draw_player_dead);

    ZeroGUI::PushNextElementY(15.0);

    ZeroGUI::Checkbox(const_cast<wchar_t*>(L"名称"), &Var::toggle_draw_player_name);
    ZeroGUI::SameLine();
    ZeroGUI::Checkbox(const_cast<wchar_t*>(L"等级"), &Var::toggle_draw_player_level);

    ZeroGUI::Checkbox(const_cast<wchar_t*>(L"部落"), &Var::toggle_draw_player_tribe);

    ZeroGUI::Checkbox(const_cast<wchar_t*>(L"生命"), &Var::toggle_draw_player_health);
    ZeroGUI::SameLine();
    ZeroGUI::Checkbox(const_cast<wchar_t*>(L"最大生命"), &Var::toggle_draw_player_max_health);

    ZeroGUI::Checkbox(const_cast<wchar_t*>(L"距离"), &Var::toggle_draw_player_distance);
    ZeroGUI::SameLine();
    ZeroGUI::Checkbox(const_cast<wchar_t*>(L"背包物品数量"), &Var::toggle_draw_player_item_count);

    ZeroGUI::PushNextElementY(15.0);

    ZeroGUI::Checkbox(const_cast<wchar_t*>(L"部落玩家骨骼"), &Var::toggle_draw_tribe_player_bone);
    ZeroGUI::SameLine();
    ZeroGUI::Checkbox(const_cast<wchar_t*>(L"敌对玩家骨骼"), &Var::toggle_draw_enemy_player_bone);

    ZeroGUI::Checkbox(const_cast<wchar_t*>(L"部落玩家信息"), &Var::toggle_draw_tribe_player_info);
    ZeroGUI::SameLine();
    ZeroGUI::ColorPicker(const_cast<wchar_t*>(L"部落玩家颜色"), &Color::颜色_友好玩家);

    ZeroGUI::Checkbox(const_cast<wchar_t*>(L"敌对玩家信息"), &Var::toggle_draw_enemy_player_info);
    ZeroGUI::SameLine();
    ZeroGUI::ColorPicker(const_cast<wchar_t*>(L"敌对玩家颜色"), &Color::颜色_敌对玩家);
}

static __forceinline void Func::绘制菜单栏_恐龙(SDK::UCanvas* canvas) {
    ZeroGUI::SliderFloat(
        const_cast<wchar_t*>(L"限制距离"),
        &Var::draw_dino_limit_distance,
        Var::min_distance_slider,
        Var::max_distance_slider
    );

    ZeroGUI::Checkbox(const_cast<wchar_t*>(L"显示死亡"), &Var::toggle_draw_dino_dead);

    ZeroGUI::PushNextElementY(15.0);

    ZeroGUI::Checkbox(const_cast<wchar_t*>(L"名称"), &Var::toggle_draw_dino_name);
    ZeroGUI::SameLine();
    ZeroGUI::Checkbox(const_cast<wchar_t*>(L"等级"), &Var::toggle_draw_dino_level);

    ZeroGUI::Checkbox(const_cast<wchar_t*>(L"性别"), &Var::toggle_draw_dino_sex);
    ZeroGUI::SameLine();
    ZeroGUI::Checkbox(const_cast<wchar_t*>(L"部落"), &Var::toggle_draw_dino_owner);

    ZeroGUI::Checkbox(const_cast<wchar_t*>(L"生命"), &Var::toggle_draw_dino_health);
    ZeroGUI::SameLine();
    ZeroGUI::Checkbox(const_cast<wchar_t*>(L"最大生命"), &Var::toggle_draw_dino_max_health);

    ZeroGUI::Checkbox(const_cast<wchar_t*>(L"距离"), &Var::toggle_draw_dino_distance);

    ZeroGUI::PushNextElementY(15.0);

    ZeroGUI::Checkbox(const_cast<wchar_t*>(L"野生恐龙"), &Var::toggle_draw_wild_dino);
    ZeroGUI::SameLine();
    ZeroGUI::ColorPicker(const_cast<wchar_t*>(L"野生恐龙颜色"), &Color::颜色_野生恐龙);

    ZeroGUI::Checkbox(const_cast<wchar_t*>(L"友好恐龙"), &Var::toggle_draw_friendly_dino);
    ZeroGUI::SameLine();
    ZeroGUI::ColorPicker(const_cast<wchar_t*>(L"友好恐龙颜色"), &Color::颜色_友好玩家);

    ZeroGUI::Checkbox(const_cast<wchar_t*>(L"敌对恐龙"), &Var::toggle_draw_enemy_dino);
    ZeroGUI::SameLine();
    ZeroGUI::ColorPicker(const_cast<wchar_t*>(L"敌对恐龙颜色"), &Color::颜色_敌对恐龙);
}

static __forceinline void Func::绘制菜单栏_建筑(SDK::UCanvas* canvas) {
    ZeroGUI::SliderFloat(
        const_cast<wchar_t*>(L"绘制距离"),
        &Var::draw_structure_limit_distance,
        Var::min_distance_slider,
        Var::max_distance_slider
    );

    ZeroGUI::Checkbox(const_cast<wchar_t*>(L"距离"), &Var::toggle_draw_structure_distance);

    ZeroGUI::PushNextElementY(15.0);

    ZeroGUI::Checkbox(const_cast<wchar_t*>(L"光柱|宝箱|空投"), &Var::toggle_draw_structure_airdrop);
    ZeroGUI::SameLine();
    ZeroGUI::ColorPicker(const_cast<wchar_t*>(L"补给箱颜色"), &Color::颜色_补给箱);

    ZeroGUI::Checkbox(const_cast<wchar_t*>(L"物品缓存"), &Var::toggle_draw_structure_item_cache);
    ZeroGUI::SameLine();
    ZeroGUI::ColorPicker(const_cast<wchar_t*>(L"物品缓存颜色"), &Color::颜色_物品缓存);
}

static __forceinline void Func::绘制菜单栏_自瞄(SDK::UCanvas* canvas) {
    ZeroGUI::Checkbox(const_cast<wchar_t*>(L"自瞄"), &Var::toggle_aim);

    ZeroGUI::Checkbox(const_cast<wchar_t*>(L"绘制FOV"), &Var::toggle_aim_fov);
    ZeroGUI::SameLine();
    ZeroGUI::ColorPicker(const_cast<wchar_t*>(L"绘制FOV颜色"), &Color::颜色_自瞄FOV);

    ZeroGUI::SliderFloat(const_cast<wchar_t*>(L"范围"), &Var::aim_fov, 60.0f, 600.0f);

    ZeroGUI::Hotkey(SDK::FVector2D {60, 25}, &Var::aim_hotkey);
}

static __forceinline void Func::绘制菜单栏_功能(SDK::UCanvas* canvas) {
    if (ZeroGUI::Button(const_cast<wchar_t*>(L"一键自杀"), SDK::FVector2D {70, 20})) {
        World::world_player_character->BPSuicide();
    }
}

static __forceinline void Func::绘制服务器信息(SDK::UCanvas* canvas, SDK::FLinearColor color) {
    std::wstring ws = std::format(
        L"在线玩家 {}\n"
        "已驯养恐龙{}",
        World::world_game_state->NumPlayerConnected,
        World::world_game_state->NumTamedDinos
    );

    SDK::FString fstring(ws.c_str());

    canvas->K2_DrawText(
        ZeroGUI::Font,
        fstring,
        SDK::FVector2D {0, 0},
        SDK::FVector2D {ZeroGUI::FontScale * 1.3, ZeroGUI::FontScale * 1.3},
        color,
        1.0f,
        SDK::FLinearColor(0, 0, 0, 1.0f),
        SDK::FVector2D {0, 0},
        false,
        false,
        true,
        SDK::FLinearColor(0, 0, 0, 1.0f)
    );
}

static __forceinline void Func::自瞄_获取最佳目标(SDK::AShooterCharacter* actor, SDK::UCanvas* canvas) {
    static float last_size_x = 0;
    if (last_size_x != canvas->SizeX) {
        last_size_x = (float)canvas->SizeX;
        Var::center_x = (last_size_x / Var::dpi_scale) * 0.5f;
        Var::center_y = ((float)canvas->SizeY / Var::dpi_scale) * 0.5f;
    }

    auto* mesh = actor->Mesh;
    if (!mesh)
        return;

    static SDK::FName head_name = SDK::UKismetStringLibrary::Conv_StringToName(L"Cnt_Head_JNT_SKL");
    SDK::FVector world_loc = mesh->GetSocketLocation(head_name);
    SDK::FVector2D screen_pos;

    if (World::world_player_controller->ProjectWorldLocationToScreen(world_loc, &screen_pos, false)) {
        float logic_x = screen_pos.X / Var::dpi_scale;
        float logic_y = screen_pos.Y / Var::dpi_scale;

        float dx = logic_x - Var::center_x;
        float dy = logic_y - Var::center_y;
        float dist_sq = (dx * dx) + (dy * dy);

        static float last_fov = -1.0f;
        static float fov_sq = 0;
        if (last_fov != Var::aim_fov) {
            last_fov = Var::aim_fov;
            fov_sq = last_fov * last_fov;
        }

        if (dist_sq <= fov_sq && dist_sq < Var::aim_best_closest_distance) {
            Var::aim_best_closest_distance = dist_sq;
            Var::aim_best_closest_actor = actor;
        }
    }
}

static __forceinline void Func::自瞄() {
    if (!Var::aim_is_hotkey_down) {
        if (Var::aim_is_mouse_patched) {
            Var::aim_is_mouse_patched = false;
            World::world_player_controller->SetIgnoreLookInput(false);
        }

        if (Var::aim_best_closest_distance == FLT_MAX) {
            return;
        }
    }

    if ((GetAsyncKeyState(Var::aim_hotkey) & 0x8000)) {
        if (!Var::aim_is_hotkey_down) {
            Var::aim_is_hotkey_down = true;

            Var::aim_locking_actor = Var::aim_best_closest_actor;

            if (!Var::aim_is_mouse_patched) {
                Var::aim_is_mouse_patched = true;
                World::world_player_controller->SetIgnoreLookInput(true);
            }
        }

        if (!Var::aim_locking_actor || IsBadReadPtr(Var::aim_locking_actor, 8) || Var::aim_locking_actor->IsDead()) {
            Var::aim_is_hotkey_down = false;

            if (Var::aim_is_mouse_patched) {
                Var::aim_is_mouse_patched = false;
                World::world_player_controller->SetIgnoreLookInput(false);
            }

            return;
        }

        if (Var::aim_locking_actor->IsDead()) {
            Var::aim_is_hotkey_down = false;

            if (Var::aim_is_mouse_patched) {
                Var::aim_is_mouse_patched = false;
                World::world_player_controller->SetIgnoreLookInput(false);
            }

            return;
        }

        static SDK::FName head_name = SDK::UKismetStringLibrary::Conv_StringToName(L"Cnt_Head_JNT_SKL");

        SDK::USkeletalMeshComponent* aim_locking_actor_mesh = Var::aim_locking_actor->Mesh;
        if (!aim_locking_actor_mesh) {
            return;
        }

        SDK::FVector world_location_3d =
            aim_locking_actor_mesh->GetBoneTransform(head_name, SDK::ERelativeTransformSpace::RTS_World).Translation;

        SDK::FVector camera_location = World::world_player_camera_manager->GetCameraLocation();

        SDK::FRotator TargetRotation = SDK::UKismetMathLibrary::FindLookAtRotation(camera_location, world_location_3d);

        SDK::FRotator world_player_control_rotation = World::world_player_controller->GetControlRotation();

        SDK::FRotator rotation = SDK::UKismetMathLibrary::RInterpTo(world_player_control_rotation, TargetRotation, 0.3, 3.0f);

        World::world_player_controller->SetControlRotation(rotation);
    } else {
        Var::aim_is_hotkey_down = false;

        if (Var::aim_is_mouse_patched) {
            Var::aim_is_mouse_patched = false;
            World::world_player_controller->SetIgnoreLookInput(false);
        }
    }

    Var::aim_best_closest_distance = FLT_MAX;
}

static __forceinline void Func::绘制自瞄FOV(SDK::UCanvas* canvas) {
    ZeroGUI::DrawCircle(SDK::FVector2D {Var::center_x, Var::center_y}, Var::aim_fov, 16, Color::颜色_自瞄FOV);
}

void inline Func::on_actors(SDK::UCanvas* canvas) {
    for (auto level : World::world_persistent_levels) {
        if (!level) {
            continue;
        }

        for (SDK::AActor* actor : level->Actors) {
            if (!actor) {
                continue;
            }

            if (actor == World::world_player_character) {
                continue;
            }

            if (Var::toggle_draw_others) {
                Func::绘制其他(actor, canvas);
            }

            if (actor->IsPrimalDino()) {
                Func::绘制恐龙ALL(reinterpret_cast<SDK::APrimalDinoCharacter*>(actor), canvas);
            }

            if (actor->IsPrimalStructureItemContainer()) {
                Func::绘制容器ALL(reinterpret_cast<SDK::APrimalStructureItemContainer*>(actor), canvas);
            }

            if (actor->IsShooterCharacter()) {
                Func::绘制玩家ALL(reinterpret_cast<SDK::AShooterCharacter*>(actor), canvas);
            }

            if (Var::toggle_aim) {
                Func::自瞄();
            }
        }
    }
}

static __forceinline void Func::绘制其他(SDK::AActor* actor, SDK::UCanvas* canvas) {
    SDK::FVector world_location_3d = actor->K2_GetActorLocation();

    float distance = World::world_player_controller->GetDistanceTo(actor);

    distance *= Var::distance_div;

    if (distance > 100.0) {
        return;
    }

    SDK::FVector2D screen_location_2d = {0, 0};
    if (!World::world_player_controller->ProjectWorldLocationToScreen(world_location_3d, &screen_location_2d, true)) {
        return;
    }

    screen_location_2d.X /= Var::dpi_scale;
    screen_location_2d.Y /= Var::dpi_scale;

    std::string s = actor->Class->GetFullName();

    std::wstring ws = std::wstring(s.begin(), s.end());

    SDK::FString fstring(ws.c_str());

    canvas->K2_DrawText(
        ZeroGUI::Font,
        fstring,
        screen_location_2d,
        SDK::FVector2D {ZeroGUI::FontScale, ZeroGUI::FontScale},
        Color::颜色_服务器信息,
        1.0f,
        SDK::FLinearColor(0, 0, 0, 1.0f),
        SDK::FVector2D {0, 0},
        true,
        true,
        true,
        SDK::FLinearColor(0, 0, 0, 1.0f)
    );
}

static __forceinline void Func::绘制玩家ALL(SDK::AShooterCharacter* actor, SDK::UCanvas* canvas) {
    if ((!Var::toggle_draw_player_dead && actor->bIsDead) || (!Var::toggle_draw_player_offline && actor->bIsSleeping)) {
        return;
    }

    const bool is_friendly = actor->IsPrimalCharFriendly(World::world_player_character);

    const SDK::FLinearColor& draw_color = is_friendly ? Color::颜色_友好玩家 : Color::颜色_敌对玩家;

    if (is_friendly) {
        if (Var::toggle_draw_tribe_player_info) {
            Func::绘制玩家信息(actor, canvas, draw_color);
        }
        if (Var::toggle_draw_tribe_player_bone) {
            Func::绘制玩家骨骼(actor, canvas, draw_color);
        }

    } else {
        if (Var::toggle_draw_enemy_player_info) {
            if (Var::toggle_aim) {
                Func::自瞄_获取最佳目标(actor, canvas);
            }
            Func::绘制玩家信息(actor, canvas, draw_color);
        }
        if (Var::toggle_draw_enemy_player_bone) {
            Func::绘制玩家骨骼(actor, canvas, draw_color);
        }
    }
}

static __forceinline void Func::绘制玩家信息(SDK::AShooterCharacter* actor, SDK::UCanvas* canvas, SDK::FLinearColor color) {
    SDK::FVector world_location_3d = actor->K2_GetActorLocation();

    float distance = World::world_player_controller->GetDistanceTo(actor);

    distance *= Var::distance_div;

    if (distance > Var::draw_player_limit_distance) {
        return;
    }

    SDK::FVector2D screen_location_2d = {0, 0};
    if (!World::world_player_controller->ProjectWorldLocationToScreen(world_location_3d, &screen_location_2d, true)) {
        return;
    }

    screen_location_2d.X /= Var::dpi_scale;
    screen_location_2d.Y /= Var::dpi_scale;

    SDK::UPrimalCharacterStatusComponent* character_status_component = actor->MyCharacterStatusComponent;
    if (!character_status_component || IsBadReadPtr(character_status_component, 8)) {
        return;
    }

    wchar_t buffer[64] = {0};
    auto out = buffer;

    {
        if (Var::toggle_draw_player_name && actor->PlayerName.IsValid()) {
            out = std::format_to(out, L"{}\n", actor->PlayerName.CStr());
        }

        if (Var::toggle_draw_player_level) {
            out = std::format_to(
                out,
                L"{}\n",
                character_status_component->BaseCharacterLevel + character_status_component->ExtraCharacterLevel
            );
        }

        if (Var::toggle_draw_player_tribe && actor->TribeName.IsValid()) {
            out = std::format_to(out, L"{}\n", actor->TribeName.CStr());
        }

        if (Var::toggle_draw_player_health) {
            out = std::format_to(out, L"{:.2f}\n", (float)actor->ReplicatedCurrentHealth);
        }

        if (Var::toggle_draw_player_max_health) {
            out = std::format_to(out, L"{:.2f}\n", (float)actor->ReplicatedMaxHealth);
        }

        if (Var::toggle_draw_player_distance) {
            out = std::format_to(out, L"{:.2f}\n", (float)distance);
        }

        if (Var::toggle_draw_player_item_count && actor->MyInventoryComponent) {
            out = std::format_to(out, L"{}\n", actor->MyInventoryComponent->GetNumItems(false, false));
        }
    }

    *out = L'\0';
    SDK::FString fstring(buffer);

    canvas->K2_DrawText(
        ZeroGUI::Font,
        fstring,
        screen_location_2d,
        SDK::FVector2D {ZeroGUI::FontScale, ZeroGUI::FontScale},
        color,
        1.0f,
        SDK::FLinearColor(0, 0, 0, 1.0f),
        SDK::FVector2D {0, 0},
        true,
        true,
        true,
        SDK::FLinearColor(0, 0, 0, 1.0f)
    );
}

static __forceinline void Func::绘制玩家骨骼(SDK::AShooterCharacter* actor, SDK::UCanvas* canvas, SDK::FLinearColor color) {
    static SDK::FName b_8, b_5, b_1, b_152, b_179, b_206, b_212;
    static bool names_init = false;
    if (!names_init) {
        auto* m = actor->Mesh;
        b_8 = m->GetBoneName(8);
        b_5 = m->GetBoneName(5);
        b_1 = m->GetBoneName(1);
        b_152 = m->GetBoneName(152);
        b_179 = m->GetBoneName(179);
        b_206 = m->GetBoneName(206);
        b_212 = m->GetBoneName(212);
        names_init = true;
    }

    struct BonePoint {
        int32_t id;
        SDK::FName* name;
    };

    static const BonePoint points[7] =
        {{8, &b_8}, {5, &b_5}, {1, &b_1}, {152, &b_152}, {179, &b_179}, {206, &b_206}, {212, &b_212}};

    static const struct {
        int a_idx;
        int b_idx;
    } links[6] = {{0, 1}, {1, 2}, {1, 3}, {1, 4}, {2, 5}, {2, 6}};

    SDK::FVector2D pos_cache[7];
    bool is_valid[7] = {false};

    auto* mesh = actor->Mesh;
    const float inv_dpi = 1.0f / Var::dpi_scale; // 乘法比除法快

    for (int i = 0; i < 7; ++i) {
        SDK::FVector w_pos = mesh->GetSocketLocation(*points[i].name);
        if (World::world_player_controller->ProjectWorldLocationToScreen(w_pos, &pos_cache[i], false)) {
            pos_cache[i].X *= inv_dpi;
            pos_cache[i].Y *= inv_dpi;
            is_valid[i] = true;
        }
    }

    for (int i = 0; i < 6; ++i) {
        if (is_valid[links[i].a_idx] && is_valid[links[i].b_idx]) {
            canvas->K2_DrawLine(pos_cache[links[i].a_idx], pos_cache[links[i].b_idx], 1.0f, color);
        }
    }
}

static __forceinline void Func::绘制恐龙ALL(SDK::APrimalDinoCharacter* actor, SDK::UCanvas* canvas) {
    if (!Var::toggle_draw_dino_dead && actor->IsDead()) {
        return;
    }

    const bool is_wild = actor->IsWildSlow();

    if (is_wild) {
        if (Var::toggle_draw_wild_dino) {
            Func::绘制恐龙(actor, canvas, Color::颜色_野生恐龙);
        }
        return;
    }

    const bool is_friendly = actor->IsPrimalCharFriendly(World::world_player_character);

    if (is_friendly) {
        if (Var::toggle_draw_friendly_dino) {
            Func::绘制恐龙(actor, canvas, Color::颜色_友好恐龙);
        }
    } else {
        if (Var::toggle_draw_enemy_dino) {
            Func::绘制恐龙(actor, canvas, Color::颜色_敌对恐龙);
        }
    }
}

static __forceinline void Func::绘制恐龙(SDK::APrimalDinoCharacter* actor, SDK::UCanvas* canvas, SDK::FLinearColor color) {
    SDK::FVector world_location_3d = actor->K2_GetActorLocation();

    float distance = World::world_player_controller->GetDistanceTo(actor);
    distance *= Var::distance_div;

    if (distance > Var::draw_dino_limit_distance) {
        return;
    }

    SDK::FVector2D screen_location_2d = {0, 0};
    if (!World::world_player_controller->ProjectWorldLocationToScreen(world_location_3d, &screen_location_2d, true)) {
        return;
    }

    screen_location_2d.X /= Var::dpi_scale;
    screen_location_2d.Y /= Var::dpi_scale;

    SDK::UPrimalCharacterStatusComponent* character_status_component = actor->MyCharacterStatusComponent;
    if (!character_status_component || IsBadReadPtr(character_status_component, 8)) {
        return;
    }

    wchar_t buffer[64] = {0};
    auto out = buffer;

    if (Var::toggle_draw_dino_name && actor->DescriptiveName.IsValid()) {
        out = std::format_to(out, L"{}\n", actor->DescriptiveName.CStr());
    }

    if (Var::toggle_draw_dino_level) {
        out = std::format_to(
            out,
            L"{}\n",
            character_status_component->BaseCharacterLevel + character_status_component->ExtraCharacterLevel
        );
    }

    if (Var::toggle_draw_dino_sex) {
        out = std::format_to(out, L"{}\n", actor->bIsFemale ? L"母" : L"公");
    }

    if (Var::toggle_draw_dino_owner && actor->TribeName.IsValid()) {
        out = std::format_to(out, L"{}\n", actor->TribeName.CStr());
    }

    if (Var::toggle_draw_dino_health) {
        out = std::format_to(out, L"{:.2f}\n", (float)actor->ReplicatedCurrentHealth);
    }

    if (Var::toggle_draw_dino_max_health) {
        out = std::format_to(out, L"{:.2f}\n", (float)actor->ReplicatedMaxHealth);
    }

    if (Var::toggle_draw_dino_distance) {
        out = std::format_to(out, L"{:.2f}\n", (float)distance);
    }

    SDK::FString fstring(buffer);

    canvas->K2_DrawText(
        ZeroGUI::Font,
        fstring,
        screen_location_2d,
        SDK::FVector2D {ZeroGUI::FontScale, ZeroGUI::FontScale},
        color,
        1.0f,
        SDK::FLinearColor(0, 0, 0, 1.0f),
        SDK::FVector2D {0, 0},
        true,
        true,
        true,
        SDK::FLinearColor(0, 0, 0, 1.0f)
    );
}

static __forceinline void Func::绘制容器ALL(SDK::APrimalStructureItemContainer* actor, SDK::UCanvas* canvas) {
    if (Var::toggle_draw_structure_airdrop) {
        if (actor->IsA(SDK::APrimalStructureItemContainer_SupplyCrate::StaticClass())) {
            Func::绘制补给箱(static_cast<SDK::APrimalStructureItemContainer_SupplyCrate*>(actor), canvas);
        }
    }

    if (Var::toggle_draw_structure_item_cache && actor->IsA(SDK::ADeathItemCache_C::StaticClass())) {
        Func::绘制物品缓存((SDK::ADeathItemCache_C*)actor, canvas);
    }
}

static __forceinline void Func::绘制补给箱(SDK::APrimalStructureItemContainer_SupplyCrate* actor, SDK::UCanvas* canvas) {
    SDK::FVector world_location_3d = actor->K2_GetActorLocation();

    float distance = World::world_player_controller->GetDistanceTo(actor);
    distance *= Var::distance_div;

    if (distance > Var::draw_structure_limit_distance) {
        return;
    }

    SDK::FVector2D screen_location_2d = {0, 0};
    if (!World::world_player_controller->ProjectWorldLocationToScreen(world_location_3d, &screen_location_2d, true)) {
        return;
    }

    screen_location_2d.X /= Var::dpi_scale;
    screen_location_2d.Y /= Var::dpi_scale;
    wchar_t buffer[64] = {0};
    auto out = buffer;

    if (Var::toggle_draw_structure_airdrop) {
        out = std::format_to(out, L"{}\n{}\n", actor->DescriptiveName.CStr(), actor->RequiredLevelToAccess);
    }

    if (Var::toggle_draw_structure_distance) {
        out = std::format_to(out, L"{:.2f}\n", (float)distance);
    }

    SDK::FString fstring(buffer);

    canvas->K2_DrawText(
        ZeroGUI::Font,
        fstring,
        screen_location_2d,
        SDK::FVector2D {ZeroGUI::FontScale, ZeroGUI::FontScale},
        Color::颜色_补给箱,
        1.0f,
        SDK::FLinearColor(0, 0, 0, 1.0f),
        SDK::FVector2D {0, 0},
        true,
        true,
        true,
        SDK::FLinearColor(0, 0, 0, 1.0f)
    );
}

static __forceinline void Func::绘制物品缓存(SDK::ADeathItemCache_C* actor, SDK::UCanvas* canvas) {
    SDK::FVector world_location_3d = actor->K2_GetActorLocation();

    float distance = World::world_player_controller->GetDistanceTo(actor);
    distance *= Var::distance_div;

    if (distance > Var::draw_structure_limit_distance) {
        return;
    }

    SDK::FVector2D screen_location_2d = {0, 0};
    if (!World::world_player_controller->ProjectWorldLocationToScreen(world_location_3d, &screen_location_2d, true)) {
        return;
    }

    screen_location_2d.X /= Var::dpi_scale;
    screen_location_2d.Y /= Var::dpi_scale;

    wchar_t buffer[64] = {0};
    auto out = buffer;

    if (Var::toggle_draw_structure_item_cache) {
        out = std::format_to(out, L"{}\n", actor->DescriptiveName.CStr());
    }

    if (Var::toggle_draw_structure_distance) {
        out = std::format_to(out, L"{:.2f}\n", (float)distance);
    }

    SDK::FString fstring(buffer);

    canvas->K2_DrawText(
        ZeroGUI::Font,
        fstring,
        screen_location_2d,
        SDK::FVector2D {ZeroGUI::FontScale, ZeroGUI::FontScale},
        Color::颜色_物品缓存,
        1.0f,
        SDK::FLinearColor(0, 0, 0, 1.0f),
        SDK::FVector2D {0, 0},
        true,
        true,
        true,
        SDK::FLinearColor(0, 0, 0, 1.0f)
    );
}

static __forceinline void Func::绘制炮塔(SDK::APrimalStructureTurret* actor, SDK::UCanvas* canvas) {
    SDK::FVector world_location_3d = actor->K2_GetActorLocation();

    float distance = World::world_player_controller->GetDistanceTo(actor);
    distance *= Var::distance_div;

    if (distance > Var::draw_structure_limit_distance) {
        return;
    }

    SDK::FVector2D screen_location_2d = {0, 0};
    if (!World::world_player_controller->ProjectWorldLocationToScreen(world_location_3d, &screen_location_2d, true)) {
        return;
    }

    screen_location_2d.X /= Var::dpi_scale;
    screen_location_2d.Y /= Var::dpi_scale;

    wchar_t buffer[64] = {0};
    auto out = buffer;

    if (Var::toggle_draw_structure_turret) {
        out = std::format_to(out, L"{}\n", actor->DescriptiveName.CStr());
    }

    if (Var::toggle_draw_structure_distance) {
        out = std::format_to(out, L"{:.2f}\n", (float)distance);
    }

    SDK::FString fstring(buffer);

    canvas->K2_DrawText(
        ZeroGUI::Font,
        fstring,
        screen_location_2d,
        SDK::FVector2D {ZeroGUI::FontScale, ZeroGUI::FontScale},
        Color::颜色_物品缓存,
        1.0f,
        SDK::FLinearColor(0, 0, 0, 1.0f),
        SDK::FVector2D {0, 0},
        true,
        true,
        true,
        SDK::FLinearColor(0, 0, 0, 1.0f)
    );
}

static bool Func::更新世界() {
    World::world_gworld = SDK::UWorld::GetWorld();
    if (World::world_gworld == nullptr) {
        return false;
    }

    World::world_game_instance = reinterpret_cast<SDK::UShooterGameInstance*>(World::world_gworld->OwningGameInstance);
    if (World::world_game_instance == nullptr) {
        return false;
    }

    World::world_game_state = reinterpret_cast<SDK::AShooterGameState*>(World::world_gworld->GameState);
    if (World::world_game_state == nullptr) {
        return false;
    }

    if (!World::world_game_instance->LocalPlayers.IsValid()) {
        return false;
    }

    World::world_local_player = World::world_game_instance->LocalPlayers[0];
    if (World::world_local_player == nullptr) {
        return false;
    }

    World::world_player_controller = (SDK::AShooterPlayerController*)World::world_local_player->PlayerController;
    if (World::world_player_controller == nullptr) {
        return false;
    }

    World::world_player_character = (SDK::AShooterCharacter*)World::world_player_controller->AcknowledgedPawn;
    if (World::world_player_character == nullptr) {
        return false;
    }

    World::world_player_camera_manager = World::world_player_controller->PlayerCameraManager;
    if (World::world_player_camera_manager == nullptr) {
        return false;
    }

    World::world_player_camera_location = World::world_player_camera_manager->CameraCachePrivate.POV.Location;

    World::world_persistent_level = World::world_gworld->PersistentLevel;
    if (World::world_persistent_level == nullptr) {
        return false;
    }

    World::world_persistent_levels = World::world_gworld->Levels;
    if (!World::world_persistent_levels.IsValid()) {
        return false;
    }

    World::world_actors = World::world_persistent_level->Actors;
    if (!World::world_actors.IsValid()) {
        return false;
    }

    return true;
}

static __forceinline void init() {
    SDK::UEngine* engine = nullptr;
    SDK::UGameViewportClient* viewport = nullptr;

    do {
        engine = SDK::UEngine::GetEngine();
    } while (!engine);

    do {
        viewport = engine->GameViewport;
    } while (!viewport);

    do {
        ZeroGUI::Font = (SDK::UFont*)SDK::UObject::FindObject("Font SansationBold18.SansationBold18");
    } while (!ZeroGUI::Font);

    void* p_post_render = nullptr;
    // mov rax, [rcx] jmp [rax + offset] ... mov [rsp + offset], rbp mov [rsp + offset], rdi push r12 push r14
    auto aob_results = AOB::Scan("48 8B 01 48 FF A0 ? ? ? ? ? ? ? ? ? ? 48 89 6C 24 ? 48 89 7C 24 ? 41 54 41 56");

    if (aob_results && aob_results.size() > 0 && aob_results[0]) {
        p_post_render = aob_results[0];
    } else {
        void** VTable = *(void***)viewport;
        if (VTable)
            p_post_render = VTable[122];
    }

    if (p_post_render) {
        Var::dpi_scale = Func::get_dpi_scale();
        MH_Initialize();
        MH_CreateHook(p_post_render, reinterpret_cast<void*>(&hook_post_render), (LPVOID*)&original_post_render);
        MH_EnableHook(p_post_render);
    }
}

/*
static __forceinline void init() {
    SDK::UEngine* engine;
    SDK::UGameViewportClient* viewport;

    do {
        engine = SDK::UEngine::GetEngine();

    } while (!engine);

    do {
        viewport = engine->GameViewport;
    } while (!viewport);

    do {
        ZeroGUI::Font = (SDK::UFont*)SDK::UObject::FindObject("Font SansationBold18.SansationBold18");
    } while (!ZeroGUI::Font);

    void** VTable = *(void***)viewport;

    int Index = 122;
    void* p_post_render = VTable[Index];

    Var::dpi_scale = Func::get_dpi_scale();

    MH_Initialize();

    MH_CreateHook(p_post_render, reinterpret_cast<void*>(&hook_post_render), (LPVOID*)&original_post_render);
    MH_EnableHook(p_post_render);
}
*/

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved) {
    if (fdwReason == DLL_PROCESS_ATTACH) {
        // FILE* fp;
        // freopen_s(&fp, "1my.txt", "w", stdout);

        // if (AllocConsole()) {
        //     FILE* fp;

        //     freopen_s(&fp, "CONOUT$", "w", stdout);

        //     freopen_s(&fp, "CONOUT$", "w", stderr);
        //     freopen_s(&fp, "CONIN$", "r", stdin);
        // }

        CreateThread(nullptr, NULL, (LPTHREAD_START_ROUTINE)init, nullptr, NULL, nullptr);
    }

    return TRUE;
}

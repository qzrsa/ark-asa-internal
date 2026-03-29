#include "../vendor/minhook/include/MinHook.h"

#include "../vendor/CppSDK/SDK/Engine_classes.hpp"
#include "../vendor/CppSDK/SDK/StorageBox_TekTransmitter_classes.hpp"

#include "../vendor/CppSDK/SDK/PrimalInventoryBP_SupplyCrate_classes.hpp"
#include "../vendor/CppSDK/SDK/PrimalInventoryBP_SupplyCrate_parameters.hpp"

#include "../vendor/CppSDK/SDK/ShooterGame_classes.hpp"
#include "../vendor/CppSDK/SDK/ShooterGame_parameters.hpp"

#include "../vendor/CppSDK/SDK/DeathItemCache_classes.hpp"
#include "../vendor/CppSDK/SDK/PlayerControllerBlueprint_classes.hpp"

#include "ZeroGUI.h"

#include <basetsd.h>
#include <cfloat>
#include <cmath>
#include <consoleapi.h>
#include <cstdarg>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <format>
#include <libloaderapi.h>
#include <memory>
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

    // static SDK::UNetConnection* world_net_connection = nullptr;
    // static SDK::UNetDriver* world_net_driver = nullptr;
    // static SDK::UNetConnection* world_server_connection = nullptr;

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

    static __forceinline void 绘制遍历ACTORS(SDK::UCanvas*);

    static __forceinline void 绘制玩家ALL(SDK::AShooterCharacter*, SDK::UCanvas*);
    static __forceinline void 绘制玩家信息(SDK::AShooterCharacter*, SDK::UCanvas*, SDK::FLinearColor);
    static __forceinline void 绘制玩家骨骼(SDK::AShooterCharacter* r, SDK::UCanvas*, SDK::FLinearColor);

    static __forceinline void 绘制恐龙ALL(SDK::APrimalDinoCharacter*, SDK::UCanvas*);
    static __forceinline void 绘制恐龙(SDK::APrimalDinoCharacter*, SDK::UCanvas*, SDK::FLinearColor color);

    static __forceinline void 绘制容器ALL(SDK::APrimalStructureItemContainer*, SDK::UCanvas*);
    static __forceinline void 绘制空投(SDK::APrimalStructureItemContainer_SupplyCrate*, SDK::UCanvas*);
    static __forceinline void 绘制物品缓存(SDK::ADeathItemCache_C*, SDK::UCanvas*);

    static __forceinline void 解锁笔记();
    static __forceinline void 原地上传();
    static __forceinline void 灵魂出窍();

    static __forceinline void print_all_fonts();
} // namespace Func

namespace Var {

    const float distance_div = 0.01;
    const float min_distance_slider = 1.0;
    const float max_distance_slider = 10000.0;

    static float dpi_scale = 1.25f; // 初始化
    static float center_x = 0.0f;
    static float center_y = 0.0f;

    // Menu
    static auto menu_pos = SDK::FVector2D {0, 0};
    static bool toggle_menu = true;
    static bool toggle_menu_last_state = false;

    // esp
    static bool toggle_draw_server_info = false;

    // esp player
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

    static bool toggle_draw_structure_supply_crate = false;
    static bool toggle_draw_structure_item_cache = false;

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
    static bool toggle_last_state_灵魂出窍 = false;

    static bool toggle_原地上传 = false;
    static int hotkey_原地上传 = VK_F12;

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
    static SDK::FLinearColor 颜色_空投 = {1.0f, 1.0f, 0.0f, 1.0f}; // 空投

    static SDK::FLinearColor 颜色_自瞄FOV = {1.0f, 1.0f, 1.0f, 1.0f};
} // namespace Color

namespace HookFunc {

    // ========== hook_process_event ==========
    using TypeProcessEvent = void(__fastcall*)(SDK::UObject*, SDK::UFunction*, void*);

    static TypeProcessEvent original_process_event = nullptr;

    static int32_t function_index_0 = -1;
    static int32_t function_index_1 = -1;
    static int32_t function_index_2 = -1;

    void __fastcall hook_process_event(SDK::UObject* object, SDK::UFunction* function, void* params) {
        // if (function) {
        //     printf("%s\n", function->GetFullName().c_str());
        // }

        // return original_process_event(object, function, params);

        if (!object || !function) {
            return original_process_event(object, function, params);
        }

        if (function_index_0 == -1) {
            auto s = function->GetFullName();

            if (function_index_0 == -1
                && s.compare(
                       "Function PrimalInventoryBP_SupplyCrate.PrimalInventoryBP_SupplyCrate_C.BPRemoteInventoryAllowViewing"
                   ) == 0) {
                function_index_0 = function->Index;
            }
        }

        if (function->Index == function_index_0) {
            if (!World::world_player_controller) {
                return original_process_event(object, function, params);
            }

            auto para = reinterpret_cast<SDK::Params::PrimalInventoryBP_SupplyCrate_C_BPRemoteInventoryAllowViewing*>(params);
            para->PC = World::world_player_controller;
            para->ReturnValue = true;

            auto obj = reinterpret_cast<SDK::UPrimalInventoryBP_SupplyCrate_C*>(object);
            obj->MaxInventoryAccessDistance = static_cast<float>(FLT_MAX);
            obj->MaxRemoteInventoryViewingDistance = static_cast<float>(FLT_MAX);
            obj->bReceivingArkInventoryItems = false;
            obj->bReceivingInventoryItems = false;
            obj->bReceivingEquippedItems = false;
        }

        if (function_index_1 == -1) {
            auto s = function->GetFullName();

            if (function_index_1 == -1 && s.compare("Function ShooterGame.ShooterWeapon_Instant.SimulateInstantHit") == 0) {
                function_index_1 = function->Index;
            }
        }

        if (function->Index == function_index_1) {
            if (!Var::aim_best_closest_actor || Var::aim_best_closest_actor->bIsDead || !Var::aim_best_closest_actor->Mesh) {
                return original_process_event(object, function, params);
            }

            static SDK::FName head_name = SDK::UKismetStringLibrary::Conv_StringToName(L"Cnt_Head_JNT_SKL");
            SDK::FVector target_loc = Var::aim_best_closest_actor->Mesh->GetSocketLocation(head_name);

            auto para = reinterpret_cast<SDK::Params::ShooterWeapon_Instant_SimulateInstantHit*>(params);

            // 计算从开火起点指向目标的法线向量
            SDK::FVector new_dir = SDK::UKismetMathLibrary::GetDirectionUnitVector(para->Origin, target_loc);

            para->ShootDir = (SDK::FVector_NetQuantizeNormal)new_dir;
        }

        if (function_index_2 == -1) {
            auto s = function->GetFullName();

            if (function_index_2 == -1 && s.compare("Function ShooterGame.ShooterWeapon_Instant.ServerNotifyShot") == 0) {
                function_index_2 = function->Index;
            }
        }

        if (function->Index == function_index_2) {
            if (Var::aim_best_closest_actor && !Var::aim_best_closest_actor->bIsDead && Var::aim_best_closest_actor->Mesh) {
                auto para = reinterpret_cast<SDK::Params::ShooterWeapon_Instant_ServerNotifyShot*>(params);
                SDK::AShooterWeapon_Instant* weapon = (SDK::AShooterWeapon_Instant*)object;

                static SDK::FName head_name = SDK::UKismetStringLibrary::Conv_StringToName(L"Cnt_Head_JNT_SKL");
                SDK::FVector target_loc = Var::aim_best_closest_actor->Mesh->GetSocketLocation(head_name);
                SDK::FVector muzzle = weapon->GetMuzzleLocation();

                auto& mutableImpacts = const_cast<SDK::TArray<SDK::FHitResult>&>(para->Impacts);
                auto& mutableDirs = const_cast<SDK::TArray<SDK::FVector_NetQuantizeNormal>&>(para->ShootDirs);

                // 确保至少有一个命中记录
                if (mutableImpacts.Num() == 0) {
                    SDK::FHitResult fake_hit {};
                    mutableImpacts.Add(fake_hit);
                }
                if (mutableDirs.Num() == 0) {
                    SDK::FVector_NetQuantizeNormal fake_dir {};
                    mutableDirs.Add(fake_dir);
                }

                SDK::FVector shoot_dir = SDK::UKismetMathLibrary::GetDirectionUnitVector(muzzle, target_loc);
                SDK::FVector diff = {target_loc.X - muzzle.X, target_loc.Y - muzzle.Y, target_loc.Z - muzzle.Z};
                float real_distance = sqrtf(diff.X * diff.X + diff.Y * diff.Y + diff.Z * diff.Z);

                for (int i = 0; i < mutableImpacts.Num(); i++) {
                    SDK::FHitResult& hit = mutableImpacts[i];

                    // ? 关键修复：正确构造 TWeakObjectPtr
                    // 方法1：如果 SDK 支持 Get() 方法
                    hit.HitObjectHandle.ReferenceObject.ObjectIndex = Var::aim_best_closest_actor->Index;
                    hit.HitObjectHandle.ReferenceObject.ObjectSerialNumber = 0; // 通常为0

                    hit.Component.ObjectIndex = Var::aim_best_closest_actor->Mesh->Index;
                    hit.Component.ObjectSerialNumber = 0;

                    // 方法2：如果上面不行，尝试直接赋值内部指针（某些 SDK 版本）
                    // *reinterpret_cast<SDK::AActor**>(&hit.Actor) = Var::aim_best_closest_actor;
                    // *reinterpret_cast<SDK::UPrimitiveComponent**>(&hit.Component) = Var::aim_best_closest_actor->Mesh;

                    // 物理参数
                    hit.bBlockingHit = true;
                    hit.Distance = real_distance;
                    hit.Time = real_distance / 100000.0f; // 根据武器调整
                    hit.BoneName = head_name;
                    *(int32_t*)(&hit.PhysMaterial) = -1;
                    hit.FaceIndex = -1;
                    hit.Item = -1;

                    // 射线路径
                    hit.TraceStart = (SDK::FVector_NetQuantize)muzzle;
                    hit.TraceEnd = (SDK::FVector_NetQuantize)target_loc;
                    hit.Location = (SDK::FVector_NetQuantize)target_loc;
                    hit.ImpactPoint = (SDK::FVector_NetQuantize)target_loc;

                    // 法线方向
                    SDK::FVector impact_normal = shoot_dir * -1.0f;
                    hit.Normal = (SDK::FVector_NetQuantizeNormal)impact_normal;
                    hit.ImpactNormal = hit.Normal;
                }

                // 修正射击方向
                for (int i = 0; i < mutableDirs.Num(); i++) {
                    mutableDirs[i] = (SDK::FVector_NetQuantizeNormal)shoot_dir;
                }
            }
            return original_process_event(object, function, params);
        }

        original_process_event(object, function, params);
    }

    // ========== hook_post_render ==========

    using TypePostRender = void(__fastcall*)(SDK::UGameViewportClient*, SDK::UCanvas* Canvas);

    static TypePostRender original_post_render = nullptr;

    void __fastcall hook_post_render(SDK::UGameViewportClient* _this, SDK::UCanvas* canvas) {
        original_post_render(_this, canvas);

        if (!canvas || !Func::更新世界()) {
            return;
        }

        Var::center_x = (canvas->SizeX / Var::dpi_scale) * 0.5f;
        Var::center_y = (canvas->SizeY / Var::dpi_scale) * 0.5f;

        if (GetAsyncKeyState(VK_F9) & 1) {
            Var::toggle_menu = !Var::toggle_menu;
        }

        if (GetAsyncKeyState(Var::hotkey_原地上传) & 1) {
            Func::原地上传();
        }

        Func::绘制菜单(canvas);

        if (Var::toggle_menu != Var::toggle_menu_last_state) {
            if (Var::toggle_menu) {
                World::world_player_controller->SetIgnoreLookInput(true); // 禁用转向
                World::world_player_controller->SetIgnoreMoveInput(true); // 禁用移动
            } else {
                World::world_player_controller->SetIgnoreLookInput(false); // 禁用转向
                World::world_player_controller->SetIgnoreMoveInput(false); // 禁用移动
            }

            Var::toggle_menu_last_state = Var::toggle_menu;
        }

        Func::绘制遍历ACTORS(canvas);

        if (Var::toggle_draw_server_info) {
            Func::绘制服务器信息(canvas, Color::颜色_服务器信息);
        }

        if (Var::toggle_aim_fov) {
            Func::绘制自瞄FOV(canvas);
        }

        // for (SDK::ULevel* level : world_persistent_levels) {
        //     auto actors = level->Actors;

        //     if (!actors.IsValid()) {
        //         continue;
        //     }
    }

} // namespace HookFunc

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
    ZeroGUI::Checkbox(const_cast<wchar_t*>(L"拥有者"), &Var::toggle_draw_dino_owner);

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

    ZeroGUI::Checkbox(const_cast<wchar_t*>(L"补给箱"), &Var::toggle_draw_structure_airdrop);
    ZeroGUI::SameLine();
    ZeroGUI::ColorPicker(const_cast<wchar_t*>(L"补给箱颜色"), &Color::颜色_空投);

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
    ZeroGUI::Checkbox(const_cast<wchar_t*>(L"灵魂出窍"), &Var::toggle_灵魂出窍);
    Func::灵魂出窍();

    if (ZeroGUI::Button(const_cast<wchar_t*>(L"一键秒级"), SDK::FVector2D {70, 20})) {
        Func::解锁笔记();
    }

    if (ZeroGUI::Button(const_cast<wchar_t*>(L"一键自杀"), SDK::FVector2D {70, 20})) {
        World::world_player_character->BPSuicide();
    }

    ZeroGUI::Text(const_cast<wchar_t*>(L"原地上传"));

    ZeroGUI::Hotkey(SDK::FVector2D {60, 25}, &Var::hotkey_原地上传);
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

    // 1. 检查按键状态
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

// static __forceinline  void Func::自瞄_更新玩家(SDK::AShooterCharacter* actor) {
//     SDK::FName HeadBoneName = SDK::UKismetStringLibrary::Conv_StringToName(L"head");
//     if (!actor->Mesh) {
//         return;
//     }
//     auto s = actor->Mesh->GetAllSocketNames();

//     printf("%s\n", actor->Class->GetFullName().c_str());
//     printf("11111111111111111111111\n");
//     for (auto a : s) {
//         printf("%s\t%d\n", a.ToString().c_str(), actor->Mesh->GetBoneIndex(a));
//     }

//     printf("==========================================\n");
// }

void inline Func::绘制遍历ACTORS(SDK::UCanvas* canvas) {
    for (SDK::AActor* actor : World::world_persistent_level->Actors) {
        if (!actor || IsBadReadPtr(actor, 8)) {
            continue;
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
    }

    if (Var::toggle_aim) {
        Func::自瞄();
    }
}

static __forceinline void Func::绘制玩家ALL(SDK::AShooterCharacter* actor, SDK::UCanvas* canvas) {
    if ((!Var::toggle_draw_player_dead && actor->bIsDead) || (!Var::toggle_draw_player_offline && actor->bIsSleeping)) {
        return;
    }

    const bool is_friendly = actor->IsPrimalCharFriendly(World::world_player_character);

    // 3. 获取对应颜色引用（避免在分支中重复查找）
    const SDK::FLinearColor& draw_color = is_friendly ? Color::颜色_友好玩家 : Color::颜色_敌对玩家;

    if (is_friendly) {
        // 友军逻辑块
        if (Var::toggle_draw_tribe_player_info) {
            Func::绘制玩家信息(actor, canvas, draw_color);
        }
        if (Var::toggle_draw_tribe_player_bone) {
            Func::绘制玩家骨骼(actor, canvas, draw_color);
        }

        // if (Var::toggle_aim) {
        Func::自瞄_获取最佳目标(actor, canvas);
        // }
    } else {
        if (Var::toggle_draw_enemy_player_info) {
            // if (Var::toggle_aim) {
            Func::自瞄_获取最佳目标(actor, canvas);
            // }
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
    // 1. 预缓存所有需要的 FName（静态初始化一次，彻底消除字符串查找）
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

    // 2. 使用更紧凑的局部缓存
    struct BoneLink {
        int32_t a;
        int32_t b;
        SDK::FName* name_a;
        SDK::FName* name_b;
    };

    static const BoneLink links[] = {
        {8, 5, &b_8, &b_5},
        {5, 1, &b_5, &b_1},
        {5, 152, &b_5, &b_152},
        {5, 179, &b_5, &b_179},
        {1, 206, &b_1, &b_206},
        {1, 212, &b_1, &b_212}
    };

    // 3. 栈分配：仅需记录这几个点的坐标和状态
    SDK::FVector2D pos_cache[233];
    bool is_valid[233] = {false};
    auto* mesh = actor->Mesh;

    for (const auto& link : links) {
        // 处理 A 点
        if (!is_valid[link.a]) {
            // 直接传入缓存的 FName，避开 GetBoneName
            SDK::FVector w_pos = mesh->GetSocketLocation(*link.name_a);
            if (World::world_player_controller->ProjectWorldLocationToScreen(w_pos, &pos_cache[link.a], false)) {
                pos_cache[link.a].X /= Var::dpi_scale;
                pos_cache[link.a].Y /= Var::dpi_scale;
                is_valid[link.a] = true;
            }
        }
        // 处理 B 点
        if (!is_valid[link.b]) {
            SDK::FVector w_pos = mesh->GetSocketLocation(*link.name_b);
            if (World::world_player_controller->ProjectWorldLocationToScreen(w_pos, &pos_cache[link.b], false)) {
                pos_cache[link.b].X /= Var::dpi_scale;
                pos_cache[link.b].Y /= Var::dpi_scale;
                is_valid[link.b] = true;
            }
        }

        if (is_valid[link.a] && is_valid[link.b]) {
            canvas->K2_DrawLine(pos_cache[link.a], pos_cache[link.b], 1.0f, color);
        }
    }
}

static __forceinline void Func::绘制恐龙ALL(SDK::APrimalDinoCharacter* actor, SDK::UCanvas* canvas) {
    if (!Var::toggle_draw_dino_dead && actor->IsDead()) {
        return;
    }

    // 2. 状态分流逻辑

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

    if (Var::toggle_draw_dino_owner && actor->OwningPlayerName.IsValid()) {
        out = std::format_to(out, L"{}\n", actor->OwningPlayerName.CStr());
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

    // 3. 直接构造 FString，buffer 已经是标准的 C 风格字符串
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
    if (Var::toggle_draw_structure_supply_crate && actor->IsA(SDK::APrimalStructureItemContainer_SupplyCrate::StaticClass())) {
        Func::绘制空投((SDK::APrimalStructureItemContainer_SupplyCrate*)actor, canvas);
    }

    if (Var::toggle_draw_structure_item_cache && actor->IsA(SDK::ADeathItemCache_C::StaticClass())) {
        Func::绘制物品缓存((SDK::ADeathItemCache_C*)actor, canvas);
    }
}

static __forceinline void Func::绘制空投(SDK::APrimalStructureItemContainer_SupplyCrate* actor, SDK::UCanvas* canvas) {
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

    SDK::FString fstring(actor->DescriptiveName.CStr());

    canvas->K2_DrawText(
        ZeroGUI::Font,
        fstring,
        screen_location_2d,
        SDK::FVector2D {ZeroGUI::FontScale, ZeroGUI::FontScale},
        Color::颜色_空投,
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

    std::wstring info = std::format(
        L"",

        actor->DescriptiveName.IsValid() ? std::format(L"名称 {}\n", actor->DescriptiveName.CStr()) : L"",

        Var::toggle_draw_dino_distance ? std::format(L"距离 {:.2f}\n", (float)distance) : L""

    );
    SDK::FString fstring(info.c_str());

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

    // world_net_connection = world_player_controller->NetConnection;
    // if (world_net_connection == nullptr) {
    //     return false;
    // }

    // world_net_driver = world_net_connection->Driver;
    // if (world_net_driver == nullptr) {
    //     return false;
    // }

    // world_server_connection = world_net_driver->ServerConnection;
    // if (world_server_connection == nullptr) {
    //     return false;
    // }

    // world_player_controller_net =
    //     (SDK::APlayerController*)world_server_connection->OwningActor;
    // if (world_player_controller_net == nullptr) {
    //     return false;
    // }

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

static __forceinline void Func::解锁笔记() {
    uintptr_t module_address = reinterpret_cast<uintptr_t>(GetModuleHandleA("ArkAscended.exe"));

    typedef __int64(__fastcall * UnlockExplorerNote_V2)(uintptr_t a1, int a2, unsigned char a3, unsigned char a4, char a5);

    UnlockExplorerNote_V2 unlock_explorer_v2 = (UnlockExplorerNote_V2)(module_address + 0x713C0A0);

    for (int inner = 0; inner < 1050; inner++) {
        // 这里可以写你需要的逻辑，n就是1-500的连续值
        unlock_explorer_v2((uintptr_t)World::world_player_controller, inner, 0, 0, 0);
    }
}

static __forceinline void Func::原地上传() {
    SDK::UPrimalInventoryComponent* component = nullptr;
    float best_distance = FLT_MAX;

    for (SDK::AActor* actor : World::world_persistent_level->Actors) {
        if (!actor || IsBadReadPtr(actor, 8)) {
            continue;
        }

        if (actor->IsPrimalStructureItemContainer()) {
            auto actor_ = reinterpret_cast<SDK::APrimalStructureItemContainer*>(actor);

            if (actor_->DescriptiveName.ToWString() == L"补给箱") {
                float current_distance = World::world_player_controller->GetDistanceTo(actor);

                if (current_distance < best_distance) {
                    best_distance = current_distance;
                    component = ((SDK::APrimalStructureItemContainer_SupplyCrate*)actor)->MyInventoryComponent;
                }
            }
        }
    }

    if (World::world_player_character->MyInventoryComponent && World::world_player_controller->MyHUD && component) {
        // ClientReceiveInventoryItems
        // ServerSetViewingInventory
        // BPNotifyInventoryItemChange
        // ServerProcessNetExecCommand
        // ServerRequestActorItems
        // ServerActorViewRemoteInventory

        component->MaxInventoryAccessDistance = FLT_MAX;
        component->MaxRemoteInventoryViewingDistance = FLT_MAX;
        // component->bIsTributeInventory = true;

        World::world_player_character->ServerSetViewingInventory(true, true);

        auto HUD = (SDK::AShooterHUD*)World::world_player_controller->MyHUD;
        if (HUD) {
            HUD->ShowInventory(component);
        }
    }

    component = nullptr;
    best_distance = FLT_MAX;
}

static __forceinline void Func::灵魂出窍() {
    auto* movement = World::world_player_character->CharacterMovement;
    if (!movement)
        return;

    if (Var::toggle_灵魂出窍 != Var::toggle_last_state_灵魂出窍) {
        if (Var::toggle_灵魂出窍) {
            SDK::UCharacterMovementComponent* movement = World::world_player_character->CharacterMovement;
            if (World::world_player_character && movement) {
                World::world_player_character->SetReplicateMovement(false);
                World::world_player_character->SetActorEnableCollision(false);
                movement->SetMovementMode(SDK::EMovementMode::MOVE_Flying, 0);
                movement->MaxFlySpeed = 10000.f;
                movement->MaxAcceleration = 5000.f;
                movement->BrakingDecelerationFlying = 10000.f;

                movement->BrakingDecelerationFlying = 500000.f;
                movement->FallingLateralFriction = 500000.f;
                movement->Acceleration = SDK::FVector {0.f, 0.f, 0.f};
                movement->BrakingFriction = 500000.f;
                movement->bUseSeparateBrakingFriction = true;

                if (movement->MovementMode != SDK::EMovementMode::MOVE_Flying) {
                    movement->SetMovementMode(SDK::EMovementMode::MOVE_Flying, 0);
                }
            }
        } else {
            if (World::world_player_character && World::world_player_character->CharacterMovement) {
                movement->StopMovementImmediately();
                movement->Velocity = SDK::FVector {0.f, 0.f, 0.f};
                movement->Acceleration = SDK::FVector {0.f, 0.f, 0.f};
                movement->BrakingDecelerationFlying = 0.f;
                movement->FallingLateralFriction = 0.f;
                movement->SetMovementMode(SDK::EMovementMode::MOVE_Walking, 0);
                World::world_player_character->SetActorEnableCollision(true);
                World::world_player_character->SetReplicateMovement(true);
                auto controller = (SDK::APlayerControllerBlueprint_C*)World::world_player_controller;
                controller->TogglePhotoMode();
                controller->TogglePhotoMode();
            }
        }

        Var::toggle_last_state_灵魂出窍 = Var::toggle_灵魂出窍;
    }
}

// 枚举所有 UFont 对象
static __forceinline void Func::print_all_fonts() {
    for (int i = 0; i < SDK::UObject::GObjects->Num(); i++) {
        SDK::UObject* Object = SDK::UObject::GObjects->GetByIndex(i);

        if (!Object)
            continue;

        // Object->IsA(SDK::UFont::StaticClass())
        if (Object->IsA(SDK::UUserInterfaceSettings::StaticClass())) {
            std::string FullName = Object->GetFullName();

            // 打印所有字体对象
            printf("Font: %s\n", FullName.c_str());
        }
    }
}

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

    int Index = 121;
    void* Target = VTable[Index];

    Var::dpi_scale = Func::get_dpi_scale();

    MH_Initialize();

    MH_CreateHook(Target, reinterpret_cast<void*>(&HookFunc::hook_post_render), (LPVOID*)&HookFunc::original_post_render);
    MH_EnableHook(Target);

    void* pProcessEvent = reinterpret_cast<void*>(SDK::InSDKUtils::GetImageBase() + SDK::Offsets::ProcessEvent);

    MH_CreateHook(
        pProcessEvent,
        reinterpret_cast<void*>(&HookFunc::hook_process_event),
        reinterpret_cast<void**>(&HookFunc::original_process_event)
    );
    MH_EnableHook(pProcessEvent);
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved) {
    if (fdwReason == DLL_PROCESS_ATTACH) {
        FILE* fp;
        // freopen_s(&fp, "1.txt", "w", stdout);

        if (AllocConsole()) {
            FILE* fp;

            freopen_s(&fp, "CONOUT$", "w", stdout);

            freopen_s(&fp, "CONOUT$", "w", stderr);
            freopen_s(&fp, "CONIN$", "r", stdin);
        }

        // printf("========================================\n");
        // printf("ATTACH\n");
        // printf("========================================\n");

        // 这些代码执行了一次
        DisableThreadLibraryCalls(hinstDLL);

        CreateThread(nullptr, NULL, (LPTHREAD_START_ROUTINE)init, nullptr, NULL, nullptr);
    }

    // printf("========================================\n");
    // printf("DllMainBody\n");
    // printf("========================================\n");

    static bool call_once = false;

    if (!call_once) {
        call_once = true;

        static wchar_t file_name[MAX_PATH] = L"";
        GetModuleFileNameW(hinstDLL, file_name, _countof(file_name));
        LoadLibraryW(file_name);

        // printf("========================================\n");
        // printf("LoadLibraryW\n");
        // printf("========================================\n");
    }

    return TRUE;
}

extern "C" __declspec(dllexport) LRESULT decl_func(int32_t code, WPARAM w_param, LPARAM l_param) {
    // printf("========================================\n");
    // printf("decl_func\n");
    // printf("========================================\n");

    // 这些代码执行了三次
    static auto call_once = false;
    const auto p_msg = reinterpret_cast<MSG*>(l_param);

    if (!call_once && p_msg->message == 0x5B0) {
        // printf("========================================\n");
        // printf("message == 0x5B0\n");
        // printf("========================================\n");

        // 这些代码没有执行
        UnhookWindowsHookEx(reinterpret_cast<HHOOK>(l_param));
        call_once = true;
    }

    return CallNextHookEx(nullptr, code, w_param, l_param);
}

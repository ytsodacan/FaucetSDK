/**
 * @file ExampleMod.cpp
 * @brief Template mod — copy this folder and rename to start a new mod.
 *
 * Demonstrates calling real Minecraft LCE game functions directly.
 * All calls use instance methods on real classes — no function pointers,
 * no static wrappers, full IDE autocomplete on everything.
 */

#include <IMod.h>
#include <SDK.h>

class ExampleMod final : public IMod {
public:

    // -------------------------------------------------------------------------
    // Metadata
    // -------------------------------------------------------------------------

    const ModInfo* GetInfo() const override {
        static const ModInfo info {
            "com.yourname.examplemod",  // unique ID — change this!
            "Example Mod",
            "Your Name",
            "A starting-point mod.",
            { 1, 0, 0 }
        };
        return &info;
    }

    // -------------------------------------------------------------------------
    // OnLoad — all mod DLLs are loaded, but the game hasn't initialised yet.
    // Do NOT call Minecraft/MinecraftServer APIs here.
    // -------------------------------------------------------------------------

    bool OnLoad() override {
        SDK::Log(L"ExampleMod: OnLoad");
        return true;
    }

    // -------------------------------------------------------------------------
    // OnInit — Minecraft and MinecraftServer are fully initialised.
    // Safe to call any game API from here onwards.
    // -------------------------------------------------------------------------

    bool OnInit() override {
        SDK::Log(L"ExampleMod: OnInit");

        Minecraft*       client = Minecraft::GetInstance();
        MinecraftServer* server = MinecraftServer::getInstance();

        // ---- Local player (client side) -------------------------------------
        if (client) {
            auto* player = client->localplayers[0].get();
            if (player) {
                float hp    = player->getHealth();
                float maxHp = player->getMaxHealth();
                SDK::Log(L"Local player HP: " + std::to_wstring((int)hp) +
                         L" / "               + std::to_wstring((int)maxHp));

                // Full heal
                player->setHealth(maxHp);
                SDK::Log(L"Healed local player to full HP.");
            }
        }

        // ---- All connected players (server side) ----------------------------
        if (server) {
            PlayerList* list = server->getPlayers();
            if (list) {
                SDK::Log(L"Players online: " + std::to_wstring(list->getPlayerCount()));

                // Announce to everyone
                list->sendMessage(L"", L"ExampleMod has loaded on this server!");

                // Iterate every connected player
                for (auto& sp : list->players) {
                    if (!sp) continue;
                    sp->sendMessage(
                        L"[ExampleMod] Welcome, " + sp->getName() + L"!",
                        0 /* ChatPacket::e_ChatCustom */
                    );
                }
            }

            // ---- Server level (Overworld = dimension 0) ---------------------
            ServerLevel* level = server->getLevel(0);
            if (level) {
                // Send a particle burst at world origin
                level->sendParticles(L"explode", 0.0, 64.0, 0.0, 5);
                SDK::Log(L"Overworld level found.");
            }
        }

        return true;
    }

    // -------------------------------------------------------------------------
    // OnUpdate — called every game tick (~20/sec).
    // -------------------------------------------------------------------------

    bool OnUpdate(float deltaTime) override {
        m_tickCount++;

        // Do something once per second (every 20 ticks)
        if (m_tickCount % 20 == 0) {
            onSecondTick();
        }

        return true;
    }

    // -------------------------------------------------------------------------
    // OnShutdown — clean up before the DLL is unloaded.
    // -------------------------------------------------------------------------

    void OnShutdown() override {
        SDK::Log(L"ExampleMod: shutting down after " +
                 std::to_wstring(m_tickCount) + L" ticks.");
    }

private:

    // -------------------------------------------------------------------------
    // Called once per second from OnUpdate
    // -------------------------------------------------------------------------

    void onSecondTick() {
        MinecraftServer* server = MinecraftServer::getInstance();
        if (!server) return;

        PlayerList* list = server->getPlayers();
        if (!list || list->getPlayerCount() == 0) return;

        for (auto& sp : list->players) {
            if (!sp) continue;

            float hp    = sp->getHealth();
            float maxHp = sp->getMaxHealth();

            // Auto-heal any player below 5 HP (2.5 hearts)
            if (hp > 0.0f && hp < 5.0f) {
                sp->heal(4.0f); // restore 2 hearts

                sp->sendMessage(
                    L"[ExampleMod] You were auto-healed!",
                    0 /* ChatPacket::e_ChatCustom */
                );

                SDK::Log(L"Auto-healed " + sp->getName() +
                         L" (" + std::to_wstring((int)hp) + L" -> " +
                         std::to_wstring((int)(hp + 4.0f)) + L" HP)");
            }
        }
    }

    int m_tickCount = 0;
};

// -----------------------------------------------------------------------------
// Required factory export — the modloader calls this to create your mod object.
// Must be extern "C" to prevent name mangling.
// -----------------------------------------------------------------------------
extern "C" __declspec(dllexport) IMod* CreateMod() {
    return new ExampleMod();
}

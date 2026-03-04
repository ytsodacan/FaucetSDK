/**
 * @file ExampleMod.cpp
 * @brief Template mod — copy this folder and rename to start a new mod.
 *
 * This mod demonstrates calling game functions directly.
 * Notice: no casts, no function pointers, full IDE autocomplete on everything.
 */

// Include the SDK — this is all you need
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
    // Lifecycle
    // -------------------------------------------------------------------------

    bool OnLoad() override {
        Log::Print("ExampleMod loaded.");

        // Register a console command — type "heal_me" in the in-game console
        Game::RegisterCommand("heal_me", [](const char** argv, int argc) {
            Player* p = Game::GetLocalPlayer();
            if (!p) { Log::Warn("No local player!"); return; }

            float max = Player::GetMaxHealth(p);
            Player::SetHealth(p, max);
            Log::Printf("Healed player to %.0f HP.", max);
        });

        // Subscribe to events
        Events::OnLevelLoaded([](const char* levelName) {
            Log::Printf("Level loaded: %s", levelName);
        });

        Events::OnEntitySpawn([](Entity* entity) {
            // Called for every entity spawned — keep this fast!
        });

        return true;
    }

    bool OnInit() override {
        // Game APIs are now safe to use

        if (!Game::IsInGame()) return true; // might not be in a level yet

        Player* player = Game::GetLocalPlayer();
        if (player) {
            Log::Printf("Player spawned with %.0f / %.0f HP",
                Player::GetHealth(player),
                Player::GetMaxHealth(player));
        }

        World* world = Game::GetWorld();
        if (world) {
            Entity* ents[512];
            int count = World::GetEntities(world, ents, 512);
            Log::Printf("World has %d entities.", count);
        }

        return true;
    }

    bool OnUpdate(float dt) override {
        // Nothing this mod needs to do every frame
        return true;
    }

    void OnShutdown() override {
        Log::Print("ExampleMod shutting down.");

        // Always unregister commands and events in shutdown!
        Game::UnregisterCommand("heal_me");
    }
};

// -----------------------------------------------------------------------------
// Required export — the modloader calls this to create your mod object
// -----------------------------------------------------------------------------
extern "C" __declspec(dllexport) IMod* CreateMod() {
    return new ExampleMod();
}

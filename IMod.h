#pragma once

/**
 * @file IMod.h
 * @brief Core interface every mod must implement.
 *
 * @par Minimal mod
 * @code
 *   #include <IMod.h>
 *   #include <SDK.h>
 *
 *   class MyMod : public IMod {
 *   public:
 *       const ModInfo* GetInfo() const override {
 *           static const ModInfo info {
 *               "com.me.mymod", "My Mod", "Me", "Does stuff", {1,0,0}
 *           };
 *           return &info;
 *       }
 *       bool OnLoad()           override { return true; }
 *       bool OnInit()           override { return true; }
 *       bool OnUpdate(float dt) override { return true; }
 *       void OnShutdown()       override {}
 *   };
 *
 *   extern "C" __declspec(dllexport) IMod* CreateMod() { return new MyMod(); }
 * @endcode
 */

/** @brief Mod semantic version. */
struct ModVersion {
    int major; ///< Breaking changes
    int minor; ///< New features, backwards-compatible
    int patch; ///< Bug fixes
};

/**
 * @brief Static metadata returned by IMod::GetInfo().
 * All string pointers must remain valid for the mod's entire lifetime.
 */
struct ModInfo {
    const char* id;          ///< Unique reverse-domain ID, e.g. "com.yourname.mymod"
    const char* name;        ///< Human-readable display name
    const char* author;      ///< Author name(s)
    const char* description; ///< Short description
    ModVersion  version;     ///< Current version
};

/**
 * @brief Interface every mod DLL must implement.
 *
 * Lifecycle order:
 * | Step | Call           | When                                        |
 * |------|----------------|---------------------------------------------|
 * | 1    | CreateMod()    | DLL loaded — factory function               |
 * | 2    | GetInfo()      | Immediately after creation                  |
 * | 3    | OnLoad()       | All mods loaded, before server/client init  |
 * | 4    | OnInit()       | After Minecraft and MinecraftServer are up  |
 * | 5    | OnUpdate()     | Every game tick (20/sec)                    |
 * | 6    | OnShutdown()   | On exit                                     |
 */
class IMod {
public:
    virtual ~IMod() = default;

    /** @brief Return static metadata. Pointer must stay valid forever. */
    virtual const ModInfo* GetInfo() const = 0;

    /**
     * @brief First call. Set up hooks, register commands here.
     * Do NOT call game APIs (Level, Player, etc.) — use OnInit() for that.
     * @return false to abort this mod.
     */
    virtual bool OnLoad() = 0;

    /**
     * @brief Called after Minecraft and MinecraftServer finish initializing.
     * Safe to call Minecraft::GetInstance(), MinecraftServer::getInstance(), etc.
     * @return false to abort this mod.
     */
    virtual bool OnInit() = 0;

    /**
     * @brief Called every game tick (~20 times per second).
     * @param deltaTime Seconds since last tick (usually ~0.05).
     * @return false to stop receiving ticks (not an error).
     */
    virtual bool OnUpdate(float deltaTime) = 0;

    /**
     * @brief Called on shutdown. Unregister commands/events, free resources.
     */
    virtual void OnShutdown() = 0;

    virtual void OnRegister() = 0;

    virtual void OnLevelLoad() = 0;

    virtual void OnLevelUnload() = 0;
};

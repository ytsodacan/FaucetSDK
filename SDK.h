#pragma once
#include "ModExport.h"
#include <string>
#include <vector>
#include <memory>

/**
 * @file SDK.h
 * @brief Public modding API for Minecraft Legacy Console Edition.
 *
 * Include this single header in your mod to access all game systems.
 * Every function declared here maps directly to a real method in the game —
 * your IDE will show full autocomplete, parameter types, and docs.
 *
 * @par Quick start
 * @code
 *   #include <SDK.h>
 *
 *   bool MyMod::OnInit() {
 *       // Get the server and client instances
 *       MinecraftServer* server = SDK::GetServer();
 *       Minecraft*       client = SDK::GetClient();
 *
 *       // Get the local player and manipulate them
 *       if (auto* player = SDK::GetLocalPlayer()) {
 *           SDK::Log(L"Player health: " + std::to_wstring(player->getHealth()));
 *           player->setHealth(20.0f);
 *       }
 *
 *       // Send a message to all players
 *       SDK::BroadcastMessage(L"A mod has loaded!");
 *       return true;
 *   }
 * @endcode
 *
 * @par Linking
 * In your mod's Visual Studio project:
 *   - Additional Include Directories: path to sdk/
 *   - Additional Dependencies: Minecraft.lib  (from your build output)
 */

// ============================================================================
// Forward declarations — real game classes, not wrappers
// ============================================================================

class Minecraft;
class MinecraftServer;
class Level;
class ServerLevel;
class MultiPlayerLevel;
class Player;
class LocalPlayer;
class MultiplayerLocalPlayer;
class ServerPlayer;
class RemotePlayer;
class PlayerList;
class Options;
class ServerScoreboard;
class ServerPlayerGameMode;
class GameMode;
class SurvivalMode;
class CreativeMode;
class MultiPlayerGameMode;
class EntityTracker;
class Entity;
class ItemInstance;
class GameType;
class Stat;
class CompoundTag;
class DamageSource;
class MobEffectInstance;
class Container;
class Inventory;

using namespace std;


// ============================================================================
// namespace SDK — convenience wrappers around the real singletons
// ============================================================================

/**
 * @brief Top-level mod API. Wraps singleton access and common operations.
 *
 * You can use these helpers, or call the real game classes directly after
 * obtaining them via SDK::GetServer() / SDK::GetClient().
 */
namespace SDK {

    // -----------------------------------------------------------------------
    // Singleton access
    // -----------------------------------------------------------------------

    /**
     * @brief Get the MinecraftServer singleton.
     *
     * Equivalent to MinecraftServer::getInstance().
     * Returns nullptr if the server is not running yet (check in OnInit or later).
     *
     * @par Example
     * @code
     *   MinecraftServer* srv = SDK::GetServer();
     *   if (srv) srv->broadcastStartSavingPacket();
     * @endcode
     */
    MODAPI MinecraftServer* GetServer();

    /**
     * @brief Get the Minecraft client singleton.
     *
     * Equivalent to Minecraft::GetInstance().
     *
     * @par Example
     * @code
     *   Minecraft* mc = SDK::GetClient();
     *   if (mc) mc->pauseGame();
     * @endcode
     */
    MODAPI Minecraft* GetClient();

    /**
     * @brief Get the server-side level for a given dimension.
     *
     * @param dimension 0 = Overworld, -1 = Nether, 1 = End.
     * @return ServerLevel pointer, or nullptr if that dimension isn't loaded.
     *
     * @par Example
     * @code
     *   ServerLevel* overworld = SDK::GetServerLevel(0);
     * @endcode
     */
    MODAPI ServerLevel* GetServerLevel(int dimension = 0);

    /**
     * @brief Get the client-side level for a given dimension.
     * @param dimension 0 = Overworld, -1 = Nether, 1 = End.
     */
    MODAPI MultiPlayerLevel* GetClientLevel(int dimension = 0);

    /**
     * @brief Get the PlayerList (all connected server players).
     *
     * Use this to iterate all online players, send messages, kick, etc.
     *
     * @par Example
     * @code
     *   PlayerList* list = SDK::GetPlayerList();
     *   if (list) {
     *       for (auto& sp : list->players)
     *           sp->sendMessage(L"Hello!", ChatPacket::e_ChatCustom);
     *   }
     * @endcode
     */
    MODAPI PlayerList* GetPlayerList();

    /**
     * @brief Get the primary local player on the client.
     *
     * Returns the first active local player (splitscreen index 0).
     * Returns nullptr if no level is loaded.
     */
    MODAPI MultiplayerLocalPlayer* GetLocalPlayer();

    /**
     * @brief Get a local player by splitscreen index (0–3).
     * @param index Splitscreen slot index.
     */
    MODAPI MultiplayerLocalPlayer* GetLocalPlayer(int index);

    // -----------------------------------------------------------------------
    // Logging
    // -----------------------------------------------------------------------

    /**
     * @brief Write a message to mods/modloader.log and OutputDebugString.
     * @param message Wide string message.
     *
     * @par Example
     * @code
     *   SDK::Log(L"MyMod: tick " + std::to_wstring(tickCount));
     * @endcode
     */
    MODAPI void Log(const wstring& message);

    /**
     * @brief Write a warning to the log (prefixed with [WARN]).
     * @param message Warning text.
     */
    MODAPI void LogWarn(const wstring& message);

    /**
     * @brief Write an error to the log (prefixed with [ERROR]).
     * @param message Error text.
     */
    MODAPI void LogError(const wstring& message);

    // -----------------------------------------------------------------------
    // Messaging
    // -----------------------------------------------------------------------

    /**
     * @brief Send a chat message to all connected players.
     *
     * Internally calls PlayerList::sendMessage for every connected player.
     *
     * @param message Wide string message to broadcast.
     *
     * @par Example
     * @code
     *   SDK::BroadcastMessage(L"Server restarting in 60 seconds!");
     * @endcode
     */
    MODAPI void BroadcastMessage(const wstring& message);

    /**
     * @brief Send a private chat message to a specific player by name.
     * @param playerName Target player's username.
     * @param message    Message to send.
     */
    MODAPI void SendMessageToPlayer(const wstring& playerName, const wstring& message);

    // -----------------------------------------------------------------------
    // Server control
    // -----------------------------------------------------------------------

    /**
     * @brief Execute a server console command.
     *
     * Equivalent to typing into the server console.
     *
     * @param command Command string, e.g. L"time set day".
     *
     * @par Example
     * @code
     *   SDK::ExecuteCommand(L"time set 0");
     *   SDK::ExecuteCommand(L"weather clear");
     * @endcode
     */
    MODAPI void ExecuteCommand(const wstring& command);

    /**
     * @brief Set the world time for a dimension.
     *
     * Wraps MinecraftServer::SetTimeOfDay().
     *
     * @param time      Ticks (0 = dawn, 6000 = noon, 12000 = dusk, 18000 = midnight).
     *
     * @par Example
     * @code
     *   SDK::SetTimeOfDay(6000);  // Set to noon
     * @endcode
     */
    MODAPI void SetTimeOfDay(__int64 time);

    /**
     * @brief Save all worlds immediately.
     * @param force If true, saves even if nothing is dirty.
     */
    MODAPI void SaveAll(bool force = false);

    // -----------------------------------------------------------------------
    // PvP / game rules
    // -----------------------------------------------------------------------

    /**
     * @brief Check whether PvP is currently allowed on the server.
     */
    MODAPI bool IsPvpAllowed();

    /**
     * @brief Enable or disable PvP on the server.
     * @param enabled True to allow PvP.
     */
    MODAPI void SetPvpAllowed(bool enabled);

    /**
     * @brief Check whether flight is allowed on the server.
     */
    MODAPI bool IsFlightAllowed();

    /**
     * @brief Enable or disable flight on the server.
     * @param enabled True to allow flight.
     */
    MODAPI void SetFlightAllowed(bool enabled);

} // namespace SDK


// ============================================================================
// Real class declarations with MODAPI — IDE sees these as normal functions
// ============================================================================
// These mirror the actual methods in the game. Add #include "SDK.h" to your
// mod project and you'll get autocomplete on all of these.
// ============================================================================


// ----------------------------------------------------------------------------
// Minecraft — client singleton
// ----------------------------------------------------------------------------

/**
 * @brief The main Minecraft client class. One instance per process.
 *
 * Obtain via SDK::GetClient() or Minecraft::GetInstance().
 */
class Minecraft {
public:
    /** @brief Returns the global Minecraft client instance. */
    MODAPI static Minecraft* GetInstance();

    /** @brief Pause the game (shows pause menu). */
    MODAPI void pauseGame();

    /** @brief Stop the client (exit to main menu). */
    MODAPI void stop();

    /** @brief True if this is a client-side (not dedicated server) instance. */
    MODAPI bool isClientSide();

    /**
     * @brief Get the local player at a splitscreen index.
     * @param idx Splitscreen slot (0–3).
     */
    MODAPI shared_ptr<MultiplayerLocalPlayer> getLocalPlayer(int idx = 0);

    /**
     * @brief Set the active screen (UI overlay).
     * Pass nullptr to close the current screen.
     * @param screen New screen to display.
     */
    MODAPI void setScreen(class Screen* screen);

    /** @brief Get the current game options. */
    MODAPI Options* getOptions();

    /** @brief Handle an in-game console/chat command from the client side. */
    MODAPI bool handleClientSideCommand(const wstring& chatMessage);

    /** @brief Connect to a remote server. */
    MODAPI void connectTo(const wstring& server, int port);

    /** @brief Width of the render viewport in pixels. */
    int width;
    /** @brief Height of the render viewport in pixels. */
    int height;
    /** @brief Whether the game is paused. */
    volatile bool pause;
    /** @brief Whether the main loop is running. */
    volatile bool running;
    /** @brief Current FPS string (for display). */
    wstring fpsString;

    /** @brief The active game mode. */
    MultiPlayerGameMode* gameMode;
    /** @brief The active client-side level (Overworld). */
    MultiPlayerLevel* level;
    /** @brief The client-side Options object. */
    Options* options;
};


// ----------------------------------------------------------------------------
// MinecraftServer — server singleton
// ----------------------------------------------------------------------------

/**
 * @brief The dedicated server / integrated server class.
 *
 * Obtain via SDK::GetServer() or MinecraftServer::getInstance().
 */
class MinecraftServer {
public:
    /** @brief Returns the global MinecraftServer instance. */
    MODAPI static MinecraftServer* getInstance();

    /**
     * @brief Get the ServerLevel for a dimension.
     * @param dimension 0 = Overworld, -1 = Nether, 1 = End.
     * @return ServerLevel pointer or nullptr.
     */
    MODAPI ServerLevel* getLevel(int dimension);

    /** @brief Get the PlayerList containing all connected ServerPlayers. */
    MODAPI PlayerList* getPlayers();

    /** @brief Get the server's CommandDispatcher. */
    MODAPI class CommandDispatcher* getCommandDispatcher();

    /** @brief Check if PvP is currently allowed. */
    MODAPI bool isPvpAllowed();
    /** @brief Enable or disable PvP. */
    MODAPI void setPvpAllowed(bool pvp);

    /** @brief Check if flight is allowed. */
    MODAPI bool isFlightAllowed();
    /** @brief Enable or disable flight. */
    MODAPI void setFlightAllowed(bool allowFlight);

    /** @brief Check if the Nether is enabled. */
    MODAPI bool isNetherEnabled();

    /** @brief Check if hardcore mode is active. */
    MODAPI bool isHardcore();

    /** @brief Check if command blocks are enabled. */
    MODAPI bool isCommandBlockEnabled();

    /** @brief Get the max build height (default 256). */
    MODAPI int getMaxBuildHeight();
    /** @brief Set the max build height. */
    MODAPI void setMaxBuildHeight(int height);

    /** @brief Get player idle timeout in minutes. */
    MODAPI int getPlayerIdleTimeout();
    /** @brief Set player idle timeout. */
    MODAPI void setPlayerIdleTimeout(int minutes);

    /** @brief Broadcast start-saving packet to all clients. */
    MODAPI void broadcastStartSavingPacket();
    /** @brief Broadcast stop-saving packet to all clients. */
    MODAPI void broadcastStopSavingPacket();

    /** @brief Log an info message to the server console. */
    MODAPI void info(const wstring& string);
    /** @brief Log a warning to the server console. */
    MODAPI void warn(const wstring& string);

    /** @brief Handle a console command string. */
    MODAPI void handleConsoleInput(const wstring& msg, class ConsoleInputSource* source);

    /**
     * @brief Set the world time at the end of the current tick.
     * @param time Ticks (0=dawn, 6000=noon, 12000=dusk, 18000=midnight).
     */
    MODAPI static void SetTimeOfDay(__int64 time);

    /**
     * @brief Set the absolute world time at the end of the current tick.
     * @param time Absolute tick count.
     */
    MODAPI static void SetTime(__int64 time);

    /** @brief Whether the server has been halted. */
    MODAPI static bool serverHalted();

    /** @brief Halt the server. */
    MODAPI static void HaltServer(bool bPrimaryPlayerSignedOut = false);

    /** @brief True if animals spawn. */
    bool animals;
    /** @brief True if NPCs (villagers etc.) spawn. */
    bool npcs;
    /** @brief True if PvP is allowed. */
    bool pvp;
    /** @brief True if flight is allowed. */
    bool allowFlight;
    /** @brief Server MOTD string. */
    wstring motd;
    /** @brief Current tick count. */
    int tickCount;
};


// ----------------------------------------------------------------------------
// Player — base class for all player types
// ----------------------------------------------------------------------------

/**
 * @brief Base class for all player types (Local, Server, Remote).
 *
 * Don't use this directly — use MultiplayerLocalPlayer (client) or
 * ServerPlayer (server) for concrete player references.
 */
class Player {
public:
    /**
     * @brief Get the player's current health.
     * @return Health points (0.0 = dead, 20.0 = full with no buffs).
     */
    MODAPI float getHealth() const;

    /**
     * @brief Set the player's health directly.
     *
     * Does not trigger death events if set to 0 — use hurt() for that.
     * Clamped to [0, getMaxHealth()] internally.
     *
     * @param health New health value.
     *
     * @par Example
     * @code
     *   player->setHealth(20.0f);  // Full heal
     *   player->setHealth(1.0f);   // Half a heart
     * @endcode
     */
    MODAPI void setHealth(float health);

    /**
     * @brief Get the player's maximum health.
     * @return Max health (default 20.0, affected by Absorption etc.).
     */
    MODAPI float getMaxHealth() const;

    /**
     * @brief Apply damage from a damage source.
     *
     * Respects armor, effects, and god-mode. Prefer this over setHealth()
     * when simulating real damage, as it triggers death/hurt animations.
     *
     * @param source Damage source (can be nullptr for generic damage).
     * @param amount Damage in half-hearts.
     * @return true if the damage was applied.
     */
    MODAPI bool hurt(DamageSource* source, float amount);

    /**
     * @brief Heal the player by a given amount.
     * @param amount Half-hearts to restore.
     */
    MODAPI void heal(float amount);

    /**
     * @brief Get the player's current food level.
     * @return Food level (0–20).
     */
    MODAPI int getFoodLevel() const;

    /**
     * @brief Get the player's experience level.
     * @return XP level.
     */
    MODAPI int getExperienceLevel() const;

    /**
     * @brief Give experience levels to the player.
     * @param amount Number of levels to add (can be negative).
     */
    MODAPI void giveExperienceLevels(int amount);

    /**
     * @brief Get the player's username.
     * @return Wide string username.
     */
    MODAPI wstring getName() const;

    /**
     * @brief Check if the player is currently sneaking.
     * @return true if sneaking.
     */
    MODAPI bool isSneaking() const;

    /**
     * @brief Check if the player is currently sprinting.
     * @return true if sprinting.
     */
    MODAPI bool isSprinting() const;

    /**
     * @brief Check if the player is flying.
     * @return true if flying.
     */
    MODAPI bool isFlying() const;

    /**
     * @brief Set the player's flying state.
     * @param flying True to enable flight.
     */
    MODAPI void setFlying(bool flying);

    /**
     * @brief Get the player's current X position.
     */
    MODAPI double getX() const;
    /**
     * @brief Get the player's current Y position (feet level).
     */
    MODAPI double getY() const;
    /**
     * @brief Get the player's current Z position.
     */
    MODAPI double getZ() const;

    /**
     * @brief Teleport the player to a world position.
     * @param x,y,z Target coordinates.
     */
    MODAPI void teleportTo(double x, double y, double z);

    /**
     * @brief Get the item currently held in the player's hand.
     * @return ItemInstance pointer, or nullptr if hand is empty.
     */
    MODAPI virtual shared_ptr<ItemInstance> getCarriedItem();

    /**
     * @brief Add an item to the player's inventory.
     *
     * @par Example
     * @code
     *   // Give the player a diamond sword (item ID 276)
     *   auto sword = std::make_shared<ItemInstance>(276, 1, 0);
     *   player->getInventory()->add(sword);
     * @endcode
     */
    MODAPI class Inventory* getInventory();

    /** @brief Trigger the player's respawn. */
    MODAPI virtual void respawn();

    /** @brief Kill the player immediately. Triggers death screen. */
    MODAPI void kill();

    /**
     * @brief Play a sound effect at the player's position.
     * @param soundId  Sound ID.
     * @param volume   Volume multiplier (1.0 = normal).
     * @param pitch    Pitch multiplier (1.0 = normal).
     */
    MODAPI virtual void playSound(int soundId, float volume, float pitch);

    /**
     * @brief Check whether this player has a given game permission.
     * @param command The permission/command to check.
     * @return true if allowed.
     */
    MODAPI bool hasPermission(class EGameCommand command);

    /** @brief X coordinate (double precision). */
    double x;
    /** @brief Y coordinate (feet position). */
    double y;
    /** @brief Z coordinate. */
    double z;
    /** @brief Yaw rotation in degrees. */
    float yRot;
    /** @brief Pitch rotation in degrees. */
    float xRot;
};


// ----------------------------------------------------------------------------
// LocalPlayer — client-side local player
// ----------------------------------------------------------------------------

/**
 * @brief Client-side representation of a local (human-controlled) player.
 *
 * Extends Player with client-specific input handling, portal effects,
 * splitscreen management, and view control.
 *
 * Obtain via SDK::GetLocalPlayer() or Minecraft::GetInstance()->localplayers[idx].
 */
class LocalPlayer : public Player {
public:
    /** @brief Current sprint time remaining (ticks). */
    int sprintTime;

    /** @brief Portal transition effect timer. */
    float portalTime;

    /**
     * @brief Respawn the player and trigger the respawn animation.
     */
    MODAPI virtual void respawn() override;

    /**
     * @brief Trigger the respawn animation on the client.
     */
    MODAPI virtual void animateRespawn();

    /**
     * @brief Send a chat message from this player.
     * @param message Message text.
     */
    MODAPI virtual void chat(const wstring& message);

    /**
     * @brief Get the active third-person view mode.
     * @return 0 = first person, 1 = third person, 2 = front-facing third person.
     */
    MODAPI virtual int ThirdPersonView();

    /**
     * @brief Set the third-person view mode.
     * @param val 0 = first person, 1 = third person, 2 = reversed.
     */
    MODAPI virtual void SetThirdPersonView(int val);

    /**
     * @brief Reset the idle timer (prevents idle animations).
     */
    MODAPI void ResetInactiveTicks();

    /**
     * @brief Get the number of ticks the player has been idle.
     * @return Idle tick count (capped at 255).
     */
    MODAPI unsigned int GetInactiveTicks();

    /**
     * @brief Set the player's sprint state.
     * @param value True to enable sprinting.
     */
    MODAPI void setSprinting(bool value);

    /**
     * @brief Update the player's XP display values.
     * @param experienceProgress  Progress to next level (0.0–1.0).
     * @param totalExp            Total XP points.
     * @param experienceLevel     Current XP level.
     */
    MODAPI void setExperienceValues(float experienceProgress, int totalExp, int experienceLevel);

    /**
     * @brief Trigger a critical hit visual on an entity.
     * @param entity The entity that was critically hit.
     */
    MODAPI virtual void crit(shared_ptr<Entity> entity);

    /**
     * @brief Trigger a magic critical hit visual on an entity.
     * @param entity The entity that was magic-crit'd.
     */
    MODAPI virtual void magicCrit(shared_ptr<Entity> entity);

    /**
     * @brief Get the Xbox pad index assigned to this player.
     * @return Controller index (0–3).
     */
    MODAPI int GetXboxPad();

    /**
     * @brief Set the Xbox pad index for this player.
     * @param iPad Controller index.
     */
    MODAPI void SetXboxPad(int iPad);

    /**
     * @brief Change the player's dimension (triggers dimension change logic).
     * @param i Target dimension (0=Overworld, -1=Nether, 1=End).
     */
    MODAPI virtual void changeDimension(int i);
};


// ----------------------------------------------------------------------------
// ServerPlayer — server-side player
// ----------------------------------------------------------------------------

/**
 * @brief Server-side representation of a connected player.
 *
 * Contains the player's server game mode, connection, and chunk state.
 * Access via SDK::GetPlayerList()->players or PlayerList::getPlayer().
 */
class ServerPlayer : public Player {
public:
    /** @brief Network latency in milliseconds. */
    int latency;

    /** @brief True if the player has won the game (defeated the Ender Dragon). */
    bool wonGame;

    /** @brief This player's server-side game mode instance. */
    ServerPlayerGameMode* gameMode;

    /**
     * @brief Send a chat message directly to this player's screen.
     *
     * @param message         Message text.
     * @param type            Message type (e_ChatCustom for mod messages).
     * @param customData      Optional custom data (use -1 for none).
     * @param additionalMsg   Optional additional message text.
     *
     * @par Example
     * @code
     *   serverPlayer->sendMessage(L"You have been warned!", ChatPacket::e_ChatCustom);
     * @endcode
     */
    MODAPI void sendMessage(const wstring& message,
                            int type = 0 /* ChatPacket::e_ChatCustom */,
                            int customData = -1,
                            const wstring& additionalMessage = L"");

    /**
     * @brief Kick this player from the server.
     * Calls disconnect() and closes the connection.
     */
    MODAPI void disconnect();

    /**
     * @brief Teleport this player to a world position.
     * @param x,y,z Target coordinates.

    /**
     * @brief Check if this player can harm another player (PvP check).
     * @param target Target player.
     * @return true if PvP is allowed and the player can attack.
     */
    MODAPI bool canHarmPlayer(shared_ptr<Player> target);

    /**
     * @brief Set the game mode for this player.
     * @param mode GameType to apply (Survival, Creative, Adventure, Spectator).
     */
    MODAPI void setGameMode(GameType* mode);

    /**
     * @brief Get the ServerLevel this player is currently in.
     * @return Pointer to the ServerLevel.
     */
    MODAPI ServerLevel* getLevel();

    /**
     * @brief Check if this player has a server-side permission.
     * @param command Permission to check.
     * @return true if allowed.
     */
    MODAPI bool hasPermission(class EGameCommand command);

    /**
     * @brief Reset this player's last-action timer (anti-idle).
     */
    MODAPI void resetLastActionTime();

    /**
     * @brief Check if this player is a server operator.
     * Only valid if called via PlayerList. Use PlayerList::isOp() instead.
     */
    // Use PlayerList::isOp(player) — not a direct method on ServerPlayer
};


// ----------------------------------------------------------------------------
// PlayerList — all connected players
// ----------------------------------------------------------------------------

/**
 * @brief Manages all connected ServerPlayers on the server.
 *
 * Obtain via SDK::GetPlayerList() or MinecraftServer::getInstance()->getPlayers().
 */
class PlayerList {
public:
    /** @brief All currently connected players. Iterate this for all players. */
    vector<shared_ptr<ServerPlayer>> players;

    /**
     * @brief Find a player by username.
     * @param name Player's username (wide string).
     * @return Shared pointer to ServerPlayer, or nullptr if not found.
     *
     * @par Example
     * @code
     *   auto player = SDK::GetPlayerList()->getPlayer(L"Steve");
     *   if (player) player->sendMessage(L"Hello Steve!");
     * @endcode
     */
    MODAPI shared_ptr<ServerPlayer> getPlayer(const wstring& name);

    /**
     * @brief Get the number of currently connected players.
     * @return Connected player count.
     */
    MODAPI int getPlayerCount();

    /**
     * @brief Get the server's configured max players.
     * @return Max player count.
     */
    MODAPI int getMaxPlayers();

    /**
     * @brief Find the nearest player to a world position.
     * @param position  World position (Pos*).
     * @param range     Search radius in blocks.
     * @return Nearest ServerPlayer in range, or nullptr.
     */
    MODAPI shared_ptr<ServerPlayer> getNearestPlayer(class Pos* position, int range);

    /**
     * @brief Get a comma-separated string of all online player names.
     * @return Wide string of player names.
     */
    MODAPI wstring getPlayerNames();

    /**
     * @brief Send a chat message to a specific player.
     * @param name    Target player's username.
     * @param message Message text.
     */
    MODAPI void sendMessage(const wstring& name, const wstring& message);

    /**
     * @brief Broadcast a packet to all players in a dimension.
     * @param packet    Packet to broadcast.
     * @param dimension Target dimension (0=Overworld, -1=Nether, 1=End).
     */
    MODAPI void broadcastAll(shared_ptr<class Packet> packet, int dimension = 0);

    /**
     * @brief Save all player data to disk.
     * @param progressListener Optional progress listener (can be nullptr).
     */
    MODAPI void saveAll(class ProgressListener* progressListener, bool bDeleteGuestMaps = false);

    /**
     * @brief Check if a player is a server operator.
     * @param player Target player.
     * @return true if the player is op'd.
     */
    MODAPI bool isOp(shared_ptr<ServerPlayer> player);

    /**
     * @brief Kick a player by their small network ID.
     * @param networkSmallId The player's small network ID byte.
     */
    MODAPI void kickPlayerByShortId(unsigned char networkSmallId);

    /**
     * @brief Set the view distance for all players.
     * @param newViewDistance New view distance in chunks.
     */
    MODAPI void setViewDistance(int newViewDistance);

    /**
     * @brief Respawn a server player (after death or dimension change).
     * @param serverPlayer       Player to respawn.
     * @param targetDimension    Target dimension.
     * @param keepAllPlayerData  True to keep inventory/XP.
     * @return New ServerPlayer instance.
     */
    MODAPI shared_ptr<ServerPlayer> respawn(shared_ptr<ServerPlayer> serverPlayer,
                                            int targetDimension, bool keepAllPlayerData);
};


// ----------------------------------------------------------------------------
// ServerLevel — server-side world
// ----------------------------------------------------------------------------

/**
 * @brief The server-side representation of a loaded dimension/world.
 *
 * Obtain via SDK::GetServerLevel(dimension) or MinecraftServer::getLevel(dimension).
 */
class ServerLevel {
public:
    /**
     * @brief Spawn an explosion in the world.
     *
     * @param source        Entity that caused the explosion, or nullptr.
     * @param x,y,z         Centre of the explosion.
     * @param radius        Explosion radius in blocks.
     * @param fire          True to start fires.
     * @param destroyBlocks True to destroy blocks.
     * @return Explosion object.
     *
     * @par Example
     * @code
     *   level->explode(nullptr, 0, 64, 0, 4.0f, false, true);
     * @endcode
     */
    MODAPI shared_ptr<class Explosion> explode(shared_ptr<Entity> source,
                                               double x, double y, double z,
                                               float radius, bool fire, bool destroyBlocks);

    /**
     * @brief Get an entity by its integer ID.
     * @param id Entity ID.
     * @return Entity shared_ptr or nullptr.
     */
    MODAPI shared_ptr<Entity> getEntity(int id);

    /**
     * @brief Add an entity to this level.
     * @param entity Entity to add.
     * @return true if successfully added.
     */
    MODAPI virtual bool addEntity(shared_ptr<Entity> entity);

    /**
     * @brief Force a tile tick at a position on the next tick.
     * @param x,y,z    Block coordinates.
     * @param tileId   ID of the tile to tick.
     * @param tickDelay Delay in ticks.
     */
    MODAPI void addToTickNextTick(int x, int y, int z, int tileId, int tickDelay);

    /**
     * @brief Broadcast a tile update to all nearby players.
     * @param x,y,z Block coordinates to update.
     */
    MODAPI void queueSendTileUpdate(int x, int y, int z);

    /**
     * @brief Get all tile entities in a rectangular region.
     * @param x0,y0,z0  Min corner.
     * @param x1,y1,z1  Max corner.
     * @return Vector of tile entities in the region.
     */
    MODAPI vector<shared_ptr<class TileEntity>>* getTileEntitiesInRegion(
        int x0, int y0, int z0, int x1, int y1, int z1);

    /**
     * @brief Save the level to disk.
     * @param force       Force save even if nothing is dirty.
     * @param listener    Progress listener, or nullptr.
     * @param bAutosave   True if this is an auto-save.
     */
    MODAPI void save(bool force, class ProgressListener* listener, bool bAutosave = false);

    /**
     * @brief Send a named particle effect to all players near a position.
     * @param name        Particle name (e.g. L"explode", L"flame", L"smoke").
     * @param x,y,z       Origin position.
     * @param count       Number of particles.
     */
    MODAPI void sendParticles(const wstring& name, double x, double y, double z, int count);

    /**
     * @brief Send a named particle effect with velocity spread.
     * @param name          Particle name.
     * @param x,y,z         Origin position.
     * @param count         Number of particles.
     * @param xDist,yDist,zDist  Spread distance per axis.
     * @param speed         Particle speed.
     */
    MODAPI void sendParticles(const wstring& name, double x, double y, double z,
                              int count, double xDist, double yDist, double zDist, double speed);

    /**
     * @brief Check if a block position is under spawn protection.
     * @param level  Level the position is in.
     * @param x,y,z  Block coordinates.
     * @param player Player trying to modify the block.
     * @return true if protected.
     */
    MODAPI bool mayInteract(shared_ptr<Player> player, int x, int y, int z, int content);

    /** @brief Get the server that owns this level. */
    MODAPI MinecraftServer* getServer();

    /** @brief Get the EntityTracker for this level. */
    MODAPI EntityTracker* getTracker();

    /** @brief Set the world time (adjusting scheduled ticks). */
    MODAPI void setTimeAndAdjustTileTicks(__int64 newTime);

    /** @brief True if the level's save-on-exit flag is set to not save. */
    bool noSave;

    /** @brief True if admins can edit the spawn area. */
    bool canEditSpawn;

    // Entity count limits (read-only, enforced by the engine)
    static const int MAX_ITEM_ENTITIES = 200;
    static const int MAX_ARROW_ENTITIES = 200;
    static const int MAX_PRIMED_TNT = 20;
};


// ----------------------------------------------------------------------------
// ServerPlayerGameMode — controls a ServerPlayer's game mode
// ----------------------------------------------------------------------------

/**
 * @brief Manages a ServerPlayer's game mode state and block breaking.
 *
 * Access via ServerPlayer::gameMode.
 */
class ServerPlayerGameMode {
public:
    /** @brief The level this game mode is operating in. */
    Level* level;
    /** @brief The player this game mode belongs to. */
    shared_ptr<ServerPlayer> player;

    /**
     * @brief Check if the player is in Survival mode.
     * @return true if Survival.
     */
    MODAPI bool isSurvival();

    /**
     * @brief Check if the player is in Creative mode.
     * @return true if Creative.
     */
    MODAPI bool isCreative();

    /**
     * @brief Change the player's game mode.
     * @param gameType New GameType.
     */
    MODAPI void setGameModeForPlayer(GameType* gameType);

    /**
     * @brief Get the current GameType for this player.
     * @return Pointer to the current GameType.
     */
    MODAPI GameType* getGameModeForPlayer();

    /**
     * @brief Force-destroy a block at a position (ignores survival rules).
     * @param x,y,z Block coordinates.
     * @return true if the block was destroyed.
     */
    MODAPI bool destroyBlock(int x, int y, int z);

    /** @brief Get the game rules instance. */
    MODAPI class GameRulesInstance* getGameRules();
};


// ----------------------------------------------------------------------------
// Options — client game options
// ----------------------------------------------------------------------------

/**
 * @brief Client-side game options (graphics, audio, controls, etc.)
 *
 * Obtain via SDK::GetClient()->options or Minecraft::GetInstance()->options.
 */
class Options {
public:
    float music;          ///< Music volume (0.0–1.0)
    float sound;          ///< Sound effects volume (0.0–1.0)
    float sensitivity;    ///< Mouse/stick sensitivity
    bool  invertYMouse;   ///< True if Y-axis is inverted
    int   viewDistance;   ///< Render distance (chunks)
    bool  bobView;        ///< Camera bobbing enabled
    bool  fancyGraphics;  ///< Fancy (vs fast) graphics
    bool  ambientOcclusion; ///< Ambient occlusion enabled
    bool  renderClouds;   ///< Clouds visible
    bool  hideGui;        ///< HUD hidden
    bool  thirdPersonView;///< Third-person view active
    int   difficulty;     ///< 0=Peaceful, 1=Easy, 2=Normal, 3=Hard
    float fov;            ///< Field of view (degrees)
    float gamma;          ///< Brightness
    int   particles;      ///< 0=All, 1=Decreased, 2=Minimal

    /** @brief Save options to disk. */
    MODAPI void save();
    /** @brief Load options from disk. */
    MODAPI void load();
};


// ----------------------------------------------------------------------------
// ServerScoreboard — server-side scoreboard
// ----------------------------------------------------------------------------

/**
 * @brief Server-side scoreboard management.
 *
 * Obtain via ServerLevel::getServer() then cast the Scoreboard.
 */
class ServerScoreboard {
public:
    /**
     * @brief Called when a player's score changes. Broadcasts to clients.
     * @param score The modified Score object.
     */
    MODAPI void onScoreChanged(class Score* score);

    /**
     * @brief Remove all scores for a player (e.g. on disconnect).
     * @param player Player username.
     */
    MODAPI void onPlayerRemoved(const wstring& player);

    /**
     * @brief Set which Objective is displayed in a scoreboard slot.
     * @param slot      Display slot (0=list, 1=sidebar, 2=belowName).
     * @param objective Objective to display (nullptr to clear).
     */
    MODAPI void setDisplayObjective(int slot, class Objective* objective);

    /**
     * @brief Add a player to a team.
     * @param player Player username.
     * @param team   Target team.
     */
    MODAPI void addPlayerToTeam(const wstring& player, class PlayerTeam* team);

    /**
     * @brief Remove a player from their team.
     * @param player Player username.
     * @param team   Team to remove from.
     */
    MODAPI void removePlayerFromTeam(const wstring& player, class PlayerTeam* team);
};

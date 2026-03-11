#pragma once
#include "RegistryTypes.h"
#include "IdRegistry.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <mutex>

// ============================================================================
// Registry
//
// The single public API mods use to register custom content.
// Call these from IMod::OnInit() (or a dedicated OnRegister() if you add one).
//
// Usage example:
//
//   static RegisteredBlock* MyOre;
//
//   void OnInit() override {
//       MyOre = Registry::Block::Register(
//           Identifier(L"mymod:my_ore"),
//           BlockProperties()
//               .Material(MaterialType::Stone)
//               .Hardness(3.0f)
//               .Resistance(15.0f)
//               .Sound(SoundType::Stone)
//               .Icon(L"mymod:my_ore")
//               .Name(L"My Ore")
//               .RequiredHarvestLevel(2)
//               .RequiredTool(ToolType::Pickaxe)
//               .InCreativeTab(CreativeTab::BuildingBlocks));
//
//       Registry::Item::Register(
//           Identifier(L"mymod:my_ingot"),
//           ItemProperties()
//               .MaxStackSize(64)
//               .Icon(L"mymod:my_ingot")
//               .Name(L"My Ingot")
//               .InCreativeTab(CreativeTab::Materials));
//
//       Registry::Recipe::AddFurnace(
//           Identifier(L"mymod:my_ore"),
//           Identifier(L"mymod:my_ingot"),
//           1.0f);
//   }
// ============================================================================

// ============================================================================
// Internal storage for tool materials
// ============================================================================

class ToolMaterialStore
{
public:
    static ToolMaterialStore& Instance();

    void Register(const Identifier& id, const ToolMaterialDefinition& def);
    bool TryGet(const Identifier& id, ToolMaterialDefinition& outDef) const;

private:
    struct IdHash {
        size_t operator()(const Identifier& id) const {
            std::hash<std::wstring> h;
            return h(id.ToString());
        }
    };
    std::unordered_map<Identifier, ToolMaterialDefinition, IdHash> m_materials;
    mutable std::mutex m_mutex;
};

// ============================================================================
// Pending creative inventory item (processed after Minecraft::init)
// ============================================================================

struct PendingCreativeItem
{
    int itemId;
    int count;
    int auxValue;
    int groupIndex;
};

// ============================================================================
// Registry — top-level namespace mods call
// ============================================================================

namespace Registry
{
    // ----------------------------------------------------------------
    // Block
    // ----------------------------------------------------------------
    namespace Block
    {
        // Register a static block (no custom callbacks).
        RegisteredBlock Register(const Identifier& id, const BlockProperties& props);

        // Register a slab pair (half + double).
        // Returns RegisteredSlabBlock with both numeric IDs populated.
        RegisteredSlabBlock RegisterSlab(const Identifier& id, const BlockProperties& props);

        // Register a falling block (like gravel/sand).
        RegisteredBlock RegisterFalling(const Identifier& id, const BlockProperties& props);

        // Lookup a previously registered block's numeric ID.
        int GetNumericId(const Identifier& id);
    }

    // ----------------------------------------------------------------
    // Item
    // ----------------------------------------------------------------
    namespace Item
    {
        // Register a plain item.
        RegisteredItem Register(const Identifier& id, const ItemProperties& props);

        // Register tool items.
        RegisteredItem RegisterPickaxe(const Identifier& id, const ItemProperties& props, ToolTier tier = ToolTier::Diamond);
        RegisteredItem RegisterShovel (const Identifier& id, const ItemProperties& props, ToolTier tier = ToolTier::Diamond);
        RegisteredItem RegisterAxe    (const Identifier& id, const ItemProperties& props, ToolTier tier = ToolTier::Diamond);
        RegisteredItem RegisterHoe    (const Identifier& id, const ItemProperties& props, ToolTier tier = ToolTier::Diamond);
        RegisteredItem RegisterSword  (const Identifier& id, const ItemProperties& props, ToolTier tier = ToolTier::Diamond);

        // Register a named tool material (for custom harvest level / destroy speed).
        void RegisterToolMaterial(const Identifier& id, const ToolMaterialDefinition& def);

        // Lookup a previously registered item's numeric ID.
        int GetNumericId(const Identifier& id);
    }

    // ----------------------------------------------------------------
    // Entity
    // ----------------------------------------------------------------
    namespace Entity
    {
        RegisteredEntity Register(const Identifier& id, const EntityDefinition& def);

        // Lookup a previously registered entity's numeric ID.
        int GetNumericId(const Identifier& id);
    }

    // ----------------------------------------------------------------
    // Recipe
    // ----------------------------------------------------------------
    namespace Recipe
    {
        // Add a furnace / smelting recipe.
        void AddFurnace(const Identifier& input, const Identifier& output, float xp);

        // Add a shaped crafting recipe.
        // pattern: up to 3 rows, e.g. { L"XXX", L" | ", L" | " }
        // keys: pairs of (character, ingredient identifier)
        struct ShapedKey { wchar_t key; Identifier ingredient; };
        void AddShaped(const Identifier& result, int resultCount,
                       const std::vector<std::wstring>& pattern,
                       const std::vector<ShapedKey>& keys);
    }

    // ----------------------------------------------------------------
    // Internal: called by ModLoader to flush pending registrations
    //           into the live game after Minecraft::init completes.
    // ----------------------------------------------------------------
    namespace Internal
    {
        // Queue a creative item for injection (called from Block/Item Register).
        void QueueCreativeItem(int numericId, int count, int auxValue, int groupIndex);

        // Access the pending creative queue (ModLoader calls this after init).
        const std::vector<PendingCreativeItem>& GetPendingCreativeItems();

        // Clear the queue after injection.
        void ClearPendingCreativeItems();
    }
}

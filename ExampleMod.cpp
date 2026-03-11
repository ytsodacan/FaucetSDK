#include "pch.h"
#include "IMod.h"
#include "SDK.h"
#include "Registry.h"
#include "RegistryTypes.h"

class ExampleMod final : public IMod
{
public:
    const ModInfo* GetInfo() const override
    {
        static const ModInfo info{
            "com.SillyProotSoda.examplemod",
            "Example Mod",
            "SillyProotSoda",
            "Testing modloading :p",
            { 1, 0, 0 }
        };
        return &info;
    }

    bool OnLoad() override
    {
        SDK::Log(L"ExampleMod: OnLoad");
        return true;
    }

    bool OnInit() override
    {
        SDK::Log(L"ExampleMod: OnInit");
        return true;
    }

    void OnRegister() override
    {
        Registry::Item::Register(
            Identifier(L"sillyprootsoda:exampleitem"),
            ItemProperties()
            .MaxStackSize(64)
            .Icon(L"sillyprootsoda:exampleitem")
            .Name(L"Example Item")
            .InCreativeTab(CreativeTab::Materials));

        SDK::Log(L"ExampleMod: registered items");
    }

    bool OnUpdate(float deltaTime) override
    {
        return true;
    }

    void OnShutdown() override
    {
        SDK::Log(L"ExampleMod: shutting down");
    }
    void OnLevelLoad()   override {}
    void OnLevelUnload() override {}
};

extern "C" __declspec(dllexport) IMod* CreateMod()
{
    return new ExampleMod();
}

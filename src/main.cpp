
#define DLLEXPORT __declspec(dllexport)


#ifdef SKYRIM_SUPPORT_AE
extern "C" DLLEXPORT constinit auto SKSEPlugin_Version = []()
	{
		SKSE::PluginVersionData v;
		v.PluginName(Plugin::NAME);
		v.AuthorName("SkyHorizon");
		v.PluginVersion(Plugin::VERSION);
		v.UsesAddressLibrary();
		v.UsesUpdatedStructs();
		v.CompatibleVersions({ SKSE::RUNTIME_SSE_LATEST });
		return v;
	}
();
#else
extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Query(const SKSE::QueryInterface* skse, SKSE::PluginInfo* info)
{
	info->name = Plugin::NAME;
	info->infoVersion = SKSE::PluginInfo::kVersion;
	info->version = Plugin::VERSION.pack();

	if (skse->IsEditor())
	{
		SKSE::log::critical("Loaded in editor, marking as incompatible"sv);
		return false;
	}

	const auto ver = skse->RuntimeVersion();
	if (ver < SKSE::RUNTIME_SSE_1_5_39)
	{
		const auto verStr = ver.string();
		SKSE::log::critical("Unsupported runtime version {}", verStr);
		return false;
	}

	return true;
}
#endif

SKSEPluginLoad(const SKSE::LoadInterface* skse)
{
	SKSE::Init(skse, true);

	spdlog::set_pattern("[%H:%M:%S:%e] [%l] %v"s);

#ifndef NDEBUG
	spdlog::set_level(spdlog::level::trace);
	spdlog::flush_on(spdlog::level::trace);
#else
	spdlog::set_level(spdlog::level::info);
	spdlog::flush_on(spdlog::level::info);
#endif

	SKSE::log::info("Game version: {}", skse->RuntimeVersion());

	return true;
}


void OnInit(SKSE::MessagingInterface::Message* a_msg)
{
	switch (a_msg->type)
	{
	case SKSE::MessagingInterface::kDataLoaded:
		break;
	case SKSE::MessagingInterface::kPostLoad:
		break;
	case SKSE::MessagingInterface::kInputLoaded:
		break;
	default:
		break;
	}
}

void InitializeLog()
{
	auto path = SKSE::log::log_directory();
	if (!path) {
		stl::report_and_fail("Failed to find standard logging directory"sv);
	}

	*path /= std::format("{}.log", Plugin::NAME);
	auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);

	auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));

#ifndef NDEBUG
	log->set_level(spdlog::level::trace);
	log->flush_on(spdlog::level::trace);
#else
	log->set_level(spdlog::level::info);
	log->flush_on(spdlog::level::info);
#endif

	spdlog::set_default_logger(std::move(log));
	spdlog::set_pattern("[%H:%M:%S:%e] [%l] %v"s);

	SKSE::log::info("{} v{}", Plugin::NAME, Plugin::VERSION);
}

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
	InitializeLog();

	SKSE::Init(skse, false);

	SKSE::log::info("Game version: {}", skse->RuntimeVersion());

	auto messaging = SKSE::GetMessagingInterface();
	messaging->RegisterListener("SKSE", OnInit);

	return true;
}

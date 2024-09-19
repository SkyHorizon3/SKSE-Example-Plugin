
void SetupLog()
{
	auto logsFolder = SKSE::log::log_directory();
	if (!logsFolder)
	{
		SKSE::stl::report_and_fail("SKSE log_directory not provided, logs disabled.");
	}

	auto pluginName = SKSE::PluginDeclaration::GetSingleton()->GetName();
	auto logFilePath = *logsFolder / std::format("{}.log", pluginName);
	auto fileLoggerPtr = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logFilePath.string(), true);
	auto loggerPtr = std::make_shared<spdlog::logger>("log", std::move(fileLoggerPtr));

#ifndef NDEBUG
	loggerPtr->set_level(spdlog::level::trace);
	loggerPtr->flush_on(spdlog::level::trace);
#else
	loggerPtr->set_level(spdlog::level::info);
	loggerPtr->flush_on(spdlog::level::info);
#endif

	spdlog::set_default_logger(std::move(loggerPtr));
}

void MessageListener(SKSE::MessagingInterface::Message* message)
{
	switch (message->type)
	{
		// https://github.com/ianpatt/skse64/blob/09f520a2433747f33ae7d7c15b1164ca198932c3/skse64/PluginAPI.h#L193-L212
	case SKSE::MessagingInterface::kDataLoaded:
	{


	}
	break;


	default:
		break;

	}
}

SKSEPluginLoad(const SKSE::LoadInterface* skse)
{
	SKSE::Init(skse);

	SetupLog();

	SKSE::GetMessagingInterface()->RegisterListener(MessageListener);
	SKSE::log::info("{} v{} loaded", Plugin::NAME, Plugin::VERSION);

	return true;
}

#define DLLEXPORT __declspec(dllexport)

extern "C" DLLEXPORT const auto SKSEPlugin_Version = []() noexcept {
	SKSE::PluginVersionData v;
	v.PluginName(Plugin::NAME.data());
	v.PluginVersion(Plugin::VERSION);
	v.UsesAddressLibrary(true);
	v.HasNoStructUse();
	return v;
	}
();

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Query(const SKSE::QueryInterface*, SKSE::PluginInfo * pluginInfo)
{
	pluginInfo->name = SKSEPlugin_Version.pluginName;
	pluginInfo->infoVersion = SKSE::PluginInfo::kVersion;
	pluginInfo->version = SKSEPlugin_Version.pluginVersion;
	return true;
}
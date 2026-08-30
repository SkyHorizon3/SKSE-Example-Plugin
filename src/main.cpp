
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

#ifdef SKYRIM_SUPPORT_AE
SKSE_PLUGIN_VERSION = []() {
	SKSE::PluginVersionData v;
	v.PluginVersion(Plugin::VERSION);
	v.PluginName(Plugin::NAME);
	v.AuthorName("SkyHorizon");
	v.UsesAddressLibrary();
	v.UsesUpdatedStructs();
	v.CompatibleVersions({ SKSE::RUNTIME_SSE_LATEST });

	if constexpr (SKSE::RUNTIME_SSE_LATEST < SKSE::RUNTIME_SSE_1_7_99)
	{
		v.MinimumRequiredXSEVersion(REL::Version{ 2, 2, 5 });
	}
	else
	{
		// address library v5 support
		v.MinimumRequiredXSEVersion(REL::Version{ 2, 3, 0 });
	}

	return v;
	}();
#else
SKSE_PLUGIN_QUERY(const SKSE::QueryInterface* skse, SKSE::PluginInfo* info)
{
	info->name = Plugin::NAME;
	info->infoVersion = SKSE::PluginInfo::kVersion;
	info->version = Plugin::VERSION.pack();

	if (skse->IsEditor())
	{
		REX::CRITICAL("Loaded in editor, marking as incompatible");
		return false;
	}

	const auto ver = skse->RuntimeVersion();
	if (ver < SKSE::RUNTIME_SSE_1_5_39)
	{
		const auto verStr = ver.string();
		REX::CRITICAL("Unsupported runtime version {}", verStr);
		return false;
	}

	return true;
}
#endif

SKSE_PLUGIN_LOAD(const SKSE::LoadInterface* skse)
{
	SKSE::Init(skse, SKSE::InitInfo{
					   .log = true,
					   .logName = Plugin::NAME,
					   //.trampoline = true,
					   // .trampolineSize = 100,
		});


	const auto runtimeVer = skse->RuntimeVersion();
	REX::INFO("Game version: {}", runtimeVer);


#ifdef SKYRIM_SUPPORT_AE
	if constexpr (SKSE::RUNTIME_SSE_LATEST < SKSE::RUNTIME_SSE_1_7_99)
	{
		if (runtimeVer >= SKSE::RUNTIME_SSE_1_7_99)
		{
			REX::FAIL(
				"You are using a newer version of Skyrim than this version of {0} supports.\n"
				"Install the correct version of {0} for your game version.\n"
				"Runtime: {1}\n"
				"Supported: 1.6.1170 (Steam) / 1.6.1179 (GOG)",
				Plugin::NAME, runtimeVer);
		}
	}
#endif

	auto messaging = SKSE::GetMessagingInterface();
	messaging->RegisterListener(OnInit);

	REX::DEBUG("Hello!");

	return true;
}

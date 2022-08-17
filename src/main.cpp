namespace
{
	void InitializeLog()
	{
#ifndef NDEBUG
		auto sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
#else
		auto path = logger::log_directory();
		if (!path) {
			util::report_and_fail("Failed to find standard logging directory"sv);
		}

		*path /= fmt::format("{}.log"sv, Plugin::NAME);
		auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
#endif

		#ifndef NDEBUG
				const auto level = spdlog::level::trace;
		#else
				const auto level = spdlog::level::info;
		#endif

		auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));
		log->set_level(level);
		log->flush_on(level);

		spdlog::set_default_logger(std::move(log));
		spdlog::set_pattern("%g(%#): [%^%l%$] %v"s);
	}

	void MakeItemsWeightless()
	{
		RE::BSTArray<RE::AlchemyItem*> poision_arr;

		auto playerref = RE::PlayerCharacter::GetSingleton();

		if (!playerref) {
			logger::error("Playerref not found");
			return;
		}

		auto inventory = playerref->GetInventory();

		logger::info("Making {} items weightless"sv, inventory.size());
		for (auto& item : inventory) {
			auto thisitem = item.first;
			const auto formType = thisitem->formType.get();

			switch (formType) {
			case RE::FormType::AlchemyItem:
			case RE::FormType::Book:
			case RE::FormType::Ingredient:
			case RE::FormType::Scroll:
			case RE::FormType::SoulGem:
				{
					auto weightedItem = thisitem->As<RE::TESWeightForm>();
					if (weightedItem && weightedItem->weight > 0) {
						//#ifndef NDEBUG
						const auto name = thisitem->As<RE::TESFullName>()->GetFullName();
						const auto formString = static_cast<std::underlying_type<RE::FormType>::type>(formType);
						// TODO: Why does this log never show up?
						logger::info("{}: {} has weight: {}"sv, formString, name, weightedItem->weight);
						//#endif
						weightedItem->weight = 0;
					}
					break;
				}
			}
		}
	}
}

class MenuOpenCloseEventHandler : public RE::BSTEventSink<RE::MenuOpenCloseEvent>
{
public:
	virtual RE::BSEventNotifyControl ProcessEvent(const RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>* /* a_dispatcher*/) override
	{
		if (a_event && a_event->opening) {
			logger::trace("Opening Event: {}"sv, a_event->menuName);
			if (a_event->menuName == "InventoryMenu" || a_event->menuName == "Cursor Menu") {
				logger::trace("Attemping to set weight");
				MakeItemsWeightless();
			}
		}

		return RE::BSEventNotifyControl::kContinue;
	}
	void Register()
	{
		auto ui = RE::UI::GetSingleton();
		ui->GetEventSource<RE::MenuOpenCloseEvent>()->AddEventSink(GetSingleton());
	}
	MenuOpenCloseEventHandler* GetSingleton()
	{
		static MenuOpenCloseEventHandler singleton;
		return std::addressof(singleton);
	}
};
MenuOpenCloseEventHandler g_menuOpenCloseEventHandler;

void MessageHandler(SKSE::MessagingInterface::Message* a_message)
{
	switch (a_message->type) {
	case SKSE::MessagingInterface::kDataLoaded:
		g_menuOpenCloseEventHandler.Register();
		break;
	}
}

extern "C" DLLEXPORT constinit auto SKSEPlugin_Version = []() {
	SKSE::PluginVersionData v;

	v.PluginVersion(Plugin::VERSION);
	v.PluginName(Plugin::NAME);

	v.UsesAddressLibrary(true);
	v.CompatibleVersions({ SKSE::RUNTIME_LATEST });

	return v;
}();

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{
	InitializeLog();
	logger::info("{} v{}"sv, Plugin::NAME, Plugin::VERSION.string());

	SKSE::Init(a_skse);

	const auto messaging = SKSE::GetMessagingInterface();
	messaging->RegisterListener(MessageHandler);

	return true;
}

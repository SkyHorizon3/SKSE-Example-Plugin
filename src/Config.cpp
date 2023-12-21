#include "../include/Globals.h"
#include "../include/Config.h"

using json = nlohmann::json;

std::vector<std::string> Config::GetLoadOrder()
{
	// Get user directory path
	wchar_t userDir[MAX_PATH];
	if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_PROFILE, NULL, 0, userDir)))
	{
		// Append the rest of the path
		std::wstring loadOrderPath = userDir;
		loadOrderPath += L"\\AppData\\Local\\Skyrim Special Edition\\loadorder.txt";

		std::vector<std::string> loadOrder;

		std::ifstream file(loadOrderPath);
		if (file.is_open())
		{
			std::string line;
			while (std::getline(file, line))
			{
				loadOrder.push_back(line);
			}
			file.close();
		}

		return loadOrder;
	}

	return std::vector<std::string>();
}

void Config::EnumerateFolder() //Get all folders in DynamicStringReplacer directory
{
	const std::filesystem::path Directory = L"Data\\SKSE\\DynamicStringReplacer\\";

	for (const auto& entry : std::filesystem::recursive_directory_iterator(Directory))
	{
		if (entry.is_directory())
		{
			m_Folders.push_back(entry.path().filename().string());
		}
	}


	for (auto it = m_Folders.begin(); it != m_Folders.end();)
	{
		const auto& folder = *it;

		if (folder.find(".esp") == std::string::npos &&
			folder.find(".esm") == std::string::npos &&
			folder.find(".esl") == std::string::npos)
		{
			g_Logger->error("Unexpected pluginfolder file extension in {}", folder);
			it = m_Folders.erase(it);
		}
		else
		{
			RE::TESDataHandler* handler = RE::TESDataHandler::GetSingleton();
			auto IsInDataFolder = handler->LookupModByName(folder);

			if (!IsInDataFolder)
			{
				g_Logger->info("Plugin {} not found, skipping all files in the folder", folder);
				it = m_Folders.erase(it);
			}
			else
			{
				++it;
			}
		}
	}

	// Sort folders based on load order of plugins
	std::vector<std::string> loadOrder = GetLoadOrder();
	std::sort(m_Folders.begin(), m_Folders.end(), [&loadOrder](const std::string& a, const std::string& b)
		{
			auto itA = std::find(loadOrder.begin(), loadOrder.end(), a);
			auto itB = std::find(loadOrder.begin(), loadOrder.end(), b);
			if (itA == loadOrder.end()) return false;
			if (itB == loadOrder.end()) return true;
			return std::distance(loadOrder.begin(), itA) < std::distance(loadOrder.begin(), itB);
		});

}


void Config::EnumerateFilesInFolders(const std::string folders) //Get all files in each of the folders directory
{
	const std::string folderPath = "Data\\SKSE\\DynamicStringReplacer\\" + folders;
	m_Files.clear();
	m_Files.shrink_to_fit();

	for (const auto& entry : std::filesystem::recursive_directory_iterator(folderPath))
	{
		if (entry.is_regular_file() && entry.path().extension() == L".json")
		{
			m_Files.push_back(entry.path().filename().string());

			/*
			for (const auto& mfile : m_Files)
			{
				DEBUG_LOG(g_Logger, "m_Files: {} ", mfile);
			}
			*/
		}

	}


	static int folderCount = 1; //Only for Debug log
	for (const auto& file : m_Files)
	{
		m_FilesInPluginFolder.push_back(folderPath + "\\" + file);

		DEBUG_LOG(g_Logger, "Datei{}: {}", folderCount++, (folderPath + "\\" + file));
	}
}

bool Config::isSpecialType(const std::string& type)
{
	return type == "INFO NAM1" || type == "QUST CNAM" || type == "QUST NNAM" || type == "INFO RNAM";
}

void Config::ParseTranslationFiles()
{
	for (const auto& files : m_FilesInPluginFolder)
	{

		DEBUG_LOG(g_Logger, "Parsing file  {}", files);

		std::ifstream file(files);
		if (!file.is_open())
		{
			g_Logger->error("Couldn't open file {}", files);
			continue;
		}

		json jsonData;
		file >> jsonData;

		//Read data out of json file
		for (const auto& entry : jsonData)
		{
			std::string types = entry["type"];
			std::string stringValue = entry["string"];

			if (!isSpecialType(types))
			{
				std::string editorId = entry["editor_id"];

				// extract first four letters to get the record type
				std::string type = types.substr(0, 4);

				// All the other letters are the subrecord type
				std::string subrecord;
				size_t spacePos = types.find(' ');
				if (spacePos != std::string::npos && spacePos + 1 < types.length())
				{
					subrecord = types.substr(spacePos + 1);
				}

				RE::TESForm* form = RE::TESForm::LookupByEditorID(editorId);
				if (!form)
				{
					g_Logger->error("Couldn't find Editor ID {} out of file {}", editorId, files);
					continue;
				}

				g_ConfigurationInformationStruct.emplace_back(form->formID, form, stringValue, type, subrecord);
				//These information will be used in Hooks and Processor 
				//to get the right description to right place

				/*
				DEBUG_LOG(g_Logger, "Form ID: {0:08X}", g_ConfigurationInformationStruct.FormID);
				DEBUG_LOG(g_Logger, "String: {}", g_ConfigurationInformationStruct.ReplacerText.c_str());
				DEBUG_LOG(g_Logger, "RecordType: {}", g_ConfigurationInformationStruct.RecordType.c_str());
				DEBUG_LOG(g_Logger, "SubrecordType: {}", g_ConfigurationInformationStruct.SubrecordType.c_str());
				*/
			}
			else
			{
				std::string original = entry["original"];

				if (types == "INFO NAM1")
				{
					g_INFO_NAM1_Map.emplace(original, stringValue);
				}
				else if (types == "INFO RNAM")
				{
					g_INFO_RNAM_Map.emplace(original, stringValue);
				}
				else if (types == "QUST NNAM")
				{
					g_QUST_NNAM_Map.emplace(original, stringValue);
				}
				else if (types == "QUST CNAM")
				{
					g_QUST_CNAM_Map.emplace(original, stringValue);
				}

			}
		}


	}
}

void Config::LoadFiles()
{
	for (const auto& folders : m_Folders)
	{

		EnumerateFilesInFolders(folders);

	}

	ParseTranslationFiles();

}
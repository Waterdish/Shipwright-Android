#include "Main.h"
#include "Exporter.h"
#include "BackgroundExporter.h"
#include "TextureExporter.h"
#include "RoomExporter.h"
#include "CollisionExporter.h"
#include "DisplayListExporter.h"
#include "PlayerAnimationExporter.h"
#include "SkeletonExporter.h"
#include "SkeletonLimbExporter.h"
#include "ArrayExporter.h"
#include "VtxExporter.h"
#include "AnimationExporter.h"
#include "CutsceneExporter.h"
#include "PathExporter.h"
#include "TextExporter.h"
#include "BlobExporter.h"
#include "MtxExporter.h"
#include "AudioExporter.h"
#include <Globals.h>
#include <Utils/DiskFile.h>
#include <Utils/Directory.h>
#include <Utils/MemoryStream.h>
#include <Utils/BinaryWriter.h>
#include <Utils/BitConverter.h>
#include <bit>
#include <mutex>

std::string otrFileName = "oot.otr";
std::string customOtrFileName = "";
std::string customAssetsPath = "";
std::string portVersionString = "0.0.0";

std::shared_ptr<LUS::Archive> otrArchive;
BinaryWriter* fileWriter;
std::chrono::steady_clock::time_point fileStart, resStart;
std::map<std::string, std::vector<char>> files;
std::mutex fileMutex;

void InitVersionInfo();

enum class ExporterFileMode
{
	BuildOTR = (int)ZFileMode::Custom + 1,
};

static void ExporterParseFileMode(const std::string& buildMode, ZFileMode& fileMode)
{
	if (buildMode == "botr")
	{
		fileMode = (ZFileMode)ExporterFileMode::BuildOTR;

		printf("BOTR: Generating OTR Archive...\n");

		if (DiskFile::Exists(otrFileName))
			otrArchive = std::shared_ptr<LUS::Archive>(new LUS::Archive(otrFileName, true));
		else
			otrArchive = LUS::Archive::CreateArchive(otrFileName, 40000);

		auto lst = Directory::ListFiles("Extract");

		for (auto item : lst)
		{
			auto fileData = DiskFile::ReadAllBytes(item);
			otrArchive->AddFile(StringHelper::Split(item, "Extract/")[1], (uintptr_t)fileData.data(), fileData.size());
		}
	}
}

static void ExporterProgramEnd()
{
	uint32_t crc = 0xFFFFFFFF;
	const uint8_t endianness = (uint8_t)Endianness::Big;

	std::vector<uint16_t> portVersion = {};
	std::vector<std::string> versionParts = StringHelper::Split(portVersionString, ".");

	// If a major.minor.patch string was not passed in, fallback to 0 0 0 
	if (versionParts.size() != 3) {
		portVersion = { 0, 0, 0 };
	} else {
		// Parse version values to number
		for (const auto& val : versionParts) {
			uint16_t num = 0;
			try {
				num = (uint16_t)std::stoi(val, nullptr);
			} catch (std::invalid_argument &e) {
				num = 0;
			} catch (std::out_of_range &e) {
				num = 0;
			}

			portVersion.push_back(num);
		}
	}

	MemoryStream *portVersionStream = new MemoryStream();
	BinaryWriter portVerWriter(portVersionStream);
	portVerWriter.SetEndianness(Endianness::Big);
	portVerWriter.Write(endianness);
	portVerWriter.Write(portVersion[0]); // Major
	portVerWriter.Write(portVersion[1]); // Minor
	portVerWriter.Write(portVersion[2]); // Patch
	portVerWriter.Close();
	
	if (Globals::Instance->fileMode == ZFileMode::ExtractDirectory)
	{
		std::string romPath = Globals::Instance->baseRomPath.string();
		std::vector<uint8_t> romData = DiskFile::ReadAllBytes(romPath);

		BitConverter::RomToBigEndian(romData.data(), romData.size());

		crc = BitConverter::ToUInt32BE(romData, 0x10);
		printf("Creating version file...\n");

		// Get crc from rom

		MemoryStream *versionStream = new MemoryStream();
		BinaryWriter writer(versionStream);
		writer.SetEndianness(Endianness::Big);
		writer.Write(endianness);
		writer.Write(crc);
		writer.Close();

		printf("Created version file.\n");

		printf("Generating OTR Archive...\n");
		otrArchive = LUS::Archive::CreateArchive(otrFileName, 40000);

		printf("Adding game version file.\n");
		otrArchive->AddFile("version", (uintptr_t)versionStream->ToVector().data(), versionStream->GetLength());

		printf("Adding portVersion file.\n");
		otrArchive->AddFile("portVersion", (uintptr_t)portVersionStream->ToVector().data(), portVersionStream->GetLength());

		for (const auto& item : files)
		{
			std::string fName = item.first;
			if (fName.find("gTitleZeldaShieldLogoMQTex") != std::string::npos && !ZRom(romPath).IsMQ())
			{
				size_t pos = 0;
				if ((pos = fName.find("gTitleZeldaShieldLogoMQTex", 0)) != std::string::npos)
				{
					fName.replace(pos, 27, "gTitleZeldaShieldLogoTex");
				}
			}
			const auto& fileData = item.second;
			otrArchive->AddFile(fName, (uintptr_t)fileData.data(),
								fileData.size());
		}
	}

	otrArchive = nullptr;
	delete fileWriter;
	files.clear();

	// Generate custom otr file for extra assets
	if (customAssetsPath == "" || customOtrFileName == "" || DiskFile::Exists(customOtrFileName)) {
		printf("No Custom Assets path or otr file name provided, otr file already exists. Nothing to do.\n");
		return;
	}

	if (!customAssetsPath.ends_with("/")) {
		customAssetsPath += "/";
	}

	const auto& lst = Directory::ListFiles(customAssetsPath);

	printf("Generating Custom OTR Archive...\n");
	std::shared_ptr<LUS::Archive> customOtr = LUS::Archive::CreateArchive(customOtrFileName, 4096);

	printf("Adding portVersion file.\n");
	customOtr->AddFile("portVersion", (uintptr_t)portVersionStream->ToVector().data(), portVersionStream->GetLength());

	for (const auto& item : lst)
	{
		size_t filenameSepAt = item.find_last_of("/\\");
		const std::string filename = item.substr(filenameSepAt + 1);

		if (std::count(filename.begin(), filename.end(), '.') >= 2)
		{
			size_t extensionSepAt = filename.find_last_of(".");
			size_t formatSepAt = filename.find_last_of(".", extensionSepAt - 1);

			const std::string extension = filename.substr(extensionSepAt + 1);
			const std::string format = filename.substr(formatSepAt + 1, extensionSepAt - formatSepAt - 1);
			std::string afterPath = item.substr(0, filenameSepAt + formatSepAt + 1);

			if (extension == "png" && (format == "rgba32" || format == "rgb5a1" || format == "i4" || format == "i8" || format == "ia4" || format == "ia8" || format == "ia16" || format == "ci4" || format == "ci8"))
			{
				ZTexture tex(nullptr);
				Globals::Instance->buildRawTexture = true;
				tex.FromPNG(item, ZTexture::GetTextureTypeFromString(format));
				printf("customOtr->AddFile(%s)\n", StringHelper::Split(afterPath, customAssetsPath)[1].c_str());

				OTRExporter_Texture exporter;

				MemoryStream* stream = new MemoryStream();
				BinaryWriter writer(stream);

				exporter.Save(&tex, "", &writer);

				std::string src = tex.GetBodySourceCode();
				writer.Write((char *)src.c_str(), src.size());

				std::vector<char> fileData = stream->ToVector();
				customOtr->AddFile(StringHelper::Split(afterPath, customAssetsPath)[1], (uintptr_t)fileData.data(), fileData.size());
				continue;
			}
		}

		if (item.find("accessibility") != std::string::npos)
		{
			std::string extension = filename.substr(filename.find_last_of(".") + 1);
			if (extension == "json")
			{
				const auto &fileData = DiskFile::ReadAllBytes(item);
				printf("Adding accessibility texts %s\n", StringHelper::Split(item, customAssetsPath)[1].c_str());
				customOtr->AddFile(StringHelper::Split(item, customAssetsPath)[1], (uintptr_t)fileData.data(), fileData.size());
			}
			continue;
		}

		const auto& fileData = DiskFile::ReadAllBytes(item);
		printf("customOtr->AddFile(%s)\n", StringHelper::Split(item, customAssetsPath)[1].c_str());
		customOtr->AddFile(StringHelper::Split(item, customAssetsPath)[1], (uintptr_t)fileData.data(), fileData.size());
	}

	customOtr = nullptr;
}

static void ExporterParseArgs(int argc, char* argv[], int& i)
{
	std::string arg = argv[i];

	if (arg == "--otrfile") {
		otrFileName = argv[i + 1];
		i++;
	} else if (arg == "--customOtrFile") {
		customOtrFileName = argv[i + 1];
		i++;
	} else if (arg == "--customAssetsPath") {
		customAssetsPath = argv[i + 1];
		i++;
	} else if (arg == "--portVer") {
		portVersionString = argv[i + 1];
		i++;
	}
}

static bool ExporterProcessFileMode(ZFileMode fileMode)
{
	// Do whatever work is associated with these custom file modes...
	// Return true to indicate one of our own file modes is being processed
	if (fileMode == (ZFileMode)ExporterFileMode::BuildOTR)
		return true;

	return false;
}

static void ExporterFileBegin(ZFile* file)
{
	fileStart = std::chrono::steady_clock::now();

	MemoryStream* stream = new MemoryStream();
	fileWriter = new BinaryWriter(stream);
}

static void ExporterFileEnd(ZFile* file)
{
	// delete fileWriter;
}

static void ExporterResourceEnd(ZResource* res, BinaryWriter& writer)
{
	auto streamShared = writer.GetStream();
	MemoryStream* strem = (MemoryStream*)streamShared.get();

	auto start = std::chrono::steady_clock::now();

	if (res->GetName() != "")
	{
		std::string oName = res->parent->GetOutName();
		std::string rName = res->GetName();
		std::string prefix = OTRExporter_DisplayList::GetPrefix(res);

		//auto xmlFilePath = res->parent->GetXmlFilePath();
		//prefix = StringHelper::Split(StringHelper::Split(xmlFilePath.string(), "xml\\")[1], ".xml")[0];

		if (StringHelper::Contains(oName, "_scene"))
		{
			auto split = StringHelper::Split(oName, "_");
			oName = "";
			for (size_t i = 0; i < split.size() - 1; i++)
				oName += split[i] + "_";

			oName += "scene";
		}
		else if (StringHelper::Contains(oName, "_room"))
		{
			oName = StringHelper::Split(oName, "_room")[0] + "_scene";
		}

		std::string fName = "";

		if (prefix != "")
			fName = StringHelper::Sprintf("%s/%s/%s", prefix.c_str(), oName.c_str(), rName.c_str());
		else
			fName = StringHelper::Sprintf("%s/%s", oName.c_str(), rName.c_str());

		if (Globals::Instance->fileMode == ZFileMode::ExtractDirectory)
		{
			std::unique_lock Lock(fileMutex);
			files[fName] = strem->ToVector();
		}
		else
			DiskFile::WriteAllBytes("Extract/" + fName, strem->ToVector());
	}

	auto end = std::chrono::steady_clock::now();
	size_t diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

	//if (diff > 10)
		//printf("Exported Resource End %s in %zums\n", res->GetName().c_str(), diff);
}

static void ExporterProcessCompilable(tinyxml2::XMLElement* reader)
{
	std::string nodeName = reader->Name();
}

static void ExporterXMLBegin()
{
}

static void ExporterXMLEnd()
{
}

void AddFile(std::string fName, std::vector<char> data)
{
	if (Globals::Instance->fileMode != ZFileMode::ExtractDirectory)
		DiskFile::WriteAllBytes("Extract/" + fName, data);
	else
	{
		std::unique_lock Lock(fileMutex);
		files[fName] = data;
	}
}

void ImportExporters()
{
	// In this example we set up a new exporter called "EXAMPLE".
	// By running ZAPD with the argument -se EXAMPLE, we tell it that we want to use this exporter for our resources.
	ExporterSet* exporterSet = new ExporterSet();
	exporterSet->processFileModeFunc = ExporterProcessFileMode;
	exporterSet->parseFileModeFunc = ExporterParseFileMode;
	exporterSet->processCompilableFunc = ExporterProcessCompilable;
	exporterSet->parseArgsFunc = ExporterParseArgs;
	exporterSet->beginFileFunc = ExporterFileBegin;
	exporterSet->endFileFunc = ExporterFileEnd;
	exporterSet->beginXMLFunc = ExporterXMLBegin;
	exporterSet->endXMLFunc = ExporterXMLEnd;
	exporterSet->resSaveFunc = ExporterResourceEnd;
	exporterSet->endProgramFunc = ExporterProgramEnd;

	exporterSet->exporters[ZResourceType::Background] = new OTRExporter_Background();
	exporterSet->exporters[ZResourceType::Texture] = new OTRExporter_Texture();
	exporterSet->exporters[ZResourceType::Room] = new OTRExporter_Room();
	exporterSet->exporters[ZResourceType::AltHeader] = new OTRExporter_Room();
	exporterSet->exporters[ZResourceType::Scene] = new OTRExporter_Room();
	exporterSet->exporters[ZResourceType::CollisionHeader] = new OTRExporter_Collision();
	exporterSet->exporters[ZResourceType::DisplayList] = new OTRExporter_DisplayList();
	exporterSet->exporters[ZResourceType::PlayerAnimationData] = new OTRExporter_PlayerAnimationExporter();
	exporterSet->exporters[ZResourceType::Skeleton] = new OTRExporter_Skeleton();
	exporterSet->exporters[ZResourceType::Limb] = new OTRExporter_SkeletonLimb();
	exporterSet->exporters[ZResourceType::Animation] = new OTRExporter_Animation();
	exporterSet->exporters[ZResourceType::Cutscene] = new OTRExporter_Cutscene();
	exporterSet->exporters[ZResourceType::Vertex] = new OTRExporter_Vtx();
	exporterSet->exporters[ZResourceType::Array] = new OTRExporter_Array();
	exporterSet->exporters[ZResourceType::Path] = new OTRExporter_Path();
	exporterSet->exporters[ZResourceType::Text] = new OTRExporter_Text();
	exporterSet->exporters[ZResourceType::Blob] = new OTRExporter_Blob();
	exporterSet->exporters[ZResourceType::Mtx] = new OTRExporter_MtxExporter();
	exporterSet->exporters[ZResourceType::Audio] = new OTRExporter_Audio();

	Globals::AddExporter("OTR", exporterSet);

	InitVersionInfo();
}

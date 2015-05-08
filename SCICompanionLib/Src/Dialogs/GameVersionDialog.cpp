// GamePropertiesDialog.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "GameVersionDialog.h"
#include "format.h"
using namespace fmt;

// CGameVersionDialog dialog

CGameVersionDialog::CGameVersionDialog(SCIVersion &version, CWnd* pParent /*=NULL*/)
    : CExtNCW<CExtResizableDialog>(CGameVersionDialog::IDD, pParent), _version(version)
{
    _Sync();
}

void CGameVersionDialog::_Sync()
{
    _fHasPalette = _version.HasPalette ? 1 : 0;
    _viewFormat =  (int)_version.ViewFormat;
    _picFormat = (int)_version.PicFormat;
    _fGrayscaleCursors = _version.GrayScaleCursors ? 1 : 0;
    _fCodeSCI1 = _version.lofsaOpcodeIsAbsolute ? 1 : 0;
    _fVocab900 = (_version.MainVocabResource == 900);
    _fEarlySCI0Script = _version.HasOldSCI0ScriptHeader ? 1 : 0;
    _fSCI11Palettes = _version.sci11Palettes ? 1 : 0;

    _resourceMapVersion = (int)_version.MapFormat;
    _resourcePackVersion = (int)_version.PackageFormat;
    _compressionVersion = (int)_version.CompressionFormat;
    _soundVersion = (int)_version.SoundFormat;
    _fSeparateHeapResources = (int)_version.SeparateHeapResources;
}

SCIVersion CGameVersionDialog::_ReverseSync()
{
    SCIVersion version = { };
    version.HasPalette = (_fHasPalette != 0);
    version.ViewFormat = (ViewFormat)_viewFormat;
    version.PicFormat = (PicFormat)_picFormat;
    version.GrayScaleCursors = (_fGrayscaleCursors != 0);
    version.lofsaOpcodeIsAbsolute = (_fCodeSCI1 != 0);
    version.MainVocabResource = _fVocab900 ? 900 : 0;
    version.HasOldSCI0ScriptHeader = (_fEarlySCI0Script != 0);
    version.sci11Palettes = (_fSCI11Palettes != 0);

    version.MapFormat = (ResourceMapFormat)_resourceMapVersion;
    version.PackageFormat = (ResourcePackageFormat)_resourcePackVersion;
    version.CompressionFormat = (CompressionFormat)_compressionVersion;
    version.SoundFormat = (SoundFormat)_soundVersion;
    version.SeparateHeapResources = (_fSeparateHeapResources != 0);
    return version;
}

void CGameVersionDialog::OnOK()
{
    __super::OnOK();
    _version = _ReverseSync();
}

void CGameVersionDialog::DoDataExchange(CDataExchange* pDX)
{
    ShowSizeGrip(FALSE);
    CExtResizableDialog::DoDataExchange(pDX);
      
    DDX_Control(pDX, IDC_CHECK1, m_wndHasPalette);
    DDX_Control(pDX, IDC_CHECK4, m_wndGrayscaleCursors);
    DDX_Control(pDX, IDC_CHECK5, m_wndSCI1Code);
    DDX_Control(pDX, IDC_CHECK6, m_wndSepHeap);
    DDX_Control(pDX, IDC_CHECK7, m_wndParserVocab900);
    DDX_Control(pDX, IDC_CHECK8, m_wndEarlySCI0Script);
    DDX_Control(pDX, IDC_CHECK9, m_wndSCI11Palettes);
    

    DDX_Check(pDX, IDC_CHECK1, _fHasPalette);
    DDX_Check(pDX, IDC_CHECK4, _fGrayscaleCursors);
    DDX_Check(pDX, IDC_CHECK5, _fCodeSCI1);
    DDX_Check(pDX, IDC_CHECK6, _fSeparateHeapResources);
    DDX_Check(pDX, IDC_CHECK7, _fVocab900);
    DDX_Check(pDX, IDC_CHECK8, _fEarlySCI0Script);
    DDX_Check(pDX, IDC_CHECK9, _fSCI11Palettes);
    
    DDX_Control(pDX, IDC_STATIC1, m_wndGroupResourceMap);
    DDX_Control(pDX, IDC_STATIC2, m_wndGroupSound);
    DDX_Control(pDX, IDC_STATIC3, m_wndGroupCompression);
    DDX_Control(pDX, IDC_STATIC4, m_wndLabel4);
    DDX_Control(pDX, IDC_STATIC5, m_wndLabel5);
    DDX_Control(pDX, IDC_STATIC6, m_wndGroupResourcePack);

    DDX_Control(pDX, IDC_RADIOSCI0, m_wndRadioResourceMapSCI0);
    DDX_Control(pDX, IDC_RADIOSCI1, m_wndRadioResourceMapSCI1);
    DDX_Control(pDX, IDC_RADIOSCI6, m_wndRadioResourceMapSCI11);
    DDX_Control(pDX, IDC_RADIOSCI7, m_wndRadioResourcePackSCI0);
    DDX_Control(pDX, IDC_RADIOSCI8, m_wndRadioResourcePackSCI1);
    DDX_Control(pDX, IDC_RADIOSCI9, m_wndRadioResourcePackSCI11);
    DDX_Control(pDX, IDC_RADIOSCI2, m_wndRadioSoundSCI0);
    DDX_Control(pDX, IDC_RADIOSCI3, m_wndRadioSoundSCI1);
    DDX_Control(pDX, IDC_RADIOSCI4, m_wndRadioCompressionSCI0);
    DDX_Control(pDX, IDC_RADIOSCI5, m_wndRadioCompressionSCI1);

    DDX_Radio(pDX, IDC_RADIOSCI0, _resourceMapVersion);
    DDX_Radio(pDX, IDC_RADIOSCI7, _resourcePackVersion);
    DDX_Radio(pDX, IDC_RADIOSCI2, _compressionVersion);
    DDX_Radio(pDX, IDC_RADIOSCI4, _soundVersion);

    DDX_Control(pDX, IDC_COMBO1, m_wndViewCombo);
    DDX_CBIndex(pDX, IDC_COMBO1, _viewFormat);

    DDX_Control(pDX, IDC_COMBO2, m_wndPicCombo);
    DDX_CBIndex(pDX, IDC_COMBO2, _picFormat);

    DDX_Control(pDX, IDOK, m_wndOk);
    DDX_Control(pDX, IDCANCEL, m_wndCancel);
    DDX_Control(pDX, IDC_BUTTON1, m_wndViewResMap);
    DDX_Control(pDX, IDC_BUTTON2, m_wndViewMessageMap);
    
    
}

BEGIN_MESSAGE_MAP(CGameVersionDialog, CExtNCW<CExtResizableDialog>)
    ON_COMMAND(IDC_BUTTON1, OnViewResourceMap)
    ON_COMMAND(IDC_BUTTON2, OnViewMessageMap)
END_MESSAGE_MAP()

void _PrintMapEntry(MemoryWriter &writer, uint32_t position, const std::vector<uint8_t> &rawData, const ResourceMapEntryAgnostic &entry)
{
    // [address:] [bytes] // type, number, pacakge, offset

    // Address
    writer << pad(hexu(position), 4, '0') << ": ";

    // bytes
    for (uint8_t byte : rawData)
    {
        writer << pad(hexu(byte), 2, '0') << " ";
    }

    if (ValidateResourceType(entry.Type) != ResourceType::None)
    {
        writer << "\t" << GetResourceInfo(entry.Type).pszTitleDefault << "";
    }
    else
    {
        writer << "\tUNKNOWN ";
    }
    writer << "(" << entry.Number << ", " << entry.PackageNumber << ") ";
    writer << "\toffset: " << pad(hexu(entry.Offset), 8, '0');

    writer << "\n";
}

// CGameVersionDialog message handlers
template<typename _TResourceSource>
void _OnViewResourceMap(const _TResourceSource &fileDescriptor, SCIVersion versionTest, const std::string &gameFolder)
{
    std::unique_ptr<sci::streamOwner> streamOwner = fileDescriptor.OpenMap();   // Keep in scope
    sci::istream reader = streamOwner->getReader();

    fmt::MemoryWriter writer;

    // The game title
    writer << "Resource map for ";
    std::string strGameName = appState->GetGameName();
    if (strGameName.empty())
    {
        strGameName = PathFindFileName(gameFolder.c_str());
    }
    writer << strGameName;
    writer << "\n";

    std::unique_ptr<ResourceSource> resourceSource;
    std::vector<RESOURCEMAPPREENTRY_SCI1> lookupPointers;
    ResourceHeaderReadWrite dummy;

    if (versionTest.MapFormat == ResourceMapFormat::SCI0)
    {
        resourceSource = std::make_unique<MapAndPackageSource<SCI0MapNavigator, _TResourceSource>>(versionTest, dummy, gameFolder);
    }
    else if (versionTest.MapFormat == ResourceMapFormat::SCI1)
    {
        auto temp = std::make_unique<MapAndPackageSource<SCI1MapNavigator<RESOURCEMAPENTRY_SCI1>, _TResourceSource>>(versionTest, dummy, gameFolder);
        lookupPointers = temp->GetLookupPointers(reader);
        resourceSource = move(temp);
    }
    else if (versionTest.MapFormat == ResourceMapFormat::SCI11)
    {
        auto temp = std::make_unique<MapAndPackageSource<SCI1MapNavigator<RESOURCEMAPENTRY_SCI1_1>, _TResourceSource>>(versionTest, dummy, gameFolder);
        lookupPointers = temp->GetLookupPointers(reader);
        resourceSource = move(temp);
    }

    std::unordered_map<int, std::vector<uint32_t>> volumeToResourceOffsets;

    // Print out the data
    if (resourceSource)
    {
        // Lookup table (SCI1)
        for (const RESOURCEMAPPREENTRY_SCI1 &preEntry : lookupPointers)
        {
            writer << "Type: " << pad(hex(preEntry.bType), 2, '0') << "\tOffset: " << pad(hex(preEntry.wOffset), 4, '0') << "\n";
        }

        // The main data
        ResourceMapEntryAgnostic mapEntry;
        std::vector<uint8_t> rawData;
        uint32_t currentPosition = reader.tellg();
        IteratorState state;
        while (resourceSource->ReadNextEntry(ResourceTypeFlags::All, state, mapEntry, &rawData))
        {
            // more info
            _PrintMapEntry(writer, currentPosition, rawData, mapEntry);
            currentPosition = reader.tellg();

            volumeToResourceOffsets[mapEntry.PackageNumber].push_back(mapEntry.Offset);
        }
        ShowTextFile(writer.c_str(), "resourcemap.txt");
    }

    // Let's print out the resource map volumes too. We'll want a map of vectors for each volume?
    for (auto &volumeAndOffsets : volumeToResourceOffsets)
    {
        fmt::MemoryWriter writer;
        writer << "Resource package " << volumeAndOffsets.first << " for " << strGameName << "\n";
        
        std::sort(volumeAndOffsets.second.begin(), volumeAndOffsets.second.end());

        // I want the agnostic header, and then also the raw header data and the raw other data.
        // For now we'll just show the raw data
        std::unique_ptr<sci::streamOwner> streamOwner = fileDescriptor.OpenVolume(volumeAndOffsets.first);
        sci::istream reader = streamOwner->getReader();

        for (uint32_t offset : volumeAndOffsets.second)
        {
            reader.seekg(offset);
            int countOfBytesToShow = min(16, reader.getBytesRemaining());
            writer << pad(hex(offset), 6, '0') << ": ";

            for (int i = 0; i < countOfBytesToShow; i++)
            {
                uint8_t byte;
                reader >> byte;
                writer << pad(hexu(byte), 2, '0') << " ";
            }
            writer << "\n";
        }

        ShowTextFile(writer.c_str(), format("package file {}.txt", volumeAndOffsets.first));
    }
}

void CGameVersionDialog::OnViewResourceMap()
{
    // REVIEW: Not sure if we need to call data exchage???
    SCIVersion versionTest = _ReverseSync();
    FileDescriptorResourceMap resourceMapFileDescriptor(appState->GetResourceMap().GetGameFolder());
    _OnViewResourceMap(resourceMapFileDescriptor, versionTest, appState->GetResourceMap().GetGameFolder());
}

void CGameVersionDialog::OnViewMessageMap()
{
    SCIVersion versionTest = _ReverseSync();
    FileDescriptorMessageMap messageMapFileDescriptor(appState->GetResourceMap().GetGameFolder());
    _OnViewResourceMap(messageMapFileDescriptor, versionTest, appState->GetResourceMap().GetGameFolder());
}


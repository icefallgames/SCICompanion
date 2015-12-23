/***************************************************************************
    Copyright (c) 2015 Philip Fortier

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
***************************************************************************/
// GamePropertiesDialog.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "GameVersionDialog.h"
#include "format.h"
#include "ResourceContainer.h"
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
    _audioVolume = (int)_version.AudioVolumeName;
    _fGrayscaleCursors = _version.GrayScaleCursors ? 1 : 0;
    _fCodeSCI1 = _version.lofsaOpcodeIsAbsolute ? 1 : 0;
    _fVocab900 = (_version.MainVocabResource == 900);
    _fEarlySCI0Script = _version.HasOldSCI0ScriptHeader ? 1 : 0;
    _fSCI11Palettes = _version.sci11Palettes ? 1 : 0;
    _fSupportsMessages = _version.SupportsMessages ? 1 : 0;
    _fSupportsMessageAudio = _version.HasSyncResources ? 1 : 0;
    _fAudioIsWavFormat = _version.AudioIsWav ? 1 : 0;
    _fWideExports = _version.IsExportWide ? 1 : 0;

    _resourceMapVersion = (int)_version.MapFormat;
    _resourcePackVersion = (int)_version.PackageFormat;
    _compressionVersion = (int)_version.CompressionFormat;
    _soundVersion = (int)_version.SoundFormat;
    _fSeparateHeapResources = (int)_version.SeparateHeapResources;

    _mainAudioMapFormat = (int)_version.MainAudioMapVersion;
    _base36AudioMapFormat = (int)_version.Base36AudioMapVersion;
    _audioMapNumberIndex = (_version.AudioMapResourceNumber == 0) ? 0 : 1;
    _messageMapSource = (int)_version.MessageMapSource;
    _resolution = (int)_version.DefaultResolution;
    _kernelSet = (int)_version.Kernels;
}

SCIVersion CGameVersionDialog::_ReverseSync()
{
    SCIVersion version = _version;  // Covers stuff we don't expose in the UI, like DefaultVolumeFile (we should, though)
    version.HasPalette = (_fHasPalette != 0);
    version.ViewFormat = (ViewFormat)_viewFormat;
    version.PicFormat = (PicFormat)_picFormat;
    version.AudioVolumeName = (AudioVolumeName)_audioVolume;
    version.GrayScaleCursors = (_fGrayscaleCursors != 0);
    version.lofsaOpcodeIsAbsolute = (_fCodeSCI1 != 0);
    version.MainVocabResource = _fVocab900 ? 900 : 0;
    version.HasOldSCI0ScriptHeader = (_fEarlySCI0Script != 0);
    version.sci11Palettes = (_fSCI11Palettes != 0);
    version.SupportsMessages = (_fSupportsMessages != 0);
    version.HasSyncResources = (_fSupportsMessageAudio != 0);
    version.AudioIsWav = (_fAudioIsWavFormat != 0);
    version.IsExportWide = (_fWideExports != 0);

    version.MapFormat = (ResourceMapFormat)_resourceMapVersion;
    version.PackageFormat = (ResourcePackageFormat)_resourcePackVersion;
    version.CompressionFormat = (CompressionFormat)_compressionVersion;
    version.SoundFormat = (SoundFormat)_soundVersion;
    version.SeparateHeapResources = (_fSeparateHeapResources != 0);

    version.MainAudioMapVersion = (AudioMapVersion)_mainAudioMapFormat;
    version.Base36AudioMapVersion = (AudioMapVersion)_base36AudioMapFormat;
    version.AudioMapResourceNumber = (_audioMapNumberIndex == 0) ? 0 : 65535;
    version.MessageMapSource = (MessageMapSource)_messageMapSource;
    version.DefaultResolution = (NativeResolution)_resolution;
    version.Kernels = (KernelSet)_kernelSet;
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
    DDX_Control(pDX, IDC_CHECK11, m_wndSupportsMessages);
    DDX_Control(pDX, IDC_CHECK12, m_wndSupportsMessageAudio);
    DDX_Control(pDX, IDC_CHECK13, m_wndAudioIsWavFormat);
    DDX_Control(pDX, IDC_CHECK14, m_wndWideExports);

    DDX_Check(pDX, IDC_CHECK1, _fHasPalette);
    DDX_Check(pDX, IDC_CHECK4, _fGrayscaleCursors);
    DDX_Check(pDX, IDC_CHECK5, _fCodeSCI1);
    DDX_Check(pDX, IDC_CHECK6, _fSeparateHeapResources);
    DDX_Check(pDX, IDC_CHECK7, _fVocab900);
    DDX_Check(pDX, IDC_CHECK8, _fEarlySCI0Script);
    DDX_Check(pDX, IDC_CHECK9, _fSCI11Palettes);
    DDX_Check(pDX, IDC_CHECK11, _fSupportsMessages);
    DDX_Check(pDX, IDC_CHECK12, _fSupportsMessageAudio);
    DDX_Check(pDX, IDC_CHECK13, _fAudioIsWavFormat);
    DDX_Check(pDX, IDC_CHECK14, _fWideExports);
    
    DDX_Control(pDX, IDC_STATIC1, m_wndGroupResourceMap);
    DDX_Control(pDX, IDC_STATIC2, m_wndGroupSound);
    DDX_Control(pDX, IDC_STATIC3, m_wndGroupCompression);
    DDX_Control(pDX, IDC_STATIC4, m_wndLabel4);
    DDX_Control(pDX, IDC_STATIC5, m_wndLabel5);
    DDX_Control(pDX, IDC_STATIC6, m_wndGroupResourcePack);
    DDX_Control(pDX, IDC_STATIC7, m_wndLabel7);
    DDX_Control(pDX, IDC_STATIC10, m_wndLabel10);
    DDX_Control(pDX, IDC_STATIC11, m_wndLabel11);
    DDX_Control(pDX, IDC_STATIC12, m_wndLabel12);
    DDX_Control(pDX, IDC_STATIC13, m_wndLabel13);
    DDX_Control(pDX, IDC_STATIC14, m_wndLabel14);
    DDX_Control(pDX, IDC_STATIC15, m_wndLabel15);

    DDX_Control(pDX, IDC_RADIOSCI0, m_wndRadioResourceMapSCI0);
    DDX_Control(pDX, IDC_RADIOSCI0_1, m_wndRadioResourceMapSCI0_SCI1);
    DDX_Control(pDX, IDC_RADIOSCI1, m_wndRadioResourceMapSCI1);
    DDX_Control(pDX, IDC_RADIOSCI6, m_wndRadioResourceMapSCI11);
    DDX_Control(pDX, IDC_RADIOSCI11, m_wndRadioResourceMapSCI2);
    DDX_Control(pDX, IDC_RADIOSCI7, m_wndRadioResourcePackSCI0);
    DDX_Control(pDX, IDC_RADIOSCI8, m_wndRadioResourcePackSCI1);
    DDX_Control(pDX, IDC_RADIOSCI9, m_wndRadioResourcePackSCI11);
    DDX_Control(pDX, IDC_RADIOSCI10, m_wndRadioResourcePackSCI2);
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

    DDX_Control(pDX, IDC_COMBO3, m_wndAudioCombo);
    DDX_CBIndex(pDX, IDC_COMBO3, _audioVolume);

    DDX_Control(pDX, IDC_COMBO4, m_wndMainAudioMapCombo);
    DDX_CBIndex(pDX, IDC_COMBO4, _mainAudioMapFormat);

    DDX_Control(pDX, IDC_COMBO5, m_wndBase36AudioMapCombo);
    DDX_CBIndex(pDX, IDC_COMBO5, _base36AudioMapFormat);

    DDX_Control(pDX, IDC_COMBO6, m_wndAudioMapNumberCombo);
    DDX_CBIndex(pDX, IDC_COMBO6, _audioMapNumberIndex);

    DDX_Control(pDX, IDC_COMBO7, m_wndMessageMapSourceCombo);
    DDX_CBIndex(pDX, IDC_COMBO7, _messageMapSource);

    DDX_Control(pDX, IDC_COMBO8, m_wndResolution);
    DDX_CBIndex(pDX, IDC_COMBO8, _resolution);

    DDX_Control(pDX, IDC_COMBO9, m_wndKernelSet);
    DDX_CBIndex(pDX, IDC_COMBO9, _kernelSet);

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
        resourceSource = std::make_unique<MapAndPackageSource<SCI0MapNavigator<RESOURCEMAPENTRY_SCI0>, _TResourceSource>>(versionTest, dummy, gameFolder);
    }
    else if (versionTest.MapFormat == ResourceMapFormat::SCI0_LayoutSCI1)
    {
        resourceSource = std::make_unique<MapAndPackageSource<SCI0MapNavigator<RESOURCEMAPENTRY_SCI0_SCI1LAYOUT>, _TResourceSource>>(versionTest, dummy, gameFolder);
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
    else if (versionTest.MapFormat == ResourceMapFormat::SCI2)
    {
        auto temp = std::make_unique<MapAndPackageSource<SCI1MapNavigator<RESOURCEMAPENTRY_SCI1>, _TResourceSource>>(versionTest, dummy, gameFolder);
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


#pragma once

#include "Version.h"

class ResourceEntity;

class IResourceIdentifier
{
public:
    virtual int GetPackageHint() const = 0;
    virtual int GetNumber() const = 0;
    virtual ResourceType GetType() const = 0;
    virtual int GetChecksum() const = 0;
};

// fwd decl
class ResourceBlob;
std::unique_ptr<ResourceEntity> CreateResourceFromResourceData(const ResourceBlob &data, bool fallbackOnException = true);

void ExportResourceAsBitmap(const ResourceEntity &resourceEntity);

struct SCI_RESOURCE_INFO
{
    const char *pszSampleFolderName;
    const char *pszTitleDefault;         // Name of the resource in the editor, and the header in game.ini
    const char *pszFileFilter_SCI0;
    const char *pszFileFilter_SCI1;
    const char *pszNameMatch_SCI0;
    const char *pszNameMatch_SCI1;
};

extern SCI_RESOURCE_INFO g_resourceInfo[18];
SCI_RESOURCE_INFO &GetResourceInfo(ResourceType type);
ResourceType ValidateResourceType(ResourceType type);
std::string GetFileDialogFilterFor(ResourceType type, SCIVersion version);
std::string GetFileNameFor(ResourceType type, int number, SCIVersion version);
bool MatchesResourceFilenameFormat(const std::string &filename, ResourceType type, SCIVersion version, int *numberOut, std::string &nameOut);
bool MatchesResourceFilenameFormat(const std::string &filename, SCIVersion version, int *numberOut, std::string &nameOut);

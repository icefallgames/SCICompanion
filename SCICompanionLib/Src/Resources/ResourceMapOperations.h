#pragma once

class CResourceMap;
class ResourceBlob;
class ResourceSource;

void DeleteResource(CResourceMap &resourceMap, const ResourceBlob &data);
std::unique_ptr<ResourceSource> CreateResourceSource(const std::string &gameFolder, SCIVersion version, ResourceSourceFlags source, ResourceSourceAccessFlags access = ResourceSourceAccessFlags::Read, int mapContext = -1);
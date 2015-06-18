#pragma once

//
// Contains information regarding which resources are the "current ones"
//

// fwd decl
class ResourceBlob;

class ResourceRecency
{
public:
    ResourceRecency() { _idJustAdded = -1; }
    ~ResourceRecency() { ClearAllResourceTypes(); } // Clean up
    //
    // We added a new resource to the view.
    // fAddToEnd: add to end (least recent).  Otherwise, it is added as the most recent.
    //
    void AddResourceToRecency(const IResourceIdentifier *pData, BOOL fAddToEnd = FALSE);

    //
    // Someone deleted a resource from the view.
    //
    void DeleteResourceFromRecency(const ResourceBlob *pData);

    //
    // Is this resource the most recent one of the type/number/id?
    // e.g. the one that will be used by the game.
    //
    bool IsResourceMostRecent(const IResourceIdentifier *pData);

    //
    // Did the user just add this resource?
    //
    bool WasResourceJustAdded(const ResourceBlob *pData);

    //
    // Called when we reload the resources of a particular type
    //
    void ClearResourceType(int iType);
    void ClearAllResourceTypes();

private:
    //
    // A map for each of the 10 different resource types.
    // Each map contains values which are arrays of the unique resource ids
    //

    //CMap<int, int, CArray<int, int>*, CArray<int, int>* > _resourceToRecency[NumResourceTypes];

    typedef std::vector<int> ResourceIdArray;
    // hmm... each entry in this array has a bunch of resource keys (combo of number/package) that maps to lists of
    // resource ids.
	typedef std::unordered_map<int, ResourceIdArray*> RecencyMap;
    RecencyMap _resourceRecency[NumResourceTypes];

    int _idJustAdded;
};
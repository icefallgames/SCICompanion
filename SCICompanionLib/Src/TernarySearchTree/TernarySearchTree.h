/*******************************************************************
C++ Package of  Ternary Search Tree
Copyright (C) 2006  Zheyuan Yu

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

Read full GPL at http://www.gnu.org/copyleft/gpl.html

Email me at jerryy@gmail.com if you have any question or comment
WebSite: http://www.cs.dal.ca/~zyu

*************************************************************************/

/**
* File: ternarySearchTree.h
* ----------------
* This file declare TernarySearchTree class.
*
* Ternary search tree stores keys in sorted order, which can be used as a symbol table.
*
* Searching operation is lightning fast, it is reported usually comparable with hashing table,
* and substantially faster than hashing for unsuccessful searches.
*
* Ternary search tree gracefully grows and shrinks, unlike hash table which usually use
* an array and need to be rebuilt after large size changes.
*
* Advance operations such as traversal to get sorted item list, partial matching
* and near-neighbor search are supported natively.
*
* Ternary search tree is initially proposed by Jon Bentley and Bob Sedgewick.
* see references:
* Fast Algorithms for Sorting and Searching Strings: http://www.cs.princeton.edu/~rs/strings/paper.pdf
* Ternary Search Trees: http://www.ddj.com/documents/s=921/ddj9804a/9804a.htm
*
* Revisions:
*
* Jan 16, 2006. Zheyuan Yu.
* Initial creation of ternary search tree class
*
*/

#ifndef _TernarySearchTree_h
#define _TernarySearchTree_h

// uncomment following define to display tree infomation
//#define TST_INFO_ENABLE

// modify following define to enable assert
#define D(x) //x;

/**
* define tree node structure
*/

typedef struct TstNode * TstTree;

typedef struct TstNode
{
    TstNode(char c) : splitChar(c), left(0), right(0), mid(0)
    {
    }
    char splitChar;
    TstTree left, right;
    union
    {
        TstTree mid;
        int index;
    };
} tstNode;

/**
* structure to hold key/value pair, used when building balanced tree.
*/

template <class Object>
struct TstItem
{
    TstItem(const std::string & newKey, const Object & newValue) : key(newKey), value(newValue)
    {
    }

    TstItem()
    {
    }

    ~TstItem()
    {
    }

    std::string key;

    Object value;

    void operator=(const TstItem & item)
    {
        this->key = item.key;
        this->value = item.value;
    }

    bool operator>(const TstItem & item) const
    {
        return this->key > item.key;
    }

    bool operator==(const TstItem & item) const
    {
        return this->key == item.key;
    }

    bool operator<(const TstItem & item) const
    {
        return this->key < item.key;
    }

};

template <class Object>
class TernarySearchTree
{

public:

    /**
    * Class constructor
    */
    TernarySearchTree();

    /**
    * Class destructor
    */
    ~TernarySearchTree();

    /**
    * Build balanced tree by binary inserting item of a sorted item list.
    *
    * @param newItemVector - vectors that holds all item which is pair of key & value
    * Note: current TST tree will be cleared before build balanced tree.
    *
    */
    void buildBalancedTree(std::vector< TstItem<Object> > & newItemVector);

    /**
    * Determines whether the Ternary Search Tree contains a specific key.
    *
    * @param   key - The key to locate in the tree.
    * @return   true if the tree contains an element with the specified key; otherwise, false.
    */
    bool contains(const char * key) const;

    /**
    * get item with the specified key from the tree
    *
    * @param   key - The key to locate in the tree
    * @return   pointer to the item, NULL if key not found
    */
    inline TstItem<Object> * getItem(const char * key) const
    {
        int index = this->getItemIndex(key);
        return index == -1 ? NULL : this->itemVector[index];
    }

    /**
    * Gets item from the tree at specified position
    *
    * @param   index - The index of the item in the item std::vector
    * @return   pointer to the item, NULL if not found
    */
    inline TstItem<Object> * getItem(int index) const
    {
        D(assert(index >= 0 && index < this->itemCount);)
            return this->itemVector[index];
    }

    /**
    * Gets item std::vector
    * @return item std::vector
    */
    inline std::vector< TstItem<Object> * > & getItems()
    {
        return this->itemVector;
    }

    /**
    * Get key from the tree
    *
    * @param   index - The index of the item in the key std::vector.
    * @return   The key of the item with specified index, NULL if not found
    */
    inline const char * getKey(int index) const
    {
        D(assert(index >= 0 && index < this->itemCount);)
            return this->itemVector[index]->key.c_str();
    }

    /**
    * Gets value with the specified key from the tree
    *
    * @param   key - The key to locate in the tree
    * @return   pointer to the value, NULL if key not found
    */
    inline Object * getValue(const char * key) const
    {
        int index = this->getItemIndex(key);
        return index == -1 ? NULL : &(this->itemVector[index]->value);
    }

    /**
    * Gets value from the tree
    *
    * @param   index - The index of the value in the value std::vector
    * @return   pointer to the value, NULL if not found
    */
    Object * getValue(int index) const
    {
        D(assert(index >= 0 && index < (int)this->itemCount);)
            return &this->itemVector[index]->value;
    }

    /**
    * Search to find the index of the specified key in the key std::vector
    * inline to improve search performance.
    *
    * @param   key - key to be search in the ternary search tree
    * @return   index of the key in key std::vector. If key is not found, return -1
    */
    int getItemIndex(const char * key) const
    {
        TstTree tst = this->root;

        while (tst)
        {
            if (*key == tst->splitChar)
            {
                if (*key)
                {
                    ++key;
                    tst = tst->mid;
                }
                else
                { // match to the end, found the key
                    return tst->index; // get the index of the key
                }
            }
            else if (*key < tst->splitChar)
            {
                tst = tst->left;
            }
            else
            {
                tst = tst->right;
            }
        }
        // if index -1, that means the search has run off the end of the tree, the key not found
        return -1;
    }

    /**
    * This method executes a partial-match searching.
    * .o.o.o matches the single word rococo, while the pattern
    * .a.a.a matches many words, including banana, casaba, and pajama.
    * Tal* matches all word with prefix Tal
    * @param   key - pattern for the searching
    * @return   an index std::vector for all returned keys
    */
    std::vector<int> partialMatchSearch(const char * key);

    /**
    * Search near neighbors that are withing a given Hamming distance of the key.
    *
    * @param   key   - key to be searched
    * @param   distance - Hamming distance for the search.
    * @return   an index std::vector for all matching keys
    *
    * @example search for jerry with distance 1 will return berry, ferry, gerry and etc.
    *
    */
    std::vector<int> nearSearch(const char * key, int distance)
    {
        std::vector<int> nearVector;
        this->nearVectorPtr = &nearVector;
        this->nearSearch(this->root, key, distance);
        return nearVector;
    }

    /**
    * This method return all keys that has the given prefix.
    *
    * @param   prefix - prefix to search keys
    * @return   an index std::vector for all returned keys
    *
    * Note: character '?' will match any char,
    * '*' will match any char(s), which can only be used as last char in the pattern for current implementation.
    */
    std::vector<int> prefixSearch(const char * prefix)
    {
        //std::string str( prefix );
        //str.append('*');
        return this->partialMatchSearch(std::string(prefix).append('*').c_str());
    }

    /**
    * print the strings in the tree in sorted order with a recursive traversal
    */
    std::vector<int> getSortedItemIndexes();


    /**
    * Adds an element with the specified key and value into the ternary search tree
    *
    * @param key - key of the element to be inserted into the tree
    * @value value of the element to be inserted into the tree
    */
    TstNode * add(const char * key, const Object & value);

    /**
    *Gets total number of key & value pair in the tree
    */
    int count() const
    {
        return this->itemCount;
    }

    /**
    * Cleans up the tree, nodes and stored values will all released
    */
    void clear()
    {
#ifdef TST_INFO_ENABLE
        this->nodeCount = 0;
#endif
        // clean up the tree
        this->cleanup(root);
        // clean up key, value vectors and reset variables.
        /*keyVector.clear();
        valueVector.clear();
        */
        /* release memory of items */
        for (int i = 0; i < this->itemCount; i++)
        {
            delete this->itemVector[i];
        }
        this->itemVector.clear();
        this->root = NULL;
        this->itemCount = 0;
        this->existingItemIndex = -1;
#ifdef TST_INFO_ENABLE
        fprintf(stderr, "total %d node in the TST tree, node size %d, total %d bytes.\n", nodeCount, 13, nodeCount * 13);
        fprintf(stderr, "total %d bytes for strings.\n", strLenCount);
#endif
    }

private:

    /**
    * Adds a key into the ternary search tree
    *
    * @param key - key to be inserted
    * @return the leaf node of the key( node with splitChar == 0 )
    */
    TstNode * add(const char* key);

#ifdef TST_INFO_ENABLE
    int nodeCount, strLenCount;
#endif

    /**
    * Cleans up nodes in the tree. inline to improve performance
    *
    * @param tst   root of the tree
    */
    void cleanup(TstTree tst)
    {
        if (tst)
        {
#ifdef TST_INFO_ENABLE
            ++this->nodeCount;
#endif
            this->cleanup(tst->left);

            if (tst->splitChar)
            {
                this->cleanup(tst->mid);
            }

            this->cleanup(tst->right);
            delete(tst);
        }
    }

    /**
    * Recursively search a pattern
    * ?o?o?o matches the single word rococo, while the pattern
    * ?a?a?a matches many words, including Canada, banana and casaba.
    * Tal* matches all word with prefix Tal
    *
    * @param   tree - root of the tree to be searched
    * @param   key - patterns for the search
    */
    void partialMatchSearch(TstTree tree, const char * key);

    /**
    * Recursively search near neighbors that are withing a given Hamming distance of the key.
    *
    * @param   tree -   root of the tree to be searched
    * @param   key   - key to be searched
    * @param   distance - Hamming distance for the search.
    *
    * @example search for jerry with distance 1 will return berry, ferry, gerry and etc.
    *
    */
    void nearSearch(TstTree tree, const char * key, int distance);

    /**
    * Recursively build balanced tree by binary inserting item of a sorted item list
    * from specified start to end position
    *
    * @param   newItemVector - vectors that holds all item which is pair of key & value
    * @param   start - start position of the std::vector
    * @param   end - end position of the std::vector
    * Note: current TST tree will be cleared before build balanced tree.
    *
    */
    void buildBalancedTreeRecursive(std::vector< TstItem<Object> > & newItemVector, int start, int end);

    /**
    * Return a list of items sorted by key, by travering the given tst tree recursively
    * @param tst - root of the ternary search tree
    */
    void getSortedItemIndexes(TstTree tst);

    /*std::vector<std::string> keyVector; // std::vector to track all inserted keys.

    std::vector<Object> valueVector; // std::vector to track all inserted objects.
    */

    std::vector< TstItem<Object> * > itemVector; /* std::vector to track of inserted items */

    std::vector<int> * sortedItemIndexVectorPtr;   // pointer to the std::vector of sorted items, used for recursive traverse

    std::vector<int> * pmVectorPtr;   // pointer to the std::vector of partial matched items, used for recursive matching

    std::vector<int> * nearVectorPtr; // pointer to the std::vector of near neighbor items, used for recursive searching.

    TstTree root;

    int itemCount;   // total number of items in the tree

    int existingItemIndex; // when inserting, if item already existed, it will be set the index of the existing item. If no existed, set to -1

};

template <class Object>
TernarySearchTree<Object>::TernarySearchTree() :
sortedItemIndexVectorPtr(0), pmVectorPtr(0), nearVectorPtr(0), root(0), itemCount(0), existingItemIndex(-1)
{
#ifdef TST_INFO_ENABLE
    this->strLenCount = 0;
#endif
}

template <class Object>
TernarySearchTree<Object>::~TernarySearchTree()
{
    this->clear();
}

template <class Object>
TstNode * TernarySearchTree<Object>::add(const char* key, const Object & value)
{
#ifdef TST_INFO_ENABLE
    this->strLenCount += sizeof(std::string(key)) + (int)strlen(key) + 1;
#endif
    TstNode * tstNode = this->add(key);
    if (tstNode)
    {
        if (this->existingItemIndex == -1)
        {   // key is not existed in tst tree
            this->itemVector.push_back(new TstItem<Object>(key, value));
            tstNode->index = this->itemCount - 1;
        }
        else
        {
            // if key alreay existed in the tree, replace its value with new value
            this->itemVector[this->existingItemIndex]->value = value;
            tstNode->index = this->existingItemIndex;

        }
    }
    return tstNode;
}

template <class Object>
TstNode* TernarySearchTree<Object>::add(const char* key)
{
    if (key == NULL || *key == 0)
        return NULL;

    //cout<<"Inserting "<<key<<endl;
    TstTree tst = this->root, parent = 0;

    while (tst)
    {
        parent = tst;
        if (*key < tst->splitChar)
        {
            tst = tst->left;
        }
        else if (*key == tst->splitChar)
        {
            // return true, if the current character is the end-of-std::string character 0
            if (*key == 0)
            {
                this->existingItemIndex = tst->index;
                break;
            }
            tst = tst->mid;
            ++key;
        }
        else
        {
            tst = tst->right;
        }
    }


    if (!tst) // key not found
    {
        this->existingItemIndex = -1;
        tst = new TstNode(*key);
        //cout<<"char "<<p->splitChar<<endl;
        if (parent)
        {
            if (*key == parent->splitChar)
            {
                parent->mid = tst;
            }
            else if (*key < parent->splitChar)
            {
                parent->left = tst;
            }
            else
            {
                parent->right = tst;
            }

        }
        if (!this->root)
        {
            this->root = tst;
        }
        while (tst->splitChar)
        {
            tst->mid = new TstNode(*++key);
            tst = tst->mid; // move to new node
        }

        ++this->itemCount;
    }

    return tst;
}

template <class Object>
bool TernarySearchTree<Object>::contains(const char * key) const
{
    return this->getItemIndex(key) != -1;
}

template <class Object>
std::vector<int> TernarySearchTree<Object>::getSortedItemIndexes()
{
    std::vector<int> sortedItemIndexVector;
    this->sortedItemIndexVectorPtr = &sortedItemIndexVector;
    this->getSortedItemIndexes(this->root);
    return sortedItemIndexVector;
}

template <class Object>
void TernarySearchTree<Object>::getSortedItemIndexes(TstTree tst)
{
    if (tst)
    {
        this->getSortedItemIndexes(tst->left);
        if (tst->splitChar)
        {
            this->getSortedItemIndexes(tst->mid);
        }
        else
        {
            this->sortedItemIndexVectorPtr->add(tst->index);
        }
        this->getSortedItemIndexes(tst->right);
    }
}

template <class Object>
std::vector<int> TernarySearchTree<Object>::partialMatchSearch(const char * key)
{
    std::vector<int> pmVector;
    this->pmVectorPtr = &pmVector;
    this->partialMatchSearch(root, (char*)key);
    return pmVector;
}

template <class Object>
void TernarySearchTree<Object>::partialMatchSearch(TstTree tree, const char * key)
{
    if (tree && key)
    {
        // partial match left
        if (*key == '?' || *key == '*' || *key < tree->splitChar)
        {
            this->partialMatchSearch(tree->left, key);
        }
        // partial match middle
        if (*key == '?' || *key == '*' || *key == tree->splitChar)
        {
            if (tree->splitChar && *key)
            {
                if (*key == '*')
                {
                    this->partialMatchSearch(tree->mid, key);
                }
                else
                {
                    this->partialMatchSearch(tree->mid, key + 1);   // search next pattern char
                }
            }
        }
        if ((*key == 0 || *key == '*') && tree->splitChar == 0)
        {
            this->pmVectorPtr->push_back(tree->index);
        }

        if (*key == '?' || *key == '*' || *key > tree->splitChar)
        {
            this->partialMatchSearch(tree->right, key);
        }
    }
}

template <class Object>
void TernarySearchTree<Object>::nearSearch(TstTree tree, const char * key, int distance)
{
    if (!tree || distance < 0)
    {
        return;
    }

    if (distance > 0 || *key < tree->splitChar)
    {
        this->nearSearch(tree->left, key, distance);
    }

    if (tree->splitChar == 0)
    {
        if ((int)strlen(key) <= distance)
        {
            this->nearVectorPtr->add(tree->index);   // found the matched key, added it to index std::vector
        }
    }
    else
    {
        this->nearSearch(tree->mid, *key ? key + 1 : key, (*key == tree->splitChar) ? distance : distance - 1);
    }

    if (distance > 0 || *key > tree->splitChar)
    {
        this->nearSearch(tree->right, key, distance);
    }
}

template <class Object>
void TernarySearchTree<Object>::buildBalancedTree(std::vector< TstItem<Object> > & newItemVector)
{
    if (!newItemVector.empty())
    {
        this->clear();
        // sort the items by keys, and binary insert, then we will get a balanced tree
        std::sort(newItemVector.begin(), newItemVector.end());
        this->buildBalancedTreeRecursive(newItemVector, 0, (int)newItemVector.size() - 1);
    }
}

template <class Object>
void TernarySearchTree<Object>::buildBalancedTreeRecursive(std::vector< TstItem<Object> > & newItemVector, int start, int end)
{
    if (start > end || end < 0)
    {
        return;
    }
    int mid = (end - start + 1) / 2;
    add(newItemVector[start + mid].key.c_str(), newItemVector[start + mid].value);
    this->buildBalancedTreeRecursive(newItemVector, start, start + mid - 1);
    this->buildBalancedTreeRecursive(newItemVector, start + mid + 1, end);
}

#endif
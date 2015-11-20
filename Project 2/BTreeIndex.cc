/*
 * Copyright (C) 2008 by The Regents of the University of California
 * Redistribution of this file is permitted under the terms of the GNU
 * Public License (GPL).
 *
 * @author Junghoo "John" Cho <cho AT cs.ucla.edu>
 * @date 3/24/2008
 */
#include <iostream>
#include <fstream>
#include <cstring>
#include <math.h>
#include "BTreeIndex.h"
#include "BTreeNode.h"

using namespace std;

/*
 * BTreeIndex constructor
 */
BTreeIndex::BTreeIndex()
{
    rootPid = -1;
}

/*
 * Open the index file in read or write mode.
 * Under 'w' mode, the index file should be created if it does not exist.
 * @param indexname[IN] the name of the index file
 * @param mode[IN] 'r' for read, 'w' for write
 * @return error code. 0 if no error
 */
RC BTreeIndex::open(const string& indexname, char mode)
{	RC rc;
	char buffer[PageFile::PAGE_SIZE];
	if(rc = pf.open(indexname,mode) < 0){
		return rc;
	}
	//Fetching the first page of the index file to read in the Root Pid and the Tree Height.
	pf.read(0,buffer);
	char *p = buffer;
	if(p[0]) {
		//Index has already been created so read the rootPid and tree height into the BTreeIndex
		memcpy(&rootPid,p,BTLeafNode::PAGE_ID_SIZE);
		memcpy(&treeHeight,p+BTLeafNode::PAGE_ID_SIZE,sizeof(int));
	} else {
		//Index is being created for the first time
		BTreeIndex();
	}

    return 0;
}

/*
 * Close the index file.
 * @return error code. 0 if no error
 */
RC BTreeIndex::close()
{	RC rc;
	char page[PageFile::PAGE_SIZE];
	memcpy(page, &rootPid, BTNonLeafNode::PAGE_ID_SIZE);
	memcpy(page+BTNonLeafNode::PAGE_ID_SIZE, &treeHeight, sizeof(int));
	pf.write(0, page);
	if(rc = pf.close() < 0){
		return rc;
	}
    return 0;
}

/*
 * Insert (key, RecordId) pair to the index.
 * @param key[IN] the key for the value inserted into the index
 * @param rid[IN] the RecordId for the record being inserted into the index
 * @return error code. 0 if no error
 */
RC BTreeIndex::insert(int key, const RecordId& rid)
{	RC rc;
	BTNonLeafNode root;
	IndexCursor c;
	BTLeafNode leaf;

	rc = locate(key, c);
	if(rc == RC_END_OF_TREE) {
		//First Key being inserted ever
		//Create a new leaf node and insert value.Set this Node's pid as the rootPid
		PageId root_pid = pf.endPid();
		leaf.read(root_pid, pf);
		leaf.insert(key, rid);
		leaf.write(root_pid, pf);
		rootPid = root_pid;
		treeHeight = 1;
		return 0;
	} else {
		//If only leaf root exists yet
		if(treeHeight == 1) {
			leaf.read(rootPid, pf);
			if(rc = leaf.insert(key, rid) < 0) {
				BTLeafNode sibling;
				int siblingKey;
				PageId siblingPid = pf.endPid();
				sibling.read(siblingPid, pf);

				leaf.insertAndSplit(key,rid, sibling, siblingKey);
				leaf.write(rootPid, pf);

				sibling.setNextNodePtr(leaf.getNextNodePtr());
				leaf.setNextNodePtr(siblingPid);
				sibling.write(siblingPid, pf);

				BTNonLeafNode root;
				PageId new_root_pid = pf.endPid();
				root.read(new_root_pid, pf);
				root.initializeRoot(rootPid, key, siblingPid);
				root.write(new_root_pid, pf);
				rootPid = new_root_pid;
				treeHeight = treeHeight + 1;
				return 0;
			} else {
				leaf.write(rootPid, pf);
				return 0;
			}
		} else {
				//More than 1 level exists
				root.read(rootPid, pf);
				locate(key, c);
				int siblingKey;
				int insertHt = -1;
				PageId lowerPid;
				leaf.read(c.pid, pf);
				if(rc = leaf.insert(key, rid) < 0) {
					recursiveInsert(key, rid, rootPid, 1, insertHt, siblingKey, lowerPid);
				} else {
					// Else insert was successful in leaf
					leaf.write(c.pid, pf);
					return 0;
				}
			}

		}
		return 0;
}

RC BTreeIndex::recursiveInsert(int key, const RecordId& rid, PageId root_Pid, int currHeight, int& insertHeight, int& siblingKey, PageId& lowerPid) {
	PageId pid;

	if(insertHeight == -2) return 0;

	if(insertHeight == 0){
		//Creating a new root thus initializeRoot,setRootPid and increase tree height by 1
		BTNonLeafNode root;
		PageId new_root_pid = pf.endPid();
		root.initializeRoot(lowerPid, siblingKey, new_root_pid -1);
		root.write(new_root_pid, pf);
		rootPid = new_root_pid;
		treeHeight = treeHeight + 1;
		return 0; 
	}

	if(insertHeight == -1) {
		if(currHeight < treeHeight - 1) {
			BTNonLeafNode root;
			root.read(root_Pid,pf);
			root.locateChildPtr(key, pid);
			BTNonLeafNode n;
			n.read(pid, pf);
			recursiveInsert(key, rid, pid, currHeight + 1, insertHeight, siblingKey, lowerPid);
		} else {
			BTNonLeafNode root;
			root.read(root_Pid,pf);
			root.locateChildPtr(key, pid);
			BTLeafNode leaf;
			BTLeafNode sibling;
			int siblingKey;
			leaf.read(pid, pf);
			sibling.read(pf.endPid(), pf);

			sibling.setNextNodePtr(leaf.getNextNodePtr());
			leaf.setNextNodePtr(pf.endPid());


			leaf.insertAndSplit(key, rid, sibling, siblingKey);
			leaf.write(pid, pf);

			sibling.write(pf.endPid(), pf);

			if(root.insert(key, pf.endPid()) < 0) {
				BTNonLeafNode sibling;
				sibling.read(pf.endPid(),pf);
				root.insertAndSplit(key, pf.endPid(), sibling, siblingKey);
				root.write(root_Pid, pf);
				sibling.write(pf.endPid(), pf);

				insertHeight = currHeight - 1;
				lowerPid = pf.endPid();
			} else {
				insertHeight = -2;
				return 0;
			}
		}

	} else if(insertHeight > 0){
		if(currHeight == insertHeight) {
			BTNonLeafNode root;
			root.read(root_Pid, pf);
			if(root.insert(key, pf.endPid()) < 0) {
				BTNonLeafNode sibling;
				sibling.read(pf.endPid(), pf);
				root.insertAndSplit(key, pf.endPid(), sibling, siblingKey);
				root.write(root_Pid, pf);
				sibling.write(pf.endPid(), pf);
				insertHeight = currHeight - 1;
				if(insertHeight == 0){
					lowerPid = root_Pid;
				} else {
					lowerPid = pf.endPid();
				}	
			}
		}
		return 0;
	}

}


/**
 * Run the standard B+Tree key search algorithm and identify the
 * leaf node where searchKey may exist. If an index entry with
 * searchKey exists in the leaf node, set IndexCursor to its location
 * (i.e., IndexCursor.pid = PageId of the leaf node, and
 * IndexCursor.eid = the searchKey index entry number.) and return 0.
 * If not, set IndexCursor.pid = PageId of the leaf node and
 * IndexCursor.eid = the index entry immediately after the largest
 * index key that is smaller than searchKey, and return the error
 * code RC_NO_SUCH_RECORD.
 * Using the returned "IndexCursor", you will have to call readForward()
 * to retrieve the actual (key, rid) pair from the index.
 * @param key[IN] the key to find
 * @param cursor[OUT] the cursor pointing to the index entry with
 *                    searchKey or immediately behind the largest key
 *                    smaller than searchKey.
 * @return 0 if searchKey is found. Othewise an error code
 */
RC BTreeIndex::locate(int searchKey, IndexCursor& cursor)
{	RC rc;
	int eid = 0;
	BTLeafNode leaf;
	if(rootPid == -1){
		return RC_END_OF_TREE;
	}
	if(treeHeight == 1){
		//Only the leaf root exists
		leaf.read(rootPid, pf);
		if(rc = leaf.locate(searchKey, eid) < 0){
			cursor.pid = rootPid;
			cursor.eid = eid;
			return rc;
		} else {
			cursor.pid = rootPid;
			cursor.eid = eid;
			return 0;
		}
	}

	BTNonLeafNode root;
	root.read(rootPid, pf);
	if(root.getKeyCount() <= 0){
		return RC_END_OF_TREE;
	}
	PageId pid = search(searchKey, root, 1);
	leaf.read(pid, pf);
	cursor.pid = pid;

	if(rc = leaf.locate(searchKey, eid) < 0){
		cursor.eid = eid;
		return rc;
	}

	cursor.eid = eid;
    return 0;
}

PageId BTreeIndex::search(int searchKey, BTNonLeafNode current, int current_level)
{ 	PageId pid;
	BTLeafNode leaf;
	current.locateChildPtr(searchKey, pid);

	if(current_level ==  treeHeight - 1) {
		return pid;
  	} else {
  		BTNonLeafNode non_leaf;
  		non_leaf.read(pid, pf);
  		search(searchKey, non_leaf, current_level+1);
  	}
  	return pid;
}

/*
 * Read the (key, rid) pair at the location specified by the index cursor,
 * and move foward the cursor to the next entry.
 * @param cursor[IN/OUT] the cursor pointing to an leaf-node index entry in the b+tree
 * @param key[OUT] the key stored at the index cursor location.
 * @param rid[OUT] the RecordId stored at the index cursor location.
 * @return error code. 0 if no error
 */
RC BTreeIndex::readForward(IndexCursor& cursor, int& key, RecordId& rid)
{	RC rc;
	char page[PageFile::PAGE_SIZE];
	PageId pid = cursor.pid;
	int eid = cursor.eid;

	if(rc = pf.read(pid, page) < 0){
		return rc;
	}

	char *p = page;
	memcpy(&rid, p+((eid-1)*BTLeafNode::LEAF_ENTRY_SIZE), BTLeafNode::RECORD_ID_SIZE);
	memcpy(&key, p+((eid-1)*BTLeafNode::LEAF_ENTRY_SIZE)+BTLeafNode::RECORD_ID_SIZE, BTLeafNode::KEY_SIZE);
    return 0;
}

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
	treeHeight = 0;
	//std::fill(, end, 0);
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
		memcpy(&treeHeight,p+BTLeafNode::PAGE_ID_SIZE, sizeof(int));
	} else {
		//Index is being created for the first time
	rootPid = -1;
	treeHeight = 0;		
	memcpy(p, &rootPid, BTNonLeafNode::PAGE_ID_SIZE);
	memcpy(p+BTNonLeafNode::PAGE_ID_SIZE, &treeHeight, sizeof(int));
	pf.write(0, p);

	}
	fprintf(stdout, "OPENED INDEX FILE \n");

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
	//fprintf(stdout, " LOCATED ENTRY AT %d %d \n", c.pid, c.eid);
	if(rc == RC_END_OF_TREE) {
		fprintf(stdout, "REACHED END OF TREE\n");
		//First Key being inserted ever
		//Create a new leaf node and insert value.Set this Node's pid as the rootPid
		fprintf(stdout, "Creating new root : %d\n", rc);
		PageId root_pid = pf.endPid();
		leaf.read(root_pid, pf);
		fprintf(stdout, "NEw ROOT PID IS : %d\n", root_pid);
		leaf.insert(key, rid);
		fprintf(stdout, "INSERTED IN 1st Leaf/root  : %d\n", root_pid);

		leaf.write(root_pid, pf);
		leaf.print();
		rootPid = root_pid;
		treeHeight = 1;
		fprintf(stdout, "AC : %d\n", treeHeight);
		return 0;
	} else {
		//If only leaf root exists yet
		if(treeHeight == 1) {
			//fprintf(stdout, "AD : %d\n", pid);
			fprintf(stdout, "INSERTING IN TREE_HEIGHT = 1\n");
			leaf.read(rootPid, pf);
			fprintf(stdout, "TRYING TO INSERT IN LEAF AT PAGE ID:%d \n", rootPid);
			if(rc = leaf.insert(key, rid) < 0) {
				BTLeafNode sibling;
				int siblingKey;
				PageId siblingPid = pf.endPid();
				fprintf(stdout, "Newly created Sibling Pid is : %d\n", siblingPid);
				sibling.read(siblingPid, pf);

				leaf.insertAndSplit(key, rid, sibling, siblingKey);
				leaf.write(rootPid, pf);
				leaf.print();
				fprintf(stdout, "AF : %d\n", rootPid);
				sibling.setNextNodePtr(leaf.getNextNodePtr());
				leaf.setNextNodePtr(siblingPid);
				sibling.write(siblingPid, pf);
				fprintf(stdout, "BB : %d\n", siblingPid);
				PageId new_root_pid = pf.endPid();
				root.read(new_root_pid, pf);
				root.initializeRoot(rootPid, key, siblingPid);
				fprintf(stdout, "CC : %d\n", siblingPid);
				root.write(new_root_pid, pf);
				root.print();
				rootPid = new_root_pid;
				treeHeight = treeHeight + 1;
				fprintf(stdout, "DD : %d\n", treeHeight);
				return 0;
			} else {
				leaf.write(rootPid, pf);
				fprintf(stdout, "EE : %d\n", rootPid);
				leaf.print();
				return 0;
			}
		} else {
				//More than 1 level exists
				root.read(rootPid, pf);
				locate(key, c);
				int siblingKey;
				PageId lowerPid;
				int insertHt = -1;
				leaf.read(c.pid, pf);
				fprintf(stdout, "FF : %d\n", c.pid);
				if(rc = leaf.insert(key, rid) < 0) {
					recursiveInsert(key, rid, rootPid, 1, insertHt, siblingKey, lowerPid);
					fprintf(stdout, "GG : %d\n", key);
				} else {
					// Else insert was successful in leaf
					leaf.write(c.pid, pf);
					//fprintf(stdout, "HH : %d\n", pid);
					return 0;
				}
			}

		}
		return 0;
}

RC BTreeIndex::recursiveInsert(int key, const RecordId& rid, PageId root_Pid, int currHeight, int& insertHeight, int& siblingKey, PageId& lowerPid) {
	PageId pid;
	if(insertHeight == -2) {
		fprintf(stdout, "InsertHeight is= %d\n", insertHeight);
		return 0;
	}
	if(insertHeight == -1) {
		fprintf(stdout, "A\n");
		if(currHeight < treeHeight - 1) {
			fprintf(stdout, "B\n");
			BTNonLeafNode root;
			root.read(root_Pid,pf);
			fprintf(stdout, "C : %d\n",root_Pid);
			root.locateChildPtr(key, pid);
			fprintf(stdout, "D : %d\n",pid);
			BTNonLeafNode n;
			fprintf(stdout, "E\n");
			n.read(pid, pf);
			fprintf(stdout, "F\n");
			recursiveInsert(key, rid, pid, currHeight + 1, insertHeight, siblingKey, lowerPid);
			fprintf(stdout, "G\n");
		} else {
			BTNonLeafNode root;
			root.read(root_Pid,pf);
			root.locateChildPtr(key, pid);
			fprintf(stdout, "H : %d\n", pid);
			BTLeafNode leaf;
			BTLeafNode sibling;
			int siblingKey;
			PageId siblingPid = pf.endPid();
			fprintf(stdout, "I : %d\n", siblingPid);
			leaf.read(pid, pf);
			sibling.read(siblingPid, pf);

			sibling.setNextNodePtr(leaf.getNextNodePtr());
			fprintf(stdout, "J : %d\n", leaf.getNextNodePtr());
			leaf.setNextNodePtr(siblingPid);
			fprintf(stdout, "K : %d\n", siblingPid);

			leaf.insertAndSplit(key, rid, sibling, siblingKey);
			leaf.write(pid, pf);
			fprintf(stdout, "L : %d\n", pid);
			sibling.write(siblingPid, pf);

			if(root.insert(siblingKey, siblingPid) < 0) {
				BTNonLeafNode new_sibling;
				int new_sibling_key;
				new_sibling.read(pf.endPid(),pf);
				root.insertAndSplit(siblingKey, pf.endPid(), new_sibling, new_sibling_key);
				root.write(root_Pid, pf);
				fprintf(stdout, "M : %d\n", root_Pid);
				lowerPid = pf.endPid();
				new_sibling.write(pf.endPid(), pf);
				fprintf(stdout, "N : %d\n", pf.endPid());
				insertHeight = currHeight - 1;
				fprintf(stdout, "O : %d\n", insertHeight);
				return 0;
			} else {
				root.write(root_Pid, pf);
				fprintf(stdout, "P : %d\n", root_Pid);
				insertHeight = -2;
				fprintf(stdout, "Q : %d\n", insertHeight);
				return 0;
			}
		}

	} else if(insertHeight > 0){
		fprintf(stdout, "R : %d\n", insertHeight);
		if(currHeight == insertHeight) {
			BTNonLeafNode root;
			root.read(root_Pid, pf);
			fprintf(stdout, "S : %d\n", root_Pid);
			if(root.insert(siblingKey, lowerPid) < 0) {
				BTNonLeafNode new_sibling;
				fprintf(stdout, "T : %d\n", siblingKey);
				PageId new_sibling_Pid = pf.endPid();
				int new_sibling_key;
				new_sibling.read(new_sibling_Pid, pf);
				fprintf(stdout, "U : %d\n", new_sibling_Pid);
				root.insertAndSplit(siblingKey, lowerPid, new_sibling, new_sibling_key);
				root.write(root_Pid, pf);
				fprintf(stdout, "V : %d\n", root_Pid);
				new_sibling.write(new_sibling_Pid, pf);
				insertHeight = currHeight - 1;
				if(insertHeight == 0){
					//Creating a new root thus initializeRoot,setRootPid and increase tree height by 1
					fprintf(stdout, "W : %d\n", insertHeight);
					BTNonLeafNode new_root;
					PageId new_root_pid = pf.endPid();
					new_root.initializeRoot(root_Pid, siblingKey, new_sibling_Pid);
					fprintf(stdout, "X : %d\n", root_Pid);
					fprintf(stdout, "X : %d\n", new_sibling_Pid);
					new_root.write(new_root_pid, pf);
					rootPid = new_root_pid;
					treeHeight = treeHeight + 1;
					fprintf(stdout, "Y : %d\n", treeHeight);
					return 0;
					} 
				lowerPid = new_sibling_Pid;
				siblingKey = new_sibling_key;
				fprintf(stdout, "Z : %d\n", lowerPid);
				fprintf(stdout, "Z : %d\n", siblingKey);
			}
		}
		return 0;
	}
	return 0;
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
{
    RC rc;
	int eid = 0;
	BTLeafNode leaf;
	if(rootPid == -1){
		return RC_END_OF_TREE;
	}
	if(treeHeight == 1){
		//Only the leaf root exists
		//fprintf(stdout, "LOCATING IN TREE WITH HT=1 and ROOT AT PAGE: %d\n",rootPid);
		leaf.read(rootPid, pf);
		leaf.print();
		if(rc = leaf.locate(searchKey, eid) < 0){
			cursor.pid = rootPid;
			cursor.eid = eid;
			//fprintf(stdout, "ENTRY NOT FOUND BUT SHOULD BE AT %d:%d\n",cursor.pid,cursor.eid);
			return rc;
		} else {
			cursor.pid = rootPid;
			cursor.eid = eid;
			//fprintf(stdout, "ENTRY FOUND AT %d:%d\n",cursor.pid,cursor.eid);
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
{
	RC rc;

	BTLeafNode leaf;
	//Checking if cursor has a value to point to
	if(cursor.pid == -1) {
		return RC_END_OF_TREE;
	}
	rc = leaf.read(cursor.pid, pf);
	fprintf(stdout, " READING FROM INDEX CURSOR: %d %d \n",cursor.pid,cursor.eid);
	
	if(rc!=0)
		return rc;
	
	rc = leaf.readEntry(cursor.eid, key, rid);
	
	//if(rc!=0)
	//	return rc;
		
	if(cursor.eid == leaf.getKeyCount()) {
		//At last key of leaf
		if(leaf.getNextNodePtr() == -1){
			//Next leaf node does not exist,reached end of index tree.
			cursor.pid = -1;
			//fprintf(stdout, "Reached End of Tree while reading forward..");
			//return RC_END_OF_TREE;
		} else {
			//Sibling node exists
			cursor.eid = 1;
			cursor.pid = leaf.getNextNodePtr();
			fprintf(stdout, "Read forward from sibling on page: %d", leaf.getNextNodePtr());

		}	
	}
	else{
		fprintf(stdout, "%s: %d\n","Reading forward from ",cursor.eid);
		cursor.eid++;
	}	
	return 0;
}

PageId BTreeIndex::getRootPid()
{
	return rootPid;
}

int BTreeIndex::getTreeHeight()
{
	return treeHeight;
}

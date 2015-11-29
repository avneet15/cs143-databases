/**
 * Copyright (C) 2008 by The Regents of the University of California
 * Redistribution of this file is permitted under the terms of the GNU
 * Public License (GPL).
 *
 * @author Junghoo "John" Cho <cho AT cs.ucla.edu>
 * @date 3/24/2008
 */

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <limits.h>
#include <iostream>
#include <fstream>
#include "Bruinbase.h"
#include "SqlEngine.h"
#include "BTreeIndex.h"

using namespace std;

// external functions and variables for load file and sql command parsing 
extern FILE* sqlin;
int sqlparse(void);
//bool isIndex = false;
//extern bool isIndex = false;


RC SqlEngine::run(FILE* commandline)
{
  fprintf(stdout, "Bruinbase> ");

  // set the command line input and start parsing user input
  sqlin = commandline;
  sqlparse();  // sqlparse() is defined in SqlParser.tab.c generated from
               // SqlParser.y by bison (bison is GNU equivalent of yacc)

  return 0;
}

RC SqlEngine::select(int attr, const string& table, const vector<SelCond>& cond)
{
  RecordFile rf;   // contains the table for sequential reads
  RecordId   rid;
  IndexCursor ic1;
  BTreeIndex tree;


  
  RC     rc;
  int    key;     
  string value;
  int    count = 0;
  int    diff;


  bool isIndex = false;
  if ((rc = rf.open(table + ".tbl", 'r')) < 0) {
  fprintf(stderr, "Error: table %s does not exist\n", table.c_str());
  return rc;
  }

  if((rc=tree.open(table+".idx",'r')) < 0) {
      isIndex = false;
  } else {
    isIndex = true;
  }

   SelCond sc;
    bool useIndex = false; //to check if any valid select conditions exist, used for closing tree file too
    bool shouldFetch = false; //to check if any 'value' conditions exist or 
    
    int max = INT_MAX;
    int min = INT_MIN;
    int equalVal = INT_MAX;
    //bool flags
    bool maxBool = false;
    bool minBool = false;
    bool equalValBool = false;
    
  if(isIndex){
    //check if any value conditions are conflicting
    bool valueConflict = false;
    std::string valEq = "";
    //For select count(*) from movie or select key from movie, useIndex should be true
    if((attr == 4 || attr == 1) && cond.size() == 0){
      useIndex = true;
      //shouldFetch = false;
    }
    for(int i=0; i<cond.size(); i++) {
      sc = cond[i];
      int tempVal = atoi(sc.value); //store the int-form value of the select condition
       //First check if Index exists

      //we only have to worry about conditions on keys that don't involve NE
      //all other select conditions will be considered outside of the B+ Tree
      if(sc.attr==1 && sc.comp!=SelCond::NE) {
        useIndex = true; //if we ever hit a valid condition, set this to true

        switch (cond[i].comp) {
          case SelCond::EQ:
            //fprintf(stdout, "IN EQUALS\n");
            equalVal = tempVal;
            equalValBool = true;
            break;
          
          case SelCond::GT:
            if(tempVal > min || !minBool) {//if the tempVal min is larger than or equal to our current min (or it's uninitialized), set GT
            //fprintf(stdout, "IN GT\n");

            //isCondGE = false;
            min = tempVal + 1;
            minBool = true;
            }
            break;

          case SelCond::LT:
            if(tempVal < max || !maxBool) //if the tempVal max is smaller than our current max (or it's uninitialized), set LE
            {
            //fprintf(stdout, "IN LT\n");
            //isCondLE = true;
            max = tempVal - 1;
            maxBool = true;
            }
            break;

          case SelCond::GE:
            if(tempVal >= min || !minBool) //if the tempVal min is larger than our current min (or it's uninitialized), set GE
            {
            //fprintf(stdout, "IN GE\n");
            //isCondGE = true;
            min = tempVal;
            minBool = true;
            }
            break;

          case SelCond::LE:
            if(tempVal <= max || !maxBool) //if the tempVal max is smaller than our current max (or it's uninitialized), set LE
            {
            //fprintf(stdout, "IN LE\n");
            //isCondLE = true;
            max = tempVal;
            maxBool = true;
            }
            break;
        }
      }

      if(attr == 2 || attr == 3 || sc.attr == 2) //if we hit a value condition, update shouldFetch and check for contradictions
      { 
        //fprintf(stdout, "FETCH RECORD REQD\n");
        shouldFetch = true;
      }
    }
      //fprintf(stdout, "%d %d %d\n",max,min,equalVal);
  }
  
  //if the index file does not exist, use normal select
  //similarly, unless we are interested in a count(*) without conditions, an empty condition array means we use normal select
  //we do this because using "select count(*) from table" could offer a speedup using the index file
  if(!isIndex|| (!useIndex))
  {
    // scan the table file from the beginning
    rid.pid = rid.sid = 0;
    count = 0;
    while (rid < rf.endRid()) {
    // read the tuple
    if ((rc = rf.read(rid, key, value)) < 0) {
      fprintf(stderr, "Error: while reading a tuple from table %s\n", table.c_str());
      goto faultyTuple;
    }

    // check the conditions on the tuple
    for (unsigned i = 0; i < cond.size(); i++) {
      // compute the difference between the tuple value and the condition value
      switch (cond[i].attr) {
      case 1:
    diff = key - atoi(cond[i].value);
    break;
      case 2:
    diff = strcmp(value.c_str(), cond[i].value);
    break;
      }

        // skip the tuple if any condition is not met
        switch (cond[i].comp) {
          case SelCond::EQ:
          if (diff != 0) goto next_tuple;
          break;
          case SelCond::NE:
          if (diff == 0) goto next_tuple;
          break;
          case SelCond::GT:
          if (diff <= 0) goto next_tuple;
          break;
          case SelCond::LT:
          if (diff >= 0) goto next_tuple;
          break;
          case SelCond::GE:
          if (diff < 0) goto next_tuple;
          break;
          case SelCond::LE:
          if (diff > 0) goto next_tuple;
          break;
        }
      }

    // the condition is met for the tuple. 
    // increase matching tuple counter
    count++;

      // print the tuple 
      switch (attr) {
        case 1:  // SELECT key
          fprintf(stdout, "%d\n", key);
          break;
        case 2:  // SELECT value
          fprintf(stdout, "%s\n", value.c_str());
          break;
        case 3:  // SELECT *
          fprintf(stdout, "%d '%s'\n", key, value.c_str());
          break;
      }

      // move to the next tuple
      next_tuple:
      ++rid;
    } //while ends
  }
  else //otherwise, USE INDEX...
  {
    //initialize variables (rid doesn't really matter here)
    count = 0;
    rid.pid = rid.sid = 0;
    useIndex = true; //set this in order to close index properly
    //fprintf(stdout, " IN INDEX\n");
    //set the starting position for IndexCursor ic1
    if(equalValBool) {
      //fprintf(stdout, "SEARCHING KEY WITH ==\n");//key must be equalVal
      tree.locate(equalVal, ic1);
     } 
    else if(minBool) {//Min is defined
      //fprintf(stdout, "SEARCHING KEY WITH >\n");
      rc = tree.locate(min, ic1);

      if(rc < 0){
        //Set index cursor to next position which is > current key which is lower than search key
        tree.readForward(ic1, key, rid);
      }  
     } 
    else{
      //fprintf(stdout, "SEARCHING KEY FROM BEGINNING \n");
      //tree.locate(0, ic1);
      //since we know first record is at pid=1 and eid=1, so it's hard coded here:
      ic1.pid = 1;
      ic1.eid = 1;
    } 
    
    while(tree.readForward(ic1, key, rid) == 0)
    {
       if(equalValBool && key!=equalVal)
            goto rangeExceeded;
          
          if(maxBool) //if there is a condition on LT or LE that fails, we are done
          {
            if(key>max)
              goto rangeExceeded;
            
          }
          
          if(minBool) //if there is a condition on GT or GE that fails, we are done
          {
            if(key < min)
              goto rangeExceeded;
          }

      if(!shouldFetch) //no need to read the records from disk
      {   //Continue with while condition       
          
          //if key passes all of the conditions, increment count and jump to next cycle in while loop
          //in doing this if-statement, we save many reads from record file
       
      } else {
    
            if ((rc = rf.read(rid, key, value)) < 0) {
              fprintf(stderr, "Error: while reading a tuple from table %s\n", table.c_str());
              goto faultyTuple;
            } else {
            }

              for (unsigned i = 0; i < cond.size(); i++)
              {
                  switch (cond[i].attr)
                  {
                    case 1:
                      diff = key - atoi(cond[i].value);
                      break;
                    case 2:
                      diff = strcmp(value.c_str(), cond[i].value);
                      break;
                  }

                      //skip the tuple if any condition is not met
                  //for less than (LT) or less than or equal (LE) conditions on keys, once one of them fails, we are done
                  //since the tree index is sorted, every key afterwards is also going to fail the condition
                  //the same applies with EQ, since once we're past the key that was equal, everything else will fail also
                  switch (cond[i].comp)
                  {
                    case SelCond::EQ:
                      if (diff != 0)
                      {
                        if(cond[i].attr==1)
                          goto rangeExceeded;
                        goto continue_while;
                      }
                      break;
                    case SelCond::NE:
                      if (diff == 0) goto continue_while; //if keys ever match when they're not supposed to, break out of for-loop and wait for next cursor
                      break;
                    case SelCond::GT:
                      if (diff <= 0) goto continue_while; //if !(key > cond value), break out of for-loop and wait for next cursor
                      break;
                    case SelCond::LT:
                      if (diff >= 0)
                      {
                        if(cond[i].attr==1) //if this ever fails on a key, everything else after will fail anyway, so we end
                          goto rangeExceeded;
                        goto continue_while;
                      }
                      break;
                    case SelCond::GE:
                      if (diff < 0) goto continue_while; //if !(key >= cond value), break out of for-loop and wait for next cursor
                      break;
                    case SelCond::LE:
                      if (diff > 0)
                      {
                        if(cond[i].attr==1) //if this ever fails on a key, everything else after will fail anyway, so we end
                          goto rangeExceeded;
                        goto continue_while;
                      }
                      break;
               }
          }

    }
      count++;
      // print the tuple 
      switch (attr)
      {
        case 1:  // SELECT key
          fprintf(stdout, "%d\n", key);
          break;
        case 2:  // SELECT value
          fprintf(stdout, "%s\n", value.c_str());
          break;
        case 3:  // SELECT *
          fprintf(stdout, "%d '%s'\n", key, value.c_str());
          break;
      }
      
      continue_while: ;
    } //while ends
  }

  
  //all tuples that were a part of output are read/printed
  rangeExceeded:

  //if "select count(*)"
  if (attr == 4) {
    fprintf(stdout, "%d\n", count);
  }
  rc = 0;

  // close the table file and return
  faultyTuple:
  
  if(isIndex) //close the index file if applicable
    tree.close();
  
  rf.close();
  return rc;
}


RC SqlEngine::load(const string& table, const string& loadfile, bool index)
{
  RecordFile rf;   
  RecordId   rid;  
  RC     rc;
  BTreeIndex tree;  
  
  string line; 
  int    key; 
  string value; 
  
  
  ifstream tableData(loadfile.c_str());
  //SqlEngine::isIndex = index;
  
  if(!tableData.is_open())
  fprintf(stderr, "Error: loadfile %s cannot be opened\n", loadfile.c_str());
  
  
  rc = rf.open(table + ".tbl", 'w');
  
 
  if(index)
  {

  tree.open(table + ".idx", 'w');
  int z=1;
    while(getline(tableData, line))
    {
      parseLoadLine(line, key, value);
      if(rf.append(key, value, rid)!=0)
        return RC_FILE_WRITE_FAILED;
      
      if(tree.insert(key, rid)!=0) {
        return RC_FILE_WRITE_FAILED;  
      } else {
        z++;
      }
    }

  tree.close();
  }
  else
  {

    while(getline(tableData, line))
    {
    parseLoadLine(line, key, value);
    rc = rf.append(key, value, rid);  
    }
  }
  
  rf.close();
  tableData.close();
  return rc;
}

RC SqlEngine::parseLoadLine(const string& line, int& key, string& value)
{
    const char *s;
    char        c;
    string::size_type loc;
    
    // ignore beginning white spaces
    c = *(s = line.c_str());
    while (c == ' ' || c == '\t') { c = *++s; }

    // get the integer key value
    key = atoi(s);

    // look for comma
    s = strchr(s, ',');
    if (s == NULL) { return RC_INVALID_FILE_FORMAT; }

    // ignore white spaces
    do { c = *++s; } while (c == ' ' || c == '\t');
    
    // if there is nothing left, set the value to empty string
    if (c == 0) { 
        value.erase();
        return 0;
    }

    // is the value field delimited by ' or "?
    if (c == '\'' || c == '"') {
        s++;
    } else {
        c = '\n';
    }

    // get the value string
    value.assign(s);
    loc = value.find(c, 0);
    if (loc != string::npos) { value.erase(loc); }

    return 0;
}

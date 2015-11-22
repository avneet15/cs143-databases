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
#include <iostream>
#include <fstream>
#include "Bruinbase.h"
#include "SqlEngine.h"
#include "BTreeIndex.h"
 #include "BTreeIndex.cc"

using namespace std;

// external functions and variables for load file and sql command parsing 
extern FILE* sqlin;
int sqlparse(void);


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
  
  if ((rc = rf.open(table + ".tbl", 'r')) < 0) {
  fprintf(stderr, "Error: table %s does not exist\n", table.c_str());
  return rc;
  }
  
  SelCond sc;
  bool hasCond = false; //to check if any valid select conditions exist, used for closing tree file too
  bool hasValCondOrValAttr = false; //to check if any 'value' conditions exist or 
  
  bool isCondGE = false; // (is condition greater than or equal to?)
  bool isCondLE = false; // (is condition greater than or equal to?) 
  int max = -1;
  int min = -1;
  int equalVal = -1;
  
  //keep track of the index of a vital select condition
  int condIndex = -1;
  
  //check if any value conditions are conflicting
  bool valueConflict = false;
  std::string valEq = "";
  
  for(int i=0; i<cond.size(); i++) {
    sc = cond[i];
    int tempVal = atoi(sc.value); //store the int-form value of the select condition
     
    //we only have to worry about conditions on keys that don't involve NE
    //all other select conditions will be considered outside of the B+ Tree
    if(sc.attr==1 && sc.comp!=SelCond::NE) {
      hasCond = true; //if we ever hit a valid condition, set this to true

      switch (cond[i].comp) {
        case SelCond::EQ:
          equalVal = tempVal;
          condIndex = i;
          break;
        
        case SelCond::GT:
          if(tempVal >= min || min==-1) {//if the tempVal min is larger than or equal to our current min (or it's uninitialized), set GT
          isCondGE = false;
          min = tempVal;
          }
          break;

        case SelCond::LT:
          if(tempVal < max || max==-1) //if the tempVal max is smaller than our current max (or it's uninitialized), set LE
          {
          isCondLE = true;
          max = tempVal;
          }
          break;

        case SelCond::GE:
          if(tempVal > min || min==-1) //if the tempVal min is larger than our current min (or it's uninitialized), set GE
          {
          isCondGE = true;
          min = tempVal;
          }
          break;

        case SelCond::LE:
          if(tempVal < max || max==-1) //if the tempVal max is smaller than our current max (or it's uninitialized), set LE
          {
          isCondLE = true;
          max = tempVal;
          }
          break;
      }
    }

    else if(sc.attr==2) //if we hit a value condition, update hasValCondOrValAttr and check for contradictions
    {
      hasValCondOrValAttr = true;
    }
  }
  
  //if the index file does not exist, use normal select
  //similarly, unless we are interested in a count(*) without conditions, an empty condition array means we use normal select
  //we do this because using "select count(*) from table" could offer a speedup using the index file
  if(tree.open(table + ".idx", 'r')!=0 || (!hasCond && attr!=4))
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
        switch (cond[i].attr)
        {
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
  else //otherwise, table's index file exists!
  {
    //initialize variables (rid doesn't really matter here)
    count = 0;
    rid.pid = rid.sid = 0;
    hasCond = true; //set this in order to close index properly
    
    //set the starting position for IndexCursor ic1
    if(equalVal!=-1) //key must be equalVal
      tree.locate(equalVal, ic1);
    else if(min!=-1 && !isCondGE) //key must be greater than min
      tree.locate(min+1, ic1);
    else if(min!=-1 && isCondGE) //key must be at least min
      tree.locate(min, ic1);
    else
      tree.locate(0, ic1);
    
    while(tree.readForward(ic1, key, rid)==0)
    {
      if(!hasValCondOrValAttr && attr==4) //no need to read the records from disk
      {
          if(equalVal!=-1 && key!=equalVal)
            goto rangeExceeded;
          
          if(max!=-1) //if there is a condition on LT or LE that fails, we are done
          {
            if(isCondLE && key>max)
              goto rangeExceeded;
            else if(!isCondLE && key>=max)
              goto rangeExceeded;
          }
          
          if(min!=-1) //if there is a condition on GT or GE that fails, we are done
          {
            if(isCondGE && key<min)
              goto rangeExceeded;
            else if(!isCondGE && key<=min)
              goto rangeExceeded;
          }
          
          //if key passes all of the conditions, increment count and jump to next cycle in while loop
          //in doing this if-statement, we save many reads from record file
          count++;
          continue;
      }
    
      if ((rc = rf.read(rid, key, value)) < 0) {
        fprintf(stderr, "Error: while reading a tuple from table %s\n", table.c_str());
        goto faultyTuple;
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
      
      continue_while:
      ;
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
  
  if(hasCond) //close the index file if applicable
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
  
  
  if(!tableData.is_open())
  fprintf(stderr, "Error: loadfile %s cannot be opened\n", loadfile.c_str());
  
  
  rc = rf.open(table + ".tbl", 'w');
  
 
  if(index)
  {
  
  tree.open(table + ".idx", 'w');
  
  
    while(getline(tableData, line))
    {
    parseLoadLine(line, key, value);
    if(rf.append(key, value, rid)!=0)
      return RC_FILE_WRITE_FAILED;
    
    if(tree.insert(key, rid)!=0)
      return RC_FILE_WRITE_FAILED;
    }
    
    //tree.print();

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

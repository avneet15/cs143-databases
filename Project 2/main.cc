/**
 * Copyright (C) 2008 by The Regents of the University of California
 * Redistribution of this file is permitted under the terms of the GNU
 * Public License (GPL).
 *
 * @author Junghoo "John" Cho <cho AT cs.ucla.edu>
 * @date 3/24/2008
 */
 
#include "Bruinbase.h"
#include "SqlEngine.h"
#include <cstdio>

//Remove below packages 
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include "Bruinbase.h"
#include "SqlEngine.h"
#include "PageFile.h"
#include "RecordFile.h"
#include "BTreeNode.h" 

 bool SqlEngine::isIndex = false;

int main()
{
  // run the SQL engine taking user commands from standard input (console).
  /*int record_id_size = sizeof(RecordId);
  PageFile *P = new PageFile("Trial2.txt",'w');
  PageId pid = P->endPid();
  //std::string val;
  //void *c = &val;
  /*P->write(0,"ABCXYZRST");
  P->write(1,"DEF");
  P->write(2,"MHI");


  fprintf(stdout, "%d\n",P->endPid());
  BTLeafNode ln;

  ln.read(2,*P);
  ln.write(1,*P);
  //ln.getKeyCount();
  //fprintf(stdout, "%d\n",no_of_records);
  int key;
  RecordId rid;
  ln.readEntry(2, key, rid);
  fprintf(stdout, "Key is= %d\n",key);*/


  SqlEngine::run(stdin);
  return 0;
}

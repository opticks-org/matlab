/*
 * The information in this file is
 * Copyright(c) 2013 Ball Aerospace & Technologies Corporation
 * and is subject to the terms and conditions of the
 * GNU Lesser General Public License Version 2.1
 * The license text is available from   
 * http://www.gnu.org/licenses/lgpl.html
 */

#ifndef MATLABTESTS_H
#define MATLABTESTS_H

#include "PlugInShell.h"
#include "Testable.h"

class MatlabTests : public PlugInShell, public Testable
{
public:
   MatlabTests();
   virtual ~MatlabTests();

   virtual bool runAllTests(Progress* pProgress, std::ostream& failure);
   virtual bool runOperationalTests(Progress* pProgress, std::ostream& failure);
};

#endif

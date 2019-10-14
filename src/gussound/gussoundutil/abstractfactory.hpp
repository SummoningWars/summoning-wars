#ifndef GUS_LIB_GENERAL_FACTORY_H
#define GUS_LIB_GENERAL_FACTORY_H

//   This file is part of the guslib library, licensed under the terms of the MIT License.
//
//   The MIT License
//   Copyright (C) 2010-2011  Augustin Preda (thegusty999@gmail.com)
//
//   Permission is hereby granted, free of charge, to any person obtaining a copy
//   of this software and associated documentation files (the "Software"), to deal
//   in the Software without restriction, including without limitation the rights
//   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//   copies of the Software, and to permit persons to whom the Software is
//   furnished to do so, subject to the following conditions:
//   
//   The above copyright notice and this permission notice shall be included in
//   all copies or substantial portions of the Software.
//   
//   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//   THE SOFTWARE.
//
//	 Last change:	$LastChangedDate: 2011-09-04 19:06:22 +0300 (D, 04 sep. 2011) $
//	 Revision:		$Revision: 225 $

#include <map>

//#include <guslib/guslibBuildOpts.h> // TODO: remove

/**
	@file
	Template factory; modelled after the Loki library.
*/
namespace guslib
{
	/**
		A standard template factory; modelled after the Loki library.
	*/
	template
		<class AbstractClass,
		typename IdentifierType,
		typename ProductCreator = AbstractClass* (*)()>
class GeneralFactory
	{
	public:
		typedef std::map <IdentifierType, ProductCreator> AssociativeMap;

        GeneralFactory ()
		{
		}

		virtual ~GeneralFactory()
		{
			associations_.clear ();
		}

		virtual AbstractClass * CreateObject (const IdentifierType & id)
		{
			typename AssociativeMap::iterator i = associations_.find(id);
			if (i != associations_.end())
			{
				return (i->second) ();
			}
			return NULL;
		}

		virtual bool Register (const IdentifierType& id, ProductCreator creator)
		{
			return associations_.insert(
				typename AssociativeMap::value_type(id, creator)).second != 0;
		}

		virtual bool Unregister (const IdentifierType& id)
		{
			return associations_.erase(id) != 0;
		}
  
	protected:
#pragma warning (push)
#pragma warning (disable:4251)
		AssociativeMap associations_;
#pragma warning (pop)
	};

}

#endif // GUS_LIB_GENERAL_FACTORY_H

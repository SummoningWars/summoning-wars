#ifndef GUS_LIB_SMARTPTR_H
#define GUS_LIB_SMARTPTR_H

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
//	 Last change:	$LastChangedDate: 2012-09-16 11:26:05 +0300 (D, 16 sep. 2012) $
//	 Revision:		$Revision: 541 $


#include <gussoundutil/guslibbuildopts.h>


#if GUSLIB_USE_BOOST_SMART_POINTERS
	#include <boost/shared_ptr.hpp>
#endif // GUSLIB_USE_BOOST_SMART_POINTERS

#if GUSLIB_USE_YASPER_SMART_POINTER
	#include <gussoundutil/yasper.h>

#endif // GUSLIB_USE_YASPER_SMART_POINTER



namespace guslib
{
#if GUSLIB_USE_BOOST_SMART_POINTERS
	#define GSharedPtr boost::shared_ptr
#endif // GUSLIB_USE_BOOST_SMART_POINTERS


#if GUSLIB_USE_YASPER_SMART_POINTER
	#define GSmartPtr yasper::ptr
#endif // GUSLIB_USE_YASPER_SMART_POINTER

} //namespace guslib


#endif // GUS_LIB_SMARTPTR_H

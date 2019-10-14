#ifndef GUS_LIB_BUILDOPTS_H
#define GUS_LIB_BUILDOPTS_H

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
//   Build options.
//
//	 Last change:	$LastChangedDate: 2013-05-07 17:47:45 +0300 (Ma, 07 mai. 2013) $
//	 Revision:		$Revision: 553 $

// Version info
#include <gussoundutil/guslib_version.hpp>

#ifdef GUSLIB_EXP
#define GUSLIB_EXPORT_SYMBOL __declspec (dllexport)
#define GUSLIB_EXPIMP_TEMPLATE
#else
#ifdef GUSLIB_IMP
#define GUSLIB_EXPORT_SYMBOL __declspec (dllimport)
#define GUSLIB_EXPIMP_TEMPLATE extern
#else
#define GUSLIB_EXPORT_SYMBOL 
#define GUSLIB_EXPIMP_TEMPLATE 
#endif
#endif // GUSLIB_EXP


#ifndef _WINDOWS
#ifdef WIN32
#define _WINDOWS
#endif // WIN32
#endif // _WINDOWS


// control the multithread support.

// #ifndef GUSLIB_FLAG_MULTITHREAD
// #define GUSLIB_FLAG_MULTITHREAD 1
// #endif

// Some options you may set for the project:
// GUSLIB_FLAG_SINGLETONINST: specify whether the singleton can declare and set to NULL the instances of the defined classes.

#ifndef GUSLIB_FLAG_SINGLETONINST
#define GUSLIB_FLAG_SINGLETONINST 1
#endif

// Other flags:
//
// GUSLIB_USE_YASPER_SMART_POINTER

#ifndef GUSLIB_USE_YASPER_SMART_POINTER
#define GUSLIB_USE_YASPER_SMART_POINTER 1
#endif


#endif // GUS_LIB_BUILDOPTS_H
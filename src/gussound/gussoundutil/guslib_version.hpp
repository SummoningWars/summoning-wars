#ifndef GUSLIB_VERSION_HPP
#define GUSLIB_VERSION_HPP

//   This file is part of the guslib library, licensed under the terms of the MIT License.
//
//   The MIT License
//   Copyright (C) 2010-2013  Augustin Preda (thegusty999@gmail.com)
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
//   Version information file. (Using boost style version information).
//
//   This header will change between different releases of guslib. Including the header will result in a recompile
//   when a new guslib version is released.
//
//	 Last change:	$LastChangedDate: 2013-05-07 17:51:01 +0300 (Ma, 07 mai. 2013) $
//	 Revision:		$Revision: 554 $


//   GUSLIB_NUM_VERSION % 100 is the patch level
//   GUSLIB_NUM_VERSION / 100 % 1000 is the minor version
//   GUSLIB_NUM_VERSION / 100000 is the major version

#define GUSLIB_NUM_VERSION 100100
//                         ____XX = patch
//                         _XXX__ = minor version
//                         X_____ = major version

//   GUSLIB_STRING_VERSION should be defined by the user to be the same as GUSLIB_NUM_VERSION, but as a string
//   in the form "x_y[_z]" where x is the major version number, y is the minor version number, 
//   and z (optional) is the patch level; (if the patch level is 0, you should skip it).

#define GUSLIB_STRING_VERSION "1_01"

#endif // GUSLIB_VERSION_HPP

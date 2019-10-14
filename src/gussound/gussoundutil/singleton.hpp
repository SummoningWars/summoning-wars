#ifndef GUS_LIB_SINGLETON_H
#define GUS_LIB_SINGLETON_H

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
//	 Last change:	$LastChangedDate: 2013-05-07 17:47:45 +0300 (Ma, 07 mai. 2013) $
//	 Revision:		$Revision: 553 $


#include <gussoundutil/guslibbuildopts.h>
#include <stdlib.h>

// Note: for multithreaded synchronization and safe singleton access, be sure to enable the GUSLIB_FLAG_MULTITHREAD flag.
// This will require boost.

#if GUSLIB_FLAG_MULTITHREAD
#include <gussoundutil/thread.h>
#endif


namespace guslib
{
  // Utility class to encapsulate a singleton.
  // Limitation: This can only handle singletons with public, default constructors.
  // Note: a double checked locking is attempted at the object's creation, if multithreaded synchronization is enabled
  // (it will depend on 3rd party libraries for thread utilities, such as boost or Poco). To enable the multithreaded 
  // synchronization, you MUST define GUSLIB_FLAG_MULTITHREAD to 1.
  // The creation behaviour is similar to the Pheonix Creation policy:
  // singletons that were deleted can be brought back to life if they are called again.
  // Note on storing these classes in a DLL:
  // In order to have an actual singleton, with a single instance, you must export the symbol to the DLL.
  // Make sure you do this by defining the flag "GUSLIB_EXP" in the DLL. The calling code must use the 
  // 'GUSLIB_IMP' preprocessor flag.
  template <class T> class Singleton
  {
  private:
		static T * objectPtr_;

  protected:
#if GUSLIB_FLAG_MULTITHREAD
	  static guslib::GMutex creationMutex_;
#endif

  public:
    // Get a pointer to the singleton instance.
    static T * getPtr ()
    {
      // first check if it was created. If the mutex would be placed first, a performance hit would occur, since
      // all operations would create and release a lock.
      if (! objectPtr_)
      {
        // try to lock
#if GUSLIB_FLAG_MULTITHREAD
        guslib::GScopedLock myLock (creationMutex_);
#endif

        // While the lock tried to take control, another thread may have already finished instantiating the object,
        // so a new check should be done to ensure safety (double-checked locking pattern).
        if (! objectPtr_)
        {
          // Should create the object as volatile to prevent some compiler optimizations that may lead to the lines being
          // executed internally in a slightly different order than the one given here.
          // (see also: http://aszt.inf.elte.hu/~gsd/klagenfurt/material/ch03s05.html)
          volatile T * volatileptr = new T ();
          objectPtr_ = (T*) volatileptr;
		  // If possible, use the std::atexit
#ifndef ATEXIT_FIXED
          atexit (& destroy);
#endif// ATEXIT_FIXED
        }
      }
      return objectPtr_;
    }//getptr

    // Get the singleton instance.
    static T& get ()
    {
      return *getPtr ();
    }//get

    // Destroy the singleton instance. Again, try a double checked lock.
    static void destroy ()
    {
      if (objectPtr_)
      {
#if GUSLIB_FLAG_MULTITHREAD
        guslib::GScopedLock myLock (creationMutex_);
#endif
        if (objectPtr_)
        {
          delete objectPtr_;
          objectPtr_ = 0; // NULL
        }
      }
    }//destroy
  };

#if GUSLIB_FLAG_SINGLETONINST != 0
  template <class T> T* Singleton<T>::objectPtr_ = 0; // NULL
#endif

#if GUSLIB_FLAG_MULTITHREAD
  template <class T> GMutex Singleton<T>::creationMutex_;
#endif

} // namespace end

#endif //GUS_LIB_SINGLETON_H

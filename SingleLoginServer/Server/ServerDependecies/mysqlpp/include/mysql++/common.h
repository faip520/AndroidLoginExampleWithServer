/// \file common.h
/// \brief This file includes top-level definitions for use both
/// internal to the library, and outside it.  Contrast mysql++.h
///
/// This file mostly takes care of platform differences.

/***********************************************************************
 Copyright © 1998 by Kevin Atkinson, © 1999-2001 by MySQL AB,
 © 2004-2009, 2018  by Educational Technology Resources, Inc., and
 © 2009 by Warren Young.  Others may also hold copyrights on code
 in this file.  See the CREDITS.txt file in the top directory of the
 distribution for details.

 This file is part of MySQL++.

 MySQL++ is free software; you can redistribute it and/or modify it
 under the terms of the GNU Lesser General Public License as published
 by the Free Software Foundation; either version 2.1 of the License, or
 (at your option) any later version.

 MySQL++ is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with MySQL++; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301
 USA
***********************************************************************/

#if !defined(MYSQLPP_COMMON_H)
#define MYSQLPP_COMMON_H

#if !defined(DOXYGEN_IGNORE)
// Doxygen will not generate documentation for the following stuff.

// Enable SSQLS by default.  Turned off below on platforms where we
// know it doesn't work.
#define MYSQLPP_SSQLS_COMPATIBLE

// For all platforms but Visual C++ 2003, the following macro is just
// an alias for "*this".  It needs a more complicated definition on
// VC++ 2003 to work around an error in the overloaded operator lookup
// logic.  For an explanation of the problem, see:
// http://groups.google.com/group/microsoft.public.vc.stl/browse_thread/thread/9a68d84644e64f15
#define MYSQLPP_QUERY_THISPTR *this

// Work out major platform-specific stuff here.
#if defined(__WIN32__) || defined(_WIN32)
#	define MYSQLPP_PLATFORM_WINDOWS
	// Windows compiler support.  Tested with Microsoft Visual C++,
	// Borland C++ Builder, and MinGW GCC.

	// Don't let windows.h (via Connector/C) #define min/max
	#define NOMINMAX

	// Stuff for Visual C++ only
#	if defined(_MSC_VER)
#		define MYSQLPP_PLATFORM_VISUAL_CPP
		// MS *still* doesn't ship stdint.h, through VC++ 2008 at least.
		// This means we have to take a wild guess at appropriate
		// integer types in lib/sql_types.h.  See test/inttypes.cpp for
		// tests that check whether we've guessed well.
#		define MYSQLPP_NO_STDINT_H
#		if _MSC_VER < 1400
			// Workarounds for limitations of VC++ 2003 that are fixed
			// in 2005 and later.
#			undef MYSQLPP_QUERY_THISPTR
#			define MYSQLPP_QUERY_THISPTR dynamic_cast<std::ostream&>(*this)
#			undef MYSQLPP_SSQLS_COMPATIBLE
#		elif !defined(_STLP_VERSION) && !defined(_STLP_VERSION_STR)
			// VC++ 2005 or newer and not using STLport, so #define
			// portability flags indicating features we can use from
			// the compiler's native RTL.
#			define MYSQLPP_HAVE_LOCALTIME_S
#			define MYSQLPP_HAVE_STD__NOINIT
#		endif

		// Disable complaints about STL data members: VC++ believes
		// these need to be __declspec(dllexport) for some reason.
#		pragma warning(disable: 4251)
		// Disable complaint that VC++ doesn't grok throw specs
#		pragma warning(disable: 4290)
		// Disable whining about using 'this' as a member initializer on VC++.
#		pragma warning(disable: 4355)
		// Disable whining about implicit conversions to bool
#		pragma warning(disable: 4800)
		// Disable nagging about new "secure" functions like strncpy_s()
#		pragma warning(disable: 4996)

		// Disable warning about exporting a class from a DLL which is
		// derived from a non-exported class in another DLL.  This is
		// safe to do with Standard C++ library types per:
		//
		//  https://msdn.microsoft.com/en-us/library/3tdb471s.aspx
		//
		// We don't hit this any other way in MySQL++.
#		pragma warning(disable: 4275)

		// Squish warning about passing no args to MAY_THROW() when
		// building with newer C++ support.  We're doing it on purpose.
#		pragma warning(disable: 4003)

		// Prior to Visual C++ 2015, we must use _snprintf()
#		if _MSC_VER < 1900
#			define snprintf _snprintf
#		endif
#	endif

	// Define DLL import/export tags for Windows compilers, where we build
	// the library into a DLL, for LGPL license compatibility reasons.
	// (This is based on a similar mechanism in wxWindows.)

	#ifdef MYSQLPP_MAKING_DLL
		// When making the DLL, export tagged symbols, so they appear
		// in the import library.
		#define MYSQLPP_EXPORT __declspec(dllexport)
	#elif !defined(MYSQLPP_NO_DLL)
		// We must be _using_ the DLL, so import symbols instead.
		#define MYSQLPP_EXPORT __declspec(dllimport)
	#else
		// Not making a DLL at all, so no-op these declspecs
		#define MYSQLPP_EXPORT
	#endif

	// We need to use the DOS/Windows path separator here
	#define MYSQLPP_PATH_SEPARATOR '\\'
#else
	// If not VC++, MinGW, or Xcode, we assume we're on a system using
	// autoconf, so bring in the config.h file it wrote containing the
	// config test results.  Only do this during the library build, and
	// even then, not if included from a MySQL++ header file, since
	// config.h cannot be safely installed with the other headers.
#	if defined(MYSQLPP_NOT_HEADER) && !defined(MYSQLPP_XCODE)
#		include "config.h"
#	endif

	// Make DLL stuff a no-op on this platform.
	#define MYSQLPP_EXPORT

	// Assume POSIX path separator
	#define MYSQLPP_PATH_SEPARATOR '/'
#endif

// Workarounds for deprecations in C++11 and newer.  We must still
// support systems whose contemporaneous C++ compiler only understands
// C++98.  Because of the large gap between C++98 and C++11, it will
// likely be years yet until we can start using C++11 unconditionally
// within MySQL++, then years more until we can use C++14, etc.
//
// C++11 deprecated throwspecs, a feature of C++ that is only used by
// the oldest parts of MySQL++.  We can't drop the throwspecs until
// MySQL++ 4, if we ever get around to that, since that would break
// the library's ABI on systems whose C++ compiler still supports
// throwspecs.  This feature isn't completely gone until C++17, but we
// don't want the warnings on certain newer compilers expecting C++11
// or newer C++ dialects.
#if __cplusplus < 201103L
#	define MAY_THROW(what) throw(what)
#else
#	define MAY_THROW(junk) noexcept(false)
#endif

// C++11 added unique_ptr as a replacement for auto_ptr.  We don't have
// the ABI problem above with this one, so we switch to it with the
// oldest release possible.  As with the above ifdef, this one only
// currently works for g++ and clang++.
#if __cplusplus >= 201103L
#	define UNIQUE_PTR(what) std::unique_ptr<what>
#else
#	define UNIQUE_PTR(what) std::auto_ptr<what>
#endif


namespace mysqlpp {

/// \brief Alias for 'true', to make code requesting exceptions more
/// readable.
const bool use_exceptions = true;

/// \brief Used to disambiguate overloads of equal_list() in SSQLSes.
enum sql_cmp_type { sql_use_compare };

#if !defined(DOXYGEN_IGNORE)
// Figure out how to get large integer support on this system.  Suppress
// refman documentation for these typedefs, as they're system-dependent.
#if defined(MYSQLPP_NO_LONG_LONGS)
// Alias "longlong" and "ulonglong" to the regular "long" counterparts
typedef unsigned long ulonglong;
typedef long longlong;
#elif defined(_MSC_VER)
// It's VC++, so we'll use Microsoft's 64-bit integer types
typedef unsigned __int64 ulonglong;
typedef __int64 longlong;
#else
// No better idea, so assume the C99 convention.  If your compiler
// doesn't support this, please provide a patch to extend this ifdef, or
// define MYSQLPP_NO_LONG_LONGS.
typedef unsigned long long ulonglong;
typedef long long longlong;
#endif
#endif // !defined(DOXYGEN_IGNORE)

#if !defined(MYSQLPP_NO_UNSIGNED_INT_TYPES)
/// \brief Contraction for 'unsigned long'
///
/// This is not to be used within the library or directly by end-user
/// code.  It exists to make the MySQL C API headers happy: my_global.h
/// defines it, but we can't use it in MySQL++, so we do it ourselves.
typedef unsigned long ulong;
#endif

} // end namespace mysqlpp

// The MySQL headers define these macros, which is completely wrong in a
// C++ project.  Undo the damage.
#undef min
#undef max

#endif // !defined(DOXYGEN_IGNORE)


// Now that we've defined all the stuff above, we can pull in the full
// MySQL header.  Basically, the above largely replaces MySQL's my_global.h
// while actually working with C++.  This is why we disobey the MySQL
// developer docs, which recommend including my_global.h before mysql.h.
#if defined(MYSQLPP_MYSQL_HEADERS_BURIED)
#	include <mysql/mysql.h>
#else
#	include <mysql/mysql.h>
#endif

// The Unicode chapter of the user manual justifies the following.
#if MYSQL_VERSION_ID >= 50500
    /// \brief Use this macro in CREATE TABLE strings to get the best
    /// available UTF-8 character set.
    ///
    /// MySQL++ is built against MySQL or MariaDB 5.5 or newer, so these
    /// macros are defined so that programs using them get the complete
    /// UTF-8 character set.
#   define MYSQLPP_UTF8_CS  "utf8mb4"

    /// \brief Use this macro in CREATE TABLE strings to get a matching
    /// collation to the character set selected by MYSQLPP_UTF8_CS
#   define MYSQLPP_UTF8_COL "utf8mb4_general_ci"
#else
    /// \brief Use this macro in CREATE TABLE strings to get the best
    /// available UTF-8 character set and correpsonding collation.
    ///
    /// MySQL++ is built against a version of MySQL or MariaDB older than
    /// 5.5, so we must use the legacy 3-byte-limited subset of UTF-8.
#   define MYSQLPP_UTF8_CS  "utf8"

    /// \brief Use this macro in CREATE TABLE strings to get a matching
    /// collation to the character set selected by MYSQLPP_UTF8_CS
#   define MYSQLPP_UTF8_COL "utf8_general_ci"
#endif

#endif // !defined(MYSQLPP_COMMON_H)

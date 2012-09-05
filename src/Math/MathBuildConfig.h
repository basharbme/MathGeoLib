/* Copyright 2011 Jukka Jyl�nki

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

/** @file MathBuildConfig.h
	@author Jukka Jyl�nki
	@brief Specifies all build flags for the library. */
#pragma once

// If MATH_ENABLE_NAMESPACE is defined, all math symbols are put inside a namespace.
#define MATH_ENABLE_NAMESPACE

/// Adjust this #define to choose the name of the namespace math is given to.
// This flag does not have any effect if MATH_ENABLE_NAMESPACE is not defined.
#define MATH_NAMESPACE_NAME math

// If MATH_AUTO_USE_NAMESPACE is defined, a 'using namespace math;' directive is issued in all header files, causing
// the math namespace to exist only for internal symbol hiding purposes, and not for external code.
// This flag does not have any effect if MATH_ENABLE_NAMESPACE is not defined.
#define MATH_AUTO_USE_NAMESPACE

// If MATH_ASSERT_ON_ASSUME is defined, assume() resolves directly to assert().
// When not defined, assume() prints out an error if the condition fails, but continues
// execution.
//#define MATH_ASSERT_ON_ASSUME

#ifndef _DEBUG
// If MATH_SILENT_ASSUME is defined, all assume() tests are stripped from the build. This
// overrides MATH_ASSERT_ON_ASSUME.
#define MATH_SILENT_ASSUME

// If MATH_ENABLE_INSECURE_OPTIMIZATIONS, several security checks (unsafe index out of bounds accesses etc.) are disabled.
// Use this for absolutely fastest performance on MathGeoLib, but only if you know your code is 100% well-formed and uses
// MathGeoLib bug-free.
// #define MATH_ENABLE_INSECURE_OPTIMIZATIONS

#endif

#ifdef _DEBUG
// If MATH_ASSERT_CORRECTNESS is defined, special (and possibly rather costly) mathassert() 
// tests are enabled, which test the internal correctness of the library.
#define MATH_ASSERT_CORRECTNESS
#endif

// If MATH_ENABLE_STL_SUPPORT is defined, MathGeoLib utilizes STL data structures. Otherwise, 
// features requiring STL are disabled (but the library can still be built).
#ifndef MATH_ENABLE_STL_SUPPORT
#define MATH_ENABLE_STL_SUPPORT
#endif

// If MATH_TINYXML_INTEROP is defined, MathGeoLib integrates with TinyXML to provide
// serialization and deserialization to XML for the data structures.
#ifndef MATH_TINYXML_INTEROP
//#define MATH_TINYXML_INTEROP
#endif

// If MATH_CONTAINERLIB_SUPPORT is defined, MathGeoLib integrates with a certain
// STL replacement container library. Do not enable, only for internal use.
#ifndef MATH_CONTAINERLIB_SUPPORT
//#define MATH_CONTAINERLIB_SUPPORT
#endif

// If MATH_GRAPHICSENGINE_INTEROP is defined, MathGeoLib integrates with a certain
// graphics engine. Do not enable, only for internal use.
#ifndef MATH_GRAPHICSENGINE_INTEROP
//#define MATH_GRAPHICSENGINE_INTEROP
#endif

// If KNET_LOGGING_SUPPORT_ENABLED is defined, MathGeoLib utilizes logging functions
// from the kNet library.
#ifndef KNET_LOGGING_SUPPORT_ENABLED
//#define KNET_LOGGING_SUPPORT_ENABLED
#endif

// Uncomment to specify the SIMD instruction set level in use.
//#define MATH_AVX
//#define MATH_SSE41
//#define MATH_SSE3
//#define MATH_SSE2
//#define MATH_SSE // SSE1.

#ifdef ANDROID
//#define MATH_NEON
//#include <arm_neon.h>
#endif

// MATH_AVX implies MATH_SSE41, which implies MATH_SSE3, which implies MATH_SSE2, which implies MATH_SSE.
#ifdef MATH_AVX
#define MATH_SSE41
#define MATH_SSE3
#define MATH_SSE2
#define MATH_SSE
#endif

#ifdef MATH_SSE41
#define MATH_SSE3
#define MATH_SSE2
#define MATH_SSE
#endif

#ifdef MATH_SSE3
#include <intrin.h>
#define MATH_SSE2
#define MATH_SSE
#endif

#ifdef MATH_SSE2
#define MATH_SSE
#endif

#ifdef MATH_SSE
#include <xmmintrin.h>
#endif

#include "Types.h"

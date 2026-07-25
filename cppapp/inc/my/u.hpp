#pragma once

#include <my/hash.hpp>
#include <my/logging.hpp>
#include <my/time.hpp>

// Backwards compatibility wrappers in global scope / C namespace
using my::curTimeMillis;
using my::curTimeMillis_s;
using my::tmstr;
using my::merr_setargs0;
using my::merr_error;
using my::mstrhash;

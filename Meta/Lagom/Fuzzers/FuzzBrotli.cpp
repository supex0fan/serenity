/*
 * Copyright (c) 2022, the SerenityOS developers.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibCompress/Brotli.h>
#include <LibCore/MemoryStream.h>
#include <stdio.h>

extern "C" int LLVMFuzzerTestOneInput(uint8_t const* data, size_t size)
{
    auto bufstream_result = Core::Stream::MemoryStream::construct({ data, size });
    if (bufstream_result.is_error()) {
        dbgln("MemoryStream::construct() failed.");
        return 1;
    }
    auto bufstream = bufstream_result.release_value();

    auto brotli_stream = Compress::BrotliDecompressionStream { *bufstream };

    auto uncompressed = brotli_stream.read_all();
    return uncompressed.is_error();
}

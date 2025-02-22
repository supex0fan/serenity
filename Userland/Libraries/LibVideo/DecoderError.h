/*
 * Copyright (c) 2022, Gregory Bertilson <zaggy1024@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/DeprecatedString.h>
#include <AK/Error.h>
#include <AK/Format.h>
#include <AK/SourceLocation.h>
#include <errno.h>

namespace Video {

struct DecoderError;

template<typename T>
using DecoderErrorOr = ErrorOr<T, DecoderError>;

enum class DecoderErrorCategory : u32 {
    Unknown,
    IO,
    NeedsMoreInput,
    EndOfStream,
    Memory,
    // The input is corrupted.
    Corrupted,
    // Invalid call.
    Invalid,
    // The input uses features that are not yet implemented.
    NotImplemented,
};

struct DecoderError {
public:
    static DecoderError with_description(DecoderErrorCategory category, StringView description)
    {
        return DecoderError(category, description);
    }

    template<typename... Parameters>
    static DecoderError format(DecoderErrorCategory category, CheckedFormatString<Parameters...>&& format_string, Parameters const&... parameters)
    {
        AK::VariadicFormatParams variadic_format_params { parameters... };
        return DecoderError::with_description(category, DeprecatedString::vformatted(format_string.view(), variadic_format_params));
    }

    static DecoderError from_source_location(DecoderErrorCategory category, StringView description, SourceLocation location = SourceLocation::current())
    {
        return DecoderError::format(category, "[{} @ {}:{}]: {}", location.function_name(), location.filename(), location.line_number(), description);
    }

    static DecoderError corrupted(StringView description, SourceLocation location = SourceLocation::current())
    {
        return DecoderError::from_source_location(DecoderErrorCategory::Corrupted, description, location);
    }

    static DecoderError not_implemented(SourceLocation location = SourceLocation::current())
    {
        return DecoderError::format(DecoderErrorCategory::NotImplemented, "{} is not implemented", location.function_name());
    }

    DecoderErrorCategory category() const { return m_category; }
    StringView description() const { return m_description; }
    StringView string_literal() const { return m_description; }

private:
    DecoderError(DecoderErrorCategory category, DeprecatedString description)
        : m_category(category)
        , m_description(move(description))
    {
    }

    DecoderErrorCategory m_category { DecoderErrorCategory::Unknown };
    DeprecatedString m_description;
};

#define DECODER_TRY(category, expression)                                  \
    ({                                                                     \
        auto _result = ((expression));                                     \
        if (_result.is_error()) [[unlikely]] {                             \
            auto _error_string = _result.release_error().string_literal(); \
            return DecoderError::from_source_location(                     \
                ((category)), _error_string, SourceLocation::current());   \
        }                                                                  \
        _result.release_value();                                           \
    })

#define DECODER_TRY_ALLOC(expression) DECODER_TRY(DecoderErrorCategory::Memory, expression)

}

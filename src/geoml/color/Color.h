#pragma once

#include <cstdint>
#include <stdexcept>

namespace geoml {

/** An RGB color with 8-bit channels. */
class Color
{
public:
    /**
     * Construct a color from red, green, and blue channels in [0, 255].
     *
     * Keep the parameters as int so out-of-range arguments can be rejected
     * before narrowing to uint8_t; uint8_t parameters would allow implicit
     * conversion at the call site (for example, 300 becoming 44).
     */
    Color(int red, int green, int blue)
        : m_red(channel(red))
        , m_green(channel(green))
        , m_blue(channel(blue))
    {}

    constexpr std::uint8_t red() const noexcept { return m_red; }
    constexpr std::uint8_t green() const noexcept { return m_green; }
    constexpr std::uint8_t blue() const noexcept { return m_blue; }

    friend constexpr bool operator==(Color const& left, Color const& right) noexcept
    {
        return left.m_red == right.m_red &&
               left.m_green == right.m_green &&
               left.m_blue == right.m_blue;
    }

    friend constexpr bool operator!=(Color const& left, Color const& right) noexcept
    {
        return !(left == right);
    }

private:
    static std::uint8_t channel(int value)
    {
        if (value < 0 || value > 255) {
            throw std::out_of_range("Color RGB channels must be in [0, 255]");
        }
        return static_cast<std::uint8_t>(value);
    }

    std::uint8_t m_red;
    std::uint8_t m_green;
    std::uint8_t m_blue;
};

} // namespace geoml

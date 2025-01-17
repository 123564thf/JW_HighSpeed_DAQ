#pragma once

#include <cstdint>
#include <cmath>

class Welford {
private:
    uint32_t m_n = 0;
    //double m_sqsum = 0.0;
    double m_mean = 0.0;
    double m_M2 = 0.0;
    //double m_rms = 0.0;

public:
    constexpr Welford() noexcept : m_n(0), m_mean(0), m_M2(0) {}

    void addDataPoint(uint16_t x) {
        m_n++;
        double delta = x - m_mean;
        m_mean += delta / m_n;
        m_M2 += delta * (x - m_mean);
        //m_sqsum += x * x;
    }

    uint32_t getN() const noexcept { return m_n; }
    double mean() const noexcept { return m_mean; }
    double variance() const noexcept { return m_n > 1 ? m_M2 / (m_n - 1) : 0.; }
    double stddev() const noexcept { return std::sqrt(variance()); }
    uint16_t threshold() const noexcept { return std::ceil(m_mean + 10 * std::sqrt(m_M2 / m_n)); }
    //double rms() const noexcept { return std::sqrt(m_sqsum / m_n); }
};

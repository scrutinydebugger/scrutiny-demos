//    wave_function_generator.cpp
//        A class that generate a periodic wave with common shapes
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-demos)
//
//   Copyright (c) 2025 Scrutiny Debugger

#include "wave_function_generator.hpp"
#include <algorithm>
#include <cmath>

WaveFunctionGenerator::WaveFunctionGenerator(WaveType type, float frequency, float offset) :
    wave_type{ type },
    frequency{ frequency },
    offset{ offset },
    enabled{ true },
    m_phase{ 0 },
    m_output{ 0 }
{
}

void WaveFunctionGenerator::update(float const time_delta)
{
    if (!enabled)
    {
        m_output = 0;
        m_phase = 0;
        return;
    }

    constexpr float _2pi = 2.0f * M_PI;
    m_phase += time_delta * frequency * _2pi;
    if (m_phase >= _2pi)
    {
        m_phase -= std::floor(m_phase / _2pi) * _2pi;
    }

    float effective_phase = m_phase + phase_shift;
    if (effective_phase >= _2pi)
    {
        effective_phase -= std::floor(effective_phase / _2pi) * _2pi;
    }

    switch (wave_type)
    {
    case WaveType::SINE:
        m_output = std::sin(effective_phase);
        break;
    case WaveType::SQUARE:
        m_output = (effective_phase < M_PI) ? -1 : 1;
        break;
    case WaveType::TRIANGLE:
        m_output = (effective_phase < M_PI) ? effective_phase * 2 / M_PI - 1 : -2 / M_PI * effective_phase + 3;
        break;
    case WaveType::SAWTOOTH:
        m_output = effective_phase / M_PI - 1;
        break;
    default:
        m_output = 0;
    }

    m_output += offset;
}
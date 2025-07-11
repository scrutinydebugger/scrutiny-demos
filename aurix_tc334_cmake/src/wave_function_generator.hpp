//    wave_function_generator.hpp
//        A class that generate a periodic wave with common shapes
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-demos)
//
//   Copyright (c) 2025 Scrutiny Debugger

#ifndef _WAVE_FUNCTION_GENERATOR_HPP
#define _WAVE_FUNCTION_GENERATOR_HPP

class WaveFunctionGenerator
{
  public:
    enum class WaveType
    {
        SINE,
        SQUARE,
        TRIANGLE,
        SAWTOOTH
    };

    WaveType wave_type;
    float frequency;
    float offset;
    float phase_shift;
    bool enabled;

    WaveFunctionGenerator(WaveType type, float frequency, float offset);
    void update(float const time_delta);
    inline void set_phase(float const val) { m_phase = val; };

  private:
    float m_phase;
    float m_output;
};

#endif // _WAVE_FUNCTION_GENERATOR_HPP
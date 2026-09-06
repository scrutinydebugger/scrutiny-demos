#ifndef _IIR1STORDER_HPP_
#define _IIR1STORDER_HPP_

#include <stdint.h>


/// @brief An 1st order IIR filter implemented as an exponential averager.
///   Used to demonstrate how we can monitor a control system with an embedded graph.
class IIR1stOrder
{
    public:
    IIR1stOrder(float const dt);
    void init(float const tau);
    void step(float const input);
    void reset(float const output_val);

    private:
    float const m_dt;
    float m_alpha;
    float m_a1;
    float m_output;
};

#endif  // _IIR1STORDER_HPP_

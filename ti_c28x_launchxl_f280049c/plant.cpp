#include "plant.hpp"

IIR1stOrder::IIR1stOrder(float const dt) : 
    m_dt(dt), 
    m_alpha(0.0f),    
    m_a1(0.0f),
    m_output(0.0f)
{
    
}

void IIR1stOrder::init(float const tau)
{
    if (tau <= 0.0f){
        return;
    }
    m_output = m_a1 = 0.0f;
    m_alpha = m_dt/tau; // Approximation given that tau >> dt. Otherwise would be 1-exp(-dt/tau)
}

void IIR1stOrder::step(float const input)
{
    m_a1 = m_output; 
    m_output = input * m_alpha + m_a1 * (1.0f-m_alpha);
}

void IIR1stOrder::reset(float const output_val)
{
    m_output = m_a1 = output_val;
}

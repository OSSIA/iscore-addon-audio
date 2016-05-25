#include "Track.hpp"
namespace Audio {
namespace Panel {

Track::Track(double volval, double panval, int32_t outval) : m_vol(volval), m_pan(panval), m_output(outval) {}
Track::Track() : Track(100, 0, 0) {}

double Track::vol() const {
    return m_vol;
}
double Track::pan() const {
    return m_pan;
}

int32_t Track::out() const {
    return m_output;
}

void Track::setVol(double v) {
    if (v >= 0 && v <= 100)
        m_vol = v;
}

void Track::setPan(double p) {
    if (p >= -1 && p <= 1)
        m_pan = p;
}

void Track::setOut(int32_t o) {
    if (o >= 0 && o < MAX_OUTPUT)
        m_output = o;
}

}
}

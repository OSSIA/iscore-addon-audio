#pragma once
#include <cstdint>

namespace Audio {
namespace Panel {

class Track {
public:
    Track(double volval, double panval, int32_t outval);
    Track();

    double vol() const;
    double pan() const;
    int32_t out() const;

    void setVol(double);
    void setPan(double);
    void setOut(int32_t);

    static const int MAX_OUTPUT = 100;

private:
    double m_vol = 100;
    double m_pan = 0;
    long m_output = 0;
};

}
}

#pragma once

namespace Audio {
namespace Panel {

class Track {
public:
    Track(double, double, long);
    Track();

    double vol() const;
    double pan() const;
    long out() const;

    void setVol(double);
    void setPan(double);
    void setOut(long);

    static const int MAX_OUTPUT = 100;

private:
    double m_vol = 100;
    double m_pan = 0;
    long m_output = 0;
};

}
}

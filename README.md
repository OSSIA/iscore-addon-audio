# iscore-addon-audio
Audio add-on for i-score

This addon adds audio sequencer features to [i-score](www.i-score.org) by leveraging the [LibAudioStream](https://github.com/sletz/libaudiostream).

Backends supported :
* JACK (Linux / OS X)
* PortAudio (Everywhere where it is available)
* CoreAudio (OS X)

Audio effect formats supported :
* [Faust](http://faust.grame.fr/)
* [LV2](http://lv2plug.in/)

### Installing

#### Dependencies
* i-score
* libsndfile
* faust2 (Last known working commit: bffedc954e43475369dec209ccef53e4020af541)
* Unless you're on OS X, either jack or portaudio
* libsamplerate
* librubberband
* lv2 (version >= 1.14.0)

#### Procedure
You may need to build `lv2` yourself to get 1.14.0 (Ubuntu 16.10 provides 1.12.x) :

    git clone http://lv2plug.in/git/lv2.git
    cd lv2
    git checkout v1.14.0
    ./waf configure
    ./waf
    sudo ./waf install

Clone this repository inside the addons folder of i-score : https://github.com/OSSIA/i-score/tree/master/base/addons

(Re-)run CMake

The addon will be built along i-score.

It adds new processes :

* Sound : a sound file
* Effects : an effect chain
* Send / Return / Mix : audio graph features
* Input : real-time input

### Features :
Changing the speed of the time constraints will scale the pitch accordingly, unless this would affect a real-time input.
Faust effects can be given by either a path to a DSP file, or the Faust code directly.
To ensure that all effects work, the `.lib` files [here](https://github.com/OSSIA/iscore-addon-audio/tree/refactor/network/3rdparty/faust-lib) should be located where the i-score executable is.

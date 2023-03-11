/*
This code borrows heavily from Cycling '74s GenDSP library. The following license applies:

Cycling '74 License for Max-Generated Code for Export
Copyright (c) 2016 Cycling '74
The code that Max generates automatically and that end users are capable of exporting and using, and any
  associated documentation files (the “Software”) is a work of authorship for which Cycling '74 is the author
  and owner for copyright purposes.  A license is hereby granted, free of charge, to any person obtaining a
  copy of the Software (“Licensee”) to use, copy, modify, merge, publish, and distribute copies of the Software,
  and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The Software is licensed to Licensee only for non-commercial use. Users who wish to make commercial use of the
  Software must contact the copyright owner to determine if a license for commercial use is available, and the
  terms and conditions for same, which may include fees or royalties. For commercial use, please send inquiries
  to licensing@cycling74.com.  The determination of whether a use is commercial use or non-commercial use is based
  upon the use, not the user. The Software may be used by individuals, institutions, governments, corporations, or
  other business whether for-profit or non-profit so long as the use itself is not a commercialization of the
  materials or a use that generates or is intended to generate income, revenue, sales or profit.
The above copyright notice and this license shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
  THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL
  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
  CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS IN THE SOFTWARE.
*/

namespace cmags
{

  inline double wrap(double v, double lo1, double hi1)
  {
    double lo;
    double hi;
    if (lo1 == hi1)
      return lo1;
    if (lo1 > hi1)
    {
      hi = lo1;
      lo = hi1;
    }
    else
    {
      lo = lo1;
      hi = hi1;
    }
    const double range = hi - lo;
    if (v >= lo && v < hi)
      return v;
    if (range <= 0.000000001)
      return lo; // no point...
    const long numWraps = long((v - lo) / range) - (v < lo);
    return v - range * double(numWraps);
  }
  inline double sign(double v)
  {
    return v > double(0) ? double(1) : v < double(0) ? double(-1)
                                                     : double(0);
  }
  struct Change
  {
    double history;
    Change() { reset(); }
    inline void reset(double init = 0) { history = init; }

    inline double operator()(double in1)
    {
      double ret = in1 - history;
      history = in1;
      return sign(ret);
    }
  };

  struct RampToTrig
  {
    double history = 0;
    double sub_2, add_3, div_4, abs_5, gt_6, change_7, gt_8, out1;
    Change change; 
    RampToTrig() { reset(); }
    inline void reset(double init = 0) { history = init; }
    inline double operator()(double in1)
    {
      sub_2 = in1 - history;
      add_3 = in1 + history;
      div_4 = sub_2 / add_3;
      abs_5 = abs(div_4);
      gt_6 = abs_5 > 0.5;
      change_7 = change(gt_6);
      history = in1;
      return (change_7 > 0)*10;
    }
  };

  struct Phasor
  {
    double phase;
    Phasor()
    {
      reset();
    }
    void reset(double v = 0.)
    {
      phase = v;
    }
    inline double operator()(double freq, double invsamplerate)
    {
      const double pincr = freq * invsamplerate;
      phase = wrap(phase + pincr, 0., 1.);
      return phase;
    }
  };

}
#ifndef PXSORT_MODULATION_H
#define PXSORT_MODULATION_H

#include <functional>

namespace pxsort::modulator {
    using Modulator = std::function<float(float)>;


    Modulator identity();

    /**
     * Returns a Modulator that applies the sum of the two input modulations to
     * an ellipse's boundary.
     * @param m1
     * @param m2
     * @return
     */
    Modulator sum(const Modulator& m1, const Modulator& m2);

    /**
     * Returns a phase-shifted copy of the given modulator.
     * @param mod
     * @param phase
     * @return
     */
    Modulator phaseShift(const Modulator& mod, float phase);

    /**
     * Returns a Modulator that repeats the given Modulator's pattern the given
     *   number of times along an ellipse boundary.
     * @param mod
     * @param count
     * @return
     */
    Modulator freqShift(const Modulator& mod, float amount);

    /**
     * Applies a sine wave oscillation to the ellipse's boundary.
     * Represents the function: f(x) = sin(hz * (amp + phase)) + translate
     * @param hz Oscillation frequency. Integer values produce a continuous
     *           boundary.
     * @param amp Wave amplitude (i.e. half of the peak-to-trough
     *                  distance).
     * @param bias Wave translate.
     * @param phase Wave phase.
     * @return
     */
    Modulator sinOscillator(float hz, float amp,
                            float bias, float phase);

    /**
     * Applies a saw wave oscillation to the ellipse's boundary.
     * Represents the function: f(x) = saw(hz * (amp + phase)) + translate
     * @param hz Oscillation frequency. Integer values produce a continuous
     *           boundary. Negative values reverse the saw-tooth orientation.
     * @param amp Wave amplitude (i.e. half of the peak-to-trough
     *                  distance).
     * @param bias Wave translate.
     * @param phase Wave phase.
     * @return
     */
    Modulator sawOscillator(float hz, float amp,
                            float bias, float phase);

    /**
     * Applies a triangle wave oscillation to the ellipse's boundary.
     * Represents the function: f(x) = tri(hz * (amp + phase)) + translate
     * @param hz Oscillation frequency. Integer values produce a continuous
     *           boundary.
     * @param amp Wave amplitude (i.e. half of the peak-to-trough
     *                  distance).
     * @param bias Wave translate.
     * @param phase Wave phase.
     * @return
     */
    Modulator triangleOscillator(float hz, float amp,
                                 float bias, float phase);

    /**
     * Applies a square wave oscillation to the ellipse's boundary.
     * Represents the function: f(x) = sq(hz * (amp + phase)) + translate
     * @param hz Oscillation frequency. Integer values produce a continuous
     *           boundary.
     * @param amp Wave amplitude (i.e. half of the peak-to-trough
     *                  distance).
     * @param bias Wave translate.
     * @param phase Wave phase.
     * @return
     */
    Modulator squareOscillator(float hz, float amp,
                               float bias, float phase);

    /**
     * Applies a stepwise function to an ellipse boundary.
     * @param steps Values of each step.
     * @return
     */
    Modulator stepwise(const std::vector<float> &steps);
}

#endif //PXSORT_MODULATION_H

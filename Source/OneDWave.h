/*
  ==============================================================================

    OneDWave.h
    Created: 5 May 2021 9:14:12am
    Author:  Silvin Willemsen

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class OneDWave  : public juce::Component
{
public:
    OneDWave (NamedValueSet& parameters, double k);
    ~OneDWave() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    void calculateScheme();
    void updateStates();
    
    void excite();
    
    double getOutput (double ratio) {
        return u[1][static_cast<int>(round (N * ratio))];
    };

private:
    double k, c, L, h, lambdaSq;
    int N;
    
    std::vector<std::vector<double>> uStates;
    std::vector<double*> u;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OneDWave)
};

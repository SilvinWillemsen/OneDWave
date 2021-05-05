/*
  ==============================================================================

    OneDWave.cpp
    Created: 5 May 2021 9:14:12am
    Author:  Silvin Willemsen

  ==============================================================================
*/

#include <JuceHeader.h>
#include "OneDWave.h"

//==============================================================================
OneDWave::OneDWave (NamedValueSet& parameters, double k) : k (k)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    
    c = *parameters.getVarPointer ("c");
    L = *parameters.getVarPointer ("L");
    sig0 = *parameters.getVarPointer ("sig0");

    h = c * k;
    N = floor (L / h);
    h = L / static_cast<double> (N);
    
    
    lambdaSq = c * c * k * k / (h * h);

    // std::vector<double> (*amountOfElements*, *whatToInitialiseWith*)
    uStates = std::vector<std::vector<double>> (3, std::vector<double> (N+1, 0));
    
    u.resize (3, nullptr);
    
    for (int n = 0; n < 3; ++n)
    {
        u[n] = &uStates[n][0];
    }
    
}

OneDWave::~OneDWave()
{
}

void OneDWave::paint (juce::Graphics& g)
{
    // clear the background
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    
    // choose your favourite colour
    g.setColour (Colours::cyan);
    
    // visualScaling depends on your excitation
    g.strokePath (visualiseState (g, 100), PathStrokeType(2.0f));
}

Path OneDWave::visualiseState (Graphics& g, double visualScaling)
{
    // String-boundaries are in the vertical middle of the component
    double stringBoundaries = getHeight() / 2.0;
    
    // initialise path
    Path stringPath;
    
    // start path
    stringPath.startNewSubPath (0, -u[1][0] * visualScaling + stringBoundaries);
    double spacing = getWidth() / static_cast<double>(N);
    double x = spacing;
    
    for (int l = 1; l <= N; l++)
    {
        // Needs to be -u, because a positive u would visually go down
        float newY = -u[1][l] * visualScaling + stringBoundaries;
        
        if (isnan(x) || isinf(abs(x)) || isnan(u[1][l]) || isinf(abs(u[1][l])))
            std::cout << "Wait" << std::endl;
        // if we get NAN values, make sure that we don’t get an
        // exception
        
        if (isnan(newY))
            newY = 0;
        stringPath.lineTo (x, newY);
        x += spacing;
    }
    return stringPath;
}

void OneDWave::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

void OneDWave::calculateScheme()
{
    // \delta_tt u = c^2 \delta_xx u
    for (int l = 1; l < N; ++l)
    {
        u[0][l] = (2.0 * u[1][l] - u[2][l] + sig0 * k * u[2][l]
                + lambdaSq * (u[1][l+1] - 2.0 * u[1][l] + u[1][l-1]))
                / (1 + sig0 * k);
    }
}

void OneDWave::updateStates()
{
    double* uTmp = u[2];
    u[2] = u[1];
    u[1] = u[0];
    u[0] = uTmp;
}

void OneDWave::excite()
{
    //// Raised cosine excitation ////
    excitationFlag = false;
    
    // width (in grid points) of the excitation
    double width = 10;
    
    // make sure we’re not going out of bounds at the left boundary
    int start = std::max (floor((N+1) * excitationLoc) - floor(width * 0.5), 1.0);
    
    for (int l = 0; l < width; ++l)
    {
        // make sure we’re not going out of bounds
        // at the right boundary (this does ’cut off’
        // the raised cosine)
        
        if (l+start > N - 1)
            break;
        
        u[1][l+start] += 0.5 * (1 - cos(2.0 * double_Pi * l / (width-1.0)));
        u[2][l+start] += 0.5 * (1 - cos(2.0 * double_Pi * l / (width-1.0)));
    }
}

void OneDWave::mouseDown (const MouseEvent& e)
{
    excitationLoc = e.x / static_cast<double> (getWidth());
    excitationFlag = true;
}

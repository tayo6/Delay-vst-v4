#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_audio_processors/juce_audio_processors.h>

// Custom LookAndFeel for the knobs
class DelayLookAndFeel : public juce::LookAndFeel_V4 {
public:
    DelayLookAndFeel() {
        setColour(juce::Slider::thumbColourId, juce::Colour::fromString("FF5EEAD4"));
        setColour(juce::Slider::rotarySliderFillColourId, juce::Colour::fromString("FFDCE6EB"));
        setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour::fromString("FF374151"));
        setColour(juce::Slider::backgroundColourId, juce::Colour::fromString("00000000"));
    }

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                          float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) override {
        auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat();
        auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
        
        // Use animated value if available for smooth ease-out ring fill
        float displayPos = (float) slider.getProperties().getWithDefault("animVal", (double)sliderPos);
        
        auto toAngle = rotaryStartAngle + displayPos * (rotaryEndAngle - rotaryStartAngle);
        auto lineW = 4.0f; 
        auto arcRadius = radius - lineW * 0.5f;

        // Fill
        g.setColour(juce::Colour::fromString("FFDCE6EB"));
        g.fillEllipse(bounds.reduced(lineW));

        // Background Arc
        juce::Path backgroundArc;
        backgroundArc.addCentredArc(bounds.getCentreX(), bounds.getCentreY(), arcRadius, arcRadius, 0.0f,
                                    rotaryStartAngle, rotaryEndAngle, true);
        g.setColour(juce::Colour::fromString("FF374151"));
        g.strokePath(backgroundArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        // Value Arc
        juce::Path valueArc;
        valueArc.addCentredArc(bounds.getCentreX(), bounds.getCentreY(), arcRadius, arcRadius, 0.0f,
                               rotaryStartAngle, toAngle, true);
        g.setColour(slider.findColour(juce::Slider::thumbColourId));
        g.strokePath(valueArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        
        // Thumb/Pointer
        juce::Path p;
        auto pointerLength = radius * 0.33f;
        auto pointerThickness = 2.0f;
        p.addRectangle(-pointerThickness * 0.5f, -arcRadius, pointerThickness, pointerLength);
        p.applyTransform(juce::AffineTransform::rotation(toAngle).translated(bounds.getCentreX(), bounds.getCentreY()));
        g.setColour(juce::Colour::fromString("FF374151"));
        g.fillPath(p);
    }
};

// Animated 3D Wireframe Cube
class WireframeCube : public juce::Component, private juce::Timer {
public:
    WireframeCube(juce::Colour strokeColor, int strokeW) : color(strokeColor), strokeWidth(strokeW) {
        startTimerHz(60);
        angleY = 0; angleX = 0;
    }
    void timerCallback() override {
        angleY += 0.02f;
        angleX += 0.01f;
        float t = juce::Time::getMillisecondCounter() / 1000.0f;
        float period = 3.0f; // 3000ms duration
        offsetY = 8.0f * std::sin(2.0f * juce::MathConstants<float>::pi * t / period);
        repaint();
    }
    void paint(juce::Graphics& g) override {
        g.setColour(color);
        float size = 25.0f;
        auto bounds = getLocalBounds().toFloat();
        float cx = bounds.getCentreX();
        float cy = bounds.getCentreY() + offsetY;

        float vertices[8][3] = {
            {-size, -size, -size}, {size, -size, -size}, {size, size, -size}, {-size, size, -size},
            {-size, -size, size}, {size, -size, size}, {size, size, size}, {-size, size, size}
        };
        float projected[8][2];
        for(int i=0; i<8; ++i) {
            float x = vertices[i][0], y = vertices[i][1], z = vertices[i][2];
            float x1 = x * std::cos(angleY) - z * std::sin(angleY);
            float z1 = x * std::sin(angleY) + z * std::cos(angleY);
            float y1 = y * std::cos(angleX) - z1 * std::sin(angleX);
            float z2 = y * std::sin(angleX) + z1 * std::cos(angleX);
            float scale = 100.0f / (100.0f + z2);
            projected[i][0] = cx + x1 * scale;
            projected[i][1] = cy + y1 * scale;
        }
        int edges[12][2] = {{0,1}, {1,2}, {2,3}, {3,0}, {4,5}, {5,6}, {6,7}, {7,4}, {0,4}, {1,5}, {2,6}, {3,7}};
        juce::Path path;
        for(int i=0; i<12; ++i) {
            path.startNewSubPath(projected[edges[i][0]][0], projected[edges[i][0]][1]);
            path.lineTo(projected[edges[i][1]][0], projected[edges[i][1]][1]);
        }
        g.strokePath(path, juce::PathStrokeType((float)strokeWidth));
    }
private:
    juce::Colour color;
    int strokeWidth;
    float angleY, angleX;
    float offsetY = 0.0f;
};

// Header Container
class Header : public juce::Component {
public:
    Header() {
        addAndMakeVisible(cube1);
        addAndMakeVisible(cube2);
    }
    void paint(juce::Graphics& g) override {
        g.fillAll(juce::Colour::fromString("FF0A0A0A"));
        g.setColour(juce::Colour::fromString("FF5EEAD4"));
        g.setFont(juce::Font(14.0f, juce::Font::bold));
        g.drawText("DELAY", 10, 10, 100, 20, juce::Justification::left);
        juce::Path chevron;
        chevron.startNewSubPath(60.0f, 15.0f); chevron.lineTo(65.0f, 20.0f); chevron.lineTo(70.0f, 15.0f);
        g.strokePath(chevron, juce::PathStrokeType(2.0f));
    }
    void resized() override {
        auto bounds = getLocalBounds();
        auto cubeArea = bounds.removeFromRight(150).reduced(20);
        cube1.setBounds(cubeArea.removeFromLeft(80).reduced(10));
        cube2.setBounds(cubeArea.reduced(20));
    }
private:
    WireframeCube cube1 { juce::Colour::fromString("FF5EEAD4"), 2 };
    WireframeCube cube2 { juce::Colour::fromString("FF5EEAD4"), 2 };
};

// Animated Knob with Slider Rotation, Hover Scale, and Ring Fill Animation
class AnimatedKnob : public juce::Slider, private juce::Timer {
public:
    AnimatedKnob(const juce::String& name, const juce::String& valText = "") 
        : knobName(name), valueText(valText) {
        setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
        getProperties().set("animVal", 0.0);
        currentDrawVal = 0.0f; targetDrawVal = 0.0f;
    }
    void mouseEnter(const juce::MouseEvent&) override { targetScale = 1.05f; startTimerHz(60); }
    void mouseExit(const juce::MouseEvent&) override { targetScale = 1.0f; startTimerHz(60); }
    void valueChanged() override { targetDrawVal = (float)valueToProportionOfLength(getValue()); startTimerHz(60); }

    void timerCallback() override {
        bool shouldRepaint = false;
        float scaleDiff = targetScale - currentScale;
        if (std::abs(scaleDiff) > 0.001f) { currentScale += scaleDiff * 0.2f; shouldRepaint = true; } 
        else currentScale = targetScale;
        
        float valDiff = targetDrawVal - currentDrawVal;
        if (std::abs(valDiff) > 0.001f) { 
            currentDrawVal += valDiff * 0.2f; 
            getProperties().set("animVal", (double)currentDrawVal); 
            shouldRepaint = true; 
        } else { currentDrawVal = targetDrawVal; getProperties().set("animVal", (double)currentDrawVal); }
        
        if (shouldRepaint) repaint(); else stopTimer();
    }
    
    void paint(juce::Graphics& g) override {
        auto bounds = getLocalBounds().toFloat();
        juce::Graphics::ScopedSaveState state(g);
        g.addTransform(juce::AffineTransform::scale(currentScale, currentScale, bounds.getCentreX(), bounds.getCentreY()));
        juce::Slider::paint(g);
        
        if (valueText.isNotEmpty()) {
            g.setColour(juce::Colour::fromString("FF6B7280")); g.setFont(12.0f);
            g.drawText(valueText, getLocalBounds().removeFromBottom(15), juce::Justification::centred);
        }
        g.setColour(juce::Colour::fromString("FF111827")); g.setFont(10.0f);
        g.drawText(knobName, getLocalBounds().removeFromTop(15), juce::Justification::centred);
    }
private:
    juce::String knobName, valueText;
    float currentScale = 1.0f, targetScale = 1.0f;
    float currentDrawVal = 0.0f, targetDrawVal = 0.0f;
};

// Level Meter
class LevelMeter : public juce::Component {
public:
    void setLevel(float newLevel) { level = newLevel; repaint(); }
    void paint(juce::Graphics& g) override {
        auto bounds = getLocalBounds().toFloat();
        g.setColour(juce::Colour::fromString("FFE5E7EB")); g.fillRoundedRectangle(bounds, 2.0f);
        int segments = 20; float segHeight = bounds.getHeight() / (float)segments; float gap = 2.0f;
        for(int i=0; i<segments; ++i) {
            float segTop = bounds.getHeight() - (i + 1) * segHeight + gap;
            float segLevel = (float)i / (float)segments;
            juce::Colour c = (segLevel > 0.8f) ? juce::Colour::fromString("FFFB923C") : 
                             (segLevel > 0.6f) ? juce::Colour::fromString("FFFACC15") : 
                             juce::Colour::fromString("FF4ADE80");
            g.setColour((segLevel <= level) ? c : juce::Colour::fromString("FFE5E7EB"));
            g.fillRoundedRectangle(bounds.getX(), bounds.getY() + segTop, bounds.getWidth(), segHeight - gap, 1.0f);
        }
    }
private:
    float level = 0.0f;
};

// Toggle Switch
class ToggleSwitch : public juce::Component, private juce::Timer {
public:
    ToggleSwitch(juce::String lLeft, juce::String lRight, juce::Colour activeCol) 
        : labelLeft(lLeft), labelRight(lRight), activeColor(activeCol) { startTimerHz(60); }
    void paint(juce::Graphics& g) override {
        auto bounds = getLocalBounds().toFloat();
        g.setColour(juce::Colour::fromString("FFE5E7EB")); g.fillRoundedRectangle(bounds, 10.0f);
        float knobX = state ? bounds.getWidth() - bounds.getHeight() + 2.0f : 2.0f;
        currentX += (knobX - currentX) * 0.2f;
        g.setColour(activeColor);
        g.fillEllipse(currentX, 2.0f, bounds.getHeight() - 4.0f, bounds.getHeight() - 4.0f);
        g.setColour(juce::Colour::fromString("FF6B7280")); g.setFont(10.0f);
        g.drawText(labelLeft, 0, 0, (int)(bounds.getWidth()/2), (int)bounds.getHeight(), juce::Justification::centred);
        g.drawText(labelRight, (int)(bounds.getWidth()/2), 0, (int)(bounds.getWidth()/2), (int)bounds.getHeight(), juce::Justification::centred);
    }
    void mouseDown(const juce::MouseEvent&) override { state = !state; repaint(); }
    void timerCallback() override { repaint(); }
private:
    juce::String labelLeft, labelRight; juce::Colour activeColor; bool state = false; float currentX = 2.0f;
};

// Icon Button
class IconButton : public juce::Component {
public:
    IconButton(juce::String name) : label(name) {}
    void paint(juce::Graphics& g) override {
        g.setColour(juce::Colour::fromString("FFE5E7EB")); g.fillRoundedRectangle(getLocalBounds().toFloat(), 5.0f);
        g.setColour(juce::Colour::fromString("FF374151")); g.setFont(10.0f);
        g.drawText(label, getLocalBounds(), juce::Justification::centred);
    }
private:
    juce::String label;
};
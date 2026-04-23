// Copyright 2008-2022 by Rightware. All rights reserved.

#include <kanzi/kanzi.hpp>
#include <kanzi/core.ui/platform/graphics_backend/gl/gl_graphics_output.hpp>


using namespace kanzi;


// Application class.
// Implements application logic.
class CoinApplication : public ExampleApplication
{
public:

    // Constructor.
    CoinApplication():
        m_filteredDeltaTime(-1.0)
    {
    }

protected:

    // Configures application.
    void onConfigure(ApplicationProperties& configuration) override
    {
        ExampleApplication::onConfigure(configuration);
        configuration.binaryName = "Coin.kzb.cfg";
        configuration.extensionOutputEnabled = true;
        configuration.defaultSurfaceProperties.antiAliasing = 0;
        configuration.defaultSurfaceProperties.bitsDepthBuffer = 0;
        configuration.defaultSurfaceProperties.bitsStencil = 0;

        getMainLoopScheduler().appendTask(UserStage,
                                          kzMakeFixedString("updateFPS"),
                                          MainLoopScheduler::TaskRecurrence::Recurring,
                                          [this](auto) { this->updateFPS(); });
    }

    void resumeGLOverride() override
    {
        Application::resumeGLOverride();
        getRenderer3D()->getCoreRenderer()->logOpenGLInformation((unsigned int)Renderer::InformationAll);

        GLGraphicsOutputSharedPtr glGraphicsOutput = dynamic_pointer_cast<GLGraphicsOutput>(getGraphicsOutput());
        if (glGraphicsOutput)
        {
            glGraphicsOutput->swapInterval(0);
        }
    }

    // Called every frame during application update phase.
    // Calculates frame time and FPS and outputs it to the screen.
    void updateFPS()
    {
        const auto lastFrameTimeInMs = chrono::duration_cast<chrono::milliseconds>(getMainLoopScheduler().getLastFrameDuration()).count();
        const auto fps = kzuEngineGetFramesPerSecond(getEngine());

        // Calculate frame time and FPS.
        const auto dt = static_cast<float>(lastFrameTimeInMs);
        if (m_filteredDeltaTime < 0.0f)
        {
            m_filteredDeltaTime = dt;
        }
        else
        {
            m_filteredDeltaTime = 0.95f * m_filteredDeltaTime + 0.05f * dt;
        }

        // Show frame time and FPS in #FrameTime text block.
        {
            Node2DSharedPtr root = getRoot();
            if (root)
            {
                stringstream ss;
                ss.precision(1);
                ss << std::fixed << m_filteredDeltaTime << " ms ~ " << fps << " fps";

                root->lookupNode<TextBlock2D>("#FrameTime")->setText(ss.str());
            }
        }
    }

private:

    // Filtered frame time.
    float m_filteredDeltaTime;
};


// Creates application instance. Called by framework.
Application* createApplication()
{
    return new CoinApplication;
}

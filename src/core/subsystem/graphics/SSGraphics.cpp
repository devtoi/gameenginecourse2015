#include "SSGraphics.h"
#include <utility/Config.h>
#include <gfx/GraphicsEngine.h>
#include <gfx/RenderQueue.h>
#include <gfx/ModelBank.h>
#include <gfx/LightEngine.h>
#include "../../camera/CameraFirstPerson.h"
#include <glm/gtx/transform.hpp>
const pString SSGraphics::Name = "Graphics";
int SSGraphics::ID = -1;

void SSGraphics::Startup( SubsystemCollection* const subsystemCollection ) {
	m_GraphicsEngine = pNew(gfx::GraphicsEngine);
	Config gfxConfig;
	gfxConfig.ReadFile("../../../cfg/gfx.cfg");
	gfx::GraphicsSettings gs;
	gs.Width = gfxConfig.GetInt("ScreenWidth", 1600);
	gs.Height = gfxConfig.GetInt("ScreenHeight", 900);
	m_GraphicsEngine->Initialize(gs);
	m_RenderQueue = m_GraphicsEngine->GetRenderQueue();

	m_TestModel = gfx::g_ModelBank.LoadModel("../../../asset/cube.obj");
	gfx::g_ModelBank.BuildBuffers();

	m_Camera = pNew(CameraFirstPerson);
	m_Camera->CalculateViewProjection();
}

void SSGraphics::Shutdown( SubsystemCollection* const subsystemCollection ) {
	// Perform Cleanup here (Don't forget to set shutdown order priority!)
}

void SSGraphics::UpdateUserLayer( const float deltaTime ) {
	m_Camera->Update(deltaTime);
	m_Camera->CalculateViewProjection();
	gfx::ShaderInput input;
	std::vector<gfx::ShaderInput> inputList;
	for (int x = -10; x < 10; x+= 2) {
		input.World = glm::translate(glm::vec3(x, 0, -25)) * glm::rotate(-30.0f, glm::vec3(-0.5f,-1, 0));
		input.Color = glm::vec4(1);
		inputList.push_back(input);
	}
	m_RenderQueue->Enqueue(m_TestModel, inputList);

	gfx::Light dl;
	dl.Color = glm::vec4(0.27f,0.33f,0.39f, 1.0f);
	dl.Direction = glm::vec3(0) - glm::vec3(5,0.6186f, -5);
	gfx::g_LightEngine.AddDirLightToQueue(dl);

	gfx::View v;
	v.camera.Far = m_Camera->GetEditableLens().Far;
	v.camera.Near = m_Camera->GetEditableLens().Near;
	v.camera.Forward = m_Camera->GetForward();
	v.camera.Position = m_Camera->GetPosition();
	v.camera.Proj = m_Camera->GetProjection();
	v.camera.ProjView = m_Camera->GetViewProjection();
	v.camera.Right = m_Camera->GetRight();
	v.camera.Up = m_Camera->GetUp();
	v.camera.View = m_Camera->GetView();
	v.viewport.x = 0;
	v.viewport.y = 0;
	v.viewport.width = 1600;
	v.viewport.height = 900;
	m_RenderQueue->AddView(v);

	m_GraphicsEngine->Draw();

	m_RenderQueue->Clear();
	gfx::g_LightEngine.ClearDirectionalLights();
}

void SSGraphics::UpdateSimulationLayer( const float timeStep ) {
	// Perform simulation update logic here (Don't forget to set update order priority!)
}

Subsystem* SSGraphics::CreateNew( ) const {
	return pNew( SSGraphics, SSGraphics::ID );
}

int SSGraphics::GetStaticID() {
	return SSGraphics::ID;
}

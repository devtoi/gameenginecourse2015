#pragma once
namespace gfx {
	class RenderQueue;
	class GBuffer;
	class DeferedDecalProgram {
	public:
		DeferedDecalProgram();
		~DeferedDecalProgram();

		void Initialize();
		void Render(RenderQueue* rq, GBuffer* gbuffer);
	private:
		unsigned int m_Model;
		unsigned int m_ShaderProg;
	};
}

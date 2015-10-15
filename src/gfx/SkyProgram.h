#pragma once

namespace gfx {
class Texture;
typedef int ModelHandle;
typedef int ShaderProgramHandle;
struct CameraData;
class SkyProgram {
  public:
	SkyProgram();
	~SkyProgram();
	void Init();
	void Render(const CameraData& camera);
	void SetSkyTexture(const char* filename);
	void SetSkyTexture(Texture* tex);
  private:
	ModelHandle			m_Model;
	Texture*			m_Tex = nullptr;
	ShaderProgramHandle m_Shader;

};
}
#include <GL/glew.h>
namespace gfx {
	class Texture;
	typedef int TextureHandle;
class FrameBuffer {
  public:
	FrameBuffer();
	~FrameBuffer();
	void Init();
	void Apply();
	void SetTexture(Texture* tex);
	void SetTexture(GLuint tex, int width,int height);
	Texture* GetTexture();
  private:
	GLuint m_Handle;
	GLuint m_Width;
	GLuint m_Height;
	Texture* m_TargetTexture = nullptr;
};
}

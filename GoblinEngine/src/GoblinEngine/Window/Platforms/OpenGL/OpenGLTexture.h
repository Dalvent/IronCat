#pragma once
#include "GoblinEngine/Window/Render/Texture.h"
#include "GoblinEngine/File/Image.h"
#include <string>

namespace GoblinEngine
{
	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(const Image& image);
		~OpenGLTexture2D();
		
		unsigned int GetId() { return _id; }

		unsigned int GetWidth() const override { return _width; }
		unsigned int GetHeight() const override { return _height; }

		void Bind(unsigned int slot = 0) const override;
	private:
		unsigned int _id;
		unsigned int _width;
		unsigned int _height;
	};
}
#pragma once

namespace dive
{
	class Vector4
	{
	public:
		Vector4()
		{
			x = 0.0f;
			y = 0.0f;
			z = 0.0f;
			w = 0.0f;
		}

		Vector4(float x, float y, float z, float w)
		{
			this->x = x;
			this->y = y;
			this->z = z;
			this->w = w;
		}

		~Vector4() = default;

	private:
		float x, y, z, w;

	};
}
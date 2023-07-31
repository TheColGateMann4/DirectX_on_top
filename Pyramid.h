// #pragma once
// #include "Shape.h"
// #include "Graphics.h"
// #include "SimpleMesh.h"
// #include <random>
// 
// class Pyramid : public Shape
// {
// public:
// 	Pyramid(GFX& gfx,
// 		std::mt19937& rng,
// 		std::uniform_real_distribution<float>& adist,
// 		std::uniform_real_distribution<float>& ddist,
// 		std::uniform_real_distribution<float>& odist,
// 		std::uniform_real_distribution<float>& rdist);
// 
// 
// public:
// 	DirectX::XMMATRIX GetTranformMatrix() const noexcept override;
// 
// private:
// 	template<class T>
// 	static SimpleMesh<T> GetMesh();
// 
// private:
// 	// positional
// 	FLOAT r;
// 	FLOAT roll = 0.0f;
// 	FLOAT pitch = 0.0f;
// 	FLOAT yaw = 0.0f;
// 	FLOAT theta;
// 	FLOAT phi;
// 	FLOAT chi;
// 
// 	// speed (delta/s)
// 	FLOAT droll;
// 	FLOAT dpitch;
// 	FLOAT dyaw;
// 	FLOAT dtheta;
// 	FLOAT dphi;
// 	FLOAT dchi;
// };
// 

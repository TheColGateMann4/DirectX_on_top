#include "Skybox.h"
#include "SimpleMesh.h"
#include "Cube.h"
#include "BindableClassesMacro.h"
#include "SkyboxTransformBuffer.h"
#include "CubeMapTexture.h"

Skybox::Skybox(class GFX& gfx, float scale, std::string skyboxTexture)
	:
	SceneObject({0.0f, 0.0f, 0.0f})
{
	SetShape(this);


	SimpleMesh skyboxModel = Cube::GetInsideDrawnMesh(scale);

	m_pIndexBuffer = IndexBuffer::GetBindable(gfx, GetName(), skyboxModel.m_indices);
	m_pVertexBuffer = VertexBuffer::GetBindable(gfx, GetName(), skyboxModel.m_vertices);
	m_pTopology = Topology::GetBindable(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pTransformConstBuffer = SkyboxTransformBuffer::GetBindable(gfx, *this, { {TargetVertexShader, 0} });

	{
		RenderTechnique shadowTechnique("skybox");

		{
			RenderStep shadowStep("skyboxPass");

			std::shared_ptr<VertexShader> pVertexShader = VertexShader::GetBindable(gfx, "VS_Skybox.cso");

 			shadowStep.AddBindable(InputLayout::GetBindable(gfx, skyboxModel.GetLayout(), pVertexShader.get()));

			shadowStep.AddBindable(CubeMapTexture::GetBindable(gfx, "Images\\SpaceSkybox\\Space.png", 7));

			shadowStep.AddBindable(std::move(pVertexShader));

			shadowTechnique.AddRenderStep(shadowStep);
		}

		AddRenderTechnique(shadowTechnique);
	}
}
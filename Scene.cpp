#include "Scene.h"
#include "PointLight.h"

void Scene::LinkModelsToPipeline(class RenderGraph& renderGraph)
{
	for (auto& model : models)
		model->LinkSceneObjectToPipeline(renderGraph);
}

void Scene::DrawModels(GFX& gfx, DirectX::XMMATRIX CameraView_)
{
	for (const auto& model : models)
		if(const PointLight* pointLight = dynamic_cast<PointLight*>(model.get()))
		{
			// its fine only when PointLight is first in hierarchy, otherwise objects might not get light buffer with position and light settings.
			// If it wouldn't be first, we would need to first loop searching for light to bind it
			pointLight->Bind(gfx, CameraView_);
			pointLight->RenderOnScene();
		}
		else
			model->RenderOnScene();
}

void Scene::DrawModelHierarchy(float deltaTime)
{
	if (ImGui::Begin("Object Controler"))
	{
		if (ImGui::Button("Import Model"))
		{
			std::string filePathsMultiSelect(MAX_PATH, '\0');

			if (m_window->OpenFileExplorer(&filePathsMultiSelect) != 0)
			{
				std::vector<std::string> filePaths = {};

				if (m_window->MultiselectToFilePaths(&filePathsMultiSelect, &filePaths) != 0)
					for (auto& filePath : filePaths)
					{
						models.push_back(std::make_unique<Model>(m_window->Graphics, filePath, 1.0f));
					}
			}
		}

		// could remake this hierarchy stuff, but instead faster way is to check pressed nodes in here

		for (auto& model : models)
			model->MakeHierarchy(m_window->Graphics);

		CleanupPressedNodes(); // doing this between hierarchy and propeties lets us smoothly change pressed nodes without any flicking propeties(two are showing at once for 1 frame)

		ImGui::NextColumn();

		for (auto& model : models)
		{
			model->MakeTransformPropeties(m_window->Graphics);
			model->MakePropeties(m_window->Graphics);
			model->MakeAdditionalPropeties(m_window->Graphics, deltaTime);
		}

		ImGui::End();
	}
}

void Scene::CleanupPressedNodes()
{
	SceneObject* previousObject = nullptr;

	for (auto& model : models)
	{
		if (!model->GetPressedState())
			continue;

		if (model.get() != this->m_pressedNode)
		{
			if (previousObject != nullptr)
			{
				if (Model* mymodel = dynamic_cast<Model*>(previousObject))
					mymodel->m_pressedNode = nullptr;

				previousObject->SetPressedState(false);
			}

			previousObject = model.get();
		}
		else
		{
			if (previousObject != nullptr)
			{
				if(Model* mymodel = dynamic_cast<Model*>(model.get()))
					mymodel->m_pressedNode = nullptr;

				model->SetPressedState(false);
			}
			else
				previousObject = model.get();
		}
	}

	if (previousObject != nullptr)
		this->m_pressedNode = previousObject;
}
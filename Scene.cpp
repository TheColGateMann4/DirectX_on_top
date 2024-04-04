#include "Scene.h"
#include "PointLight.h"
#include "Window.h"
#include "Model.h"
#include "Camera.h"
#include "Window.h"

Scene::Scene(Window* window)
	: m_window(window), m_cameraManager(m_window->Graphics)
{

}

void Scene::LinkModelsToPipeline(class RenderGraph& renderGraph)
{
	for (auto& model : m_models)
		model->LinkSceneObjectToPipeline(renderGraph);
}

void Scene::DrawModels(GFX& gfx)
{
	for (const auto& model : m_models)
		if(const PointLight* pointLight = dynamic_cast<PointLight*>(model.get()))
		{
			// its fine only when PointLight is first in hierarchy, otherwise objects might not get light buffer with position and light settings.
			// If it wouldn't be first, we would need to first loop searching for light to bind it
			pointLight->Bind(gfx, m_cameraManager.GetActiveCamera()->GetCameraView());
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
						m_models.push_back(std::make_unique<Model>(m_window->Graphics, filePath, 1.0f));
					}
			}
		}

		// could remake this hierarchy stuff, but instead faster way is to check pressed nodes in here

		for (auto& model : m_models)
			model->MakeHierarchy(m_window->Graphics);

		CleanupPressedNodes(); // doing this between hierarchy and propeties lets us smoothly change pressed nodes without any flicking propeties(two are showing at once for 1 frame)

		ImGui::NextColumn();

		for (auto& model : m_models)
		{
			model->MakeTransformPropeties(m_window->Graphics);
			model->MakePropeties(m_window->Graphics);
			model->MakeAdditionalPropeties(m_window->Graphics, deltaTime);
		}

		ImGui::End();
	}
}

CameraManager* Scene::GetCameraManager()
{
	return &m_cameraManager;
}

void Scene::AddCameraObject(std::unique_ptr<Camera>&& model)
{
	// we want to set active only first camera that will appear on scene
	bool activeCameraIsBound = m_cameraManager.GetActiveCamera() != nullptr;
	
	m_cameraManager.AddCamera(model.get(), !activeCameraIsBound);

	AddSceneObject(std::move(model));
}

void Scene::AddSceneObject(std::unique_ptr<SceneObject>&& model)
{
	size_t currentIndex = 0;
	std::string modelName = model->GetName();
	size_t startingLength = modelName.length(); // for optimization to avoid lots of looping

	for (size_t i = 0; i < m_models.size();)
	{
		const auto& sceneModel = m_models.at(i);

		if (sceneModel->GetOriginalName(false) == modelName) [[unlikely]]
		{
			currentIndex++;

			if (currentIndex == 1)
			{
				modelName.append(std::string('_' + std::to_string(currentIndex)));
			}
			else
			{
				modelName.replace(modelName.begin() + startingLength, modelName.end(), std::string('_' + std::to_string(currentIndex)));
			}

			//go to the start of the loop
			i = 0;
			continue;
		}
		else [[likely]]
		{
			i++;
		}
	}

	model->SetSceneIndex(currentIndex);

	m_models.push_back(std::move(model));
}

void Scene::CleanupPressedNodes()
{
	SceneObject* previousObject = nullptr;

	for (auto& model : m_models)
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
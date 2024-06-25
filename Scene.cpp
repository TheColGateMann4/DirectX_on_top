#include "Scene.h"
#include "PointLight.h"
#include "Window.h"
#include "Model.h"
#include "Camera.h"
#include "Window.h"
#include "SceneVisibilityManager.h"

Scene::Scene(Window* window)
	: m_window(window)
{
	m_sceneVisibilityManager = std::make_unique<SceneVisibilityManager>(m_window->Graphics);
}

void Scene::LinkModelsToPipeline(class RenderGraph& renderGraph)
{
	for (auto& model : m_models)
	{
		model->LinkSceneObjectToPipeline(renderGraph);
		model->LinkChildrenToPipeline(renderGraph);
	}
}

void Scene::UpdateSceneVisibility(GFX& gfx)
{
	m_sceneVisibilityManager->ProcessVisibilityBuffer(gfx, m_cameraManager.GetActiveCamera(), GetHighestIndex(), &objectValidity);

	for (const auto& model : m_models)
		if (model->m_shape != nullptr)
			model->SetVisibility(m_sceneVisibilityManager->GetVisibility(model->m_sceneIndex));
}

void Scene::UpdateModels(GFX& gfx, float deltaTime)
{
	for (auto& light : m_lights)
		light->Update(gfx, deltaTime);

	for (auto& model : m_models)
		model->Update(gfx, deltaTime);
}

void Scene::DrawModels(GFX& gfx)
{
	for (const auto& light : m_lights)
		light->Bind(gfx, m_cameraManager.GetActiveCamera()->GetCameraView());

	//updating object's transform and pushing jobs to passes
	for (const auto& model : m_models) 
	{
		model->CalculateSceneTranformMatrix();
		
		if(model->m_shape != nullptr) // checking if object is valid for drawing before doing unnecessary performence stuff for it
			m_sceneVisibilityManager->PushObjectMatrixToBuffer(model->GetSceneTranformMatrix(), model->m_sceneIndex);

		model->RenderOnScene();
	}

	m_sceneVisibilityManager->UpdateTransformBuffer(gfx);

	//normally we would update scene visility now, but since we have shadow pass we don't need it since it runs before everything and overrides everything we would set right now
	//UpdateSceneVisibility(gfx);
}

void Scene::DrawModelHierarchy(bool show)
{
	if (!show)
		return;

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

		for (auto& model : m_models)
			model->MakeHierarchy(m_window->Graphics);

		CleanupPressedNodes(); // doing this between hierarchy and propeties lets us smoothly change pressed nodes without any flicking propeties(two are showing at once for 1 frame)

		ImGui::NextColumn();

		if(m_pressedNode != nullptr)
		{
			m_pressedNode->MakeTransformPropeties(m_window->Graphics);
			m_pressedNode->MakePropeties(m_window->Graphics);
			m_pressedNode->MakeAdditionalPropeties(m_window->Graphics);
		}
	}
	ImGui::End();
}

CameraManager* Scene::GetCameraManager()
{
	return &m_cameraManager;
}

void Scene::AddLightObject(PointLight* model)
{
	m_lights.push_back(model);
}

void Scene::AddCameraObject(Camera* model)
{
	// we want to set active only first camera that will appear on scene
	bool activeCameraIsBound = m_cameraManager.GetActiveCamera() != nullptr;
	
	m_cameraManager.AddCamera(model, !activeCameraIsBound);
}

void Scene::AddSceneObject(std::unique_ptr<SceneObject>&& model)
{
	size_t currentNameIndex = 0;
	{
		std::string modelName = model->GetName();
		size_t startingLength = modelName.length(); // for optimization to avoid lots of looping

		for (size_t i = 0; i < m_models.size();)
		{
			const auto& sceneModel = m_models.at(i);

			if (sceneModel->GetOriginalName(false) == modelName) [[unlikely]]
			{
				currentNameIndex++;

				if (currentNameIndex == 1)
				{
					modelName.append(std::string('_' + std::to_string(currentNameIndex)));
				}
				else
				{
					modelName.replace(modelName.begin() + startingLength, modelName.end(), std::string('_' + std::to_string(currentNameIndex)));
				}

				//go to the start of the loop
				i = 0;
				continue;
			}

			i++;
		}
	}

	bool isValidVisibleObject = model->m_shape != nullptr; // make this for Model and children of objects overall

	m_highestSceneIndex++;

	objectValidity.resize(m_highestSceneIndex + 1);

	objectValidity.at(m_highestSceneIndex) = static_cast<UINT8>(isValidVisibleObject);

	model->SetSceneIndexes(m_highestSceneIndex, currentNameIndex);

	m_sceneVisibilityManager->ResizeBuffers(m_window->Graphics, m_highestSceneIndex + 1);

	if (isValidVisibleObject) 
		model->GenerateBoundCube(m_window->Graphics, m_sceneVisibilityManager->GetCubeBoundsUAV());

	if (auto* light = dynamic_cast<PointLight*>(model.get()))
	{
		AddLightObject(light);
	}
	else if (auto* camera = dynamic_cast<Camera*>(model.get()))
	{
		camera->SetCameraManagerLinkage(&m_cameraManager);

		AddCameraObject(camera);
	}

	m_models.push_back(std::move(model));
}

std::vector<PointLight*>& Scene::GetLights()
{
	return m_lights;
}

UINT32 Scene::GetHighestIndex()
{
	return m_highestSceneIndex;
}

void Scene::CleanupPressedNodes()
{
	SceneObject* previousObject = nullptr;

	for (auto& model : m_models)
	{
		if (model->m_pressedNode == nullptr)
			continue;

		if (model->m_pressedNode != this->m_pressedNode)
		{
			if (previousObject != nullptr)
			{
				if (previousObject->m_pressedNode != nullptr)
				{
					previousObject->m_pressedNode->SetPressedState(false);

					previousObject->m_pressedNode = nullptr;
				}

				previousObject->SetPressedState(false);
			}

			previousObject = model.get();
		}
		else
		{
			if (previousObject != nullptr)
			{
				model->m_pressedNode->SetPressedState(false);

				model->m_pressedNode = nullptr;
			}
			else
				previousObject = model.get();
		}
	}

	if (previousObject != nullptr)
		this->m_pressedNode = previousObject->m_pressedNode;
}
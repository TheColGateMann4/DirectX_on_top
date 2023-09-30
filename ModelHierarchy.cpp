#include "ModelHierarchy.h"


void ModelHierarchy::DrawModels(GFX& gfx)
{
	for (auto& model : models)
		model->Draw(gfx);
}

void ModelHierarchy::DrawModelHierarchy(GFX& gfx)
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
	}

	// could remake this hierarchy stuff, but instead faster way is to check pressed nodes in here

	for (auto& model : models)
		model->MakeHierarchy(gfx);

	CleanupPressedNodes(); // doing this between hierarchy and propeties lets us smoothly change pressed nodes without any flicking propeties(two are showing at once for 1 frame)

	ImGui::NextColumn();

	for (auto& model : models)
		model->MakePropeties(gfx);

	ImGui::End();
}

void ModelHierarchy::CleanupPressedNodes()
{
	Node** previousNode = nullptr;

	for (auto& model : models)
	{
		if (model->m_pressedNode == nullptr)
			continue;

		if (model->m_pressedNode != this->m_pressedNode)
		{
			if (previousNode != nullptr)
				*previousNode = nullptr;

			previousNode = &model->m_pressedNode;
		}
		else
		{
			if (previousNode != nullptr)
				model->m_pressedNode = nullptr;
			else
				previousNode = &model->m_pressedNode;
		}
	}

	if (previousNode != nullptr)
		this->m_pressedNode = *previousNode;
}
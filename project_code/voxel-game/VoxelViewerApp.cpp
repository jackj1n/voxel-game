#include "VoxelViewerApp.hpp"

#include <iostream>
#include "gloo/external.hpp"
#include "gloo/cameras/ArcBallCameraNode.hpp"
#include "gloo/debug/AxisNode.hpp"
#include "gloo/lights/AmbientLight.hpp"
#include "gloo/lights/DirectionalLight.hpp"
#include "gloo/components/LightComponent.hpp"

#include <chrono>

#include "gloo/Scene.hpp"
#include "gloo/components/MaterialComponent.hpp"

#include "hittable/Sphere.hpp"
#include "Tracer.hpp"
#include "SceneParser.hpp"
#include "ArgParser.hpp"

namespace {
	const std::string WINDOW_TITLE = "Voxel Viewer";
	const int WINDOW_WIDTH = 800;
	const int WINDOW_HEIGHT = 600;
	const std::vector<std::string> parameters = {
		"bounces", "size", "file",
	};
}

namespace GLOO{
	VoxelViewerApp::VoxelViewerApp() 
		: Application(WINDOW_TITLE, glm::ivec2(WINDOW_HEIGHT, WINDOW_WIDTH)) {
		bounces_ = 0;
		size_ = 100;
		shadows_ = false;

	}

	void VoxelViewerApp::SetupScene() {
		SceneNode& root = scene_->GetRootNode();
		auto camera_node = make_unique<ArcBallCameraNode>(45.f, 0.75f, 3.f);
		root.AddChild(std::move(camera_node));

		root.AddChild(make_unique<AxisNode>(';'));

		auto ambient_light = std::make_shared<AmbientLight>();
		ambient_light->SetAmbientColor(glm::vec3(0.2f));
		root.CreateComponent<LightComponent>(ambient_light);

		auto sun_light = std::make_shared<DirectionalLight>();
		sun_light->SetDiffuseColor(glm::vec3(0.6f, 0.6f, 0.6f));
		sun_light->SetSpecularColor(glm::vec3(0.6f, 0.6f, 0.6f));
		sun_light->SetDirection(glm::vec3(-1.0f, -0.8f, -1.0f));
		auto sun_light_node = make_unique<SceneNode>();
		sun_light_node->CreateComponent<LightComponent>(sun_light);
		root.AddChild(std::move(sun_light_node));
	}

	void VoxelViewerApp::DrawGUI() {
		static char input[128] = "scene0";
		std::string input_file_name;
		std::string output_file_name;
		ImGui::Begin("Control Panel");
		ImGui::InputText("file", input, IM_ARRAYSIZE(input));
		ImGui::InputInt("bounces", &bounces_);
		ImGui::InputInt("size", &size_);
		if (ImGui::Button("Load")) {
			input_file_name.assign(input);
			output_file_name = input_file_name.substr(0, input_file_name.find_last_of(".")) + ".png";
			SceneParser scene_parser;
			auto scene = scene_parser.ParseScene("voxel-game/" + input_file_name);
			Tracer tracer(scene_parser.GetCameraSpec(),
				glm::ivec2(size_, size_),
				bounces_, scene_parser.GetBackgroundColor(),
				scene_parser.GetCubeMapPtr(), shadows_);
			tracer.Render(*scene, output_file_name);
		}
		ImGui::End();
	}



}
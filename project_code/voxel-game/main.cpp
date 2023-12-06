#include <iostream>
#include <chrono>

#include "gloo/Scene.hpp"
#include "gloo/components/MaterialComponent.hpp"

#include "hittable/Sphere.hpp"
#include "Tracer.hpp"
#include "SceneParser.hpp"
#include "ArgParser.hpp"
#include "VoxelViewerApp.hpp"

using namespace GLOO;

int main(int argc, const char* argv[]) {
    ArgParser arg_parser(argc, argv);
    SceneParser scene_parser;
    auto scene = scene_parser.ParseScene("voxel-game/" + arg_parser.input_file);

    /*Tracer tracer(scene_parser.GetCameraSpec(),
                glm::ivec2(arg_parser.width, arg_parser.height),
                arg_parser.bounces, scene_parser.GetBackgroundColor(),
                scene_parser.GetCubeMapPtr(), arg_parser.shadows);
    tracer.Render(*scene, arg_parser.output_file);*/

    std::unique_ptr<VoxelViewerApp> app =
	    make_unique<VoxelViewerApp>();
	app->SetupScene();
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint =
        std::chrono::time_point<Clock, std::chrono::duration<double>>;
    TimePoint last_tick_time = Clock::now();
    TimePoint start_tick_time = last_tick_time;
    while (!app->IsFinished()) {
        TimePoint current_tick_time = Clock::now();
        double delta_time = (current_tick_time - last_tick_time).count();
        double total_elapsed_time = (current_tick_time - start_tick_time).count();
        app->Tick(delta_time, total_elapsed_time);
        last_tick_time = current_tick_time;
    }

  return 0;
}

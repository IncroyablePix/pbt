#include "SuperPointFilter.h"

SuperPointFilter::SuperPointFilter(const std::filesystem::path &modelPath)
{
    torch::jit::script::Module module;
    try
    {
        // Deserialize the ScriptModule from a file using torch::jit::load().
        module = torch::jit::load(modelPath.string().c_str());
    }
    catch (const c10::Error& e)
    {
        std::cerr << "error loading the model\n";
    }

    std::cout << "ok\n";
}

void SuperPointFilter::operator()(Image &image)
{

}

#include <Empy.h>
using namespace Empy;

struct Editor : AppInterface
{
    EMPY_INLINE void OnUpdate() 
    {
       
    }

    EMPY_INLINE void OnStart() 
    {
       
    }
};

int32_t main(int32_t argc, char** argv) 
{
    auto app = new Application();
    app->AttachLayer<Editor>();
    app->RunContext();
    return 0;
}
#include <Empy.h>

int32_t main(int32_t argc, char** argv) 
{
    using namespace Empy;
    auto app = new Application();
    app->RunContext(true);
    EMPY_DELETE(app);
    return 0;
}

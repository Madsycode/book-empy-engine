#include <Empy.h>
using namespace Empy;

struct MyLayer : AppInterface
{
    EMPY_INLINE void OnStart() 
    {
        AttachCallback<MouseMotionEvent>([this] (auto e) 
        {
            EMPY_TRACE("Mouse x:{} y:{}", e.TaregtX, e.TargetY);
        });
    }
};

int32_t main(int32_t argc, char** argv) 
{
    auto app = new Application();

    app->AttachCallback<KeyPressEvent>([] (auto e) {
        EMPY_TRACE("Key: {}", e.Key);
    });

    app->AttachLayer<MyLayer>();
    app->RunContext();
    return 0;
}
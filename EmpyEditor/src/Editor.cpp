#include "Windows/Inspector.h"
#include "Windows/Hierarchy.h"
#include "Windows/Resource.h"
#include "Windows/Viewport.h"
#include "Windows/MenuBar.h"

struct Editor : GuiContext
{
    EMPY_INLINE void OnGuiStart() 
    {
       PostEvent<SelectEvent>((EntityID)4);

       AttachWindow<HierarchyWindow>();
       AttachWindow<InspectorWindow>();
       AttachWindow<ResourceWindow>();
       AttachWindow<ViewportWindow>();
       AttachWindow<MenuBarWindow>();
    }

    EMPY_INLINE void OnGuiFrame() 
    {
        
    }
};

int32_t main(int32_t argc, char** argv) 
{
    auto app = new Application();
    app->AttachLayer<Editor>();
    app->RunContext(false);
    EMPY_DELETE(app);
    return 0;
}
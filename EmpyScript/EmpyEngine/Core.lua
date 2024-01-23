-- core script class
local EmpyScript = {}
local EmpyScript_mt = { __index = EmpyScript }

-- inits script base clasee
function EmpyScript.Constructor(entity)
    local self = setmetatable({}, EmpyScript_mt)
    self.Entity = entity
    return self
end

-- destroy entity with id
function EmpyScript.Destroy(entity)
    ApiDestroy(entity)
end

-- inits script class
function Initializer()
    -- script class 
    local ScriptKlass = {}
    local ScriptKlass_mt = { __index = ScriptKlass }
    setmetatable(ScriptKlass, { __index = EmpyScript })
    
    -- constructor
    function ScriptKlass.Constructor(entity)
        local obj = EmpyScript.Constructor(entity)
        self = setmetatable(obj, ScriptKlass_mt)    
        return self
    end
    
    -- apply force to rigidbody
    function ScriptKlass:ApplyForce(force)
        ApiApplyForce(self.Entity, force)
    end
    
    -- destroy self
    function ScriptKlass:Destroy()
        ApiDestroy(self.Entity)
    end

     -- get data
    function ScriptKlass:Get(type)
        if type == TRANSFORM then
            return ApiGetTransform(self.Entity)
        end        
        print("invalid type:", type);
        return {}
    end
    
    -- export class
    return ScriptKlass
end

-- export module
return EmpyScript

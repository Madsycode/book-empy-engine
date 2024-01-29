-- call once to init script
function TestScript.OnStart(self)
    self.Transform = self:Get(TRANSFORM)
    print("script started!")
end

-- update every frame
function TestScript.OnUpdate(self, dt)
    -- move entity to the left
    if Inputs.IsKey(Inputs.KEY_A) then
        self:ApplyForce(Vec3.new(-100.0, 0.0, 0.0));
    end
    
    -- move entity to the right
    if Inputs.IsKey(Inputs.KEY_D) then
        self:ApplyForce(Vec3.new(100.0, 0.0, 0.0));
    end
end 

-- call when key down
function TestScript.OnResize(self, width, height)
    print("resize: ", width, height)
end

-- call when mouse is down
function TestScript.OnMouseDown(self, button)
    print("mousedown: ", button)
end

-- call when colliding
function TestScript.OnCollision(self, other)
    print("collision:", self.Entity, "->", other)
    --self:Destroy(other);
end

-- call when key down
function TestScript.OnKeyDown(self, key)
    print("keydown: ", key)
end

-- call when entity is detroyed
function TestScript.OnDestroy(self)
    print("destroyed:", self.Entity)
end


--[[

Shared_Trigger - Door
Doors are 'fake' items that can block the passage to certain individuals according to their diplomacy

]]

function collision(item, collisionType, data)
    if collisionType == 2 then
        if data + 0 == get(item, "diplomacy") + 0 then -- The zeros force the conversion to numbers.
            return 2
        else
            return 0
        end
    elseif collisionType == 8 then
        return 10
    end
    return 0
end

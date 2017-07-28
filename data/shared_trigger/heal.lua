--[[

Shared_Trigger - Heal

]]

function collision(item, collisionType, data)
    if collisionType == 2 then
        set(item, "recuperation", 100)
    elseif collisionType == 8 then
        return 14
    end
    return 0
end

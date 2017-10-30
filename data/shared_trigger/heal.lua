--[[

Shared_Trigger - Heal

]]

function collision(item, collisionType, data)
    if collisionType == 2 then
        set(item, "healing", 100)
    elseif collisionType == 8 then
        return 14
    end
    return 0
end

function mouseHovering(data)

end

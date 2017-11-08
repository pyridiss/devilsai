--[[

Shared_Trigger - Door
Doors are 'fake' items that can block the passage to certain individuals according to their diplomacy

]]

function collision(item, apply, data)
    if data + 0 == get(item, "diplomacy") + 0 or data + 0 == -1 then -- The zeros force the conversion to numbers.
        return 2
    end
    return 0
end

function mouseHovering(data)

end

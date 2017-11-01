--[[

Shared_Trigger - World Change

]]

function collision(item, collisionType, data)
    if collisionType == 2 then
        w, p, t = string.match(data, "(.+):(.+):(.+)")

        movementDone = moveItemTo(item, w, p)

        if movementDone then
            if item == getElement("player") then
                updateCurrentWorld(w)
            end
        end

        return 0
    end

    return 0
end

function mouseHovering(data)
    w, p, t = string.match(data, "(.+):(.+):(.+)")
    addTooltip(t)
end

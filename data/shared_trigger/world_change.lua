--[[

Shared_Trigger - World Change

]]

function collision(item, collisionType, data)
    if collisionType == 2 then
        w, p = string.match(data, "(.+):(.+)")

        moveItemTo(item, w, p)

        if item == getElement("player") then
            updateCurrentWorld(w)
        end

        return 0
    end

    return 0
end

function mouseHovering(data)

end

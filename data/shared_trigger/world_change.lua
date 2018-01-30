--[[

Shared_Trigger - World Change

]]

function collision(item, apply, data)
    if apply then
        w, p, t = string.match(data, "(.+):(.+):(.+)")

        movementDone = moveItemTo(item, w, p)

        if movementDone then
            if item == getElement("player") then
                updateCurrentWorld(w)
                stopPlayer()
            end
        end

        return 0
    end

    return 4
end

function mouseHovering(data)
    w, p, t = string.match(data, "(.+):(.+):(.+)")
    addTooltip(t)
end

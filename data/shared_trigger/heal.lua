--[[

Shared_Trigger - Heal

]]

function collision(item, apply, data)
    if apply then
        set(item, "healing", 100)
        return 7 -- 7 = COLL_LIFE
    else
        return 7 -- 7 = COLL_LIFE
    end
end

function mouseHovering(data)

end

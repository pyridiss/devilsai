--[[

Shared_Trigger - Heal

]]

function collision(item, apply, data)
    if apply then
        set(item, "healing", 100)
    else
        return 5 -- 5 = Behaviors::REGEN
    end
end

function mouseHovering(data)

end

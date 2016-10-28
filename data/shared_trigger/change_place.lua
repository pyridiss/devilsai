--[[

Shared_Trigger - Change Place

Use example:
SHARED_TRIGGER
type: change_place
position: 342 1
size: rectangle 1 100
argument_1: 1010
argument_2: village
END_SHARED_TRIGGER

]]


-- global section
-- --------------

triggers = {}

tmp_id = 0
tmp_place_name  = "not_defined"
tmp_place_type  = "not_defined"

-- functions
-- ---------

function newTriggerSetId(id)

	tmp_id = id

end

function newTriggerSetArgument(arg)

	if tmp_place_name == "not_defined" then
		tmp_place_name = arg
	elseif tmp_place_type == "not_defined" then
		tmp_place_type = arg
	end

end

function newTriggerEnd()

	table.insert(triggers, {id = tmp_id, place_name = tmp_place_name, place_type = tmp_place_type})
	tmp_place_name = "not_defined"
	tmp_place_type = "not_defined"

end

function triggerManage()

	for i = 1, #triggers, 1 do
		activated = triggerActivated(triggers[i]["id"])

		if activated == true then
			changePlace(triggers[i]["place_name"], triggers[i]["place_type"])
		end
	end
end
